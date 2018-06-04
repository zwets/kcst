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

//TESTING
#include <iostream>
#include <cstring>

#include "kmeriser.h"

kmerator::kmerator(int ksize)
{
    baserators_.reserve(ksize);
    for (int i = 0; i < ksize; ++i)
        baserators_[i] = baserator();
}

void
kmerator::set(const char *p)
{
    for (std::vector<baserator>::iterator b = baserators_.begin(); b != baserators_.end(); ++b) 
        b->set(*p++);
}

bool
kmerator::inc()
{
    std::vector<baserator>::iterator p = baserators_.end();

    while (--p != baserators_.begin() && !p->inc())
        /* carry the inc across the baserators */;

    return p != baserators_.begin() || p->inc();
}

int
kmerator::val() const
{
    int res = 0;

    for (std::vector<baserator>::const_iterator p = baserators_.begin(); p != baserators_.end(); ++p)
        res = (res<<2) | p->val();

    return res;
}

// TESTING
int main(int, char*[])
{
    const char* ss[] = { "a", "n", "acw", "nn" };

    for (int i = 0; i<sizeof(ss)/sizeof(const char*); ++i)
    {
        const char* s = ss[i];
        size_t k = std::strlen(s);

        kmerator r(k);
        r.set(s);

        std::cout << "string: " << s << std::endl;
        std::cout << "kmers:";

        do {
            std::cout << ' ' << r.val();
        } while (r.inc());
        
        std::cout << std::endl << std::endl;
    }
}

// vim: sts=4:sw=4:ai:si:et
