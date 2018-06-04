/* test-kmerator.cpp
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

#include <iostream>
#include <cstring>

#include "kmeriser.h"

int main(int argc, char* argv[])
{
    char* samples[] = { "a", "n", "acw", "nn", 0 };
    char** ss = argc > 1 ? argv+1 : samples;
    const char *s;

    while ((s = *ss++))
    {
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
