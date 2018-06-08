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

static std::vector<skey_t> EMPTY_VECTOR;

map_kmer_db::map_kmer_db(int ksize)
    : kmer_db(ksize)
{
}

void
map_kmer_db::add_kmer(knum_t knum, skey_t skey)
{
    map_[knum].push_back(skey);
}

const std::vector<skey_t>&
map_kmer_db::kmer_hits(knum_t knum) const
{
    std::map<knum_t,std::vector<skey_t> >::const_iterator p = map_.find(knum);
    return p == map_.end() ? EMPTY_VECTOR : p->second;
}

// vim: sts=4:sw=4:ai:si:et
