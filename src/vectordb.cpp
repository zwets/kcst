/* vectordb.cpp
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

#include "kmerdb.h"
#include "utils.h"

namespace kcst {


vector_kmer_db::vector_kmer_db(int ksize)
    : kmer_db(ksize), klocs_(1L<<(2*ksize-1))
{
}

void
vector_kmer_db::add_kloc(kmer_t kmer, kloc_t loc)
{
    klocs_[kmer].push_back(loc);
}

const std::vector<kloc_t>&
vector_kmer_db::get_klocs(kmer_t kmer) const
{
    return klocs_[kmer];
}


ptrvec_kmer_db::ptrvec_kmer_db(int ksize)
    : kmer_db(ksize), klocs_ptrs_(1L<<(2*ksize-1), 0), klocs_vecs_(1 /* note we put one at pos 0 */ )
{
}

void
ptrvec_kmer_db::add_kloc(kmer_t kmer, kloc_t loc)
{
    std::vector<kloc_t>::size_type pos = klocs_ptrs_[kmer];

    if (!pos)
    {
        pos = klocs_vecs_.size();
        klocs_ptrs_[kmer] = pos;
        klocs_vecs_.push_back(std::vector<kloc_t>(1, loc));
    }
    else
        klocs_vecs_[pos].push_back(loc);
}

const std::vector<kloc_t>&
ptrvec_kmer_db::get_klocs(kmer_t kmer) const
{
    return klocs_vecs_[klocs_ptrs_[kmer]];
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
