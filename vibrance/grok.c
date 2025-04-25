#include "config.h"
#include <gegl.h>
#include <gegl-plugin.h>
#include <math.h>

#define GEGL_OP_FILTER
#define GEGL_OP_NAME     grok
#define GEGL_OP_C_SOURCE grok.c

#define RGB_LUMINANCE(r,g,b) (0.2126 * (r) + 0.7152 * (g) + 0.0722 * (b))

GEGL_DEFINE_DYNAMIC_OPERATION(GEGL_TYPE_OPERATION_FILTER)

/* Operation properties */
enum_start (gegl_grok_vibrance)
  enum_value (GEGL_GROK_VIBRANCE, "vibrance", N_("Vibrance effect like G'MIC"))
enum_end (GeglGrokVibrance)

property_double (strength, _("Strength"), 1.0)
    description (_("Vibrance adjustment strength"))
    value_range (-2.0, 2.0)
    ui_range    (-2.0, 2.0)
    ui_meta     ("unit", "relative")

/* RGB to HSL conversion */
static void
rgb_to_hsl (gfloat r, gfloat g, gfloat b, gfloat *h, gfloat *s, gfloat *l)
{
  gfloat max = fmaxf (fmaxf (r, g), b);
  gfloat min = fminf (fminf (r, g), b);
  gfloat delta = max - min;

  *l = (max + min) / 2.0;

  if (delta == 0.0)
    {
      *s = 0.0;
      *h = 0.0; /* Undefined, but set to 0 */
    }
  else
    {
      *s = (*l > 0.5) ? delta / (2.0 - max - min) : delta / (max + min);
      if (max == r)
        *h = (g - b) / delta + (g < b ? 6.0 : 0.0);
      else if (max == g)
        *h = (b - r) / delta + 2.0;
      else
        *h = (r - g) / delta + 4.0;
      *h /= 6.0;
    }
}

/* HSL to RGB conversion */
static gfloat
hue_to_rgb (gfloat p, gfloat q, gfloat t)
{
  if (t < 0.0) t += 1.0;
  if (t > 1.0) t -= 1.0;
  if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
  if (t < 1.0 / 2.0) return q;
  if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
  return p;
}

static void
hsl_to_rgb (gfloat h, gfloat s, gfloat l, gfloat *r, gfloat *g, gfloat *b)
{
  if (s == 0.0)
    {
      *r = *g = *b = l; /* Achromatic */
    }
  else
    {
      gfloat q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
      gfloat p = 2.0 * l - q;
      *r = hue_to_rgb (p, q, h + 1.0 / 3.0);
      *g = hue_to_rgb (p, q, h);
      *b = hue_to_rgb (p, q, h - 1.0 / 3.0);
    }
}

/* Process function: Apply vibrance effect */
static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *input,
         GeglBuffer          *output,
         const GeglRectangle *roi,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  gfloat strength = o->strength;
  GeglBufferIterator *it;

  it = gegl_buffer_iterator_new (input, roi, level, babl_format ("R'G'B'A float"), GEGL_ACCESS_READ, GEGL_ABYSS_NONE);
  gegl_buffer_iterator_add (it, output, roi, level, babl_format ("R'G'B'A float"), GEGL_ACCESS_WRITE);

  while (gegl_buffer_iterator_next (it))
    {
      gfloat *in_data = (gfloat *) it->data[0];
      gfloat *out_data = (gfloat *) it->data[1];
      gint pixels = it->length;

      for (gint i = 0; i < pixels; i++)
        {
          gfloat r = in_data[i * 4 + 0];
          gfloat g = in_data[i * 4 + 1];
          gfloat b = in_data[i * 4 + 2];
          gfloat a = in_data[i * 4 + 3];
          gfloat h, s, l;

          /* Convert RGB to HSL */
          rgb_to_hsl (r, g, b, &h, &s, &l);

          /* Adjust saturation: boost low-saturation colors more */
          gfloat vibrance = strength * (1.0 - s); /* More boost for less saturated */
          s = CLAMP (s + vibrance * s, 0.0, 1.0);

          /* Convert back to RGB */
          hsl_to_rgb (h, s, l, &r, &g, &b);

          /* Write to output */
          out_data[i * 4 + 0] = r;
          out_data[i * 4 + 1] = g;
          out_data[i * 4 + 2] = b;
          out_data[i * 4 + 3] = a;
        }
    }

  gegl_buffer_iterator_destroy (it);
  return TRUE;
}

/* Specify input/output formats */
static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "input", babl_format ("R'G'B'A float"));
  gegl_operation_set_format (operation, "output", babl_format ("R'G'B'A float"));
}

/* GObject boilerplate */
static void
gegl_op_class_init (GeglOperationClass *klass)
{
  GeglOperationFilterClass *filter_class = GEGL_OPERATION_FILTER_CLASS (klass);

  filter_class->process = process;
  klass->prepare = prepare;

  gegl_operation_class_set_keys (klass,
    "name",        "gegl:grok",
    "title",       _("Vibrance Effect"),
    "categories",  "color",
    "description", _("Adjusts vibrance by enhancing less saturated colors, similar to G'MIC's vibrance effect"),
    NULL);
}

G_DEFINE_DYNAMIC_TYPE (GeglOp, gegl_op, GEGL_TYPE_OPERATION_FILTER)

static void
gegl_module_register (GTypeModule *module)
{
  gegl_op_register_type (module);
}

#endif
