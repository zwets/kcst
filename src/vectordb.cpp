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

vector_kmer_db::vector_kmer_db(int ksize)
    : kmer_db(ksize), vec_(1L<<(2*ksize))
{
}

void
vector_kmer_db::add_kmer(int kmer, int label)
{
    vec_[kmer].insert(label);
}

const std::set<int>&
vector_kmer_db::kmer_hits(int kmer) const
{
    return vec_[kmer];
}

// vim: sts=4:sw=4:ai:si:et