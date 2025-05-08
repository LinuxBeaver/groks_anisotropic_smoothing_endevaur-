/*
 * Pippin (2006) for making GEGL
 * Beaver for vibe coding
 * Grok 3 for making the GEGL plugin
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
#include <gegl.h>
#include <gegl-plugin.h>
#include <math.h>

#ifdef GEGL_PROPERTIES

enum_start(pattern_type251294)
  enum_value(LATTICE_1, "lattice_1", "Lattice")
  enum_value(RIPPLE_EFFECT, "ripple_effect", "Ripple Effect")
  enum_value(PETAL_SWIRLS, "petal_swirls", "Petal Swirls")
  enum_value(CHECKERBOARD_WAVES, "checkerboard_waves", "Checkerboard Waves")
  enum_value(MOSAIC_TILES, "mosaic_tiles", "Mosaic Tiles")
  enum_value(INTERLOCKING_GEARS, "interlocking_gears", "Interlocking Gears")
  enum_value(WAVE_INTERFERENCE, "wave_interference", "Wave Interference")
  enum_value(CRYSTAL_SHARDS, "crystal_shards", "Crystal Shards")
  enum_value(PRISM_SHARDS, "prism_shards", "Prism Shards")
  enum_value(ECHO_WAVES, "echo_waves", "Echo Waves")
  enum_value(INTERLOCKED_RINGS, "interlocked_rings", "Interlocked Rings")
  enum_value(PULSE_WAVES, "pulse_waves", "Pulse Waves")
  enum_value(WOVEN_THREADS, "woven_threads", "Woven Threads")
  enum_value(FRACTAL_WAVES, "fractal_waves", "Fractal Waves")
  enum_value(STAR_WEAVE, "star_weave", "Star Weave")
  enum_value(GRID_WAVES_1, "grid_waves_1", "Grid Waves")
  enum_value(ZIGZAG_WEAVE_1, "zigzag_weave_1", "Zigzag Weave")
  enum_value(DIAMOND_GRID_1, "diamond_grid_1", "Diamond Grid")
  enum_value(TRIBAL_BANDS_1, "tribal_bands_1", "Tribal Bands")
  enum_value(ASANOHA_STARS_1, "asanoha_stars_1", "Asanoha Stars")
  enum_value(INTERLOCKED_SQUARES_1, "interlocked_squares_1", "Interlocked Squares")
  enum_value(SPIRAL_MAZE_1, "spiral_maze_1", "Spiral Maze")
  enum_value(WAVE_CRESTS_1, "wave_crests_1", "Wave Crests")
  enum_value(FLORAL_LATTICE_1, "floral_lattice_1", "Floral Lattice")
  enum_value(CHEVRON_STRIPES_1, "chevron_stripes_1", "Chevron Stripes")
  enum_value(CHERRY_BLOSSOMS_1, "cherry_blossoms_1", "Cherry Blossoms")
  enum_value(SUNBURST_MOTIF_1, "sunburst_motif_1", "Sunburst Motif")
  enum_value(MEANDER_1, "meander_1", "Meander")
  enum_value(DOUBLE_CHEVRON_BANDS, "double_chevron_bands", "Double Chevron Bands")
  enum_value(OVERLAPPING_CIRCLE_WAVES, "overlapping_circle_waves", "Overlapping Circle Waves")
  enum_value(HEXAGON_WAVE_TILES, "hexagon_wave_tiles", "Hexagon Wave Tiles")
  enum_value(WAVE_FRIEZE_1, "wave_frieze_1", "Wave Frieze")
  enum_value(CHEVRON_GRID_OVERLAY, "chevron_grid_overlay", "Chevron Grid Overlay")
  enum_value(INTERLOCKED_SQUARES_2, "interlocked_squares_2", "Interlocked Squares Alternate")
  enum_value(CHEVRON_RIPPLE_EFFECT, "chevron_ripple_effect", "Chevron Ripple Effect")
  enum_value(CIRCLE_LATTICE_FLOW, "circle_lattice_flow", "Circle Lattice Flow")
  enum_value(PALMETTE_WAVES_1, "palmette_waves_1", "Palmette Waves")
  enum_value(SPIRAL_MAZE_2, "spiral_maze_2", "Spiral Maze Alternate")
  enum_value(KEY_WAVE_1, "key_wave_1", "Key Wave")
  enum_value(ANTHEMION_MOTIF_1, "anthemion_motif_1", "Anthemion Motif")
  enum_value(CHEVRON_MAZE_1, "chevron_maze_1", "Chevron Maze")
  enum_value(STAR_FRIEZE_1, "star_frieze_1", "Star Frieze")
  enum_value(LOTUS_WAVE_1, "lotus_wave_1", "Lotus Wave")
  enum_value(STAR_LATTICE_1, "star_lattice_1", "Star Lattice")
  enum_value(ROSETTE_PATTERN_1, "rosette_pattern_1", "Rosette Pattern")
  enum_value(CONCENTRIC_RINGS, "concentric_rings", "Concentric Rings")
  enum_value(WAVY_STRIPES, "wavy_stripes", "Wavy Stripes")
  enum_value(ZIGZAG_STRIPES, "zigzag_stripes", "Zigzag Stripes")
  enum_value(TWISTED_RIBBONS, "twisted_ribbons", "Twisted Ribbons")
  enum_value(INTERFERING_WAVES, "interfering_waves", "Interfering Waves")
  enum_value(CURVED_BANDS, "curved_bands", "Curved Bands")
  enum_value(WAVE_CROSS, "wave_cross", "Wave Cross")
  enum_value(RIPPLE_GRID, "ripple_grid", "Ripple Grid")
  enum_value(PETAL_GRID, "petal_grid", "Petal Grid")
  enum_value(WAVE_SPIKES, "wave_spikes", "Wave Spikes")
  enum_value(CIRCLE_WEAVE, "circle_weave", "Circle Weave")
  enum_value(GRID_SWIRLS, "grid_swirls", "Grid Swirls")
  enum_value(BRAIDED_STRIPS, "braided_strips", "Braided Strips")
  enum_value(WAVE_LATTICE, "wave_lattice", "Wave Lattice")
  enum_value(LOOP_MOTIF, "loop_motif", "Loop Motif")
  enum_value(CURVE_MAZE, "curve_maze", "Curve Maze")
  enum_value(PULSE_GRID, "pulse_grid", "Pulse Grid")
enum_end(pattern_type251294)

property_enum(pattern, _("Pattern"),
              pattern_type251294, pattern_type251294,
              LATTICE_1)
    description(_("Select the pattern to render"))

property_double(tile_size, _("Tile Size"), 500.0)
    description(_("Size of the repeating tile in pixels"))
    value_range(50.0, 2000.0)
    ui_range(50.0, 2000.0)

property_double(line_width, _("Line Width"), 0.7)
    description(_("Adjust the thickness of the pattern lines"))
    value_range(0.01, 3.0)
    ui_range(0.01, 3.0)

property_double(rotation, _("Rotation"), 0.0)
    description(_("Rotate the pattern (degrees)"))
    value_range(0.0, 360.0)
    ui_range(0.0, 360.0)

property_color(foreground_color, _("Foreground Color"), "#5BA9EA")
    description(_("Color of the pattern shapes"))

property_color(background_color, _("Background Color"), "#70D0FF")
    description(_("Color of the background"))

#else

#define GEGL_OP_POINT_FILTER
#define GEGL_OP_NAME     sinewaves
#define GEGL_OP_C_SOURCE sinewaves.c

#include "gegl-op.h"

static void prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "output", babl_format ("RGBA float"));
}

static GeglRectangle get_bounding_box (GeglOperation *operation)
{
  return gegl_rectangle_infinite_plane ();
}

static gboolean
process (GeglOperation        *operation,
         GeglOperationContext *context,
         const gchar          *output_prop,
         const GeglRectangle  *result,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  GeglBuffer *output = gegl_operation_context_get_target (context, output_prop);
  gdouble fg_color[4], bg_color[4];
  gdouble tile_size = o->tile_size;
  GeglRectangle rect = *result;
  gint x, y;

  // Get color values
  gegl_color_get_rgba (o->foreground_color, &fg_color[0], &fg_color[1], &fg_color[2], &fg_color[3]);
  gegl_color_get_rgba (o->background_color, &bg_color[0], &bg_color[1], &bg_color[2], &bg_color[3]);

  // Convert rotation to radians
  gdouble rotation_rad = o->rotation * G_PI / 180.0;
  gdouble cos_rot = cos(rotation_rad);
  gdouble sin_rot = sin(rotation_rad);

  for (y = rect.y; y < rect.y + rect.height; y++)
  {
    for (x = rect.x; x < rect.x + rect.width; x++)
    {
      gdouble px = (gdouble)x;
      gdouble py = (gdouble)y;

      // Apply rotation to the original pixel coordinates around (0, 0)
      gdouble px_rot = px * cos_rot - py * sin_rot;
      gdouble py_rot = px * sin_rot + py * cos_rot;

      // Map rotated coordinates to tile space
      gdouble tx = fmod(px_rot, tile_size);
      gdouble ty = fmod(py_rot, tile_size);
      if (tx < 0) tx += tile_size;
      if (ty < 0) ty += tile_size;

      gfloat out[4];
      gboolean in_shape = FALSE;

      switch (o->pattern)
      {
        case LATTICE_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;

          if (cx < line_width || cx > cell_size - line_width ||
              cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (fabs(cx - cy) < line_width || fabs(cx + cy - cell_size) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case RIPPLE_EFFECT:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble ripple_spacing = cell_size / 8.0;
          gdouble ripple_dist = fmod(dist, ripple_spacing);
          gdouble base_line_width = ripple_spacing * 0.2;
          gdouble line_width = base_line_width * o->line_width;
          if (ripple_dist < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case PETAL_SWIRLS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble petal_angle = fmod(angle * 180.0 / G_PI, 60.0);
          gdouble petal_dist = dist + sin(angle * 6.0) * (cell_size / 8.0);
          gdouble base_petal_width = cell_size / 10.0;
          gdouble petal_width = base_petal_width * o->line_width;
          if (petal_angle < petal_width && petal_dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CHECKERBOARD_WAVES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gint cell_x = (gint)(tx / cell_size);
          gint cell_y = (gint)(ty / cell_size);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if ((cell_x + cell_y) % 2 == 0)
          {
            if (fabs(cy - (cell_size / 2.0 + wave)) < line_width)
            {
              in_shape = TRUE;
            }
          }
          else
          {
            wave = sin((cy / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
            if (fabs(cx - (cell_size / 2.0 + wave)) < line_width)
            {
              in_shape = TRUE;
            }
          }
          break;
        }
        case MOSAIC_TILES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          gdouble wave_x = sin((cy / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble wave_y = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          if (fabs(cx - (cell_size / 2.0 + wave_x)) < line_width ||
              fabs(cy - (cell_size / 2.0 + wave_y)) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case INTERLOCKING_GEARS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble gear_teeth = sin(angle * 8.0) * (cell_size / 8.0);
          gdouble gear_dist = dist + gear_teeth;
          gdouble base_gear_width = cell_size / 10.0;
          gdouble gear_width = base_gear_width * o->line_width;
          if (gear_dist > cell_size / 4.0 - gear_width && gear_dist < cell_size / 4.0 + gear_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVE_INTERFERENCE:
        {
          gdouble wave1 = sin((tx / tile_size) * 4.0 * G_PI) * (tile_size / 10.0);
          gdouble wave2 = sin((ty / tile_size) * 4.0 * G_PI) * (tile_size / 10.0);
          gdouble interference = wave1 + wave2;
          gdouble base_line_width = tile_size / 20.0;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(interference) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CRYSTAL_SHARDS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble angle = atan2(dy, dx);
          gdouble angle_mod = fmod(angle * 180.0 / G_PI, 60.0);
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (angle_mod < line_width && dist < center_x)
          {
            in_shape = TRUE;
          }
          break;
        }
        case PRISM_SHARDS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble angle = atan2(dy, dx);
          gdouble angle_mod = fmod(angle * 180.0 / G_PI, 45.0);
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (angle_mod < line_width && dist < center_x)
          {
            in_shape = TRUE;
          }
          break;
        }
        case ECHO_WAVES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble wave_spacing = cell_size / 6.0;
          gdouble wave_mod = fmod(dist, wave_spacing);
          gdouble echo = sin(dist * 0.2) * (cell_size / 10.0);
          gdouble base_line_width = wave_spacing * 0.2;
          gdouble line_width = base_line_width * o->line_width;
          if (wave_mod + echo < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case INTERLOCKED_RINGS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble ring_spacing = cell_size / 4.0;
          gdouble ring_mod = fmod(dist, ring_spacing);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (ring_mod < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case PULSE_WAVES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble pulse = sin(dist * 0.1) * (cell_size / 8.0);
          gdouble wave_spacing = cell_size / 6.0;
          gdouble wave_mod = fmod(dist + pulse, wave_spacing);
          gdouble base_line_width = wave_spacing * 0.2;
          gdouble line_width = base_line_width * o->line_width;
          if (wave_mod < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WOVEN_THREADS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave1 = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble wave2 = sin((cy / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          gdouble over_under = sin((tx / tile_size) * 2.0 * G_PI) * sin((ty / tile_size) * 2.0 * G_PI);
          if (fabs(cx - (cell_size / 2.0 + wave1)) < line_width && over_under > 0)
          {
            in_shape = TRUE;
          }
          if (fabs(cy - (cell_size / 2.0 + wave2)) < line_width && over_under <= 0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case FRACTAL_WAVES:
        {
          gdouble wave = sin((tx / tile_size) * 4.0 * G_PI) * (tile_size / 10.0);
          gdouble sub_wave = sin((tx / (tile_size / 2.0)) * 8.0 * G_PI) * (tile_size / 20.0);
          gdouble wave_y = sin((ty / tile_size) * 4.0 * G_PI) * (tile_size / 10.0);
          gdouble interference = wave + sub_wave + wave_y;
          gdouble base_line_width = tile_size / 20.0;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(interference) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case STAR_WEAVE:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble star_angle = fmod(angle * 180.0 / G_PI, 45.0);
          gdouble star_dist = dist + sin(angle * 8.0) * (cell_size / 8.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (star_angle < line_width && star_dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case GRID_WAVES_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          if (cx < line_width || cx > cell_size - line_width ||
              cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (fabs(cy - (cx + wave)) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case ZIGZAG_WEAVE_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          gdouble zigzag = fabs(fmod(cx, cell_size / 2.0) - cell_size / 4.0);
          if (fabs(cy - zigzag) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case DIAMOND_GRID_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cx - cell_size / 2.0) + fabs(cy - cell_size / 2.0) < cell_size / 4.0 + line_width &&
              fabs(cx - cell_size / 2.0) + fabs(cy - cell_size / 2.0) > cell_size / 4.0 - line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case TRIBAL_BANDS_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          gdouble band = fmod(cy, cell_size / 2.0);
          if (band < line_width || fabs(band - cell_size / 4.0) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case ASANOHA_STARS_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cx - cell_size / 2.0) + fabs(cy - cell_size / 2.0) < cell_size / 4.0 + line_width &&
              fabs(cx - cell_size / 2.0) + fabs(cy - cell_size / 2.0) > cell_size / 4.0 - line_width)
          {
            in_shape = TRUE;
          }
          else if (fabs(cx - cy) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case INTERLOCKED_SQUARES_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (cx < line_width || cx > cell_size - line_width ||
              cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (cx > cell_size / 4.0 - line_width && cx < cell_size / 4.0 + line_width &&
                   cy > cell_size / 4.0 - line_width && cy < cell_size / 4.0 + line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case SPIRAL_MAZE_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble spiral = dist - (angle * cell_size / (4.0 * G_PI));
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (fmod(spiral, cell_size / 4.0) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVE_CRESTS_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cy - (cell_size / 2.0 + wave)) < line_width && cy < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case FLORAL_LATTICE_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (cx < line_width || cx > cell_size - line_width ||
              cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (fabs(cx - cell_size / 2.0) < line_width && fabs(cy - cell_size / 2.0) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CHEVRON_STRIPES_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cy = fmod(ty, cell_size);
          gdouble chevron = fabs(fmod(tx, cell_size) - cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cy - chevron) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CHERRY_BLOSSOMS_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble petal_angle = fmod(angle * 180.0 / G_PI, 72.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (petal_angle < line_width && dist < cell_size / 4.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case SUNBURST_MOTIF_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble angle = atan2(dy, dx);
          gdouble angle_mod = fmod(angle * 180.0 / G_PI, 30.0);
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (angle_mod < line_width && dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case MEANDER_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (cx < cell_size / 2.0 && cy > cell_size / 2.0 - line_width && cy < cell_size / 2.0 + line_width)
          {
            in_shape = TRUE;
          }
          else if (cx > cell_size / 2.0 - line_width && cx < cell_size / 2.0 + line_width && cy < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case DOUBLE_CHEVRON_BANDS:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cy = fmod(ty, cell_size);
          gdouble chevron1 = fabs(fmod(tx, cell_size) - cell_size / 2.0);
          gdouble chevron2 = fabs(fmod(tx + cell_size / 4.0, cell_size) - cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cy - chevron1) < line_width || fabs(cy - chevron2) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case OVERLAPPING_CIRCLE_WAVES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble wave = sin((cy / cell_size) * 2.0 * G_PI) * (cell_size / 8.0);
          gdouble dx = cx - center_x + wave;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (dist > cell_size / 4.0 - line_width && dist < cell_size / 4.0 + line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case HEXAGON_WAVE_TILES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble hex_x = cx / (cell_size * 0.866);
          gdouble hex_y = cy / cell_size;
          gdouble hex_center_x = floor(hex_x) + 0.5;
          gdouble hex_center_y = floor(hex_y) + 0.5;
          if ((gint)floor(hex_y) % 2 == 1) hex_center_x += 0.5;
          gdouble dx = (hex_x - hex_center_x) * cell_size * 0.866;
          gdouble dy = (hex_y - hex_center_y) * cell_size;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 8.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (dist > cell_size / 2.5 - line_width && dist < cell_size / 2.5 + line_width)
          {
            in_shape = TRUE;
          }
          else if (dist < cell_size / 2.5 && fabs(dy - wave) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVE_FRIEZE_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cy - (cell_size / 2.0 + wave)) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CHEVRON_GRID_OVERLAY:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble chevron = fabs(fmod(cx, cell_size / 2.0) - cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cy - chevron) < line_width)
          {
            in_shape = TRUE;
          }
          else if (cx < line_width || cx > cell_size - line_width ||
                   cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case INTERLOCKED_SQUARES_2:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (cx < line_width || cx > cell_size - line_width ||
              cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (cx > cell_size / 4.0 - line_width && cx < cell_size / 4.0 + line_width &&
                   cy < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          else if (cy > cell_size / 4.0 - line_width && cy < cell_size / 4.0 + line_width &&
                   cx > cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CHEVRON_RIPPLE_EFFECT:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble ripple = sin((tx / tile_size) * 2.0 * G_PI) * (cell_size / 8.0);
          gdouble chevron = fabs(fmod(cx, cell_size / 2.0) - cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (fabs(cy - (chevron + ripple)) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CIRCLE_LATTICE_FLOW:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble flow = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 8.0);
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y + flow;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (dist > cell_size / 4.0 - line_width && dist < cell_size / 4.0 + line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case PALMETTE_WAVES_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble angle = atan2(dy, dx);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 8.0);
          gdouble palmette_angle = fmod(angle * 180.0 / G_PI, 90.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (palmette_angle < line_width && dy + wave < cell_size / 4.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case SPIRAL_MAZE_2:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble spiral = dist - (angle * cell_size / (3.0 * G_PI));
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (fmod(spiral, cell_size / 3.0) < line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case KEY_WAVE_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 6.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (cx > cell_size / 2.0 - line_width && cx < cell_size / 2.0 + line_width && cy + wave < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case ANTHEMION_MOTIF_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble anthemion_angle = fmod(angle * 180.0 / G_PI, 45.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (anthemion_angle < line_width && dist < cell_size / 3.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CHEVRON_MAZE_1:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble chevron = fabs(fmod(cx, cell_size / 2.0) - cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1;
          gdouble line_width = base_line_width * o->line_width;
          if (cy < line_width || cy > cell_size - line_width)
          {
            in_shape = TRUE;
          }
          else if (fabs(cx - chevron) < line_width && cy < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case STAR_FRIEZE_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble star_angle = fmod(angle * 180.0 / G_PI, 45.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (star_angle < line_width && dist > cell_size / 4.0 && dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case LOTUS_WAVE_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble wave = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 8.0);
          gdouble lotus_angle = fmod(angle * 180.0 / G_PI, 60.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (lotus_angle < line_width && dist + wave < cell_size / 3.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case STAR_LATTICE_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble star_angle = fmod(angle * 180.0 / G_PI, 45.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (star_angle < line_width && dist > cell_size / 3.0 && dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case ROSETTE_PATTERN_1:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble rosette_angle = fmod(angle * 180.0 / G_PI, 36.0);
          gdouble base_line_width = cell_size * 0.05;
          gdouble line_width = base_line_width * o->line_width;
          if (rosette_angle < line_width && dist > cell_size / 4.0 && dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CONCENTRIC_RINGS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble ring_spacing = cell_size / 8.0;
          gdouble ring_mod = fmod(dist, ring_spacing);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (ring_mod < base_line_width && dist < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVY_STRIPES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble wave = sin((tx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble stripe = fmod(ty + wave, cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.2 * o->line_width;
          if (stripe < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case ZIGZAG_STRIPES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble diag_x = (tx + ty) / sqrt(2.0);
          gdouble diag_y = (ty - tx) / sqrt(2.0);
          gdouble cd = fmod(diag_x, cell_size);
          gdouble zigzag = fabs(fmod(cd, cell_size / 2.0) - cell_size / 4.0) * 2.0 - cell_size / 4.0;
          gdouble stripe = fmod(diag_y + zigzag, cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.2 * o->line_width;
          if (stripe < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case TWISTED_RIBBONS:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble amplitude = (cell_size / 4.0) * sin(ty * G_PI / cell_size);
          gdouble wave = sin((ty / cell_size) * 2.0 * G_PI) * amplitude;
          gdouble stripe = fmod(tx + wave, cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.2 * o->line_width;
          if (stripe < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case INTERFERING_WAVES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave_h = sin((tx / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble stripe_h = fmod(ty + wave_h, cell_size / 2.0);
          gdouble wave_v = sin((ty / cell_size) * 2.0 * G_PI) * (cell_size / 4.0);
          gdouble stripe_v = fmod(tx + wave_v, cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.2 * o->line_width;
          if (stripe_h < base_line_width || stripe_v < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CURVED_BANDS:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cy = fmod(ty, cell_size);
          gdouble curve = sin((tx / cell_size) * 1.5 * G_PI) * (cell_size / 3.0);
          gdouble band = fmod(cy + curve, cell_size / 2.0);
          gdouble base_line_width = cell_size * 0.15 * o->line_width;
          if (band < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVE_CROSS:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave_x = sin((cy / cell_size) * 2.0 * G_PI) * (cell_size / 5.0);
          gdouble wave_y = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 5.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (fabs(cx - (cell_size / 2.0 + wave_x)) < base_line_width ||
              fabs(cy - (cell_size / 2.0 + wave_y)) < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case RIPPLE_GRID:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble ripple = sin(dist * 0.2) * (cell_size / 10.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (fmod(cx + ripple, cell_size / 4.0) < base_line_width ||
              fmod(cy + ripple, cell_size / 4.0) < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case PETAL_GRID:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble petal_angle = fmod(angle * 180.0 / G_PI, 40.0);
          gdouble base_line_width = cell_size * 0.06 * o->line_width;
          if (petal_angle < base_line_width && dist < cell_size / 3.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVE_SPIKES:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble spike_angle = fmod(angle * 180.0 / G_PI, 30.0);
          gdouble wave = sin(dist * 0.3) * (cell_size / 10.0);
          gdouble base_line_width = cell_size * 0.06 * o->line_width;
          if (spike_angle < base_line_width && dist + wave < cell_size / 2.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CIRCLE_WEAVE:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble wave_x = sin((cy / cell_size) * 2.0 * G_PI) * (cell_size / 6.0);
          gdouble wave_y = sin((cx / cell_size) * 2.0 * G_PI) * (cell_size / 6.0);
          gdouble base_line_width = cell_size * 0.08 * o->line_width;
          if (dist > cell_size / 3.0 - base_line_width && dist < cell_size / 3.0 + base_line_width &&
              (fabs(dx - wave_x) < base_line_width || fabs(dy - wave_y) < base_line_width))
          {
            in_shape = TRUE;
          }
          break;
        }
        case GRID_SWIRLS:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble swirl = dist + angle * cell_size / (4.0 * G_PI);
          gdouble grid_mod = fmod(cx, cell_size / 3.0) + fmod(cy, cell_size / 3.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (fmod(swirl, cell_size / 4.0) < base_line_width && grid_mod < cell_size / 3.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case BRAIDED_STRIPS:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave1 = sin((cx / cell_size) * 1.5 * G_PI) * (cell_size / 4.0);
          gdouble wave2 = sin((cx / cell_size) * 1.5 * G_PI + G_PI / 2.0) * (cell_size / 4.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (fabs(cy - (cell_size / 2.0 + wave1)) < base_line_width ||
              fabs(cy - (cell_size / 2.0 + wave2)) < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case WAVE_LATTICE:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble wave = sin((cx + cy) / cell_size * 2.0 * G_PI) * (cell_size / 5.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (fmod(cx + wave, cell_size / 4.0) < base_line_width ||
              fmod(cy + wave, cell_size / 4.0) < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case LOOP_MOTIF:
        {
          gdouble cell_size = tile_size / 5.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble center_x = cell_size / 2.0;
          gdouble center_y = cell_size / 2.0;
          gdouble dx = cx - center_x;
          gdouble dy = cy - center_y;
          gdouble dist = sqrt(dx * dx + dy * dy);
          gdouble angle = atan2(dy, dx);
          gdouble loop_angle = fmod(angle * 180.0 / G_PI, 90.0);
          gdouble loop_dist = dist + sin(angle * 4.0) * (cell_size / 10.0);
          gdouble base_line_width = cell_size * 0.06 * o->line_width;
          if (loop_angle < base_line_width && loop_dist < cell_size / 3.0)
          {
            in_shape = TRUE;
          }
          break;
        }
        case CURVE_MAZE:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble curve = sin((cx / cell_size) * 1.5 * G_PI) * (cell_size / 3.0);
          gdouble maze = fmod(cy + curve, cell_size / 3.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (maze < base_line_width || fmod(cx, cell_size / 3.0) < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
        case PULSE_GRID:
        {
          gdouble cell_size = tile_size / 4.0;
          gdouble cx = fmod(tx, cell_size);
          gdouble cy = fmod(ty, cell_size);
          gdouble pulse = sin((cx + cy) / cell_size * 2.0 * G_PI) * (cell_size / 6.0);
          gdouble base_line_width = cell_size * 0.1 * o->line_width;
          if (fmod(cx + pulse, cell_size / 4.0) < base_line_width &&
              fmod(cy + pulse, cell_size / 4.0) < base_line_width)
          {
            in_shape = TRUE;
          }
          break;
        }
      }

      if (in_shape)
      {
        out[0] = fg_color[0];
        out[1] = fg_color[1];
        out[2] = fg_color[2];
        out[3] = 1.0;
      }
      else
      {
        out[0] = bg_color[0];
        out[1] = bg_color[1];
        out[2] = bg_color[2];
        out[3] = 1.0;
      }

      gegl_buffer_set (output, GEGL_RECTANGLE (x, y, 1, 1), 0, babl_format ("RGBA float"), out, GEGL_AUTO_ROWSTRIDE);
    }
  }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  operation_class->prepare = prepare;
  operation_class->get_bounding_box = get_bounding_box;
  operation_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "ai/lb:sine-waves",
    "title",       _("Sine Waves"),
    "reference-hash", "grok2sinewaves",
    "description", _("Renders a variety of sinewave patterns with adjustable line width, rotation, and colors"),
    "gimp:menu-path", "<Image>/Filters/AI GEGL/",
    "gimp:menu-label", _("Sine Waves..."),
    NULL);
}

#endif
