/* mapdb.cpp
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


void
map_kmer_db::add_kloc(kmer_t kmer, kloc_t loc)
{
    std::map<kmer_t,kcnt_t>::const_iterator p = vec_ptrs_.lower_bound(kmer);

    if (p == vec_ptrs_.end() || kmer != p->first)
    {
	kcnt_t pos = static_cast<kcnt_t>(kloc_vecs_.size());
        kloc_vecs_.push_back(std::vector<kloc_t>(1, loc));
        vec_ptrs_.insert(p, std::make_pair(kmer,pos));
    }
    else
        kloc_vecs_[p->second].push_back(loc);
}

const std::vector<kloc_t>&
map_kmer_db::get_klocs(kmer_t kmer) const
{
    std::map<kmer_t,kcnt_t>::const_iterator p = vec_ptrs_.find(kmer);
    return p == vec_ptrs_.end() ? kloc_vecs_[0] : kloc_vecs_[p->second];
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
