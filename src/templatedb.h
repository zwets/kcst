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


// nseq_t - number type to hold sequence count
//
typedef std::uint32_t nseq_t;

// npos_t - number type to hold position within sequence
//
typedef std::uint32_t npos_t;


// query_result - return value of template_db::query()
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
//
// It can be queried with a collection of kmers from a query sequence, and
// reports the hit counts on each template sequence.
//
// A template_db is read either from FASTA, or from an optimised binary file
// which it has previously written.  Its read method can autodetects the format.
//
// template_db uses a kmer_db to manage the kmer-klocs mapping, and a list of
// sequence ID and length of each sequence.
//
class template_db
{
    private:
        int ksize_;
        int max_mem_;       // GB, determines the type of kmer_db
        int max_variants_;  // per kmer, when there are degenerate bases
        std::unique_ptr<kmer_db> kmer_db_;
        std::vector<std::string> seq_ids_;
        std::vector<kcnt_t> seq_lens_;

        void clear();

    public:
        template_db(int ksize, int max_mem, int max_variants) 
            : ksize_(ksize), max_mem_(max_mem), max_variants_(max_variants) { }

        std::istream& read_binary(std::istream&);
        std::istream& read_fasta(std::istream&);
        void read(const std::string&);

        std::ostream& write(std::ostream&);
        bool write(const std::string&);

        query_result query(const std::string&, double min_cov_pct = 1.0) const;
};


} // namespace khc

#endif // templatedb_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
