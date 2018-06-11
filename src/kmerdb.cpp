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

namespace kcst {


kmer_db* 
kmer_db::new_db(int ksize, int max_mem)
{
    kmer_db *res;

    int kbits = 2*ksize - 1;
    int max_kbits = 8*sizeof(kmer_t) - 1;
    int max_ksize = (max_kbits + 1)/2;

    if (kbits > max_kbits)
        raise_error("kmer size %d is larger than max supported %d"
                " (it requires %d bits but kmerdb was compiled for %d bits);"
                " either reduce kmer size, or recompile with a larger kmer_t",
                ksize, max_ksize, kbits, max_kbits);

    // Determine the implementation that fits max_mem GB

    kmer_t vec_mb = sizeof(std::vector<kloc_t>) * (static_cast<std::uintmax_t>(1) << (kbits > 20 ? kbits - 20 : 0));
    kmer_t ptr_mb = vec_mb * sizeof(std::vector<kloc_t>::size_type) / sizeof(std::vector<kloc_t>);
    kmer_t max_mb = static_cast<std::uintmax_t>(max_mem) << 10;

    if (ptr_mb > max_mb)
    {
        std::cerr << std::endl << "Vector or pointer memory (" << (ptr_mb>>10) << "G) would exceed " << (max_mb>>10) << "G: using map database" << std::endl;
        res = new map_kmer_db(ksize);
    }
    else if (vec_mb > max_mb)
    {
        std::cerr << "Vector memory (" << (vec_mb>>10) << "G) would exceed " << (max_mb>>10) << "G limit; using pointer database (" << (ptr_mb>>10) << "G)" << std::endl;
        res = new ptrvec_kmer_db(ksize);
    }
    else {
        res = new vector_kmer_db(ksize);
    }

    return res;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
