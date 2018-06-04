/* kmerator.cpp
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

const int MAX_KSIZE = 8 * sizeof(int) - 1;

kmerator::kmerator(const char *begin, const char *end, int ksize)
    : pcur_(begin), pend_(end-ksize+1), ksize_(ksize)
{
    if (ksize < 1 || ksize > MAX_KSIZE)
        raise_error("invalid kmer size: %d; must be in range [1,%d]", ksize, MAX_KSIZE);

    if (pcur_ < pend_)
        for (int i = 0; i < ksize; ++i)
            baserators_.push_back(baserator(pcur_[i]));
}

bool
kmerator::inc()
{
    bool done = false;

    std::vector<baserator>::iterator p = baserators_.end();

    while (!done && p-- != baserators_.begin())
        done = p->inc();

    // if not done now, p is before begin, and we shift right

    if (!done && (done = ++pcur_ < pend_))
    {
        p = baserators_.begin();

        while (++p < baserators_.end())
            *(p-1) = *p;

        *(p-1) = pcur_[ksize_-1];
    }

    return done;
}

int
kmerator::val() const
{
    int res = 0;

    if (!(pcur_ < pend_))
        raise_error("kmerator read attempted past right bound of sequence");

    for (std::vector<baserator>::const_iterator p = baserators_.begin(); p != baserators_.end(); ++p)
        res = (res<<2) | p->val();

    return res;
}

// vim: sts=4:sw=4:ai:si:et
