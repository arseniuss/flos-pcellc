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

#ifndef CELL_GENERATORS_H
#define CELL_GENERATORS_H

#include <cell/scope.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef int(*out_func)(const char *fmt, ...);

    int cell_llvm_generate(struct cell_module *mod, out_func out);


#ifdef __cplusplus
}
#endif

#endif /* CELL_GENERATORS_H */
