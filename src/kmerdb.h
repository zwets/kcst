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

#include <iostream>
#include <memory>
#include <vector>
#include <map>

namespace khc {


// We define types kmer_t, kloc_t, and kcnt_t as the integral number types
// to respectively hold an encoded kmer, a kmer location, and a kmer count.

// kmer_t - the number type used to store an encoded kmer
// 
typedef std::uint_fast64_t kmer_t;

// kloc_t - the type used to store a kmer location
// 
typedef std::uint64_t kloc_t;

// kcnt_t - the type used to count kmers
//
typedef std::uint32_t kcnt_t;


// A 'kmer_db' hold a mapping from kmer_t to vector<kloc_t>, in other words,
// it stores for each kmer the list of locations where it occurs.  Its core
// operations are add_kloc(k,p) to add location p for kmer k, and get_klocs(k)
// to retrieve the list of locations p where k occurs
//
// We have two kmer_db implementations: vector_kmer_db and map_kmer_db.  The
// vector implementation is fast but memory hungry: O(1) time by O(4^ksize)
// storage.  The map db is O(log(ksize)) time by O(ksize) storage.
// 
// The two implementations have the same interface and semantics, but for
// performance reasons are not subclassed from an abstract base.


// vector_kmer_db
//
// Holds a vector indexed by kmer.  Each element points to its vector of klocs.
//
class vector_kmer_db
{
    private:
        std::vector<kcnt_t> vec_ptrs_;
        std::vector<std::vector<kloc_t> > kloc_vecs_;
        int ksize_;

    public:
        vector_kmer_db(int ksize);

        void add_kloc(kmer_t, kloc_t);
        const std::vector<kloc_t>& get_klocs(kmer_t) const;

        void clear();

        std::istream& read(std::istream&);
        std::ostream& write(std::ostream&) const;
};


// map_kmer_db
//
// Holds a map keyed by kmer.  Each value points to the vector of klocs.
//
class map_kmer_db
{
    private:
        std::map<kmer_t,kcnt_t> vec_ptrs_;
        std::vector<std::vector<kloc_t> > kloc_vecs_;
        int ksize_;

    public:
        map_kmer_db(int ksize);

        void add_kloc(kmer_t kmer, kloc_t loc);
        const std::vector<kloc_t>& get_klocs(kmer_t) const;

        void clear();

        std::istream& read(std::istream&);
        std::ostream& write(std::ostream&) const;
};


} // namespace khc

#endif // kmerdb_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
