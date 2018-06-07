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

#include <stdexcept>
#include <iostream>
#include <cstring>

#include "kmeriser.h"

int main(int argc, const char* argv[])
{
    const char * samples[] = { "aaaa", "nnnn", "acwt", 0 };
    const char** ss = argc > 1 ? argv+1 : samples;
    const char *b;

    try {
        while ((b = *ss++))
        {
            const char *e = b + std::strlen(b);
            int k = std::strlen(b);
            if (k > 2)
                k = k - 2;
            kmerator r(b, e, k);

            std::cout << "string: " << b << std::endl;
            std::cout << "kmers:";

            do {
                std::cout << ' ' << r.val();
            } while (r.inc());
            
            std::cout << std::endl;
        }
    }
    catch (std::runtime_error e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

// vim: sts=4:sw=4:ai:si:et
