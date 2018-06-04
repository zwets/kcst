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
#define kmerrb_h_INCLUDED

#include <set>
#include <vector>
#include <map>

class kmer_db
{
    private:
        int ksize_;

    public:
        kmer_db(int ksize) : ksize_(ksize) { }

        int ksize() const { return ksize_; }

        virtual void add_kmer(int, int) = 0;
        virtual const std::set<int>& kmer_hits(int kmer) const = 0;
};

class vector_kmer_db : public kmer_db
{
    private:
        std::vector<std::set<int> > vec_;

    public:
        vector_kmer_db(int ksize);

        void add_kmer(int kmer, int seqnum);
        const std::set<int>& kmer_hits(int kmer) const;
};

class map_kmer_db : public kmer_db
{
    private:
        std::map<int,std::set<int> > map_;

    public:
        map_kmer_db(int ksize);

        void add_kmer(int kmer, int seqnum);
        const std::set<int>& kmer_hits(int kmer) const;
};

#endif // kmerdb_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
