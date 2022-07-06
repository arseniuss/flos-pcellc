/*
 * Copyright (C) 2022 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
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

#ifndef CELL_SCANNER_H
#define CELL_SCANNER_H

#include <stdint.h>

#include <utf8.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct scanner;

    struct scanner* cell_scanner_init(const char *f);

    int cell_scanner_consume(struct scanner *scn);

    int cell_scanner_token(struct scanner *scn);

    int cell_scanner_prevtoken(struct scanner *scn);

    const char *cell_scanner_text(struct scanner *scn);

    const char * cell_scanner_str(struct scanner *scn);

    utf8_char cell_scanner_chr(struct scanner *scn);

    uintmax_t cell_scanner_num(struct scanner *scn);


#ifdef __cplusplus
}
#endif

#endif /* CELL_SCANNER_H */
