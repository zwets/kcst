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
    char sample[] = "acwt";
    const char *s = sample;
    int k = 3;

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
        int k = std::strlen(s);
        if (k > 2)
            k = k - 2 | 1;
        else
            k = 1;

        kmerator r(k, 0);
        r.set(s, s + std::strlen(s));

        std::cout << "string: " << s << std::endl;
        std::cout << "k-size: " << k << std::endl;
        std::cout << "kmers:";

        do {
            std::cout << ' ' << r.get();
        } while (r.inc());
        
        std::cout << std::endl;
    }
    catch (std::runtime_error e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

// vim: sts=4:sw=4:ai:si:et
