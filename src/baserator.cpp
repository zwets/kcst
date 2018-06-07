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

static const int A = 0;
static const int C = 1;
static const int G = 2;
static const int T = 3;
static const int O = -1;


// The number of bases each letter of the alphabet maps on
//
static const int LETTER_BASES_LEN[26] = {
//  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, O, Y, Z
    1, 3, 1, 3, 0, 0, 1, 3, 0, 0, 2, 0, 2, 4, 0, 0, 0, 2, 2, 1, 0, 3, 2, 0, 2, 0
};


// The mapping from alphabet letter to the list bases it stands for
//
static const int LETTER_BASES[26][4] = { 
    { A, O, O, O }, // A - A
    { C, G, T, O }, // B - not A
    { C, O, O, O }, // C - C
    { A, G, T, O }, // D - not C
    { O, O, O, O },
    { O, O, O, O },
    { G, O, O, O }, // G - G
    { A, C, T, O }, // H - not G
    { O, O, O, O },
    { O, O, O, O },
    { G, T, O, O }, // K - Keto       (1.)     high
    { O, O, O, O },
    { A, C, O, O }, // M - aMino      (0.)     low
    { A, C, G, T }, // N - wildcard   (..)
    { O, O, O, O },
    { O, O, O, O },
    { O, O, O, O },
    { A, G, O, O }, // R - puRine     (.0)     even
    { C, G, O, O }, // S - Strong     (01|10)  centre
    { T, O, O, O }, // T - T          (11)
    { O, O, O, O },
    { A, C, G, O }, // V - not T
    { A, T, O, O }, // W - weak       (00|11)  extreme
    { O, O, O, O },
    { C, T, O, O }, // Y - pYrimidine (.1)     odd
    { O, O, O, O }
};


// Reset the baserator to operate on letter c
//
void
baserator::set(char c)
{
    int o = c - 'a';

    if (o < 0 || o > 25)
    {
        o = c - 'A';

        if (o < 0 || o > 25)
            raise_error("invalid sequence character: %c", c);
    }

    vals_ = LETTER_BASES[o];
    end_ = LETTER_BASES_LEN[o];
    pos_ = 0;

    if (*vals_ == O)
        raise_error("invalid base: %c", c);
}

// Bump the baserator to next value and return true, or to 0 and return false
//
bool
baserator::inc()
{
    if (++pos_ == end_)
    {
        pos_ = 0;
        return false;
    }

    return true;
}

// vim: sts=4:sw=4:ai:si:et
