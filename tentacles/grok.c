/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2025 Beaver, Grok Tentacles
 */

#include <gegl.h>
#include <gegl-plugin.h>
#include <glib-object.h>
#include <math.h>
#include <stdlib.h>

G_BEGIN_DECLS

#define GEGL_TYPE_OP_GROK            (gegl_op_grok_get_type ())
#define GEGL_OP_GROK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_OP_GROK, GeglOpGrok))
#define GEGL_OP_GROK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_TYPE_OP_GROK, GeglOpGrokClass))
#define GEGL_IS_OP_GROK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_OP_GROK))
#define GEGL_IS_OP_GROK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GEGL_TYPE_OP_GROK))
#define GEGL_OP_GROK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_TYPE_OP_GROK, GeglOpGrokClass))

typedef struct _GeglOpGrok
{
  GeglOperationPointComposer parent_instance;
  gdouble tentacle_count;
  gdouble length;
  gdouble curvature;
  gdouble thickness;
  gdouble hue;
  gdouble lightness;
  gdouble opacity;
  gdouble shadow_opacity;
  guint32 seed;
} GeglOpGrok;

typedef struct _GeglOpGrokClass
{
  GeglOperationPointComposerClass parent_class;
} GeglOpGrokClass;

GType gegl_op_grok_get_type (void) G_GNUC_CONST;

static void get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static gboolean process (GeglOperation *operation,
                        void *input,
                        void *aux,
                        void *output,
                        const GeglRectangle *result,
                        gint level);
static void gegl_op_grok_class_init (GeglOpGrokClass *klass);
static void gegl_op_grok_init (GeglOpGrok *self);

G_END_DECLS

static void
hsl_to_rgb (gdouble h, gdouble s, gdouble l, gdouble *r, gdouble *g, gdouble *b)
{
  h = fmod (h, 360.0) / 360.0;
  s = CLAMP (s, 0.0, 1.0);
  l = CLAMP (l, 0.0, 1.0);

  if (s == 0) {
    *r = *g = *b = l;
  } else {
    gdouble q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    gdouble p = 2 * l - q;
    gdouble t[3] = { h + 1.0/3.0, h, h - 1.0/3.0 };
    for (int i = 0; i < 3; i++) {
      if (t[i] < 0) t[i] += 1.0;
      if (t[i] > 1) t[i] -= 1.0;
      gdouble c;
      if (t[i] < 1.0/6.0) c = p + (q - p) * 6 * t[i];
      else if (t[i] < 0.5) c = q;
      else if (t[i] < 2.0/3.0) c = p + (q - p) * 6 * (2.0/3.0 - t[i]);
      else c = p;
      if (i == 0) *r = c;
      else if (i == 1) *g = c;
      else *b = c;
    }
  }
}

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  GeglOpGrok *self = GEGL_OP_GROK (object);
  switch (prop_id) {
    case 1: g_value_set_double (value, self->tentacle_count); break;
    case 2: g_value_set_double (value, self->length); break;
    case 3: g_value_set_double (value, self->curvature); break;
    case 4: g_value_set_double (value, self->thickness); break;
    case 5: g_value_set_double (value, self->hue); break;
    case 6: g_value_set_double (value, self->lightness); break;
    case 7: g_value_set_double (value, self->opacity); break;
    case 8: g_value_set_double (value, self->shadow_opacity); break;
    case 9: g_value_set_uint (value, self->seed); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
  }
}

static void
set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  GeglOpGrok *self = GEGL_OP_GROK (object);
  switch (prop_id) {
    case 1: self->tentacle_count = g_value_get_double (value); break;
    case 2: self->length = g_value_get_double (value); break;
    case 3: self->curvature = g_value_get_double (value); break;
    case 4: self->thickness = g_value_get_double (value); break;
    case 5: self->hue = g_value_get_double (value); break;
    case 6: self->lightness = g_value_get_double (value); break;
    case 7: self->opacity = g_value_get_double (value); break;
    case 8: self->shadow_opacity = g_value_get_double (value); break;
    case 9: self->seed = g_value_get_uint (value); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
  }
}

static gboolean
process (GeglOperation *operation,
         void *input,
         void *aux,
         void *output,
         const GeglRectangle *result,
         gint level)
{
  GeglOpGrok *self = GEGL_OP_GROK (operation);
  GeglBuffer *output_buf = (GeglBuffer *) output;
  gfloat *out_buf = g_new (gfloat, result->width * result->height * 4);
  gint i;

  /* Initialize output buffer (transparent) */
  for (i = 0; i < result->width * result->height * 4; i += 4) {
    out_buf[i] = 0.0f;   /* R */
    out_buf[i+1] = 0.0f; /* G */
    out_buf[i+2] = 0.0f; /* B */
    out_buf[i+3] = 0.0f; /* A */
  }

  /* Initialize random seed */
  srand (self->seed);

  /* Render shadows first */
  gint count = (gint) CLAMP (self->tentacle_count, 1.0, 50.0);
  for (int t = 0; t < count; t++) {
    gdouble base_x = (rand () % result->width) + result->x;
    gdouble base_y = (rand () % result->height) + result->y;
    gdouble amp = self->curvature * (0.5 + (rand () % 100) / 200.0);
    gdouble freq = 0.05 + (rand () % 50) / 1000.0;
    gdouble phase = (rand () % 360) * G_PI / 180.0;
    gdouble max_len = self->length * (0.5 + (rand () % 100) / 200.0);
    gdouble base_thickness = self->thickness * (0.5 + (rand () % 100) / 200.0);

    /* Shadow: darker, offset, slightly blurred */
    for (gdouble s = 0; s < max_len; s += 0.5) {
      gdouble t = s / max_len;
      gdouble x = base_x + s + amp * sin (freq * s + phase) + 5.0; /* Offset for shadow */
      gdouble y = base_y + amp * cos (freq * s + phase) + 5.0;
      gdouble width = base_thickness * exp (-2.0 * t) * 1.2; /* Slightly wider for blur effect */

      for (int dy = -ceil(width); dy <= ceil(width); dy++) {
        for (int dx = -ceil(width); dx <= ceil(width); dx++) {
          gdouble dist = sqrt (dx * dx + dy * dy);
          if (dist <= width) {
            gint px = (gint) (x + dx - result->x);
            gint py = (gint) (y + dy - result->y);
            if (px >= 0 && px < result->width && py >= 0 && py < result->height) {
              gint idx = (py * result->width + px) * 4;
              gdouble shade = 1.0 - dist / width;
              gfloat alpha = self->shadow_opacity * shade * (1.0 - t) * 0.5;
              out_buf[idx] = 0.0f; /* Dark shadow */
              out_buf[idx+1] = 0.0f;
              out_buf[idx+2] = 0.0f;
              out_buf[idx+3] = MAX (out_buf[idx+3], alpha);
            }
          }
        }
      }
    }
  }

  /* Render tentacles */
  srand (self->seed); /* Reset seed for consistent placement */
  for (int t = 0; t < count; t++) {
    gdouble base_x = (rand () % result->width) + result->x;
    gdouble base_y = (rand () % result->height) + result->y;
    gdouble amp = self->curvature * (0.5 + (rand () % 100) / 200.0);
    gdouble freq = 0.05 + (rand () % 50) / 1000.0;
    gdouble phase = (rand () % 360) * G_PI / 180.0;
    gdouble max_len = self->length * (0.5 + (rand () % 100) / 200.0);
    gdouble base_thickness = self->thickness * (0.5 + (rand () % 100) / 200.0);

    gdouble r, g, b;
    hsl_to_rgb (self->hue + (rand () % 60 - 30), 0.7, self->lightness / 30.0 + 0.5, &r, &g, &b);

    for (gdouble s = 0; s < max_len; s += 0.5) {
      gdouble t = s / max_len;
      gdouble x = base_x + s + amp * sin (freq * s + phase);
      gdouble y = base_y + amp * cos (freq * s + phase);
      gdouble width = base_thickness * exp (-2.0 * t);

      for (int dy = -ceil(width); dy <= ceil(width); dy++) {
        for (int dx = -ceil(width); dx <= ceil(width); dx++) {
          gdouble dist = sqrt (dx * dx + dy * dy);
          if (dist <= width) {
            gint px = (gint) (x + dx - result->x);
            gint py = (gint) (y + dy - result->y);
            if (px >= 0 && px < result->width && py >= 0 && py < result->height) {
              gint idx = (py * result->width + px) * 4;
              gdouble shade = 1.0 - dist / width;
              gfloat alpha = self->opacity * shade * (1.0 - t);
              out_buf[idx] = r * shade;
              out_buf[idx+1] = g * shade;
              out_buf[idx+2] = b * shade;
              out_buf[idx+3] = MAX (out_buf[idx+3], alpha);
            }
          }
        }
      }
    }
  }

  /* Write to output buffer */
  gegl_buffer_set (output_buf, result, 0, babl_format ("RGBA float"), out_buf, GEGL_AUTO_ROWSTRIDE);
  g_free (out_buf);
  return TRUE;
}

static void
gegl_op_grok_init (GeglOpGrok *self)
{
  /* No specific initialization needed */
}

static void
gegl_op_grok_class_init (GeglOpGrokClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationPointComposerClass *point_class = GEGL_OPERATION_POINT_COMPOSER_CLASS (klass);

  object_class->set_property = set_property;
  object_class->get_property = get_property;
  point_class->process = process;

  g_object_class_install_property (object_class, 1,
    g_param_spec_double ("tentacle-count", "Tentacle Count", "Number of tentacles",
                         1.0, 50.0, 10.0, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 2,
    g_param_spec_double ("length", "Tentacle Length", "Maximum length of tentacles",
                         50.0, 500.0, 200.0, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 3,
    g_param_spec_double ("curvature", "Tentacle Curvature", "Amplitude of tentacle waves",
                         10.0, 100.0, 50.0, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 4,
    g_param_spec_double ("thickness", "Tentacle Thickness", "Base thickness of tentacles",
                         5.0, 50.0, 20.0, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 5,
    g_param_spec_double ("hue", "Hue Rotation", "Color hue for tentacles (e.g., 90 for purple)",
                         -180.0, 180.0, 90.0, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 6,
    g_param_spec_double ("lightness", "Lightness", "Lightness adjustment for tentacles",
                         -15.0, 15.0, 0.0, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 7,
    g_param_spec_double ("opacity", "Opacity", "Overall opacity of tentacles",
                         0.0, 1.0, 0.7, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 8,
    g_param_spec_double ("shadow-opacity", "Shadow Opacity", "Opacity of the drop shadow under tentacles",
                         0.0, 1.0, 0.5, G_PARAM_READWRITE));
  g_object_class_install_property (object_class, 9,
    g_param_spec_uint ("seed", "Random Seed", "Random seed for tentacle placement",
                       0, G_MAXUINT, 0, G_PARAM_READWRITE));

  gegl_operation_class_set_keys (operation_class,
    "name", "gegl:grok",
    "title", "Grok Tentacles",
    "categories", "render",
    "description", "Renders 2D octopus-like tentacles inspired by Xscreensaver Sky Tentacles",
    "gimp:menu-path", "<Image>/Filters/Grok/",
    "gimp:menu-label", "Grok Tentacles...",
    NULL);
}

#endif
