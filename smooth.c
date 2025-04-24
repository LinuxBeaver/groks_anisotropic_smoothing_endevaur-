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

property_int (iterations, _("Iterations"), 5)
  description (_("Number of iterations; more iterations cause stronger smoothing"))
  value_range (1, 20)
  ui_range (1, 10)

property_double (alpha, _("Alpha"), 0.2)
  description (_("Diffusion strength in homogeneous regions"))
  value_range (0.0, 0.5)
  ui_range (0.0, 0.3)

property_double (kappa, _("Kappa"), 20.0)
  description (_("Edge sensitivity parameter; lower values preserve sharper edges"))
  value_range (5.0, 50.0)
  ui_range (5.0, 30.0)

property_double (strength, _("Strength"), 1.0)
  description (_("Overall intensity of the smoothing effect"))
  value_range (0.0, 2.0)
  ui_range (0.0, 1.5)

property_double (delta_t, _("Delta T"), 0.1)
  description (_("Time step for numerical stability"))
  value_range (0.01, 0.2)
  ui_range (0.01, 0.15)

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
  return expf (-g * g);
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
  gfloat *temp_buf;
  gint i;

  if (result->width < 2 || result->height < 2)
    {
      if (input != output)
        gegl_buffer_copy (input, result, GEGL_ABYSS_CLAMP, output, result);
      return TRUE;
    }

  /* Allocate temporary buffer for one iteration */
  temp_buf = g_new (gfloat, result->width * result->height * 4);
  if (!temp_buf)
    {
      g_warning ("Failed to allocate temporary buffer");
      return FALSE;
    }

  /* Initialize iterator for input and output */
  iter = gegl_buffer_iterator_new (input, result, 0, format,
                                  GEGL_ACCESS_READ, GEGL_ABYSS_CLAMP, 2);
  gegl_buffer_iterator_add (iter, output, result, 0, format,
                           GEGL_ACCESS_WRITE, GEGL_ABYSS_CLAMP);

  /* Copy input to output for first iteration */
  while (gegl_buffer_iterator_next (iter))
    {
      gfloat *in_data = iter->items[0].data;
      gfloat *out_data = iter->items[1].data;
      memcpy (out_data, in_data, iter->length * 4 * sizeof (gfloat));
    }

  /* Perform diffusion iterations */
  for (i = 0; i < o->iterations; i++)
    {
      iter = gegl_buffer_iterator_new (output, result, 0, format,
                                      GEGL_ACCESS_READ, GEGL_ABYSS_CLAMP, 2);
      gegl_buffer_iterator_add (iter, output, result, 0, format,
                               GEGL_ACCESS_WRITE, GEGL_ABYSS_CLAMP);

      while (gegl_buffer_iterator_next (iter))
        {
          gfloat *src = iter->items[0].data;
          gfloat *dst = iter->items[1].data;
          GeglRectangle roi = iter->items[0].roi;
          gint x, y;

          /* Copy to temporary buffer */
          memcpy (temp_buf, src, iter->length * 4 * sizeof (gfloat));

          for (y = 0; y < roi.height; y++)
            {
              for (x = 0; x < roi.width; x++)
                {
                  gint offset = (y * roi.width + x) * 4;
                  gfloat sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                  gfloat weights[8] = {0.0f};
                  gfloat gradients[8][4];
                  gint j;

                  /* Compute gradients in eight directions */
                  if (x > 0)
                    {
                      gint idx = (y * roi.width + (x - 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[0][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[0] = conductance (gradients[0][0], o->kappa);
                    }

                  if (x < roi.width - 1)
                    {
                      gint idx = (y * roi.width + (x + 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[1][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[1] = conductance (gradients[1][0], o->kappa);
                    }

                  if (y > 0)
                    {
                      gint idx = ((y - 1) * roi.width + x) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[2][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[2] = conductance (gradients[2][0], o->kappa);
                    }

                  if (y < roi.height - 1)
                    {
                      gint idx = ((y + 1) * roi.width + x) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[3][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[3] = conductance (gradients[3][0], o->kappa);
                    }

                  if (x > 0 && y > 0)
                    {
                      gint idx = ((y - 1) * roi.width + (x - 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[4][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[4] = conductance (gradients[4][0], o->kappa) * 0.707f;
                    }

                  if (x < roi.width - 1 && y > 0)
                    {
                      gint idx = ((y - 1) * roi.width + (x + 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[5][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[5] = conductance (gradients[5][0], o->kappa) * 0.707f;
                    }

                  if (x > 0 && y < roi.height - 1)
                    {
                      gint idx = ((y + 1) * roi.width + (x - 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[6][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[6] = conductance (gradients[6][0], o->kappa) * 0.707f;
                    }

                  if (x < roi.width - 1 && y < roi.height - 1)
                    {
                      gint idx = ((y + 1) * roi.width + (x + 1)) * 4;
                      for (j = 0; j < 4; j++)
                        gradients[7][j] = temp_buf[idx + j] - temp_buf[offset + j];
                      weights[7] = conductance (gradients[7][0], o->kappa) * 0.707f;
                    }

                  /* Update pixel values */
                  for (j = 0; j < 8; j++)
                    {
                      if (weights[j] > 0.0f)
                        {
                          gfloat w = weights[j] * o->alpha * o->strength;
                          sum[0] += w * gradients[j][0];
                          sum[1] += w * gradients[j][1];
                          sum[2] += w * gradients[j][2];
                          sum[3] += w * gradients[j][3];
                        }
                    }

                  for (j = 0; j < 4; j++)
                    {
                      gfloat value = temp_buf[offset + j] + o->delta_t * sum[j];
                      /* Clamp to prevent NaN or extreme values */
                      dst[offset + j] = CLAMP (value, 0.0f, 1.0f);
                    }
                }
            }
        }
    }

  g_free (temp_buf);
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
