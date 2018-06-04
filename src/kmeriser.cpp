/* kmeriser.cpp
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

static const int BASE_VALUES[] = { A, X, C, X, X, X, G, X, X, X, X, X, X, X, X, X, X, X, X, T, X, X, X, X, X, X };

const int MAX_KSIZE = 4 * sizeof(int) - 1;

kmeriser::kmeriser(const char *begin, const char *end, int ksize)
    : pcur_(begin), pend_(end-ksize+1), ksize_(ksize)
{
    if (ksize < 1 || ksize > MAX_KSIZE)
        raise_error("invalid kmer size: %d; must be in range [1,%d]", ksize, MAX_KSIZE);
}

bool
kmeriser::inc()
{
    return ++pcur_ < pend_;
}

int
kmeriser::val() const
{
    int res = 0;

    if (!(pcur_ < pend_))
        raise_error("kmeriser read attempted past right bound of sequence");

    for (int i = 0; i < ksize_; ++i)
    {
        int c = pcur_[i];

        int o = c - 'a';
        if (o < 0 || o > 19)        // between 'a' (0) and 't' (19)
        {
            o = c - 'A';            // try between 'A' and 'T'
            if (o < 0 || o > 19)
                raise_error("invalid base: %c; must be one of [acgtACGT]", c);
        }

        int v = BASE_VALUES[o];
        if (v == X)
            raise_error("invalid base: %c; must be one of [acgtACGT]", c);

        res = res<<2 | v;
    }

    return res;
}

// vim: sts=4:sw=4:ai:si:et
