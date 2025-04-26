#include "config.h"
#include <glib/gi18n-lib.h>
#include <math.h>

#ifdef GEGL_PROPERTIES

property_double (cube_size, _("Cube Size"), 30.0)
    description (_("Size of each Tetris cube in pixels"))
    value_range (10.0, 200.0)
    ui_range (10.0, 100.0)

property_double (spacing, _("Grid Spacing"), 1.2)
    description (_("Spacing between Tetris cubes as a multiple of cube size"))
    value_range (1.0, 2.0)
    ui_range (1.0, 1.5)

property_double (rotation, _("Rotation"), 0.0)
    description (_("Rotation angle of Tetris cubes in degrees"))
    value_range (0.0, 360.0)
    ui_range (0.0, 360.0)


property_seed    (seed, _("Color Seed"), rand)
    description (_("Seed for randomizing Tetris cube colors"))


#else

#define GEGL_OP_FILTER
#define GEGL_OP_NAME     grok2
#define GEGL_OP_C_SOURCE grok2.c

#include "gegl-op.h"

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format(operation, "input", babl_format("RGBA float"));
  gegl_operation_set_format(operation, "output", babl_format("RGBA float"));
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  GeglRectangle *in_rect = gegl_operation_source_get_bounding_box(operation, "input");
  if (in_rect)
    return *in_rect;
  return gegl_rectangle_infinite_plane();
}

static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *input,
         GeglBuffer          *output,
         const GeglRectangle *roi,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES(operation);
  GeglBufferIterator *iter;
  const Babl *format = babl_format("RGBA float");

  /* Define Tetris color palette (RGB values) */
  gfloat colors[][3] = {
    {1.0, 0.0, 0.0}, /* Red */
    {0.0, 1.0, 0.0}, /* Green */
    {0.0, 0.0, 1.0}, /* Blue */
    {1.0, 1.0, 0.0}, /* Yellow */
    {0.0, 1.0, 1.0}, /* Cyan */
    {1.0, 0.0, 1.0}, /* Magenta */
    {1.0, 0.5, 0.0}  /* Orange */
  };
  gint num_colors = 7;

  iter = gegl_buffer_iterator_new(output, roi, level, format,
                                  GEGL_ACCESS_WRITE, GEGL_ABYSS_NONE, 2);
  gegl_buffer_iterator_add(iter, input, roi, level, format,
                           GEGL_ACCESS_READ, GEGL_ABYSS_NONE);

  while (gegl_buffer_iterator_next(iter))
  {
    gfloat *in_data = (gfloat *)iter->items[1].data;
    gfloat *out_data = (gfloat *)iter->items[0].data;
    gint width = iter->items[0].roi.width;
    gint height = iter->items[0].roi.height;
    gint x, y;

    for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
      {
        gint pixel_idx = (y * width + x) * 4;
        gfloat pixel_x = x + iter->items[0].roi.x;
        gfloat pixel_y = y + iter->items[0].roi.y;

        /* Calculate grid position */
        gfloat grid_size = o->cube_size * o->spacing;
        gfloat grid_x = pixel_x / grid_size;
        gfloat grid_y = pixel_y / grid_size;

        /* Find nearest cube center */
        gfloat center_x = (floor(grid_x) + 0.5) * grid_size;
        gfloat center_y = (floor(grid_y) + 0.5) * grid_size;

        /* Compute distance from cube center */
        gfloat dx = pixel_x - center_x;
        gfloat dy = pixel_y - center_y;

        /* Apply rotation */
        gfloat angle = o->rotation * G_PI / 180.0;
        gfloat rotated_dx = dx * cos(angle) - dy * sin(angle);
        gfloat rotated_dy = dx * sin(angle) + dy * cos(angle);

        /* Check if pixel is inside a cube */
        gfloat cube_half_size = o->cube_size * 0.5;
        if (fabs(rotated_dx) < cube_half_size && fabs(rotated_dy) < cube_half_size)
        {
          /* Assign color based on cube position and seed */
          gint ix = (gint)floor(grid_x);
          gint iy = (gint)floor(grid_y);
          /* Simple hash combining seed, ix, iy */
          guint hash = o->seed + ix * 73856093 + iy * 19349663;
          gint color_idx = (hash % num_colors + num_colors) % num_colors;
          gfloat *color = colors[color_idx];

          if (in_data[pixel_idx + 3] == 0.0)
          {
            /* Transparent input: black background, colored cube */
            out_data[pixel_idx + 0] = color[0]; /* Red */
            out_data[pixel_idx + 1] = color[1]; /* Green */
            out_data[pixel_idx + 2] = color[2]; /* Blue */
            out_data[pixel_idx + 3] = 1.0;      /* Opaque */
          }
          else
          {
            /* Non-transparent input: blend cube with input */
            out_data[pixel_idx + 0] = in_data[pixel_idx + 0] * (1.0 - 1.0) + color[0];
            out_data[pixel_idx + 1] = in_data[pixel_idx + 1] * (1.0 - 1.0) + color[1];
            out_data[pixel_idx + 2] = in_data[pixel_idx + 2] * (1.0 - 1.0) + color[2];
            out_data[pixel_idx + 3] = in_data[pixel_idx + 3];
          }
        }
        else
        {
          if (in_data[pixel_idx + 3] == 0.0)
          {
            /* Transparent input: black background */
            out_data[pixel_idx + 0] = 0.0;
            out_data[pixel_idx + 1] = 0.0;
            out_data[pixel_idx + 2] = 0.0;
            out_data[pixel_idx + 3] = 1.0;
          }
          else
          {
            /* Copy input pixel */
            out_data[pixel_idx + 0] = in_data[pixel_idx + 0];
            out_data[pixel_idx + 1] = in_data[pixel_idx + 1];
            out_data[pixel_idx + 2] = in_data[pixel_idx + 2];
            out_data[pixel_idx + 3] = in_data[pixel_idx + 3];
          }
        }
      }
    }
  }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass       *operation_class;
  GeglOperationFilterClass *filter_class;

  operation_class = GEGL_OPERATION_CLASS (klass);
  filter_class    = GEGL_OPERATION_FILTER_CLASS (klass);

  filter_class->process             = process;
  operation_class->prepare          = prepare;
  operation_class->get_bounding_box = get_bounding_box;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:grok2",
    "title",       _("Multicolor Cubes"),
    "categories",  "render",
    "reference-hash", "cub3dr0p",
    "description", _("Renders a grid of multi-colored Tetris-like cubes tiling the entire canvas with randomized colors"),
    NULL);
}

#endif
