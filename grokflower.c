/* hawaiian_flowers.c
 *
 * Copyright (C) 2025 LinuxBeaver and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include <math.h>

#ifdef GEGL_PROPERTIES

property_double (flower_size, _("Flower Size"), 60.0)
  description (_("Diameter of larger flowers in pixels"))
  value_range (20.0, 200.0)
  ui_range (20.0, 100.0)

property_double (flower_spacing, _("Flower Spacing"), 40.0)
  description (_("Spacing between flowers in pixels"))
  value_range (5.0, 100.0)
  ui_range (5.0, 50.0)

property_double (size_ratio, _("Small Flower Size Ratio"), 0.5)
  description (_("Ratio of small flower size to large flower size"))
  value_range (0.2, 1.0)
  ui_range (0.2, 0.8)

property_double (rotation_variation, _("Rotation Variation"), 20.0)
  description (_("Random rotation variation per flower in degrees"))
  value_range (0.0, 90.0)
  ui_range (0.0, 45.0)

property_double (petal_scale, _("Petal Roundness"), 1.0)
  description (_("Controls petal shape: lower values for rounder petals, higher for teardrop-shaped"))
  value_range (0.5, 2.0)
  ui_range (0.5, 1.5)

property_color (petal_color, _("Petal Color"), "#ff4040")
  description (_("Color of the flower petals (e.g., red for hibiscus)"))

property_color (center_color, _("Center Color"), "#ffff00")
  description (_("Color of the flower center"))

#else

#define GEGL_OP_FILTER
#define GEGL_OP_NAME     grok
#define GEGL_OP_C_SOURCE grok.c

#include "gegl-op.h"

static void
prepare (GeglOperation *operation)
{
  const Babl *format = babl_format ("RGBA float");
  gegl_operation_set_format (operation, "input", format);
  gegl_operation_set_format (operation, "output", format);
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  GeglRectangle *in_rect = gegl_operation_source_get_bounding_box (operation, "input");
  return in_rect ? *in_rect : (GeglRectangle){0, 0, 0, 0};
}

static GeglRectangle
get_required_for_output (GeglOperation       *operation,
                        const gchar         *input_pad,
                        const GeglRectangle *roi)
{
  return get_bounding_box (operation);
}

/* Simple noise function for random variation */
static gfloat
noise (gfloat x, gfloat y)
{
  return sinf (x * 12.9898f + y * 78.233f) * 43758.5453f - floorf (sinf (x * 12.9898f + y * 78.233f) * 43758.5453f);
}

static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *input,
         GeglBuffer          *output,
         const GeglRectangle *result,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  const Babl *format = babl_format ("RGBA float");
  GeglBufferIterator *iter;

  if (result->width < 1 || result->height < 1)
    {
      if (input != output)
        gegl_buffer_copy (input, result, GEGL_ABYSS_CLAMP, output, result);
      return TRUE;
    }

  iter = gegl_buffer_iterator_new (input, result, 0, format,
                                  GEGL_ACCESS_READ, GEGL_ABYSS_CLAMP, 2);
  gegl_buffer_iterator_add (iter, output, result, 0, format,
                           GEGL_ACCESS_WRITE, GEGL_ABYSS_CLAMP);

  /* Get colors */
  gfloat petal_color[4], center_color[4];
  gegl_color_get_pixel (o->petal_color, format, petal_color);
  gegl_color_get_pixel (o->center_color, format, center_color);

  /* Flower pattern parameters */
  gfloat period = o->flower_size + o->flower_spacing;
  gfloat base_radius = o->flower_size * 0.5f;
  gfloat center_radius = o->flower_size * 0.1f;

  while (gegl_buffer_iterator_next (iter))
    {
      gfloat *in_data = iter->items[0].data;
      gfloat *out_data = iter->items[1].data;
      GeglRectangle roi = iter->items[0].roi;
      gint x, y;

      for (y = 0; y < roi.height; y++)
        for (x = 0; x < roi.width; x++)
          {
            gint offset = (y * roi.width + x) * 4;
            gfloat px = x + roi.x;
            gfloat py = y + roi.y;

            /* Default to transparent background */
            gfloat *color = in_data + offset;
            gfloat alpha = 0.0f; /* Transparent by default */

            /* Find the nearest flower center with staggered grid */
            gfloat row_offset = (floorf (py / period) * 0.5f * period);
            gfloat cx = floorf ((px - row_offset) / period) * period + period * 0.5f + row_offset;
            gfloat cy = floorf (py / period) * period + period * 0.5f;

            /* Compute distance to flower center */
            gfloat dx = px - cx;
            gfloat dy = py - cy;
            gfloat dist = sqrtf (dx * dx + dy * dy);

            /* Alternate between large and small flowers */
            gfloat seed = noise (cx / period, cy / period);
            gfloat row = floorf (py / period);
            gfloat col = floorf ((px - row_offset) / period);
            gfloat size_factor = (fmodf (row + col, 2.0f) < 1.0f) ? 1.0f : o->size_ratio;

            gfloat petal_radius = base_radius * size_factor;
            gfloat flower_center_radius = center_radius * size_factor;

            /* Per-flower rotation */
            gfloat flower_rotation = o->rotation_variation * (seed - 0.5f) * G_PI / 180.0f;
            gfloat angle = atan2f (dy, dx) + flower_rotation;

            /* Draw the flower center */
            if (dist < flower_center_radius)
              {
                color = center_color;
                alpha = 1.0f; /* Fully opaque */
              }

            /* Draw five petals with dynamic shape */
            gfloat petal_angle = fmodf (angle, 2.0f * G_PI / 5.0f) - 2.0f * G_PI / 10.0f;
            gfloat petal_width = petal_radius * 0.5f;

            if (dist < petal_radius && fabsf (petal_angle) < G_PI / 5.0f)
              {
                /* Petal shape with roundness controlled by petal_scale */
                gfloat t = dist / petal_radius;
                gfloat shape_factor = (o->petal_scale - 0.5f) / 1.5f; /* Normalize to [0,1] */
                gfloat w = petal_width * (1.0f - powf(t, 2.0f + shape_factor * 2.0f)); /* Dynamic tapering */

                /* CHANGED: Sharper alpha for more opaque petals */
                gfloat angular_distance = fabsf (petal_angle) / (G_PI / 5.0f);
                gfloat petal_alpha = (angular_distance * petal_radius / w < 0.9f) ? 1.0f : 0.0f; /* Threshold for sharp edges */

                if (petal_alpha > 0.0f)
                  {
                    color = petal_color;
                    alpha = petal_alpha; /* Opaque within petal boundaries */
                  }
              }

            /* Apply color and set alpha */
            gfloat final_color[4];
            for (gint j = 0; j < 3; j++)
              final_color[j] = color[j];
            final_color[3] = alpha;

            for (gint j = 0; j < 4; j++)
              {
                if (alpha > 0.0f)
                  out_data[offset + j] = final_color[j]; /* Flowers are fully opaque */
                else
                  out_data[offset + j] = 0.0f; /* Transparent background */
                if (j == 3)
                  out_data[offset + j] = alpha; /* Set alpha for transparency */
              }
          }
    }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass     *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationFilterClass *filter_class  = GEGL_OPERATION_FILTER_CLASS (klass);

  operation_class->prepare         = prepare;
  operation_class->get_bounding_box = get_bounding_box;
  operation_class->get_required_for_output = get_required_for_output;
  filter_class->process            = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:hawaiian_flowers",
    "title",       _("Hawaiian Flowers Pattern"),
    "categories",  "render:pattern",
    "description", _("Renders a stylized Hawaiian flower pattern with teardrop-shaped petals in a staggered grid, against a transparent background"),
    "gimp:menu-path", "<Image>/Filters/Render/Pattern",
    "gimp:menu-label", _("Hawaiian Flowers Pattern"),
    NULL);
}

#endif
