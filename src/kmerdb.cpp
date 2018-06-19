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


std::unique_ptr<kmer_db>
kmer_db::new_db(int ksize, int max_mem, db_type type)
{
    int kbits = 2*ksize - 1;
    int max_kbits = 8*sizeof(kmer_t) - 1;
    int max_ksize = (max_kbits + 1)/2;

    if (kbits > max_kbits)
        raise_error("kmer size %d is larger than max supported %d"
                " (it requires %d bits but kmerdb was compiled for %d bits);"
                " either reduce kmer size, or recompile with a larger kmer_t",
                ksize, max_ksize, kbits, max_kbits);

    if (type == optimal)
    {
        // Determine the implementation that probably fits max_mem GB

        kmer_t vec_mb = sizeof(std::vector<kcnt_t>) * (static_cast<std::uintmax_t>(1) << (kbits > 20 ? kbits - 20 : 0));
        kmer_t max_mb = static_cast<std::uintmax_t>(max_mem) << 10;

        if (vec_mb > max_mb)
        {
            std::cerr << std::endl << "Vector memory (" << (vec_mb>>10) << "G) would exceed " << (max_mb>>10) << "G: using map database" << std::endl;
            type = map;
        }
        else
        {
            std::cerr << std::endl << "Vector memory (" << (vec_mb>>10) << "G) fits " << (max_mb>>10) << "G: using vector database" << std::endl;
            type = vector;
        }
    }

    switch (type)
    {
        case vector: return std::make_unique<vector_kmer_db>(ksize);
        case map: return std::make_unique<map_kmer_db>(ksize);
        default: raise_error("programmer error 42: falling out of switch"); return nullptr;
    }
}

static std::string STR_MAGIC = "~kmerdb~";
static std::string STR_VERSION = "v1";
static std::string STR_KSIZE_LABEL = "ksize";

std::unique_ptr<kmer_db>
kmer_db::read_db(std::istream &is, int max_mem, db_type type)
{
    std::string name, version, ksize_label, dummy;
    int ksize;

    is >> name >> version >> ksize_label >> ksize;

    if (!(name == STR_MAGIC && version == STR_VERSION && ksize_label == STR_KSIZE_LABEL && std::getline(is,dummy)))
        raise_error("failed to read kmer_db");

    std::unique_ptr<kmer_db> res(new_db(ksize, max_mem, type));
    res->read(is);

    return res;
}

std::ostream&
kmer_db::write(std::ostream& os) const
{
    char W = ' ';
    
    os << STR_MAGIC << W << STR_VERSION << W << STR_KSIZE_LABEL << W << ksize_ << std::endl;
    
    os << kloc_vecs_.size() << std::endl;

    for (const auto& vec : kloc_vecs_)
    {
        os << vec.size() << W;
        os.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(kloc_t));
        os << std::endl;
    }

    return os;
}


std::istream&
kmer_db::read(std::istream& is)
{
    std::vector<std::vector<kloc_t> >::size_type nvecs = kloc_vecs_.size();
    is >> nvecs;
    
    kloc_vecs_.clear();
    kloc_vecs_.reserve(nvecs);

    while (nvecs--)
    {
        std::vector<kloc_t>::size_type nlocs;
        is >> nlocs;
        is.get(); // space

        kloc_t buf[nlocs];
        is.read((char*)buf, nlocs*sizeof(kloc_t));

        kloc_vecs_.push_back(std::vector<kloc_t>(buf, buf+nlocs));

        is.get(); // newline
    }
    
    return is;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
