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

property_double (flower_size, _("Flower Size"), 80.0)
  description (_("Average diameter of each flower in pixels"))
  value_range (20.0, 200.0)
  ui_range (20.0, 150.0)

property_double (flower_spacing, _("Flower Spacing"), 30.0)
  description (_("Spacing between flowers in pixels"))
  value_range (5.0, 100.0)
  ui_range (5.0, 50.0)

property_double (size_variation, _("Size Variation"), 0.3)
  description (_("Random variation in flower size (0 = none, 1 = high)"))
  value_range (0.0, 1.0)
  ui_range (0.0, 0.5)

property_double (rotation_variation, _("Rotation Variation"), 30.0)
  description (_("Random rotation variation per flower in degrees"))
  value_range (0.0, 90.0)
  ui_range (0.0, 45.0)

property_double (petal_elongation, _("Petal Elongation"), 1.5)
  description (_("How elongated the petals are (higher = more elongated)"))
  value_range (1.0, 3.0)
  ui_range (1.0, 2.0)

property_double (shading_intensity, _("Shading Intensity"), 0.4)
  description (_("Intensity of shading for 3D effect on petals"))
  value_range (0.0, 1.0)
  ui_range (0.0, 0.5)

property_double (opacity, _("Opacity"), 1.0)
  description (_("Opacity of the flower pattern"))
  value_range (0.0, 1.0)
  ui_range (0.0, 1.0)

property_color (petal_color, _("Petal Color"), "#ff4040")
  description (_("Base color of the flower petals (e.g., red for hibiscus)"))

property_color (center_color, _("Center Color"), "#ffff00")
  description (_("Color of the flower center and stamen"))

property_color (background_color, _("Background Color"), "#000000")
  description (_("Background color behind the flowers"))

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

/* Simple noise function for texture */
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
  gfloat petal_color[4], center_color[4], bg_color[4];
  gegl_color_get_pixel (o->petal_color, format, petal_color);
  gegl_color_get_pixel (o->center_color, format, center_color);
  gegl_color_get_pixel (o->background_color, format, bg_color);

  /* Flower pattern parameters */
  gfloat period = o->flower_size + o->flower_spacing;
  gfloat base_radius = o->flower_size * 0.5f;
  gfloat center_radius = o->flower_size * 0.15f;
  gfloat stamen_length = o->flower_size * 0.4f;

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

            /* Default to background color */
            gfloat *color = bg_color;
            gfloat alpha = 1.0f;
            gfloat lighting = 1.0f;

            /* Find the nearest flower center with staggered grid */
            gfloat row_offset = (floorf (py / period) * 0.5f * period);
            gfloat cx = floorf ((px - row_offset) / period) * period + period * 0.5f + row_offset;
            gfloat cy = floorf (py / period) * period + period * 0.5f;

            /* Compute distance to flower center */
            gfloat dx = px - cx;
            gfloat dy = py - cy;
            gfloat dist = sqrtf (dx * dx + dy * dy);

            /* Per-flower variations */
            gfloat seed = noise (cx / period, cy / period);
            gfloat size_factor = 1.0f + o->size_variation * (seed - 0.5f);
            gfloat petal_radius = base_radius * size_factor;
            gfloat flower_rotation = o->rotation_variation * (seed - 0.5f) * G_PI / 180.0f;

            /* Rotate coordinates relative to flower center */
            gfloat angle = atan2f (dy, dx) + flower_rotation;
            gfloat rdist = dist / petal_radius;

            /* Draw the flower center */
            if (dist < center_radius * size_factor)
              {
                color = center_color;
                alpha = 1.0f;
                gfloat gradient = 1.0f - dist / (center_radius * size_factor);
                lighting = 1.0f + o->shading_intensity * gradient;
                /* Add small pollen-like details */
                gfloat noise_val = noise (px * 0.1f, py * 0.1f);
                if (noise_val > 0.8f && dist > center_radius * 0.3f)
                  lighting *= 1.2f;
              }

            /* Draw the stamen (thicker with anthers) */
            gfloat stamen_angle = G_PI / 4.0f + flower_rotation;
            gfloat stamen_dist = dist * cosf (angle - stamen_angle);
            if (stamen_dist > center_radius && stamen_dist < stamen_length * size_factor &&
                fabsf (dist * sinf (angle - stamen_angle)) < center_radius * 0.5f)
              {
                color = center_color;
                alpha = 1.0f;
                lighting = 1.0f - o->shading_intensity * (stamen_dist - center_radius) / (stamen_length - center_radius);
                /* Add anther at the tip */
                if (stamen_dist > stamen_length * 0.8f * size_factor)
                  lighting *= 1.3f;
              }

            /* Draw five petals with irregular shapes */
            gfloat petal_angle = fmodf (angle, 2.0f * G_PI / 5.0f) - 2.0f * G_PI / 10.0f;
            gfloat petal_dist = dist * (1.0f + 0.3f * sinf (petal_angle * 5.0f)); /* Add curve to petal shape */
            gfloat petal_width = petal_radius * 0.6f;

            if (petal_dist < petal_radius && fabsf (petal_angle) < G_PI / 5.0f)
              {
                /* Elongated petal with pointed tip */
                gfloat t = petal_dist / petal_radius;
                gfloat w = petal_width * (1.0f - t * t) * o->petal_elongation;
                gfloat petal_alpha = CLAMP (1.0f - fabsf (petal_angle) / (G_PI / 5.0f) * petal_radius / w, 0.0f, 1.0f);

                if (petal_alpha > 0.0f)
                  {
                    color = petal_color;
                    alpha = petal_alpha;

                    /* Color gradient: darker at base, lighter at tip */
                    gfloat gradient = 1.0f - t;
                    lighting = 1.0f + o->shading_intensity * gradient;

                    /* Add vein-like texture */
                    gfloat vein_noise = noise (px * 0.05f, py * 0.05f);
                    lighting += 0.1f * vein_noise * gradient;
                  }
              }

            /* Apply color and blend */
            gfloat final_color[4];
            for (gint j = 0; j < 3; j++)
              final_color[j] = CLAMP (color[j] * lighting, 0.0f, 1.0f);
            final_color[3] = alpha;

            for (gint j = 0; j < 4; j++)
              {
                out_data[offset + j] = (1.0f - o->opacity * alpha) * in_data[offset + j] +
                                       (o->opacity * alpha) * final_color[j];
                if (j == 3)
                  out_data[offset + j] = 1.0f; /* Fully opaque output */
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
    "description", _("Renders a natural pattern of Hawaiian flowers, such as hibiscus, with organic shapes and textures"),
    "gimp:menu-path", "<Image>/Filters/Render/Pattern",
    "gimp:menu-label", _("Hawaiian Flowers Pattern"),
    NULL);
}

#endif
