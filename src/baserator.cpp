/* baserator.cpp
 * 
 * Copyright (C) 2018  Marco van Zwetselaar <io@zwets.it>
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

#include "kmeriser.h"
#include "utils.h"

#define A 0
#define C 1
#define G 2
#define T 3
#define X -1

static const int LETTER_BASES[26][5] = { 
    { A, X, X, X, X }, // A
    { C, G, T, X, X }, // B
    { C, X, X, X, X }, // C
    { A, G, T, X, X }, // D
    { X, X, X, X, X },
    { X, X, X, X, X },
    { G, X, X, X, X }, // G
    { A, C, T, X, X }, // H
    { X, X, X, X, X },
    { X, X, X, X, X },
    { G, T, X, X, X }, // K
    { X, X, X, X, X },
    { A, C, X, X, X }, // M
    { A, C, G, T, X }, // N
    { X, X, X, X, X },
    { X, X, X, X, X },
    { X, X, X, X, X },
    { A, G, X, X, X }, // R
    { C, G, X, X, X }, // S
    { T, X, X, X, X }, // T
    { X, X, X, X, X },
    { A, C, G, X, X }, // V
    { A, T, X, X, X }, // W
    { X, X, X, X, X },
    { C, T, X, X, X }, // Y
    { X, X, X, X, X } };

void
baserator::set(char c)
{
    int o = c - 'a';

    if (o < 0 || o > 25)
        o = c - 'A';

    if (o < 0 || o > 25)
        raise_error("invalid base: %c", c);

    pos_ = 0;
    vals_ = LETTER_BASES[o];

    if (vals_[0] == X)
        raise_error("invalid base: %c", c);
}

bool
baserator::inc()
{
    bool exhausted;

    if ((exhausted = (vals_[++pos_] == X)))
        pos_ = 0;

    return !exhausted;
}

// vim: sts=4:sw=4:ai:si:et
