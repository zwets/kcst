/* kmerdb.h
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
#ifndef kmerdb_h_INCLUDED
#define kmerdb_h_INCLUDED

#include <set>
#include <vector>
#include <map>

namespace kcst {


typedef std::uint_fast64_t knum_t;  // kmer number
typedef std::uint32_t skey_t;   // sequence key

class kmer_db
{
    private:
        int ksize_;

    public:
        kmer_db(int ksize) : ksize_(ksize) { }

        int ksize() const { return ksize_; }

        virtual void add_kmer(knum_t, skey_t) = 0;
        virtual const std::vector<skey_t>& kmer_hits(knum_t knum) const = 0;
};

extern kmer_db* new_kmer_db(int ksize, int max_mem);

class vector_kmer_db : public kmer_db
{
    private:
        std::vector<std::vector<skey_t> > vec_;

    public:
        vector_kmer_db(int ksize);

        void add_kmer(knum_t knum, skey_t skey);
        const std::vector<skey_t>& kmer_hits(knum_t knum) const;
};

class map_kmer_db : public kmer_db
{
    private:
        std::map<knum_t,std::vector<skey_t> > map_;

    public:
        map_kmer_db(int ksize);

        void add_kmer(knum_t knum, skey_t skey);
        const std::vector<skey_t>& kmer_hits(knum_t knum) const;
};


} // namespace kcst

#endif // kmerdb_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
