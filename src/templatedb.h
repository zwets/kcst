/* templatedb.h
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
#ifndef templatedb_h_INCLUDED
#define templatedb_h_INCLUDED

#include <iostream>
#include <memory>
#include <vector>
#include "seqreader.h"
#include "kmerdb.h"

namespace khc {

// This header defines class template_db, the object that reads the template
// sequences, and can then be 'kmer-blasted' with query sequences to obtain
// seq_hits per template sequence.

// The template_db uses a kmer_db to hold the kmer->locations mapping, and
// holds for each sequence its header (ID) and length.  When queried with a
// query, it tracks not only how many times each template sequence was hit,
// but also _where_ it was hit, so we obtain precise base coverage statistics.

// Each kmer location is a pair <seq,pos>, where seq indexes into the list
// of sequences, and pos is the base position on that sequence.  As kmer_db
// holds kmer locations as 'opaque' 64-bit numbers (kloc_t), the obvious
// encoding is ((seq << 32) | pos), where seq and pos are 32-bit uints.

// nseq_t - number type to hold sequence number (index into vector)
//
typedef std::uint32_t nseq_t;

// npos_t - number type to hold position within sequence
//
typedef std::uint32_t npos_t;


// query_result - vector of seq_hits, return value of template_db::query()
//
struct seq_hits
{
    std::string seqid;  // the part between '>' and first space in FASTA header
    npos_t len;         // number of kmers in sequence (bases - ksize + 1)
    npos_t hits;        // number of kmers hit in sequence
    double phit;        // percentage hits / len
};

typedef std::vector<seq_hits> query_result;


// template_db - holds the template sequences against which to run queries

// This superclass defines the abstract interface for the two implementations
// (vector versus map based, see kmer_db), and the factory methods to read a
// template_db from either a FASTA file or an optimised binary file that it
// has previously written.

// Its main interface function is query, which takes a filename or "-" for
// stdin, and returns the list of sequences hit by the kmers in the file.

class template_db
{
    protected:
        std::vector<std::string> seq_ids_;
        std::vector<kcnt_t> seq_lens_;

        static std::unique_ptr<template_db> create_db(int ksize, int max_gb = 0);

        virtual int ksize() const = 0;
        virtual std::istream& read_binary(std::istream&, nseq_t nseq) = 0;
        virtual std::istream& read_fasta(std::istream&, int max_vars) = 0;
        virtual void write_kmer_db(std::ostream&) const = 0;

    public:
        static std::unique_ptr<template_db> read(std::istream&, int max_gb = 0, int ksize = 0, int max_vars = 0);

    public:
        virtual query_result query(const std::string&, double min_cov_pct = 1.0) const = 0;

        std::ostream& write(std::ostream&) const;
        bool write(const std::string&) const;
};

template <typename kmer_db_t>
class template_db_impl : public template_db
{
    private:
        kmer_db_t kmer_db_;

    protected:
        virtual int ksize() const { return kmer_db_.ksize(); }
        virtual std::istream& read_binary(std::istream&, nseq_t nseq);
        virtual std::istream& read_fasta(std::istream&, int max_vars);
        virtual void write_kmer_db(std::ostream& os) const { kmer_db_.write(os); }

    public:
        template_db_impl(int ksize) : kmer_db_(ksize) { }
        virtual query_result query(const std::string&, double min_cov_pct = 1.0) const;
};


} // namespace khc

#endif // templatedb_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
