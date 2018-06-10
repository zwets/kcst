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


static std::vector<kloc_t> EMPTY_VECTOR;

map_kmer_db::map_kmer_db(int ksize)
    : kmer_db(ksize)
{
}

void
map_kmer_db::add_kloc(kmer_t kmer, kloc_t loc)
{
    map_[kmer].push_back(loc);
}

const std::vector<kloc_t>&
map_kmer_db::get_klocs(kmer_t kmer) const
{
    std::map<kmer_t,std::vector<kloc_t> >::const_iterator p = map_.find(kmer);
    return p == map_.end() ? EMPTY_VECTOR : p->second;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
