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

#ifndef CELL__SCAN_H
#define CELL__SCAN_H

#include <stdint.h>

#include <cell/pos.h>
#include <cell/source.h>
#include <cell/token.h>

typedef struct cell_scanner_s cell_scanner_t;

char *cell_scanner_init(cell_source_t *src, cell_scanner_t **scn);
void cell_scanner_free(cell_scanner_t *scn);

cell_token_t cell_scanner_token(cell_scanner_t *scn);
cell_pos_t cell_scanner_pos(cell_scanner_t *scn);
uintmax_t cell_scanner_num(cell_scanner_t *scn);
const char *cell_scanner_text(cell_scanner_t *scn);
const char *cell_scanner_str(cell_scanner_t *scn);

char *cell_scanner_consume(cell_scanner_t *scn);

#endif /* CELL__SCAN_H */
