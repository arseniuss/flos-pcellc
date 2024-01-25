/*
 * Copyright (C) 2024 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef CELL__POS_H
#define CELL__POS_H

#include <stddef.h>

typedef struct {
    unsigned row;
    unsigned col;
    long line_pos;
} cell_cursor_t;

typedef struct {
    cell_cursor_t start;
    cell_cursor_t end;
} cell_pos_t;

#endif /* CELL__POS_H */
