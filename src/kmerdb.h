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


// knum_t - the number type used to store an encoded kmer
//
typedef std::uint_fast64_t kmer_t;

// kloc_t - the type used to store a kmer location
//
typedef std::uint64_t kloc_t;

// kcnt_t - the type used for counting kmers in the database
// 
typedef std::uint32_t kcnt_t;


// Abstract base class holding a collection of kmer->location mappings.
//
// The two core operations are add_kloc(k,p) to assert that kmer k occurs
// at location p, and get_klocs(k) returning the list of p's where k occurs.
//
// Concrete subclasses provide different implementations, with differing
// memory and access time characteristics.  Use factory method new_db()
// to get an appropriate implementation given ksize and memory limit.
//
class kmer_db
{
    public:
        enum db_type { optimal, vector, map };

    public:
        static std::unique_ptr<kmer_db> new_db(int ksize, int mem_gb = 16, db_type = optimal);
        static std::unique_ptr<kmer_db> read_db(std::istream&, int mem_gb = 16, db_type = optimal);

    protected:
        std::vector<std::vector<kloc_t> > kloc_vecs_;
        int ksize_;

        virtual std::istream& read(std::istream&);
        
    public:
        kmer_db(int ksize) : kloc_vecs_(1), ksize_(ksize) { }
        virtual ~kmer_db() { }

        int ksize() { return ksize_; }

        virtual void add_kloc(kmer_t, kloc_t) = 0;
        virtual const std::vector<kloc_t>& get_klocs(kmer_t) const = 0;

        virtual std::ostream& write(std::ostream&) const;
};


// Concrete kmer_db that keeps the lists of klocs in a vector indexed by kmer.
//
// The big advantage of this class is its O(1) lookup time, which makes both
// storing and looking up very quick.  Its downsize is its O(4^ksize) memory
// complexity, which gets big quickly.
//
class vector_kmer_db : public kmer_db
{
    private:
        std::vector<kcnt_t> vec_ptrs_;

    protected:
        std::istream& read(std::istream&);

    public:
        vector_kmer_db(int ksize);

        virtual void add_kloc(kmer_t, kloc_t);
        virtual const std::vector<kloc_t>& get_klocs(kmer_t) const;

        virtual std::ostream& write(std::ostream&) const;
};


// Concrete kmer_db class that holds the kmer index in a map (red-black tree).
//
// In terms of storage, this is much more efficient than the vector variant,
// as it only stores kmers we actually encounter.  This number is bound by the
// number of klocs (i.e. the number of bases stored).
//
// Access time is in the order O(log N).
//
class map_kmer_db : public kmer_db
{
    private:
        std::map<kmer_t,kcnt_t> vec_ptrs_;

    protected:
        std::istream& read(std::istream&);

    public:
        map_kmer_db(int ksize);

        virtual void add_kloc(kmer_t kmer, kloc_t loc);
        virtual const std::vector<kloc_t>& get_klocs(kmer_t) const;

        virtual std::ostream& write(std::ostream&) const;
};


} // namespace khc

#endif // kmerdb_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
