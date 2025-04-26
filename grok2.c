#include "config.h"
#include <gegl.h>
#include <gegl-plugin.h>
#include <glib/gi18n.h>
#include <math.h>



/* Define properties */
#ifdef GEGL_PROPERTIES

property_double (zoom, _("Zoom"), 0.5)
    description (_("Adjust the zoom level of the stripes"))
    value_range (0.0, 1.0)
    ui_range    (0.0, 1.0)

property_double (horizontal_position, _("Horizontal Position"), 0.5)
    description (_("Shift the stripes horizontally"))
    value_range (0.0, 1.0)
    ui_range    (0.0, 1.0)

property_double (vertical_position, _("Vertical Position"), 0.5)
    description (_("Shift the stripes vertically"))
    value_range (0.0, 1.0)
    ui_range    (0.0, 1.0)

property_double (angle, _("Angle"), 0.0)
    description (_("Rotate the stripes (in degrees)"))
    value_range (0.0, 360.0)
    ui_range    (0.0, 360.0)

property_color (color1, _("Color 1"), "#FFFFFF")
    description (_("First color of the stripes"))

property_color (color2, _("Color 2"), "#800080")
    description (_("Second color of the stripes"))



#else

#define GEGL_OP_META
#define GEGL_OP_NAME     grok2
#define GEGL_OP_C_SOURCE grok2.c

#include "gegl-op.h"




/* Operation implementation */
static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "output", babl_format ("RGBA float"));
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  return gegl_rectangle_infinite_plane ();
}

static gboolean
process (GeglOperation       *operation,
         GeglOperationContext *context,
         const gchar         *output_prop,
         const GeglRectangle *result,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  GeglBuffer *output = gegl_operation_context_get_target (context, "output");

  gfloat *out_pixel = g_new (gfloat, result->width * result->height * 4);
  gfloat *out_ptr = out_pixel;

  gfloat zoom = o->zoom * 10.0; // Scale zoom for better control
  gfloat h_pos = (o->horizontal_position - 0.5) * 2.0; // Normalize to -1 to 1
  gfloat v_pos = (o->vertical_position - 0.5) * 2.0; // Normalize to -1 to 1
  gfloat angle_rad = o->angle * G_PI / 180.0; // Convert angle to radians

  // Parse colors
  GeglColor *color1 = o->color1;
  GeglColor *color2 = o->color2;
  gdouble c1[4], c2[4];
  gegl_color_get_rgba (color1, &c1[0], &c1[1], &c1[2], &c1[3]);
  gegl_color_get_rgba (color2, &c2[0], &c2[1], &c2[2], &c2[3]);

  for (gint y = result->y; y < result->y + result->height; y++)
    {
      for (gint x = result->x; x < result->x + result->width; x++)
        {
          // Normalize coordinates
          gfloat nx = (x - result->width * 0.5) / (gfloat)result->width + h_pos;
          gfloat ny = (y - result->height * 0.5) / (gfloat)result->height + v_pos;

          // Rotate coordinates
          gfloat rx = nx * cos (angle_rad) - ny * sin (angle_rad);
          gfloat ry = nx * sin (angle_rad) + ny * cos (angle_rad);

          // Create zebra stripe pattern using a sine wave
          gfloat value = sin (rx * zoom * 10.0) * sin (ry * zoom * 2.0);
          gfloat stripe = (value > 0.0) ? 1.0 : 0.0;

          // Interpolate between the two colors
          for (gint i = 0; i < 4; i++)
            out_ptr[i] = stripe * c1[i] + (1.0 - stripe) * c2[i];

          out_ptr += 4;
        }
    }

  gegl_buffer_set (output, result, 0, babl_format ("RGBA float"), out_pixel, GEGL_AUTO_ROWSTRIDE);
  g_free (out_pixel);
  return TRUE;
}

static void
gegl_op_class_init (GeglOperationClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->prepare = prepare;
  operation_class->get_bounding_box = get_bounding_box;
  operation_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:grok2",
    "title",       _("Zebra Stripes"),
    "categories",  "render:artistic",
    "description", _("Generates a zebra stripe pattern with adjustable zoom, position, angle, and colors"),
    NULL);
}
