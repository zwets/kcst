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
#include "utils.cpp"

const int MAX_KSIZE = 8 * sizeof(int) - 1;

kmeriser::kmeriser(const char *begin, size_t len, int ksize)
    : pcur_(begin), pend_(begin + len - ksize + 1), kmerator_(ksize)
{
    if (ksize > MAX_KSIZE)
        raise_error("kmer size too large: %d (max is %d)", ksize, MAX_KSIZE);

    if (pcur_ < pend_)
        kmerator_.point_at(pcur_);
}

int
kmeriser::inc()
{
    if (pcur_ < pend_)
        if (!kmerator_.inc())
            if (++pcur_ < pend_)
            {
                kmerator_.point_at(pcur_);
                return true;
            }
            else
                return false;
        else
            return true;
    else
        return false;
}

int
kmeriser::val() const
{
    if (pcur_ < pend_)
        return kmerator_.val();
    else
        raise_error("kmeriser read attempted past right bound");
}

// vim: sts=4:sw=4:ai:si:et
