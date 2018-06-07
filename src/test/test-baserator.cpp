/* test-baserator.cpp
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
#include <stdexcept>
#include <cstring>

#include "kmeriser.h"
#include "utils.h"

static const char DEFAULT_SEQ[] = "abcdghkmnrstvw";

int main(int argc, char *argv[]) 
{
    try {
        const char  *seq = argc == 2 ? argv[1] : DEFAULT_SEQ,
                    *end = seq + strlen(seq);

        for(;seq != end;++seq)
        {
            baserator b(*seq);

            std::cout << "input: " << *seq << "  mult: " << b.len() << std::endl;

            for (int j = 0; j<b.len(); ++j) {
                std::cout << "  round: " << j;       // << std::endl;
                std::cout << "  value: " << b.val(); // << std::endl;
                std::cout << "  inc: " << b.inc(); // << std::endl;
                std::cout << std::endl;
            }
        }

    } catch (std::runtime_error e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cerr << "Usage: test-baserator [SEQUENCE]" << std::endl;
    }
}

// vim: sts=4:sw=4:ai:si:et
