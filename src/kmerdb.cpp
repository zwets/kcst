/* kmerdb.cpp
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

#include "kmerdb.h"
#include "utils.h"

kmer_db* new_kmer_db(int ksize, int max_mem)
{
    kmer_db *res;

    int kmer_bits = 2*ksize;

    if (kmer_bits > 8*sizeof(knum) - 1)
        raise_error("kmer size %d not supported; recompile with knum defined as 'long long' to support kmer sizes up to %d",
                ksize, 4*sizeof(long long)-1);

    // Vector memory use in bytes is number of distinct kmers times size of an entry.
    // Distinct kmers are 4^ksize or 2^(2*ksize) or 1<<(2*ksize) counted in units.
    // Counted in M, that's 1<<(2*ksize-20) is 1<<(2*(ksize-10))

    long long vec_mb = sizeof(std::vector<skey>) * (1L<<(2*(ksize < 11 ? 0 : ksize - 10)));
    long long vec_gb = vec_mb >> 10;

    if (vec_gb > max_mem)
    {
        std::cerr << "memory use " << vec_gb << "G greater than " << max_mem << "G: using map database";
        res = new map_kmer_db(ksize);
    }
    else
    {
        res = new vector_kmer_db(ksize);
    }

    return res;
}

// vim: sts=4:sw=4:ai:si:et
