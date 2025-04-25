/* grok.c
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
#include <gegl.h>
#include <stdio.h> /* For console debugging */

#ifdef GEGL_PROPERTIES

property_double (dot_size, _("Dot Size"), 30.0)
  description (_("Average diameter of polka dots in pixels"))
  value_range (5.0, 100.0)
  ui_range (5.0, 50.0)

property_double (dot_spacing, _("Dot Spacing"), 50.0)
  description (_("Average spacing between dots in pixels"))
  value_range (10.0, 200.0)
  ui_range (10.0, 100.0)

property_double (size_variation, _("Size Variation"), 0.5)
  description (_("Random variation in dot sizes (0.0 = uniform, 1.0 = high variation)"))
  value_range (0.0, 1.0)
  ui_range (0.0, 1.0)

property_double (color_variation, _("Color Variation"), 0.2)
  description (_("Random variation in dot colors"))
  value_range (0.0, 0.5)
  ui_range (0.0, 0.3)

property_color (dot_color, _("Dot Color"), "#ff4040")
  description (_("Base color of the polka dots"))

property_int (seed, _("Random Seed"), 0)
  description (_("Seed for random number generation"))
  value_range (0, G_MAXINT)
  ui_range (0, 1000)

#else

#define GEGL_OP_SOURCE
#define GEGL_OP_NAME     grok
#define GEGL_OP_C_SOURCE grok.c

#include "gegl-op.h"

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "output", babl_format ("RGBA float"));
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  /* Default to a reasonable size if no input; adjust as needed */
  return (GeglRectangle){0, 0, 1024, 1024};
}

/* Simple noise function for random variation */
static gfloat
noise (gfloat x, gfloat y, gint seed)
{
  gfloat val = sinf (x * 12.9898f + y * 78.233f + seed) * 43758.5453f;
  return val - floorf(val);
}

static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *output,
         const GeglRectangle *result,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  const Babl *format = babl_format ("RGBA float");
  GeglBufferIterator *iter;

  if (result->width < 1 || result->height < 1)
    return TRUE;

  /* Clear output buffer to transparent */
  iter = gegl_buffer_iterator_new (output, result, 0, format,
                                  GEGL_ACCESS_WRITE, GEGL_ABYSS_NONE, 1);
  while (gegl_buffer_iterator_next (iter))
    {
      gfloat *out_data = iter->items[0].data;
      GeglRectangle roi = iter->items[0].roi;
      gint x, y;

      for (y = 0; y < roi.height; y++)
        for (x = 0; x < roi.width; x++)
          {
            gint offset = (y * roi.width + x) * 4;
            out_data[offset + 0] = 0.0f;
            out_data[offset + 1] = 0.0f;
            out_data[offset + 2] = 0.0f;
            out_data[offset + 3] = 0.0f;
          }
    }

  /* Dot parameters */
  gfloat spacing = o->dot_spacing;
  gfloat base_radius = o->dot_size * 0.5f;
  gint max_dots = 10000; /* Cap to prevent performance issues */
  gint num_dots = MIN((gint)((result->width * result->height) / (spacing * spacing)), max_dots);
  gdouble color[3];
  gegl_color_get_rgba (o->dot_color, &color[0], &color[1], &color[2], NULL);

  /* Debug dot count */
  fprintf(stderr, "Grok: Rendering %d polka dots with seed %d\n", num_dots, o->seed);

  /* Render polka dots */
  for (gint i = 0; i < num_dots; i++)
    {
      /* Random dot center using noise with seed */
      gfloat cx = result->x + result->width * noise(i, 0.0f, o->seed);
      gfloat cy = result->y + result->height * noise(0.0f, i, o->seed);
      cx = CLAMP(cx, result->x, result->x + result->width - 1);
      cy = CLAMP(cy, result->y, result->y + result->height - 1);

      /* Random size variation */
      gfloat size_factor = 1.0f + o->size_variation * (noise(i, 1.0f, o->seed) - 0.5f);
      gfloat radius = base_radius * size_factor;

      /* Random color variation */
      gfloat dot_color[3];
      for (gint j = 0; j < 3; j++)
        dot_color[j] = CLAMP((gfloat)color[j] + o->color_variation * (noise(i, (gfloat)j, o->seed) - 0.5f), 0.0f, 1.0f);

      /* Define bounding box for the dot */
      gint dot_size = (gint)(radius * 2.0f);
      GeglRectangle dot_roi = {
        .x = (gint)(cx - radius),
        .y = (gint)(cy - radius),
        .width = dot_size,
        .height = dot_size
      };
      dot_roi.x = CLAMP(dot_roi.x, result->x, result->x + result->width - dot_roi.width);
      dot_roi.y = CLAMP(dot_roi.y, result->y, result->y + result->height - dot_roi.height);

      /* Render dot */
      iter = gegl_buffer_iterator_new (output, &dot_roi, 0, format,
                                      GEGL_ACCESS_READWRITE, GEGL_ABYSS_NONE, 1);
      while (gegl_buffer_iterator_next (iter))
        {
          gfloat *out_data = iter->items[0].data;
          GeglRectangle roi = iter->items[0].roi;
          gint x, y;

          for (y = 0; y < roi.height; y++)
            for (x = 0; x < roi.width; x++)
              {
                gint offset = (y * roi.width + x) * 4;
                gfloat px = x + roi.x;
                gfloat py = y + roi.y;

                /* Compute distance from dot center */
                gfloat dx = px - cx;
                gfloat dy = py - cy;
                gfloat dist = sqrtf (dx * dx + dy * dy);

                /* Circular dot with smooth edges */
                gfloat alpha = CLAMP(1.0f - dist / radius, 0.0f, 1.0f);
                if (alpha > 0.0f)
                  {
                    gfloat dest_alpha = out_data[offset + 3];
                    gfloat final_alpha = alpha + dest_alpha * (1.0f - alpha);
                    if (final_alpha > 0.0f)
                      {
                        for (gint j = 0; j < 3; j++)
                          out_data[offset + j] = (dot_color[j] * alpha + out_data[offset + j] * dest_alpha * (1.0f - alpha)) / final_alpha;
                        out_data[offset + 3] = final_alpha;
                      }
                  }
              }
        }
    }

  fprintf(stderr, "Grok: Polka dots rendered\n");
  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationSourceClass *source_class = GEGL_OPERATION_SOURCE_CLASS (klass);

  operation_class->prepare = prepare;
  operation_class->get_bounding_box = get_bounding_box;
  source_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:grok",
    "title",       _("Grok Polka Dots"),
    "categories",  "render:pattern",
    "description", _("Generates a random polka dots pattern with variable size and color"),
    "gimp:menu-path", "<Image>/Filters/Render/Pattern/",
    "gimp:menu-label", _("Grok Polka Dots"),
    NULL);
}

#endif
