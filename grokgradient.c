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
 * Pippin (for writing GEGL's node chain area in 2006)
 * Grok with Beaver's help on instructions
 */

#include "config.h"

// Fallback for GETTEXT_PACKAGE to avoid compilation error
#ifndef GETTEXT_PACKAGE
#define GETTEXT_PACKAGE "gegl"
#endif

#include <glib/gi18n-lib.h>
#include <math.h>
#include <gegl.h>
#include <gegl-plugin.h>

#ifdef GEGL_PROPERTIES

enum_start(grok2_gradient_type)
  enum_value(GROK2_GRADIENT_RAINBOW, "rainbow", N_("Rainbow Gradient"))
  enum_value(GROK2_GRADIENT_TROPICAL, "tropical", N_("Tropical Colors"))
  enum_value(GROK2_GRADIENT_BERRY_BLAST, "berry_blast", N_("Berry Blast"))
  enum_value(GROK2_GRADIENT_CITRUS_ZEST, "citrus_zest", N_("Citrus Zest"))
  enum_value(GROK2_GRADIENT_MANGO_TANGO, "mango_tango", N_("Mango Tango"))
  enum_value(GROK2_GRADIENT_MELON_MEDLEY, "melon_medley", N_("Melon Medley"))
  enum_value(GROK2_GRADIENT_PEACH_DREAM, "peach_dream", N_("Peach Dream"))
  enum_value(GROK2_GRADIENT_PINEAPPLE_PUNCH, "pineapple_punch", N_("Pineapple Punch"))
  enum_value(GROK2_GRADIENT_TROPICAL_BREEZE, "tropical_breeze", N_("Tropical Breeze"))
  enum_value(GROK2_GRADIENT_GOLDEN, "golden", N_("Golden"))
  enum_value(GROK2_GRADIENT_SUNRISE, "sunrise", N_("Sunrise"))
  enum_value(GROK2_GRADIENT_ABSTRACT_1, "abstract_1", N_("Abstract 1"))
  enum_value(GROK2_GRADIENT_ABSTRACT_2, "abstract_2", N_("Abstract 2"))
  enum_value(GROK2_GRADIENT_ABSTRACT_3, "abstract_3", N_("Abstract 3"))
  enum_value(GROK2_GRADIENT_BLUUE_SUNSET, "blue_sunset", N_("Blue Sunset"))
  enum_value(GROK2_GRADIENT_FIRE_GLOW, "fire_glow", N_("Fire Glow"))
  enum_value(GROK2_GRADIENT_OCEAN_WAVE, "ocean_wave", N_("Ocean Wave"))
  enum_value(GROK2_GRADIENT_FOREST_GLADE, "forest_glade", N_("Forest Glade"))
  enum_value(GROK2_GRADIENT_PASTEL_DREAM, "pastel_dream", N_("Pastel Dream"))
  enum_value(GROK2_GRADIENT_NEON_GLOW, "neon_glow", N_("Neon Glow"))
  enum_value(GROK2_GRADIENT_AUTUMN_LEAVES, "autumn_leaves", N_("Autumn Leaves"))
  enum_value(GROK2_GRADIENT_PURPLE_HAZE, "purple_haze", N_("Purple Haze"))
  enum_value(GROK2_GRADIENT_DESERT_SAND, "desert_sand", N_("Desert Sand"))
  enum_value(GROK2_GRADIENT_ICY_FROST, "icy_frost", N_("Icy Frost"))
  enum_value(GROK2_GRADIENT_CANDY_SWIRL, "candy_swirl", N_("Candy Swirl"))
  enum_value(GROK2_GRADIENT_VIOLET_DUSK, "violet_dusk", N_("Violet Dusk"))
  enum_value(GROK2_GRADIENT_GREEN_LIME, "green_lime", N_("Green Lime"))
  enum_value(GROK2_GRADIENT_RED_SUNSET, "red_sunset", N_("Red Sunset"))
  enum_value(GROK2_GRADIENT_BLUE_LAGOON, "blue_lagoon", N_("Blue Lagoon"))
  enum_value(GROK2_GRADIENT_PINK_SUNRISE, "pink_sunrise", N_("Pink Sunrise"))
  enum_value(GROK2_GRADIENT_COOL_BREEZE, "cool_breeze", N_("Cool Breeze"))
  enum_value(GROK2_GRADIENT_WARM_GLOW, "warm_glow", N_("Warm Glow"))
  enum_value(GROK2_GRADIENT_LAVENDER_MIST, "lavender_mist", N_("Lavender Mist"))
  enum_value(GROK2_GRADIENT_SKY_BLUE, "sky_blue", N_("Sky Blue"))
  enum_value(GROK2_GRADIENT_RAINBOW_CYCLE, "rainbow_cycle", N_("Rainbow Cycle"))
  enum_value(GROK2_GRADIENT_SUNSET_GLOW, "sunset_glow", N_("Sunset Glow"))
  enum_value(GROK2_GRADIENT_MINT_FRESH, "mint_fresh", N_("Mint Fresh"))
  enum_value(GROK2_GRADIENT_CORAL_REEF, "coral_reef", N_("Coral Reef"))
  enum_value(GROK2_GRADIENT_ELECTRIC_PULSE, "electric_pulse", N_("Electric Pulse"))
  enum_value(GROK2_GRADIENT_GOLD_SHIMMER, "gold_shimmer", N_("Gold Shimmer"))
  enum_value(GROK2_GRADIENT_GOLD_RADIANCE, "gold_radiance", N_("Gold Radiance"))
  enum_value(GROK2_GRADIENT_SILVER_GLEAM, "silver_gleam", N_("Silver Gleam"))
  enum_value(GROK2_GRADIENT_SILVER_LUSTER, "silver_luster", N_("Silver Luster"))
  enum_value(GROK2_GRADIENT_BRONZE_GLOW, "bronze_glow", N_("Bronze Glow"))
  enum_value(GROK2_GRADIENT_BRONZE_SHEEN, "bronze_sheen", N_("Bronze Sheen"))
  enum_value(GROK2_GRADIENT_TWILIGHT_PURPLE, "twilight_purple", N_("Twilight Purple"))
  enum_value(GROK2_GRADIENT_SUNLIT_MEADOW, "sunlit_meadow", N_("Sunlit Meadow"))
  enum_value(GROK2_GRADIENT_OCEAN_DEPTHS, "ocean_depths", N_("Ocean Depths"))
  enum_value(GROK2_GRADIENT_CHERRY_BLOSSOM, "cherry_blossom", N_("Cherry Blossom"))
  enum_value(GROK2_GRADIENT_EMERALD_DREAM, "emerald_dream", N_("Emerald Dream"))
  enum_value(GROK2_GRADIENT_SAPPHIRE_NIGHT, "sapphire_night", N_("Sapphire Night"))
  enum_value(GROK2_GRADIENT_RUBY_GLOW, "ruby_glow", N_("Ruby Glow"))
  enum_value(GROK2_GRADIENT_AMETHYST_HAZE, "amethyst_haze", N_("Amethyst Haze"))
  enum_value(GROK2_GRADIENT_TOPAZ_SUNSET, "topaz_sunset", N_("Topaz Sunset"))
  enum_value(GROK2_GRADIENT_AQUAMARINE_WAVE, "aquamarine_wave", N_("Aquamarine Wave"))
  enum_value(GROK2_GRADIENT_COTTON_CANDY, "cotton_candy", N_("Cotton Candy"))
  enum_value(GROK2_GRADIENT_SWEET_CANDIES, "sweet_candies", N_("Sweet Candies"))
  enum_value(GROK2_GRADIENT_STARRY_SKY, "starry_sky", N_("Starry Sky"))
  enum_value(GROK2_GRADIENT_MOONLIT_FOG, "moonlit_fog", N_("Moonlit Fog"))
  enum_value(GROK2_GRADIENT_SUNFLOWER_FIELD, "sunflower_field", N_("Sunflower Field"))
  enum_value(GROK2_GRADIENT_LILAC_DUSK, "lilac_dusk", N_("Lilac Dusk"))
  enum_value(GROK2_GRADIENT_TURQUOISE_TIDE, "turquoise_tide", N_("Turquoise Tide"))
  enum_value(GROK2_GRADIENT_CRIMSON_SKY, "crimson_sky", N_("Crimson Sky"))
  enum_value(GROK2_GRADIENT_PERIWINKLE_BREEZE, "periwinkle_breeze", N_("Periwinkle Breeze"))
  enum_value(GROK2_GRADIENT_GALACTIC_HORIZON, "galactic_horizon", N_("Galactic Horizon"))
  enum_value(GROK2_GRADIENT_PEPPERMINT_TWIST, "peppermint_twist", N_("Peppermint Twist"))
  enum_value(GROK2_GRADIENT_ROSE_QUARTZ, "rose_quartz", N_("Rose Quartz"))
  enum_value(GROK2_GRADIENT_MIDNIGHT_BLUE, "midnight_blue", N_("Midnight Blue"))
  enum_value(GROK2_GRADIENT_SAFFRON_SUNRISE, "saffron_sunrise", N_("Saffron Sunrise"))
  enum_value(GROK2_GRADIENT_JADE_MIST, "jade_mist", N_("Jade Mist"))
enum_end(Grok2GradientType)

enum_start(grok2_gradient_shape)
  enum_value(GROK2_SHAPE_LINEAR, "linear", N_("Linear"))
  enum_value(GROK2_SHAPE_BILINEAR, "bilinear", N_("Bilinear"))
  enum_value(GROK2_SHAPE_RADIAL, "radial", N_("Radial"))
  enum_value(GROK2_SHAPE_SPIRAL, "spiral", N_("Spiral"))
  enum_value(GROK2_SHAPE_SPIRAL_CCW, "spiral_ccw", N_("Spiral Counter-Clockwise"))
  enum_value(GROK2_SHAPE_SQUARE, "square", N_("Square"))
enum_end(Grok2GradientShape)

property_enum (gradient_type, _("Gradient Type"),
               Grok2GradientType, grok2_gradient_type,
               GROK2_GRADIENT_RAINBOW)
    description (_("Type of gradient to apply"))

property_enum (gradient_shape, _("Gradient Shape"),
               Grok2GradientShape, grok2_gradient_shape,
               GROK2_SHAPE_LINEAR)
    description (_("Shape of the gradient pattern"))

property_double (angle, _("Gradient Angle"), 0.0)
    description (_("Angle of the gradient in degrees (affects Linear and Bilinear shapes; rotates starting point for Spiral shapes)"))
    value_range (0.0, 360.0)
    ui_range (0.0, 360.0)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")
    ui_meta ("visible", "!gradient_shape{radial,square}")

property_double (frequency, _("Frequency"), 1.0)
    description (_("Number of gradient cycles across the image"))
    value_range (0.1, 10.0)
    ui_range (0.1, 5.0)
    ui_meta ("visible", "!gradient_shape{spiral,spiral_ccw}")

property_int (frequency_2, _("Spiral Frequency"), 1)
    description (_("Number of spiral gradient cycles across the image (whole numbers for seamless spirals)"))
    value_range (1, 10)
    ui_range (1, 10)
    ui_meta ("visible", "gradient_shape{spiral,spiral_ccw}")

property_double (saturation, _("Saturation"), 1.0)
    description (_("Color intensity (0.0 = grayscale, 1.0 = full color)"))
    value_range (0.0, 1.0)
    ui_range (0.5, 1.0)

property_double (brightness, _("Brightness"), 1.0)
    description (_("Color brightness (0.0 = dark, 1.0 = bright)"))
    value_range (0.0, 1.0)
    ui_range (0.5, 1.0)

property_double (offset_x, _("X Offset"), 0.0)
    description (_("Horizontal offset of the gradient center (as a percentage of image width, -100 to 100)"))
    value_range (-100.0, 100.0)
    ui_range (-100.0, 100.0)
    ui_meta ("unit", "percent")

property_double (offset_y, _("Y Offset"), 0.0)
    description (_("Vertical offset of the gradient center (as a percentage of image height, -100 to 100)"))
    value_range (-100.0, 100.0)
    ui_range (-100.0, 100.0)
    ui_meta ("unit", "percent")

property_double (blend, _("Blend"), 0.0)
    description (_("Blending with the input image"))
    value_range (0.0, 0.1)
    ui_range (0.0, 0.1)
    ui_meta ("unit", "percent")
    ui_meta     ("role", "output-extent")

property_boolean (alpha_lock, _("Lock Alpha Channel"), FALSE)
    description (_("Lock the alpha channel to preserve the input image’s transparency (when enabled, gradient does not affect transparent areas)"))

#else

#define GEGL_OP_POINT_FILTER
#define GEGL_OP_NAME     grokgradient
#define GEGL_OP_C_SOURCE grokgradient.c

#include "gegl-op.h"

// HSV to RGB conversion
static void
hsv_to_rgb (gfloat h, gfloat s, gfloat v, gfloat *r, gfloat *g, gfloat *b)
{
  h = fmodf(h, 360.0);
  if (h < 0.0)
    h += 360.0;
  s = CLAMP(s, 0.0, 1.0);
  v = CLAMP(v, 0.0, 1.0);

  gfloat c = v * s;
  gfloat x = c * (1.0 - fabsf(fmodf(h / 60.0, 2.0) - 1.0));
  gfloat m = v - c;

  gfloat r1, g1, b1;
  if (h < 60.0)
    { r1 = c; g1 = x; b1 = 0.0; }
  else if (h < 120.0)
    { r1 = x; g1 = c; b1 = 0.0; }
  else if (h < 180.0)
    { r1 = 0.0; g1 = c; b1 = x; }
  else if (h < 240.0)
    { r1 = 0.0; g1 = x; b1 = c; }
  else if (h < 300.0)
    { r1 = x; g1 = 0.0; b1 = c; }
  else
    { r1 = c; g1 = 0.0; b1 = x; }

  *r = r1 + m;
  *g = g1 + m;
  *b = b1 + m;
}

// RGB to HSV conversion
static void
rgb_to_hsv (gfloat r, gfloat g, gfloat b, gfloat *h, gfloat *s, gfloat *v)
{
  r = CLAMP(r, 0.0, 1.0);
  g = CLAMP(g, 0.0, 1.0);
  b = CLAMP(b, 0.0, 1.0);

  gfloat max = fmaxf(fmaxf(r, g), b);
  gfloat min = fminf(fminf(r, g), b);
  gfloat delta = max - min;

  *v = max;

  if (max == 0.0 || delta == 0.0) {
    *s = 0.0;
    *h = 0.0;
  } else {
    *s = delta / max;
    if (r == max)
      *h = 60.0 * (g - b) / delta;
    else if (g == max)
      *h = 60.0 * (2.0 + (b - r) / delta);
    else
      *h = 60.0 * (4.0 + (r - g) / delta);
    if (*h < 0.0)
      *h += 360.0;
  }
}

// Generic gradient interpolation function
static void
interpolate_gradient(gfloat t, const gfloat colors[][3], const gfloat stops[], gint n_segments, gfloat *r, gfloat *g, gfloat *b)
{
  for (gint i = 0; i < n_segments; i++) {
    gfloat start = stops[i];
    gfloat end = stops[i + 1];
    if (t <= end) {
      gfloat t_scaled = (t - start) / (end - start);
      *r = colors[i][0] + t_scaled * (colors[i + 1][0] - colors[i][0]);
      *g = colors[i][1] + t_scaled * (colors[i + 1][1] - colors[i][1]);
      *b = colors[i][2] + t_scaled * (colors[i + 1][2] - colors[i][2]);
      return;
    }
  }
  // Fallback to last segment
  *r = colors[n_segments][0];
  *g = colors[n_segments][1];
  *b = colors[n_segments][2];
}

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format(operation, "input", babl_format("RGBA float"));
  gegl_operation_set_format(operation, "output", babl_format("RGBA float"));
}

static gboolean
process (GeglOperation *operation, void *in_buf, void *out_buf, glong n_pixels, const GeglRectangle *roi, gint level)
{
  GeglProperties *o = GEGL_PROPERTIES(operation);
  gfloat *in_pixel = (gfloat *)in_buf;
  gfloat *out_pixel = (gfloat *)out_buf;

  // Get canvas dimensions
  GeglRectangle *canvas = gegl_operation_source_get_bounding_box(operation, "input");
  gfloat width = canvas ? canvas->width : roi->width;
  gfloat height = canvas ? canvas->height : roi->height;

  // Precompute angle in radians
  gfloat angle_rad = o->angle * G_PI / 180.0;
  gfloat cos_a = cosf(angle_rad);
  gfloat sin_a = sinf(angle_rad);

  // Select frequency based on shape
  gfloat freq = (o->gradient_shape == GROK2_SHAPE_SPIRAL || o->gradient_shape == GROK2_SHAPE_SPIRAL_CCW) ? (gfloat)o->frequency_2 : o->frequency;

  // Convert percentage offsets to normalized offsets
  gfloat offset_x = o->offset_x / 100.0;
  gfloat offset_y = o->offset_y / 100.0;

  // Define gradient color stops and positions
  static const gfloat tropical_colors[][3] = {{0.0, 0.75, 0.75}, {0.5, 1.0, 0.0}, {1.0, 0.5, 0.5}, {0.0, 0.75, 0.75}};
  static const gfloat tropical_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat berry_blast_colors[][3] = {{0.5, 0.0, 0.5}, {1.0, 0.5, 0.75}, {0.0, 0.5, 1.0}, {0.5, 0.0, 0.5}};
  static const gfloat berry_blast_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat citrus_zest_colors[][3] = {{1.0, 1.0, 0.0}, {1.0, 0.5, 0.0}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}};
  static const gfloat citrus_zest_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat mango_tango_colors[][3] = {{1.0, 0.5, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.5, 0.0}};
  static const gfloat mango_tango_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat melon_medley_colors[][3] = {{0.0, 1.0, 0.0}, {1.0, 0.5, 0.75}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}};
  static const gfloat melon_medley_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat peach_dream_colors[][3] = {{1.0, 0.75, 0.5}, {1.0, 0.5, 0.75}, {1.0, 0.5, 0.0}, {1.0, 0.75, 0.5}};
  static const gfloat peach_dream_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat pineapple_punch_colors[][3] = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.5, 0.0}, {1.0, 1.0, 0.0}};
  static const gfloat pineapple_punch_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat tropical_breeze_colors[][3] = {{0.0, 1.0, 1.0}, {0.5, 0.0, 0.5}, {1.0, 1.0, 0.0}, {0.0, 1.0, 1.0}};
  static const gfloat tropical_breeze_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat golden_colors[][3] = {{1.0, 0.84, 0.0}, {1.0, 0.5, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.84, 0.0}};
  static const gfloat golden_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sunrise_colors[][3] = {{1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
  static const gfloat sunrise_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat abstract_1_colors[][3] = {{0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 1.0}};
  static const gfloat abstract_1_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat abstract_2_colors[][3] = {{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
  static const gfloat abstract_2_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat abstract_3_colors[][3] = {{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
  static const gfloat abstract_3_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat blue_sunset_colors[][3] = {{0.0, 0.0, 1.0}, {1.0, 0.5, 0.0}, {0.5, 0.0, 0.5}, {0.0, 0.0, 1.0}};
  static const gfloat blue_sunset_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat fire_glow_colors[][3] = {{1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.5, 0.0}, {1.0, 0.0, 0.0}};
  static const gfloat fire_glow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat ocean_wave_colors[][3] = {{0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 1.0}};
  static const gfloat ocean_wave_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat forest_glade_colors[][3] = {{0.0, 1.0, 0.0}, {0.5, 0.25, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}};
  static const gfloat forest_glade_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat pastel_dream_colors[][3] = {{1.0, 0.75, 0.75}, {0.75, 0.75, 1.0}, {1.0, 1.0, 0.75}, {1.0, 0.75, 0.75}};
  static const gfloat pastel_dream_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat neon_glow_colors[][3] = {{0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}};
  static const gfloat neon_glow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat autumn_leaves_colors[][3] = {{1.0, 0.0, 0.0}, {1.0, 0.5, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 0.0}};
  static const gfloat autumn_leaves_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat purple_haze_colors[][3] = {{0.5, 0.0, 0.5}, {0.0, 0.0, 1.0}, {1.0, 0.5, 0.75}, {0.5, 0.0, 0.5}};
  static const gfloat purple_haze_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat desert_sand_colors[][3] = {{1.0, 1.0, 0.0}, {1.0, 0.5, 0.0}, {1.0, 0.75, 0.5}, {1.0, 1.0, 0.0}};
  static const gfloat desert_sand_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat icy_frost_colors[][3] = {{0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 0.0, 1.0}};
  static const gfloat icy_frost_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat candy_swirl_colors[][3] = {{1.0, 0.5, 0.75}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {1.0, 0.5, 0.75}};
  static const gfloat candy_swirl_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat violet_dusk_colors[][3] = {{0.5, 0.0, 0.5}, {0.0, 0.0, 1.0}, {1.0, 0.5, 0.0}, {0.5, 0.0, 0.5}};
  static const gfloat violet_dusk_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat green_lime_colors[][3] = {{0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {0.5, 1.0, 0.0}, {0.0, 1.0, 0.0}};
  static const gfloat green_lime_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat red_sunset_colors[][3] = {{1.0, 0.0, 0.0}, {1.0, 0.5, 0.0}, {0.5, 0.0, 0.5}, {1.0, 0.0, 0.0}};
  static const gfloat red_sunset_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat blue_lagoon_colors[][3] = {{0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
  static const gfloat blue_lagoon_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat pink_sunrise_colors[][3] = {{1.0, 0.5, 0.75}, {1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.5, 0.75}};
  static const gfloat pink_sunrise_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat cool_breeze_colors[][3] = {{0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.5, 0.0, 0.5}, {0.0, 1.0, 1.0}};
  static const gfloat cool_breeze_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat warm_glow_colors[][3] = {{1.0, 0.5, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.5, 0.0}};
  static const gfloat warm_glow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat lavender_mist_colors[][3] = {{0.75, 0.5, 1.0}, {1.0, 0.75, 0.75}, {0.5, 0.5, 1.0}, {0.75, 0.5, 1.0}};
  static const gfloat lavender_mist_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sky_blue_colors[][3] = {{0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 0.0, 1.0}};
  static const gfloat sky_blue_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat rainbow_cycle_colors[][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
  static const gfloat rainbow_cycle_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sunset_glow_colors[][3] = {{1.0, 0.5, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.5, 0.0}};
  static const gfloat sunset_glow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat mint_fresh_colors[][3] = {{0.0, 1.0, 0.0}, {0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}};
  static const gfloat mint_fresh_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat coral_reef_colors[][3] = {{1.0, 0.5, 0.5}, {0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {1.0, 0.5, 0.5}};
  static const gfloat coral_reef_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat electric_pulse_colors[][3] = {{0.0, 0.0, 1.0}, {0.5, 0.0, 0.5}, {0.0, 1.0, 1.0}, {0.0, 0.0, 1.0}};
  static const gfloat electric_pulse_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat gold_shimmer_colors[][3] = {{1.0, 0.84, 0.0}, {1.0, 0.75, 0.5}, {1.0, 1.0, 0.0}, {1.0, 0.84, 0.0}};
  static const gfloat gold_shimmer_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat gold_radiance_colors[][3] = {{1.0, 0.9, 0.2}, {1.0, 0.6, 0.0}, {1.0, 0.8, 0.4}, {1.0, 0.9, 0.2}};
  static const gfloat gold_radiance_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat silver_gleam_colors[][3] = {{0.8, 0.8, 0.9}, {1.0, 1.0, 1.0}, {0.6, 0.6, 0.7}, {0.8, 0.8, 0.9}};
  static const gfloat silver_gleam_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat silver_luster_colors[][3] = {{0.9, 0.9, 1.0}, {0.7, 0.7, 0.8}, {1.0, 1.0, 1.0}, {0.9, 0.9, 1.0}};
  static const gfloat silver_luster_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat bronze_glow_colors[][3] = {{0.8, 0.5, 0.2}, {1.0, 0.7, 0.4}, {0.6, 0.4, 0.2}, {0.8, 0.5, 0.2}};
  static const gfloat bronze_glow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat bronze_sheen_colors[][3] = {{0.9, 0.6, 0.3}, {0.7, 0.4, 0.2}, {1.0, 0.8, 0.5}, {0.9, 0.6, 0.3}};
  static const gfloat bronze_sheen_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat twilight_purple_colors[][3] = {{0.4, 0.2, 0.6}, {0.6, 0.4, 0.8}, {0.2, 0.0, 0.4}, {0.4, 0.2, 0.6}};
  static const gfloat twilight_purple_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sunlit_meadow_colors[][3] = {{0.4, 0.8, 0.2}, {1.0, 1.0, 0.0}, {0.6, 0.9, 0.4}, {0.4, 0.8, 0.2}};
  static const gfloat sunlit_meadow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat ocean_depths_colors[][3] = {{0.0, 0.2, 0.6}, {0.0, 0.4, 0.8}, {0.0, 0.0, 0.4}, {0.0, 0.2, 0.6}};
  static const gfloat ocean_depths_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat cherry_blossom_colors[][3] = {{1.0, 0.7, 0.8}, {1.0, 0.9, 0.9}, {0.8, 0.5, 0.6}, {1.0, 0.7, 0.8}};
  static const gfloat cherry_blossom_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat emerald_dream_colors[][3] = {{0.0, 0.6, 0.4}, {0.2, 0.8, 0.6}, {0.0, 0.4, 0.2}, {0.0, 0.6, 0.4}};
  static const gfloat emerald_dream_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sapphire_night_colors[][3] = {{0.0, 0.2, 0.8}, {0.2, 0.4, 1.0}, {0.0, 0.0, 0.6}, {0.0, 0.2, 0.8}};
  static const gfloat sapphire_night_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat ruby_glow_colors[][3] = {{0.8, 0.2, 0.2}, {1.0, 0.4, 0.4}, {0.6, 0.0, 0.0}, {0.8, 0.2, 0.2}};
  static const gfloat ruby_glow_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat amethyst_haze_colors[][3] = {{0.6, 0.4, 0.8}, {0.8, 0.6, 1.0}, {0.4, 0.2, 0.6}, {0.6, 0.4, 0.8}};
  static const gfloat amethyst_haze_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat topaz_sunset_colors[][3] = {{1.0, 0.6, 0.2}, {1.0, 0.8, 0.4}, {0.8, 0.4, 0.0}, {1.0, 0.6, 0.2}};
  static const gfloat topaz_sunset_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat aquamarine_wave_colors[][3] = {{0.2, 0.8, 0.8}, {0.4, 1.0, 1.0}, {0.0, 0.6, 0.6}, {0.2, 0.8, 0.8}};
  static const gfloat aquamarine_wave_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat cotton_candy_colors[][3] = {{1.0, 0.8, 0.9}, {0.8, 0.9, 1.0}, {1.0, 0.6, 0.8}, {1.0, 0.8, 0.9}};
  static const gfloat cotton_candy_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sweet_candies_colors[][3] = {
    {1.0, 0.4, 0.6},  // Bright Pink (candyfloss)
    {0.4, 1.0, 0.6},  // Mint Green (peppermint)
    {1.0, 0.8, 0.2},  // Lemon Yellow (lemon drop)
    {0.4, 0.6, 1.0}   // Bubblegum Blue
  };
  static const gfloat sweet_candies_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat starry_sky_colors[][3] = {{0.0, 0.0, 0.4}, {0.2, 0.2, 0.8}, {0.0, 0.0, 0.6}, {0.0, 0.0, 0.4}};
  static const gfloat starry_sky_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat moonlit_fog_colors[][3] = {{0.8, 0.8, 1.0}, {0.6, 0.6, 0.8}, {0.9, 0.9, 1.0}, {0.8, 0.8, 1.0}};
  static const gfloat moonlit_fog_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat sunflower_field_colors[][3] = {{1.0, 0.8, 0.0}, {0.4, 0.8, 0.2}, {1.0, 1.0, 0.0}, {1.0, 0.8, 0.0}};
  static const gfloat sunflower_field_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat lilac_dusk_colors[][3] = {{0.8, 0.6, 1.0}, {0.6, 0.4, 0.8}, {1.0, 0.8, 1.0}, {0.8, 0.6, 1.0}};
  static const gfloat lilac_dusk_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat turquoise_tide_colors[][3] = {{0.0, 0.8, 0.8}, {0.2, 1.0, 1.0}, {0.0, 0.6, 0.6}, {0.0, 0.8, 0.8}};
  static const gfloat turquoise_tide_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat crimson_sky_colors[][3] = {{0.8, 0.2, 0.2}, {1.0, 0.4, 0.0}, {0.6, 0.0, 0.0}, {0.8, 0.2, 0.2}};
  static const gfloat crimson_sky_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat periwinkle_breeze_colors[][3] = {{0.6, 0.6, 1.0}, {0.8, 0.8, 1.0}, {0.4, 0.4, 0.8}, {0.6, 0.6, 1.0}};
  static const gfloat periwinkle_breeze_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat galactic_horizon_colors[][3] = {{0.2, 0.0, 0.4}, {0.4, 0.2, 0.8}, {0.0, 0.0, 0.6}, {0.2, 0.0, 0.4}};
  static const gfloat galactic_horizon_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat peppermint_twist_colors[][3] = {{1.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}};
  static const gfloat peppermint_twist_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat rose_quartz_colors[][3] = {{1.0, 0.7, 0.7}, {0.8, 0.6, 0.6}, {1.0, 0.9, 0.9}, {1.0, 0.7, 0.7}};
  static const gfloat rose_quartz_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat midnight_blue_colors[][3] = {{0.0, 0.0, 0.6}, {0.0, 0.0, 0.8}, {0.0, 0.0, 0.4}, {0.0, 0.0, 0.6}};
  static const gfloat midnight_blue_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat saffron_sunrise_colors[][3] = {{1.0, 0.6, 0.0}, {1.0, 0.8, 0.2}, {1.0, 0.4, 0.0}, {1.0, 0.6, 0.0}};
  static const gfloat saffron_sunrise_stops[] = {0.0, 0.333, 0.667, 1.0};
  static const gfloat jade_mist_colors[][3] = {{0.2, 0.8, 0.6}, {0.4, 1.0, 0.8}, {0.0, 0.6, 0.4}, {0.2, 0.8, 0.6}};
  static const gfloat jade_mist_stops[] = {0.0, 0.333, 0.667, 1.0};

  for (glong i = 0; i < n_pixels; i++)
    {
      // Compute normalized coordinates, centered at (0.5, 0.5) with offset
      gfloat x = ((i % roi->width) + roi->x) / width - 0.5 - offset_x;
      gfloat y = ((i / roi->width) + roi->y) / height - 0.5 - offset_y;

      // Compute t based on gradient shape
      gfloat t;
      switch (o->gradient_shape) {
        case GROK2_SHAPE_LINEAR:
          t = (x * cos_a + y * sin_a + 0.5) * freq;
          break;
        case GROK2_SHAPE_BILINEAR:
          t = fabsf(x * cos_a + y * sin_a) * freq;
          break;
        case GROK2_SHAPE_RADIAL:
          t = sqrtf(x * x + y * y) * freq;
          break;
        case GROK2_SHAPE_SPIRAL:
          {
            gfloat r = sqrtf(x * x + y * y);
            gfloat theta = atan2f(y, x) + angle_rad; // Include rotation
            // Create seamless spiral using periodic function
            t = 0.5 * (1.0 + sinf(2.0 * G_PI * freq * (theta / (2.0 * G_PI) + r)));
          }
          break;
        case GROK2_SHAPE_SPIRAL_CCW:
          {
            gfloat r = sqrtf(x * x + y * y);
            gfloat theta = -atan2f(y, x) + angle_rad; // Reverse direction and include rotation
            // Create seamless spiral using periodic function
            t = 0.5 * (1.0 + sinf(2.0 * G_PI * freq * (theta / (2.0 * G_PI) + r)));
          }
          break;
        case GROK2_SHAPE_SQUARE:
          t = fmaxf(fabsf(x), fabsf(y)) * freq;
          break;
        default:
          t = 0.0;
          break;
      }

      // Ensure t is in [0,1] for seamlessness
      t = fmodf(t, 1.0);
      if (t < 0.0)
        t += 1.0;

      // Initialize gradient RGB
      gfloat grad_r, grad_g, grad_b;

      // Select gradient type
      if (o->gradient_type == GROK2_GRADIENT_RAINBOW) {
        // Rainbow Gradient: Use HSV
        gfloat hue = t * 360.0;
        hsv_to_rgb(hue, o->saturation, o->brightness, &grad_r, &grad_g, &grad_b);
      } else {
        // RGB-based gradients
        switch (o->gradient_type) {
          case GROK2_GRADIENT_TROPICAL:
            interpolate_gradient(t, tropical_colors, tropical_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_BERRY_BLAST:
            interpolate_gradient(t, berry_blast_colors, berry_blast_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_CITRUS_ZEST:
            interpolate_gradient(t, citrus_zest_colors, citrus_zest_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_MANGO_TANGO:
            interpolate_gradient(t, mango_tango_colors, mango_tango_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_MELON_MEDLEY:
            interpolate_gradient(t, melon_medley_colors, melon_medley_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PEACH_DREAM:
            interpolate_gradient(t, peach_dream_colors, peach_dream_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PINEAPPLE_PUNCH:
            interpolate_gradient(t, pineapple_punch_colors, pineapple_punch_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_TROPICAL_BREEZE:
            interpolate_gradient(t, tropical_breeze_colors, tropical_breeze_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_GOLDEN:
            interpolate_gradient(t, golden_colors, golden_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SUNRISE:
            interpolate_gradient(t, sunrise_colors, sunrise_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_ABSTRACT_1:
            interpolate_gradient(t, abstract_1_colors, abstract_1_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_ABSTRACT_2:
            interpolate_gradient(t, abstract_2_colors, abstract_2_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_ABSTRACT_3:
            interpolate_gradient(t, abstract_3_colors, abstract_3_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_BLUUE_SUNSET:
            interpolate_gradient(t, blue_sunset_colors, blue_sunset_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_FIRE_GLOW:
            interpolate_gradient(t, fire_glow_colors, fire_glow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_OCEAN_WAVE:
            interpolate_gradient(t, ocean_wave_colors, ocean_wave_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_FOREST_GLADE:
            interpolate_gradient(t, forest_glade_colors, forest_glade_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PASTEL_DREAM:
            interpolate_gradient(t, pastel_dream_colors, pastel_dream_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_NEON_GLOW:
            interpolate_gradient(t, neon_glow_colors, neon_glow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_AUTUMN_LEAVES:
            interpolate_gradient(t, autumn_leaves_colors, autumn_leaves_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PURPLE_HAZE:
            interpolate_gradient(t, purple_haze_colors, purple_haze_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_DESERT_SAND:
            interpolate_gradient(t, desert_sand_colors, desert_sand_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_ICY_FROST:
            interpolate_gradient(t, icy_frost_colors, icy_frost_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_CANDY_SWIRL:
            interpolate_gradient(t, candy_swirl_colors, candy_swirl_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_VIOLET_DUSK:
            interpolate_gradient(t, violet_dusk_colors, violet_dusk_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_GREEN_LIME:
            interpolate_gradient(t, green_lime_colors, green_lime_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_RED_SUNSET:
            interpolate_gradient(t, red_sunset_colors, red_sunset_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_BLUE_LAGOON:
            interpolate_gradient(t, blue_lagoon_colors, blue_lagoon_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PINK_SUNRISE:
            interpolate_gradient(t, pink_sunrise_colors, pink_sunrise_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_COOL_BREEZE:
            interpolate_gradient(t, cool_breeze_colors, cool_breeze_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_WARM_GLOW:
            interpolate_gradient(t, warm_glow_colors, warm_glow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_LAVENDER_MIST:
            interpolate_gradient(t, lavender_mist_colors, lavender_mist_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SKY_BLUE:
            interpolate_gradient(t, sky_blue_colors, sky_blue_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_RAINBOW_CYCLE:
            interpolate_gradient(t, rainbow_cycle_colors, rainbow_cycle_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SUNSET_GLOW:
            interpolate_gradient(t, sunset_glow_colors, sunset_glow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_MINT_FRESH:
            interpolate_gradient(t, mint_fresh_colors, mint_fresh_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_CORAL_REEF:
            interpolate_gradient(t, coral_reef_colors, coral_reef_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_ELECTRIC_PULSE:
            interpolate_gradient(t, electric_pulse_colors, electric_pulse_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_GOLD_SHIMMER:
            interpolate_gradient(t, gold_shimmer_colors, gold_shimmer_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_GOLD_RADIANCE:
            interpolate_gradient(t, gold_radiance_colors, gold_radiance_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SILVER_GLEAM:
            interpolate_gradient(t, silver_gleam_colors, silver_gleam_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SILVER_LUSTER:
            interpolate_gradient(t, silver_luster_colors, silver_luster_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_BRONZE_GLOW:
            interpolate_gradient(t, bronze_glow_colors, bronze_glow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_BRONZE_SHEEN:
            interpolate_gradient(t, bronze_sheen_colors, bronze_sheen_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_TWILIGHT_PURPLE:
            interpolate_gradient(t, twilight_purple_colors, twilight_purple_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SUNLIT_MEADOW:
            interpolate_gradient(t, sunlit_meadow_colors, sunlit_meadow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_OCEAN_DEPTHS:
            interpolate_gradient(t, ocean_depths_colors, ocean_depths_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_CHERRY_BLOSSOM:
            interpolate_gradient(t, cherry_blossom_colors, cherry_blossom_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_EMERALD_DREAM:
            interpolate_gradient(t, emerald_dream_colors, emerald_dream_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SAPPHIRE_NIGHT:
            interpolate_gradient(t, sapphire_night_colors, sapphire_night_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_RUBY_GLOW:
            interpolate_gradient(t, ruby_glow_colors, ruby_glow_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_AMETHYST_HAZE:
            interpolate_gradient(t, amethyst_haze_colors, amethyst_haze_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_TOPAZ_SUNSET:
            interpolate_gradient(t, topaz_sunset_colors, topaz_sunset_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_AQUAMARINE_WAVE:
            interpolate_gradient(t, aquamarine_wave_colors, aquamarine_wave_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_COTTON_CANDY:
            interpolate_gradient(t, cotton_candy_colors, cotton_candy_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SWEET_CANDIES:
            interpolate_gradient(t, sweet_candies_colors, sweet_candies_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_STARRY_SKY:
            interpolate_gradient(t, starry_sky_colors, starry_sky_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_MOONLIT_FOG:
            interpolate_gradient(t, moonlit_fog_colors, moonlit_fog_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SUNFLOWER_FIELD:
            interpolate_gradient(t, sunflower_field_colors, sunflower_field_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_LILAC_DUSK:
            interpolate_gradient(t, lilac_dusk_colors, lilac_dusk_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_TURQUOISE_TIDE:
            interpolate_gradient(t, turquoise_tide_colors, turquoise_tide_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_CRIMSON_SKY:
            interpolate_gradient(t, crimson_sky_colors, crimson_sky_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PERIWINKLE_BREEZE:
            interpolate_gradient(t, periwinkle_breeze_colors, periwinkle_breeze_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_GALACTIC_HORIZON:
            interpolate_gradient(t, galactic_horizon_colors, galactic_horizon_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_PEPPERMINT_TWIST:
            interpolate_gradient(t, peppermint_twist_colors, peppermint_twist_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_ROSE_QUARTZ:
            interpolate_gradient(t, rose_quartz_colors, rose_quartz_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_MIDNIGHT_BLUE:
            interpolate_gradient(t, midnight_blue_colors, midnight_blue_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_SAFFRON_SUNRISE:
            interpolate_gradient(t, saffron_sunrise_colors, saffron_sunrise_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          case GROK2_GRADIENT_JADE_MIST:
            interpolate_gradient(t, jade_mist_colors, jade_mist_stops, 3, &grad_r, &grad_g, &grad_b);
            break;
          default:
            grad_r = grad_g = grad_b = 0.0; // Fallback
            break;
        }
        // Apply saturation and brightness adjustments
        gfloat h, s, v;
        rgb_to_hsv(grad_r, grad_g, grad_b, &h, &s, &v);
        s *= o->saturation;
        v *= o->brightness;
        hsv_to_rgb(h, s, v, &grad_r, &grad_g, &grad_b);
      }

      // Get input pixel color
      gfloat in_r = in_pixel[0];
      gfloat in_g = in_pixel[1];
      gfloat in_b = in_pixel[2];
      gfloat in_a = in_pixel[3];

      // Initialize output RGB and alpha
      gfloat final_r, final_g, final_b, final_a;

      if (o->blend == 0.0) {
        // No blending: use gradient color directly
        final_r = grad_r;
        final_g = grad_g;
        final_b = grad_b;
        final_a = o->alpha_lock ? in_a : 1.0;
      } else {
        // Convert input pixel and gradient to HSV for blending
        gfloat in_h, in_s, in_v;
        rgb_to_hsv(in_r, in_g, in_b, &in_h, &in_s, &in_v);

        gfloat grad_h, grad_s, grad_v;
        rgb_to_hsv(grad_r, grad_g, grad_b, &grad_h, &grad_s, &grad_v);

        // Blend in HSV space
        // Use gradient's hue, interpolate saturation and brightness
        gfloat final_h = grad_h; // Always use gradient's hue for Rainbowify effect
        gfloat final_s = in_s * (1.0 - o->blend) + grad_s * o->blend;
        gfloat final_v = in_v * (1.0 - o->blend) + grad_v * o->blend;

        // Convert blended HSV back to RGB
        hsv_to_rgb(final_h, final_s, final_v, &final_r, &final_g, &final_b);

        // Handle alpha channel
        final_a = in_a; // Default to input alpha
        if (!o->alpha_lock) {
          // If alpha_lock is disabled, set output alpha to 1.0 (opaque)
          final_a = 1.0;
        } else if (in_a < 1.0) {
          // If alpha_lock is enabled and input is transparent, blend towards input
          final_r = final_r * in_a + in_r * (1.0 - in_a);
          final_g = final_g * in_a + in_g * (1.0 - in_a);
          final_b = final_b * in_a + in_b * (1.0 - in_a);
        }
      }


      // Write output pixel
      out_pixel[0] = final_r;
      out_pixel[1] = final_g;
      out_pixel[2] = final_b;
      out_pixel[3] = final_a;

      // Move to next pixel
      in_pixel += 4;
      out_pixel += 4;
    }

  return TRUE;
}

static void
gegl_op_class_init(GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS(klass);
  GeglOperationPointFilterClass *point_filter_class = GEGL_OPERATION_POINT_FILTER_CLASS(klass);

  operation_class->prepare = prepare;
  point_filter_class->process = process;

  gegl_operation_class_set_keys(operation_class,
    "name",        "ai/lb:gradient",
    "title",       _("Gradients"),
    "reference-hash", "grokgradient2025",
    "description", _("Applies seamless gradient patterns, including spiral shapes with integer frequencies for seamlessness, to the image with customizable shape, direction, frequency, saturation, brightness, and transparency handling. To blend in a quality way, use GIMP's HSV-Hue or LCH Hue blend modes."),
    "gimp:menu-path", "<Image>/Filters/AI GEGL",
    "gimp:menu-label", _("Gradients..."),
    NULL);
}


#endif
