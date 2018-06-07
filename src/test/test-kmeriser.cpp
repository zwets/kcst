/* test-kmeriser.cpp
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
#include <cstdlib>

#include "kmeriser.h"

int main(int argc, char* argv[])
{
    char sample[] = "acgtacgtacgtacgt";
    const char *s = sample;
    int k = 5;

    if (argc > 2)
    {
        k = std::atoi(argv[1]);
        s = argv[2];
    }
    else if (argc > 1)
    {
        s = argv[1];
    }

    try {
        int l = strlen(s);
        const char *e = s + l;

        if (k < 0 || k > l)
        {
            k = l/2 + 1;
        }

        kmeriser r(s, e, k);

        std::cout << "string: " << s << std::endl;
        std::cout << "kmers:";

        do {
            std::cout << ' ' << r.val();
        } while (r.inc());
            
        std::cout << std::endl;
    }
    catch (std::runtime_error e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

// vim: sts=4:sw=4:ai:si:et
