/* 
 *
 * Copyright (C) 2006 Øyvind Kolås,
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
  description (_("Number of iterations for the diffusion process"))
  value_range (1, 20)
  ui_range (1, 15)

property_double (strength, _("Strength"), 10.0)
  description (_("Overall strength of the smoothing effect"))
  value_range (0.0, 20.0)
  ui_range (0.0, 15.0)

property_double (edge_threshold, _("Edge Threshold"), 0.9)
  description (_("Threshold for edge preservation; higher values preserve sharper edges"))
  value_range (0.0, 2.0)
  ui_range (0.0, 1.5)

property_double (anisotropy, _("Anisotropy"), 0.3)
  description (_("Preference for smoothing along edges vs. across them"))
  value_range (0.0, 1.0)
  ui_range (0.0, 0.8)

property_double (tensor_sigma, _("Tensor Smoothness"), 1.0)
  description (_("Spatial scale for structure tensor smoothing"))
  value_range (0.5, 2.0)
  ui_range (0.5, 1.5)

property_double (dt, _("Time Step"), 0.1)
  description (_("Time step for diffusion stability"))
  value_range (0.01, 0.25)
  ui_range (0.01, 0.2)

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

/* Gaussian kernel for tensor smoothing */
static void
gaussian_blur (gfloat *data, gint width, gint height, gfloat sigma)
{
  gint kernel_size = (gint)(3.0f * sigma + 0.5f) * 2 + 1;
  if (kernel_size < 3) kernel_size = 3;
  gint k = kernel_size / 2;
  gfloat *kernel = g_new (gfloat, kernel_size);
  gfloat sum = 0.0f;

  for (gint i = 0; i < kernel_size; i++)
    {
      gint x = i - k;
      kernel[i] = expf (-(x * x) / (2.0f * sigma * sigma));
      sum += kernel[i];
    }
  for (gint i = 0; i < kernel_size; i++)
    kernel[i] /= sum;

  gfloat *temp = g_new (gfloat, width * height);
  for (gint y = 0; y < height; y++)
    for (gint x = 0; x < width; x++)
      {
        gfloat value = 0.0f;
        for (gint i = 0; i < kernel_size; i++)
          {
            gint nx = x + (i - k);
            if (nx < 0) nx = 0;
            if (nx >= width) nx = width - 1;
            value += kernel[i] * data[y * width + nx];
          }
        temp[y * width + x] = value;
      }

  for (gint y = 0; y < height; y++)
    for (gint x = 0; x < width; x++)
      {
        gfloat value = 0.0f;
        for (gint i = 0; i < kernel_size; i++)
          {
            gint ny = y + (i - k);
            if (ny < 0) ny = 0;
            if (ny >= height) ny = height - 1;
            value += kernel[i] * temp[ny * width + x];
          }
        data[y * width + x] = value;
      }

  g_free (temp);
  g_free (kernel);
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
  GeglBuffer *temp;
  gint i;

  if (result->width < 3 || result->height < 3)
    {
      if (input != output)
        gegl_buffer_copy (input, result, GEGL_ABYSS_CLAMP, output, result);
      return TRUE;
    }

  temp = gegl_buffer_new (result, format);
  gegl_buffer_copy (input, result, GEGL_ABYSS_CLAMP, temp, result);

  for (i = 0; i < o->iterations; i++)
    {
      GeglBufferIterator *iter = gegl_buffer_iterator_new (temp, result, 0, format,
                                                         GEGL_ACCESS_READ, GEGL_ABYSS_CLAMP, 2);
      gegl_buffer_iterator_add (iter, output, result, 0, format,
                              GEGL_ACCESS_WRITE, GEGL_ABYSS_CLAMP);

      while (gegl_buffer_iterator_next (iter))
        {
          gfloat *in_data = iter->items[0].data;
          gfloat *out_data = iter->items[1].data;
          GeglRectangle roi = iter->items[0].roi;
          GeglRectangle in_roi = roi;
          in_roi.x -= 2;
          in_roi.y -= 2;
          in_roi.width += 4;
          in_roi.height += 4;
          gegl_rectangle_intersect (&in_roi, &in_roi, result);

          gfloat *in_expanded = g_new0 (gfloat, in_roi.width * in_roi.height * 4);
          GeglBufferIterator *in_iter = gegl_buffer_iterator_new (temp, &in_roi, 0, format,
                                                                GEGL_ACCESS_READ, GEGL_ABYSS_CLAMP, 1);
          while (gegl_buffer_iterator_next (in_iter))
            {
              gfloat *data = in_iter->items[0].data;
              GeglRectangle r = in_iter->items[0].roi;
              for (gint y = 0; y < r.height; y++)
                for (gint x = 0; x < r.width; x++)
                  {
                    gint src_idx = (y * r.width + x) * 4;
                    gint dst_idx = ((y + r.y - in_roi.y) * in_roi.width + (x + r.x - in_roi.x)) * 4;
                    memcpy (in_expanded + dst_idx, data + src_idx, 4 * sizeof (gfloat));
                  }
            }

          gfloat *Ix2 = g_new0 (gfloat, in_roi.width * in_roi.height);
          gfloat *Iy2 = g_new0 (gfloat, in_roi.width * in_roi.height);
          gfloat *IxIy = g_new0 (gfloat, in_roi.width * in_roi.height);

          for (gint y = 0; y < in_roi.height; y++)
            for (gint x = 0; x < in_roi.width; x++)
              {
                gint idx = (y * in_roi.width + x);
                gfloat Ix[4] = {0.0f}, Iy[4] = {0.0f};

                if (x > 0 && x < in_roi.width - 1 && y > 0 && y < in_roi.height - 1)
                  {
                    for (gint j = 0; j < 4; j++)
                      {
                        Ix[j] = (in_expanded[(y * in_roi.width + (x + 1)) * 4 + j] -
                                 in_expanded[(y * in_roi.width + (x - 1)) * 4 + j]) / 2.0f;
                        Iy[j] = (in_expanded[((y + 1) * in_roi.width + x) * 4 + j] -
                                 in_expanded[((y - 1) * in_roi.width + x) * 4 + j]) / 2.0f;
                      }
                  }

                gfloat ix2 = 0.0f, iy2 = 0.0f, ixy = 0.0f;
                for (gint j = 0; j < 4; j++)
                  {
                    ix2 += Ix[j] * Ix[j];
                    iy2 += Iy[j] * Iy[j];
                    ixy += Ix[j] * Iy[j];
                  }
                Ix2[idx] = ix2 / 4.0f;
                Iy2[idx] = iy2 / 4.0f;
                IxIy[idx] = ixy / 4.0f;
              }

          gaussian_blur (Ix2, in_roi.width, in_roi.height, o->tensor_sigma);
          gaussian_blur (Iy2, in_roi.width, in_roi.height, o->tensor_sigma);
          gaussian_blur (IxIy, in_roi.width, in_roi.height, o->tensor_sigma);

          for (gint y = 0; y < roi.height; y++)
            for (gint x = 0; x < roi.width; x++)
              {
                gint offset = (y * roi.width + x) * 4;
                gint in_x = x + roi.x - in_roi.x;
                gint in_y = y + roi.y - in_roi.y;
                gint idx = (in_y * in_roi.width + in_x);
                gfloat sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};

                gfloat a = Ix2[idx], b = IxIy[idx], c = Iy2[idx];
                gfloat trace = a + c;
                gfloat det = a * c - b * b;
                gfloat discriminant = sqrtf (MAX (0.0f, trace * trace / 4.0f - det));
                gfloat lambda1 = trace / 2.0f + discriminant;
                gfloat lambda2 = trace / 2.0f - discriminant;

                gfloat coherence = 0.0f;
                gfloat grad_mag = sqrtf (lambda1 + lambda2);
                if (grad_mag > 1e-5f)
                  coherence = ((lambda1 - lambda2) / (lambda1 + lambda2 + 1e-5f)) * expf (-1.0f / (grad_mag + 1e-5f));
                coherence = CLAMP (coherence, 0.0f, 1.0f);

                gfloat v1x = b, v1y = lambda1 - a;
                gfloat norm = sqrtf (v1x * v1x + v1y * v1y);
                if (norm < 1e-5f) { v1x = 1.0f; v1y = 0.0f; }
                else { v1x /= norm; v1y /= norm; }
                gfloat v2x = -v1y, v2y = v1x;

                gfloat c1 = o->strength / (1.0f + o->edge_threshold * powf (coherence, 2.0f));
                gfloat c2 = o->strength * (1.0f - o->anisotropy + o->anisotropy * expf (-powf (coherence, 2.0f)));
                c1 = CLAMP (c1, 0.1f, o->strength);
                c2 = CLAMP (c2, 0.1f, o->strength);

                gfloat Dxx = c1 * v1x * v1x + c2 * v2x * v2x;
                gfloat Dxy = c1 * v1x * v1y + c2 * v2x * v2y;
                gfloat Dyy = c1 * v1y * v1y + c2 * v2y * v2y;

                if (in_x > 0 && in_x < in_roi.width - 1 && in_y > 0 && in_y < in_roi.height - 1)
                  {
                    for (gint j = 0; j < 4; j++)
                      {
                        gfloat grad_x = (in_expanded[(in_y * in_roi.width + (in_x + 1)) * 4 + j] -
                                         in_expanded[(in_y * in_roi.width + (in_x - 1)) * 4 + j]) / 2.0f;
                        gfloat grad_y = (in_expanded[((in_y + 1) * in_roi.width + in_x) * 4 + j] -
                                         in_expanded[((in_y - 1) * in_roi.width + in_x) * 4 + j]) / 2.0f;

                        gfloat Dx_grad = Dxx * grad_x + Dxy * grad_y;
                        gfloat Dy_grad = Dxy * grad_x + Dyy * grad_y;

                        gfloat div_x = (in_expanded[(in_y * in_roi.width + (in_x + 1)) * 4 + j] -
                                        2.0f * in_expanded[(in_y * in_roi.width + in_x) * 4 + j] +
                                        in_expanded[(in_y * in_roi.width + (in_x - 1)) * 4 + j]) * Dxx +
                                       (in_expanded[((in_y + 1) * in_roi.width + in_x) * 4 + j] -
                                        in_expanded[((in_y - 1) * in_roi.width + in_x) * 4 + j]) * Dxy;
                        gfloat div_y = (in_expanded[((in_y + 1) * in_roi.width + in_x) * 4 + j] -
                                        2.0f * in_expanded[(in_y * in_roi.width + in_x) * 4 + j] +
                                        in_expanded[((in_y - 1) * in_roi.width + in_x) * 4 + j]) * Dyy +
                                       (in_expanded[(in_y * in_roi.width + (in_x + 1)) * 4 + j] -
                                        in_expanded[(in_y * in_roi.width + (in_x - 1)) * 4 + j]) * Dxy;

                        sum[j] = (div_x + div_y) / 2.0f;
                        sum[j] = CLAMP (sum[j], -0.2f, 0.2f);
                      }
                  }

                for (gint j = 0; j < 4; j++)
                  {
                    gfloat value = in_data[offset + j] + o->dt * sum[j];
                    value = 0.9f * value + 0.1f * in_data[offset + j];
                    out_data[offset + j] = CLAMP (value, 0.0f, 1.0f);
                  }
              }

          g_free (in_expanded);
          g_free (Ix2);
          g_free (Iy2);
          g_free (IxIy);
        }

      gegl_buffer_copy (output, result, GEGL_ABYSS_CLAMP, temp, result);
    }

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
    "name",        "gegl:grok",
    "title",       _("Anisotropic Smooth"),
    "categories",  "blur:edge-preserving",
    "description", _("Performs edge-preserving anisotropic smoothing inspired by G'MIC, minimizing outline artifacts"),
    "gimp:menu-path", "<Image>/Filters/Blur",
    "gimp:menu-label", _("Grok's GEGL plugin"),
    NULL);
}

#endif
