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

namespace khc {

static std::string STR_MAGIC = "~kmerdb~";
static std::string STR_VERSION = "v1";
static std::string STR_KSIZE_LABEL = "ksize";

// constructor - note we initialise kloc_vecs_[0] as the empty vector
// 
map_kmer_db::map_kmer_db(int ksize)
    : kloc_vecs_(1), ksize_(ksize)
{
}

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

std::istream&
map_kmer_db::read(std::istream& is)
{
    std::string name, version, ksize_label, dummy;
    int ksize;

    is >> name >> version >> ksize_label >> ksize;

    if (!(name == STR_MAGIC && version == STR_VERSION && ksize_label == STR_KSIZE_LABEL && std::getline(is,dummy)))
        raise_error("failed to read kmer_db: expected %s %s %s %d",
                STR_MAGIC, STR_VERSION, STR_KSIZE_LABEL, ksize_);

    if (ksize != ksize_)
        raise_error("kmer size of database (%d) does not match expected: %d", ksize, ksize_);

    std::vector<std::vector<kloc_t> >::size_type nvecs = kloc_vecs_.size();
    is >> nvecs;

    kloc_vecs_.clear();
    kloc_vecs_.reserve(nvecs);

    while (nvecs--)
    {
        std::vector<kloc_t>::size_type nlocs;
        is >> nlocs;
        is.get(); // space

        std::vector<kloc_t> vec(nlocs);
        is.read(reinterpret_cast<char*>(&vec[0]), nlocs * sizeof(kloc_t));
        is.get(); // newline

        kloc_vecs_.push_back(vec);
    }

    char buf[sizeof(kmer_t) + sizeof(kcnt_t)];
    kmer_t* pkmer = reinterpret_cast<kmer_t*>(buf);
    kcnt_t* pkcnt = reinterpret_cast<kcnt_t*>(buf + sizeof(kmer_t));

    while (is.read(buf, sizeof(buf)))
    {
        std::pair<kmer_t,kcnt_t> p(*pkmer, *pkcnt);
        vec_ptrs_.insert(vec_ptrs_.end(), p);
    }

    return is;
}

std::ostream&
map_kmer_db::write(std::ostream& os) const
{
    static char W = ' ';
    os << STR_MAGIC << W << STR_VERSION << W << STR_KSIZE_LABEL << W << ksize_ << std::endl;
    os << kloc_vecs_.size() << std::endl;

    for (const auto& vec : kloc_vecs_)
    {
        os << vec.size() << W;
        os.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(kloc_t));
        os << std::endl;
    }

    char buf[sizeof(kmer_t) + sizeof(kcnt_t)];
    kmer_t* pkmer = reinterpret_cast<kmer_t*>(buf);
    kcnt_t* pkcnt = reinterpret_cast<kcnt_t*>(buf + sizeof(kmer_t));

    for (const auto& e : vec_ptrs_)
    {
        *pkmer = e.first;
        *pkcnt = e.second;
        os.write(buf, sizeof(buf));
    }
    
    return os;
}


} // namespace khc

// vim: sts=4:sw=4:ai:si:et
