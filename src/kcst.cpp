/* kcst.cpp - kmer-counting sequence typer
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
#include <string>
#include <cstring>
#include <cstdlib>

#include "fastareader.h"
#include "kmeriser.h"
#include "utils.h"

static const int MIN_KSIZE = 1;
static const int MAX_KSIZE = 4 * sizeof(int) - 1;
static const int DEFAULT_KSIZE = 10;

static const std::string USAGE("\n"
"Usage: kcst [-k KSIZE]\n"
"\n"
"  Read FASTA on stdin, write to stdout knums.\n"
"\n");

static int ksize = DEFAULT_KSIZE;

int main (int argc, char *argv[]) 
{
    try {
        while (*++argv) 
        {
            if (!std::strcmp("-k", *argv) && *++argv)
            {
                ksize = std::atoi(*argv);
                if (ksize < MIN_KSIZE || ksize > MAX_KSIZE) 
                    raise_error("invalid KSIZE: %d", ksize);
            }
            else {
                std::cerr << USAGE;
                return 1;
            }
        }

        fasta_reader reader(std::cin);
        sequence seq;

        while (reader.next_sequence(seq))
        {
            std::cout << seq.header << std::endl;
            kmerator k(seq.data.c_str(), seq.data.c_str() + seq.data.length(), ksize);
            std::cout << k.val();
            while (k.inc())
                std::cout << ' ' << k.val();
            std::cout << std::endl;
        }

        return 0;
    }
    catch (std::runtime_error e) {
        std::cerr << "kcst: " << e.what() << std::endl;
        return 1;
    }
}

// vim: sts=4:sw=4:et:si:ai
