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
 * Credit to Øvind Kolas (pippin) for major GEGL contributions
 *           2022 Liam Quin (barefootliam) pango-markup
 *           2025 Modified for GUI sliders, font input, text size, letter spacing, and rotation
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_string (text, _("Markup"), "Hello")
    description(_("Pango XML markup fragment to display (utf8, no outer markup element)"))
    ui_meta    ("multiline", "true")
    ui_meta    ("role", "editor") /* Hints at a multiline text editor or drop-down */

property_string (font, _("Font Name"), "Sans")
    description(_("Font family name for the text (e.g., Sans, Serif, Monospace)"))
    ui_meta    ("role", "entry") /* Hints at a single-line text box */

property_color  (color, _("Color"), "black")
    /* TRANSLATORS: the string 'black' should not be translated */
    description(_("Color for the text (defaults to 'black')"))

property_double (font_size, _("Font Size"), 12.0)
    description(_("Font size in points"))
    value_range (1.0, 1000.0)
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "1.0")
    ui_meta ("maximum", "1000.0")
    ui_meta ("step", "1.0")

property_double (letter_spacing, _("Letter Spacing"), 0.0)
    description(_("Spacing between characters in pixels (positive to space apart, negative to draw closer)"))
    value_range (-10.0, 50.0)
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "-10.0")
    ui_meta ("maximum", "50.0")
    ui_meta ("step", "0.1")

property_double (rotation, _("Rotation"), 0.0)
    description(_("Rotation angle of the text in degrees"))
    value_range (-180.0, 180.0)
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "-180.0")
    ui_meta ("maximum", "180.0")
    ui_meta ("step", "1.0")

property_int  (wrap, _("Wrap Width"), -1)
    description (_("Sets the width in pixels at which long lines will wrap. Use -1 for no wrapping."))
    value_range (-1, 1000)
    ui_meta ("unit", "pixel-distance")
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "-1")
    ui_meta ("maximum", "1000")
    ui_meta ("step", "1")

property_int  (vertical_wrap, _("Wrap Height"), -1)
    description (_("Sets the height in pixels according to which the text is vertically justified. Use -1 for no vertical justification."))
    value_range (-1, 1000)
    ui_meta ("unit", "pixel-distance")
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "-1")
    ui_meta ("maximum", "1000")
    ui_meta ("step", "1")

property_int    (alignment, _("Justification"), 0)
    description (_("Alignment for multi-line text (0=Left, 1=Center, 2=Right)"))
    value_range (0, 2)
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "0")
    ui_meta ("maximum", "2")
    ui_meta ("step", "1")

property_int    (vertical_alignment, _("Vertical Justification"), 0)
    description (_("Vertical text alignment (0=Top, 1=Middle, 2=Bottom)"))
    value_range (0, 2)
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "0")
    ui_meta ("maximum", "2")
    ui_meta ("step", "1")

property_double (line_spacing, _("Default Line Spacing"), 1.15)
    description (_("Line spacing multiplier"))
    value_range (0.5, 3.0)
    ui_meta ("role", "slider") /* GUI slider */
    ui_meta ("minimum", "0.5")
    ui_meta ("maximum", "3.0")
    ui_meta ("step", "0.05")

#else

#include <gegl-plugin.h>
#include <cairo.h>
#include <pango/pango-attributes.h>
#include <pango/pangocairo.h>
#include <math.h>

struct _GeglOp
{
  GeglOperationSource parent_instance;
  gpointer            properties;
  void *user_data;
};

typedef struct
{
  GeglOperationSourceClass parent_class;
} GeglOpClass;

#define GEGL_OP_NAME     pango_markup
#define GEGL_OP_C_SOURCE pango-markup.c

#include "gegl-op.h"
GEGL_DEFINE_DYNAMIC_OPERATION (GEGL_TYPE_OPERATION_SOURCE)

typedef struct {
  gchar         *text;
  gchar         *font;
  gdouble        font_size;
  gdouble        letter_spacing;
  gdouble        rotation; /* Added for rotation caching */
  gint           wrap;
  gint           vertical_wrap;
  gint           alignment;
  gint           vertical_alignment;
  gdouble       line_spacing;
  GeglRectangle  defined;

  GeglOperationSource parent_instance;
  gpointer            properties;
} PM_UserData;

static void
markup_layout_text (GeglOp        *self,
                    cairo_t       *cr,
                    gdouble        rowstride,
                    GeglRectangle *bounds,
                    int            component_set)
{
  GeglProperties       *o = GEGL_PROPERTIES (self);
  PangoLayout          *layout;
  PangoAttrList        *attrs;
  PangoFontDescription *desc;
  guint16               color[4];
  gchar                *text;
  gint                  alignment = 0;
  PangoRectangle        ink_rect;
  PangoRectangle        logical_rect;
  gint                  vertical_offset = 0;

  layout = pango_cairo_create_layout (cr);
  PM_UserData *userData = o->user_data;
  if (!userData) {
    o->user_data = userData = g_malloc0 (sizeof (PM_UserData));
    userData->text = 0;
    userData->font = 0;
    userData->font_size = 0.0;
    userData->letter_spacing = 0.0;
    userData->rotation = 0.0; /* Initialize rotation cache */
  }
  if (!o->text) {
    return;
  }

  /* Set up font */
  desc = pango_font_description_new();
  pango_font_description_set_family(desc, o->font ? o->font : "Sans");
  pango_font_description_set_size(desc, o->font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout, desc);

  text = g_strcompress (o->text);
  pango_layout_set_line_spacing (layout, o->line_spacing);
  pango_layout_set_markup_with_accel (layout, text, -1, 0, NULL);
  g_free (text);

  switch (o->alignment)
  {
  case 0:
    alignment = PANGO_ALIGN_LEFT;
    break;
  case 1:
    alignment = PANGO_ALIGN_CENTER;
    break;
  case 2:
    alignment = PANGO_ALIGN_RIGHT;
    break;
  }
  pango_layout_set_alignment (layout, alignment);
  pango_layout_set_width (layout, o->wrap * PANGO_SCALE);

  attrs = pango_attr_list_new ();

  /* Apply letter spacing */
  if (o->letter_spacing != 0.0) {
    PangoAttribute *spacing = pango_attr_letter_spacing_new(o->letter_spacing * PANGO_SCALE);
    pango_attr_list_insert(attrs, spacing);
  }

  switch (component_set)
  {
    case 0:
      gegl_color_get_pixel (o->color, babl_format ("R'G'B'A u16"), color);
      break;
    case 1:
      gegl_color_get_pixel (o->color, babl_format ("cykA u16"), color);
      break;
    case 2:
      gegl_color_get_pixel (o->color, babl_format ("cmkA u16"), color);
      break;
  }

  pango_attr_list_insert (
    attrs,
    pango_attr_foreground_new (color[0], color[1], color[2]));
  pango_attr_list_insert (
    attrs,
    pango_attr_foreground_alpha_new (color[3]));

  pango_layout_set_attributes (layout, attrs);

  pango_cairo_update_layout (cr, layout);

  pango_layout_get_pixel_extents (layout, &ink_rect, &logical_rect);

  /* Apply rotation */
  if (o->rotation != 0.0) {
    double radians = o->rotation * G_PI / 180.0;
    /* Translate to center of text for rotation */
    double center_x = logical_rect.x + logical_rect.width / 2.0;
    double center_y = logical_rect.y + logical_rect.height / 2.0;
    cairo_save(cr);
    cairo_translate(cr, center_x, center_y);
    cairo_rotate(cr, radians);
    cairo_translate(cr, -center_x, -center_y);
  }

  if (o->vertical_wrap >= 0)
    {
      switch (o->vertical_alignment)
      {
      case 0: /* top */
        vertical_offset = 0;
        break;
      case 1: /* middle */
        vertical_offset = (o->vertical_wrap - logical_rect.height) / 2;
        break;
      case 2: /* bottom */
        vertical_offset = o->vertical_wrap - logical_rect.height;
        break;
      }
    }

  if (bounds)
    {
      /* Calculate bounding box for rotated text */
      double angle = fabs(o->rotation * G_PI / 180.0);
      double cos_a = cos(angle);
      double sin_a = sin(angle);
      double w = logical_rect.width;
      double h = logical_rect.height;
      /* Compute new bounding box dimensions after rotation */
      double new_width = fabs(w * cos_a) + fabs(h * sin_a);
      double new_height = fabs(w * sin_a) + fabs(h * cos_a);
      double new_x = logical_rect.x + (w - new_width) / 2.0;
      double new_y = logical_rect.y + (h - new_height) / 2.0 + vertical_offset;
      *bounds = *GEGL_RECTANGLE (new_x, new_y, new_width, new_height);
    }
  else
    {
      if (color[3] > 0)
        {
          cairo_translate(cr, 0, vertical_offset);
          pango_cairo_show_layout (cr, layout);
        }
    }

  if (o->rotation != 0.0) {
    cairo_restore(cr); /* Restore Cairo context after rotation */
  }

  pango_font_description_free(desc);
  pango_attr_list_unref (attrs);
  g_object_unref (layout);
}

static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *output,
         const GeglRectangle *result,
         gint                 level)
{
  GeglOp *self = GEGL_OP (operation);
  const Babl *format = gegl_operation_get_format (operation, "output");
  const Babl *formats[4] = {NULL, NULL, NULL, NULL};
  int is_cmyk = babl_get_model_flags (format) & BABL_MODEL_FLAG_CMYK ? 1 : 0;

  cairo_t         *cr;
  cairo_surface_t *surface;
  if (is_cmyk)
  {
    formats[0]=babl_format ("cairo-ACYK32");
    formats[1]=babl_format ("cairo-ACMK32");
  }
  else
  {
    formats[0]=babl_format ("cairo-ARGB32");
  }

  for (int i = 0; formats[i]; i++)
  {
    guchar *data;
    data  = g_new0 (guchar, result->width * result->height * 4);

    surface = cairo_image_surface_create_for_data (data,
                                                 CAIRO_FORMAT_ARGB32,
                                                 result->width,
                                                 result->height,
                                                 result->width * 4);
    cr = cairo_create (surface);
    cairo_translate (cr, -result->x, -result->y);
    markup_layout_text (self, cr, 0, NULL, i+is_cmyk);

    gegl_buffer_set (output, result, 0, formats[i], data,
                     GEGL_AUTO_ROWSTRIDE);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    g_free (data);
  }

  return TRUE;
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  GeglOp *self = GEGL_OP (operation);
  GeglProperties *o = GEGL_PROPERTIES (self);
  PM_UserData *userData = o->user_data;
  gint status = FALSE;

  if (!userData) {
    o->user_data = userData = g_malloc0 (sizeof (PM_UserData));
    userData->text = 0;
    userData->font = 0;
    userData->font_size = 0.0;
    userData->letter_spacing = 0.0;
    userData->rotation = 0.0;
  }

  if ((userData->text && strcmp (userData->text, o->text)) ||
      (userData->font && strcmp (userData->font, o->font)) ||
      userData->font_size != o->font_size ||
      userData->letter_spacing != o->letter_spacing ||
      userData->rotation != o->rotation || /* Check rotation changes */
      !userData->defined.width ||
      userData->wrap != o->wrap ||
      userData->vertical_wrap != o->vertical_wrap ||
      userData->alignment != o->alignment ||
      userData->vertical_alignment != o->vertical_alignment ||
      userData->line_spacing != o->line_spacing)
      {
      cairo_t *cr;
      cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                                             1, 1);
      cr = cairo_create (surface);
      markup_layout_text (self, cr, 0, &userData->defined, 0);
      cairo_destroy (cr);
      cairo_surface_destroy (surface);

      if (userData->text)
        {
          g_free (userData->text);
        }
      if (userData->font)
        {
          g_free (userData->font);
        }
      userData->text = g_strdup (o->text);
      userData->font = g_strdup (o->font);
      userData->font_size = o->font_size;
      userData->letter_spacing = o->letter_spacing;
      userData->rotation = o->rotation; /* Cache rotation */
      userData->wrap = o->wrap;
      userData->vertical_wrap = o->vertical_wrap;
      userData->alignment = o->alignment;
      userData->vertical_alignment = o->vertical_alignment;
      userData->line_spacing = o->line_spacing;
    }

  if (status)
    {
      g_warning ("get defined region for text '%s' failed", o->text);
    }

  return userData->defined;
}

static void
finalize (GObject *object)
{
  GeglOp *self = GEGL_OP (object);
  GeglProperties *o = GEGL_PROPERTIES (self);
  PM_UserData *userData = o->user_data;

  if (userData)
    {
      if (userData->text)
        {
          g_free (userData->text);
        }
      if (userData->font)
        {
          g_free (userData->font);
        }
      g_free (userData);
      o->user_data = NULL;
    }
  G_OBJECT_CLASS (gegl_op_parent_class)->finalize (object);
}

static void
prepare (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  const Babl *color_format = gegl_color_get_format (o->color);
  BablModelFlag model_flags = babl_get_model_flags (color_format);
  PM_UserData *userData = o->user_data;

  if (model_flags & BABL_MODEL_FLAG_CMYK)
  {
    gegl_operation_set_format (operation, "output",
                               babl_format ("camayakaA u8"));
  }
  else
  {
    gegl_operation_set_format (operation, "output",
                               babl_format ("RaGaBaA float"));
  }
  if (!userData) {
    o->user_data = userData = g_malloc0 (sizeof (PM_UserData));
    userData->text = 0;
    userData->font = 0;
    userData->font_size = 0.0;
    userData->letter_spacing = 0.0;
    userData->rotation = 0.0;
  }
}

static const gchar *composition =
    "<?xml version='1.0' encoding='UTF-8'?>"
    "<gegl>"
    "<node operation='gegl:crop' width='200' height='200'/>"
    "<node operation='boy:pango-markup'>"
    "  <params>"
    "    <param name='wrap'>200</param>"
    "    <param name='color'>green</param>"
    "    <param name='text'>loves or pursues or desires to <i>obtain</i> pain of itself, because it is pain, but occasionally circumstances occur in which toil and pain can procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious <b>physical exercise</b>, except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure that has no annoying consequences, or one who avoids a pain that produces no</param>"
    "  </params>"
    "</node>"
    "</gegl>";

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GObjectClass             *object_class;
  GeglOperationClass       *operation_class;
  GeglOperationSourceClass *operation_source_class;

  object_class    = G_OBJECT_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);
  operation_source_class = GEGL_OPERATION_SOURCE_CLASS (klass);

  object_class->finalize = finalize;
  operation_class->prepare = prepare;
  operation_class->get_bounding_box = get_bounding_box;
  operation_source_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "reference-composition", composition,
    "title",          _("Render Pango Markup"),
    "name",           "boy:pango-markup",
    "categories",     "render",
    "reference-hash", "deafbededeafbededeafbededeafbede",
    "description",  _("Display a string containing XML-style marked-up text using Pango and Cairo, with customizable font, size, spacing, letter spacing, rotation, and color."),
    NULL);
}

#endif
