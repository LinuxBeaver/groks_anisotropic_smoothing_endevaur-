/* smooth.c
 *
 * Copyright (C) 2006-2013 Øyvind Kolås, Nicolas Robidoux, Geert Jordaens, 
 * Sven Neumann, Martin Nordholts, Richard D. Worth, Mukund Sivaraman, 
 * and others
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

property_int (iterations, _("Iterations"), 10)
  description (_("Number of iterations; more iterations cause stronger smoothing"))
  value_range (1, 20)
  ui_range (1, 15)

property_double (alpha, _("Alpha"), 0.6)
  description (_("Diffusion strength in homogeneous regions"))
  value_range (0.1, 1.0)
  ui_range (0.1, 0.8)

property_double (kappa, _("Kappa"), 4.0)
  description (_("Edge sensitivity parameter; lower values preserve sharper edges"))
  value_range (1.0, 15.0)
  ui_range (1.0, 10.0)

property_double (strength, _("Strength"), 2.5)
  description (_("Overall intensity of the smoothing effect"))
  value_range (0.5, 5.0)
  ui_range (0.5, 4.0)

property_double (delta_t, _("Delta T"), 0.3)
  description (_("Time step for numerical stability"))
  value_range (0.05, 0.5)
  ui_range (0.05, 0.4)

#else

#define GEGL_OP_FILTER
#define GEGL_OP_NAME     smooth
#define GEGL_OP_C_SOURCE smooth.c

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

static inline gfloat
conductance (gfloat gradient, gfloat kappa)
{
  gfloat g = gradient / kappa;
  return expf (-g * g); /* Gaussian conductance for edge-preserving smoothing */
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
  GeglBuffer *temp;
  gint i;

  if (result->width < 2 || result->height < 2)
    {
      if (input != output)
        gegl_buffer_copy (input, result, GEGL_ABYSS_CLAMP, output, result);
      return TRUE;
    }

  /* Create a temporary buffer for intermediate results */
  temp = gegl_buffer_new (result, format);
  gegl_buffer_copy (input, result, GEGL_ABYSS_CLAMP, temp, result);

  /* Perform diffusion iterations */
  for (i = 0; i < o->iterations; i++)
    {
      iter = gegl_buffer_iterator_new (temp, result, 0, format,
                                      GEGL_ACCESS_READ, GEGL_ABYSS_LOOP, 2);
      gegl_buffer_iterator_add (iter, output, result, 0, format,
                               GEGL_ACCESS_WRITE, GEGL_ABYSS_CLAMP);

      while (gegl_buffer_iterator_next (iter))
        {
          gfloat *in_data = iter->items[0].data;
          gfloat *out_data = iter->items[1].data;
          GeglRectangle roi = iter->items[0].roi;
          GeglRectangle in_roi = roi;
          /* Expand ROI by 2 pixels for gradient calculations */
          in_roi.x -= 2;
          in_roi.y -= 2;
          in_roi.width += 4;
          in_roi.height += 4;
          gegl_rectangle_intersect (&in_roi, &in_roi, result);

          /* Create a temporary buffer for the expanded ROI */
          gfloat *in_expanded = g_new0 (gfloat, in_roi.width * in_roi.height * 4);
          GeglBufferIterator *in_iter = gegl_buffer_iterator_new (temp, &in_roi, 0, format,
                                                                 GEGL_ACCESS_READ, GEGL_ABYSS_LOOP, 1);
          while (gegl_buffer_iterator_next (in_iter))
            {
              gfloat *data = in_iter->items[0].data;
              GeglRectangle r = in_iter->items[0].roi;
              gint x, y;
              for (y = 0; y < r.height; y++)
                for (x = 0; x < r.width; x++)
                  {
                    gint src_idx = (y * r.width + x) * 4;
                    gint dst_idx = ((y + r.y - in_roi.y) * in_roi.width + (x + r.x - in_roi.x)) * 4;
                    memcpy (in_expanded + dst_idx, data + src_idx, 4 * sizeof (gfloat));
                  }
            }

          for (gint y = 0; y < roi.height; y++)
            {
              for (gint x = 0; x < roi.width; x++)
                {
                  gint offset = (y * roi.width + x) * 4;
                  gint in_x = x + roi.x - in_roi.x;
                  gint in_y = y + roi.y - in_roi.y;
                  gint in_offset = (in_y * in_roi.width + in_x) * 4;
                  gfloat sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                  gfloat weights[4] = {0.0f};
                  gfloat gradients[4][4] = {{0.0f}};
                  gint j;

                  /* Compute gradients in four directions (N, S, E, W) */
                  if (in_x > 0) /* West */
                    {
                      gint idx = (in_y * in_roi.width + (in_x - 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[0][j] = in_expanded[idx + j] - in_expanded[in_offset + j];
                      weights[0] = conductance (fabsf (gradients[0][0]), o->kappa);
                    }

                  if (in_x < in_roi.width - 1) /* East */
                    {
                      gint idx = (in_y * in_roi.width + (in_x + 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[1][j] = in_expanded[idx + j] - in_expanded[in_offset + j];
                      weights[1] = conductance (fabsf (gradients[1][0]), o->kappa);
                    }

                  if (in_y > 0) /* North */
                    {
                      gint idx = ((in_y - 1) * in_roi.width + in_x) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[2][j] = in_expanded[idx + j] - in_expanded[in_offset + j];
                      weights[2] = conductance (fabsf (gradients[2][0]), o->kappa);
                    }

                  if (in_y < in_roi.height - 1) /* South */
                    {
                      gint idx = ((in_y + 1) * in_roi.width + in_x) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[3][j] = in_expanded[idx + j] - in_expanded[in_offset + j];
                      weights[3] = conductance (fabsf (gradients[3][0]), o->kappa);
                    }

                  /* Update pixel values */
                  gfloat weight_sum = weights[0] + weights[1] + weights[2] + weights[3];
                  if (weight_sum > 1e-6f)
                    {
                      gfloat w = o->alpha * o->strength / weight_sum;
                      for (j = 0; j < 4; j++)
                        {
                          sum[j] = w * (weights[0] * gradients[0][j] +
                                        weights[1] * gradients[1][j] +
                                        weights[2] * gradients[2][j] +
                                        weights[3] * gradients[3][j]);
                          sum[j] = CLAMP (sum[j], -2.0f, 2.0f); /* Wider clamp for stronger effect */
                        }
                    }

                  for (j = 0; j < 4; j++)
                    {
                      gfloat value = in_data[offset + j] + o->delta_t * sum[j];
                      out_data[offset + j] = CLAMP (value, 0.0f, 1.0f);
                    }
                }
            }

          g_free (in_expanded);
        }

      /* Copy output to temp for next iteration */
      gegl_buffer_copy (output, result, GEGL_ABYSS_CLAMP, temp, result);
    }

  /* Clean up */
  g_object_unref (temp);
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
    "name",        "gegl:smooth",
    "title",       _("Intense Anisotropic Smooth"),
    "categories",  "blur:edge-preserving",
    "description", _("Applies intense anisotropic diffusion to smooth the image while preserving edges"),
    "gimp:menu-path", "<Image>/Filters/Blur",
    "gimp:menu-label", _("Intense Anisotropic Smooth"),
    NULL);
}

#endif
