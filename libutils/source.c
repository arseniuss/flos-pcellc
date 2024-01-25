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

#include <assert.h>
#include <stdio.h>

#include <cell/source.h>

#include "../libutils/utils.h"

enum {
    FILE_TYPE_SOURCE,
    TEXT_TYPE_SOURCE
};

typedef struct {
    const char *filename;

    FILE *f;
} file_source_t;

typedef struct {
    char *text;
    size_t len;
} text_source_t;

typedef struct {
    int type;
    union {
        file_source_t file;
        text_source_t text;
    } data;
} source_t;

static const char *file_filename_get(cell_source_t *src) {
    source_t *fsrc = (source_t *)src->data;

    assert(fsrc->type == FILE_TYPE_SOURCE);

    return fsrc->data.file.filename;
}

static int file_getc(cell_source_t *src) {
    source_t *fsrc = (source_t *)src->data;

    assert(fsrc->type == FILE_TYPE_SOURCE);

    return fgetc(fsrc->data.file.f);
}

static long file_ftell(cell_source_t *src) {
    source_t *fsrc = (source_t *)src->data;

    assert(fsrc->type == FILE_TYPE_SOURCE);

    return ftell(fsrc->data.file.f);
}

static void file_rewind(cell_source_t *src) {
    source_t *fsrc = (source_t *)src->data;

    assert(fsrc->type == FILE_TYPE_SOURCE);

    rewind(fsrc->data.file.f);
}

static int file_fseek(cell_source_t *src, long pos) {
    source_t *fsrc = (source_t *)src->data;

    assert(fsrc->type == FILE_TYPE_SOURCE);

    return fseek(fsrc->data.file.f, pos, SEEK_SET);
}

char *cell_source_init_file(const char *filename, cell_source_t **src) {
    char *err = NULL;
    cell_source_t *ret;
    source_t *s;

    alloc_mem(ret, err, ret);
    memset(ret, 0, sizeof(cell_source_t));
    alloc_mem(s, err, error0);
    memset(s, 0, sizeof(source_t));

    s->type = FILE_TYPE_SOURCE;
    s->data.file.filename = filename;

    if (!(s->data.file.f = fopen(filename, "r"))) {
        err = reportf(NULL, NULL, SEV_ERROR, "cannot open file: '%s'", filename);
        goto error1;
    }

    ret->data = s;
    ret->filename_get = file_filename_get;
    ret->getc = file_getc;
    ret->ftell = file_ftell;
    ret->rewind = file_rewind;
    ret->fseek = file_fseek;

    *src = ret;

ret:
    return err;

error1:
    free(s);
error0:
    free(ret);
    return err;
}

void cell_source_free(cell_source_t *src) {
    source_t *fsrc = (source_t *)src->data;

    switch (fsrc->type) {
        case FILE_TYPE_SOURCE:
            fclose(fsrc->data.file.f);
            break;
        case TEXT_TYPE_SOURCE:
            free(fsrc->data.text.text);
            break;
    }

    free(src->data);
    free(src);
}
