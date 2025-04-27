#include "config.h"
#include <glib/gi18n-lib.h>
#include <math.h>
#include <gegl.h>
#include <gegl-plugin.h>

#ifdef GEGL_PROPERTIES

property_color (color1, _("Spiral Color 1"), "red")
    description (_("First color of the spiral arms"))

property_color (color2, _("Spiral Color 2"), "blue")
    description (_("Second color of the spiral arms"))

property_color (color3, _("Spiral Color 3"), "green")
    description (_("Third color of the spiral arms"))

property_color (color4, _("Spiral Color 4"), "yellow")
    description (_("Fourth color of the spiral arms"))

property_color (color5, _("Spiral Color 5"), "purple")
    description (_("Fifth color of the spiral arms"))

property_color (bg_color, _("Background Color"), "black")
    description (_("Color of the background"))

property_int (arms, _("Number of Arms"), 4)
    description (_("Number of spiral arms"))
    value_range (1, 10)
    ui_range (1, 8)

property_double (twist, _("Twist"), 0.2)
    description (_("Tightness of the spiral"))
    value_range (0.1, 0.5)
    ui_range (0.1, 0.4)

property_double (thickness, _("Arm Thickness"), 10.0)
    description (_("Thickness of the spiral arms"))
    value_range (0.5, 15.0)
    ui_range (0.5, 15.0)

property_double (x, _("X"), 0.5)
    description (_("X position of the spiral center (relative to image width)"))
    value_range (0.0, 1.0)

property_double (y, _("Y"), 0.5)
    description (_("Y position of the spiral center (relative to image height)"))
    value_range (0.0, 1.0)

property_boolean (ccw, _("Counter-Clockwise"), FALSE)
    description (_("Draw spiral counter-clockwise"))

property_boolean (shade_edge, _("Shade Edge"), FALSE)
    description (_("Enable smooth shading for spiral edges"))

#else

#define GEGL_OP_POINT_FILTER
#define GEGL_OP_NAME     grok2
#define GEGL_OP_C_SOURCE grok2.c

#include "gegl-op.h"

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "input", babl_format ("RGBA float"));
  gegl_operation_set_format (operation, "output", babl_format ("RGBA float"));
}

static gboolean
process (GeglOperation       *operation,
         void               *in_buf,
         void               *out_buf,
         glong               n_pixels,
         const GeglRectangle *roi,
         gint                level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  gfloat *out_pixel = (gfloat *) out_buf;

  gfloat c1[4], c2[4], c3[4], c4[4], c5[4], bg[4];
  gegl_color_get_pixel (o->color1, babl_format ("RGBA float"), c1);
  gegl_color_get_pixel (o->color2, babl_format ("RGBA float"), c2);
  gegl_color_get_pixel (o->color3, babl_format ("RGBA float"), c3);
  gegl_color_get_pixel (o->color4, babl_format ("RGBA float"), c4);
  gegl_color_get_pixel (o->color5, babl_format ("RGBA float"), c5);
  gegl_color_get_pixel (o->bg_color, babl_format ("RGBA float"), bg);

  // Get full canvas dimensions
  GeglRectangle *canvas = gegl_operation_source_get_bounding_box (operation, "input");
  gfloat canvas_width = canvas ? canvas->width : roi->width;
  gfloat canvas_height = canvas ? canvas->height : roi->height;

  // Center relative to full canvas
  gfloat cx = o->x * canvas_width;
  gfloat cy = o->y * canvas_height;
  gfloat max_radius = sqrt (canvas_width * canvas_width + canvas_height * canvas_height) / 2.0;
  gfloat base_arm_width = G_PI / o->arms; // Base width per arm
  gfloat arm_width = base_arm_width * o->thickness; // Scale with thickness
  gfloat color_segment_width = base_arm_width / 5.0; // Width per color segment

  for (glong i = 0; i < n_pixels; i++)
  {
    // Compute global coordinates
    gint x = (i % roi->width) + roi->x;
    gint y = (i / roi->width) + roi->y;

    gfloat dx = x - cx;
    gfloat dy = y - cy;
    gfloat dist = sqrt (dx * dx + dy * dy);
    gfloat norm_dist = dist / max_radius;
    gfloat angle = atan2 (dy, dx);
    if (angle < 0)
      angle += 2.0 * G_PI;

    // Archimedean spiral: theta = k * r
    gfloat spiral_angle = norm_dist * o->twist * 2.0 * G_PI;
    if (o->ccw)
      spiral_angle = -spiral_angle;

    gfloat total_angle = angle + spiral_angle;
    if (total_angle < 0)
      total_angle += 2.0 * G_PI;

    // Normalize angle to current arm
    gfloat arm_angle = fmod (total_angle, 2.0 * G_PI / o->arms);
    if (arm_angle < 0)
      arm_angle += 2.0 * G_PI / o->arms;

    // Calculate color segment within arm
    gfloat color_angle = fmod (total_angle, 2.0 * G_PI / o->arms);
    if (color_angle < 0)
      color_angle += 2.0 * G_PI / o->arms;
    gint color_index = (gint) (color_angle / color_segment_width) % 5;
    gfloat *color;
    switch (color_index)
    {
      case 0: color = c1; break;
      case 1: color = c2; break;
      case 2: color = c3; break;
      case 3: color = c4; break;
      case 4: color = c5; break;
      default: color = bg; // Fallback
    }

    // Shading for smooth edges
    if (o->shade_edge)
    {
      gfloat t = arm_angle / (arm_width * 1.5); // Wider blending zone
      if (t <= 1.0)
      {
        // Cosine-based blending for smoother transition
        gfloat alpha = 0.5 * (1.0 - cos(t * G_PI));
        out_pixel[0] = color[0] * alpha + bg[0] * (1.0 - alpha);
        out_pixel[1] = color[1] * alpha + bg[1] * (1.0 - alpha);
        out_pixel[2] = color[2] * alpha + bg[2] * (1.0 - alpha);
        out_pixel[3] = 1.0;
      }
      else
      {
        out_pixel[0] = bg[0];
        out_pixel[1] = bg[1];
        out_pixel[2] = bg[2];
        out_pixel[3] = bg[3];
      }
    }
    else
    {
      // Hard-edged rendering
      if (arm_angle <= arm_width)
      {
        out_pixel[0] = color[0];
        out_pixel[1] = color[1];
        out_pixel[2] = color[2];
        out_pixel[3] = 1.0;
      }
      else
      {
        out_pixel[0] = bg[0];
        out_pixel[1] = bg[1];
        out_pixel[2] = bg[2];
        out_pixel[3] = bg[3];
      }
    }

    out_pixel += 4;
  }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationPointFilterClass *point_filter_class = GEGL_OPERATION_POINT_FILTER_CLASS (klass);

  operation_class->prepare = prepare;
  point_filter_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name", "gegl:grok2",
    "title", _("Candy Spiral Starburst"),
    "reference-hash", "candy_spiral",
    "description", _("Generates a multicolor Archimedean spiral with five customizable colors, creating a vibrant starburst effect"),
    NULL);
}

#endif
