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

#ifndef CELL__SOURCE_H
#define CELL__SOURCE_H

struct cell_source_s;
typedef struct cell_source_s cell_source_t;

struct cell_source_s {
    const char *(*filename_get)(cell_source_t *src);

    void (*rewind)(cell_source_t *src);
    int (*getc)(cell_source_t *src);
    long (*ftell)(cell_source_t *src);
    int (*fseek)(cell_source_t *src, long pos);

    void *data;
};

char *cell_source_init_file(const char *filename, cell_source_t **src);
void cell_source_free(cell_source_t *src);

#endif /* CELL__SOURCE_H */
