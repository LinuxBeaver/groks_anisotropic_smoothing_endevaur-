/* grok.c
 *
 * Copyright (C) 2025 LinuxBeaver and contributors
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
#include <stdio.h>

#define GEGL_OP_POINT_FILTER
#define GEGL_OP_NAME     grok
#define GEGL_OP_C_SOURCE grok.c

#ifndef GEGL_OP_C_SOURCE

#include <glib-object.h>







static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationPointFilterClass *point_filter_class = GEGL_OPERATION_POINT_FILTER_CLASS (klass);


  operation_class->prepare = prepare;
  point_filter_class->process = process;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:grok2",
    "title",       "Vibrance Effect",
    "categories",  "color",
    "description", "Adjusts vibrance by enhancing less saturated colors, similar to G'MIC's vibrance effect",
    NULL);
}

#endif
