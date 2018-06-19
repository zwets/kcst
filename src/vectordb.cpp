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
    : kmer_db(ksize), vec_ptrs_(1L<<(2*ksize-1), 0)
{
}

void
vector_kmer_db::add_kloc(kmer_t kmer, kloc_t loc)
{
    kcnt_t pos = vec_ptrs_[kmer];

    if (!pos)
    {
        pos = static_cast<kcnt_t>(kloc_vecs_.size());
        kloc_vecs_.push_back(std::vector<kloc_t>(1, loc));
        vec_ptrs_[kmer] = pos;
    }
    else
        kloc_vecs_[pos].push_back(loc);
}

const std::vector<kloc_t>&
vector_kmer_db::get_klocs(kmer_t kmer) const
{
    return kloc_vecs_[vec_ptrs_[kmer]];
}

std::istream&
vector_kmer_db::read(std::istream& is)
{
    kmer_db::read(is);

    char buf[sizeof(kmer_t) + sizeof(kcnt_t)];
    kmer_t* pkmer = reinterpret_cast<kmer_t*>(buf);
    kcnt_t* pkcnt = reinterpret_cast<kcnt_t*>(buf + sizeof(kmer_t));

    while (is.read(buf, sizeof(buf)))
        vec_ptrs_[*pkmer] = *pkcnt;

    return is;
}

std::ostream&
vector_kmer_db::write(std::ostream& os) const
{
    kmer_db::write(os);

    char buf[sizeof(kmer_t) + sizeof(kcnt_t)];
    kmer_t* pkmer = reinterpret_cast<kmer_t*>(buf);
    kcnt_t* pkcnt = reinterpret_cast<kcnt_t*>(buf + sizeof(kmer_t));

    std::vector<kcnt_t>::const_iterator p = vec_ptrs_.begin();
    while (p != vec_ptrs_.end())
    {
        if (*p)
        {
            *pkmer = p - vec_ptrs_.begin();
            *pkcnt = *p;
            os.write(buf, sizeof(buf));
        }
        ++p;
    }

    return os;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
