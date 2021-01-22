/* templatedb.cpp
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

#include "templatedb.h"

#include <fstream>

#include "kmerise.h"
#include "utils.h"

namespace khc {

static const std::string MAGIC("~khc~");
static const std::string NSEQ_LABEL("nseq");
static const std::string NBASES_LABEL("nbases");
static const std::string KSIZE_LABEL("ksize");
static const std::string MAXVARS_LABEL("maxvars");


std::unique_ptr<template_db>
template_db::create_db(int ksize, int max_vars, int max_gb)
{
    template_db *ret;

    int kbits = 2*ksize - 1;
    int max_kbits = 8*sizeof(kmer_t) - 1;
    int max_ksize = (max_kbits + 1)/2;

    if (kbits > max_kbits)
        raise_error("kmer size %d is larger than max supported %d"
                " (it requires %d bits but kmerdb was compiled for %d bits);"
                " either reduce kmer size, or recompile with a larger kmer_t",
                ksize, max_ksize, kbits, max_kbits);

        // Note std::vector<kcnt_t> is the element type of the large k-mer lookup vector.
        // It has 24-byte size, so at k-size 15 we are dealing with 24 * 2^29 = 12GB.
        // We could reduce memory consumption by introducing an indirection.
        // Either factor 3 (if we use 64-bit 'pointers'), or 6 (at 32-bit pointers, but
        // restricting k-mer variety to 4G distinct k-mers).  However, even if this
        // brings memory at k=15 down to 2G, at k-size 17 it is already back at 32G,
        // while introducing complexity and the slight indirection performance hit.

    kmer_t vec_mb = sizeof(std::vector<kcnt_t>) * (static_cast<std::uintmax_t>(1) << (kbits > 20 ? kbits - 20 : 0));
    kmer_t max_mb = static_cast<std::uintmax_t>(max_gb) << 10;

    if (max_gb == 0)
    {
        unsigned long phy_mb = get_system_memory() >> 20;
        max_mb = phy_mb > 2048 ? phy_mb - 2048 : phy_mb;

        verbose_emit("defaulting max memory to all%s physical memory: %luG",
                phy_mb > 2048 ? " but 2G" : "",
                static_cast<unsigned long>(max_mb >> 10));
    }

    if (vec_mb > max_mb)
    {
        verbose_emit("vector memory (%luG) would exceed %luG: creating map database",
                static_cast<unsigned long>(vec_mb >> 10),
                static_cast<unsigned long>(max_mb >> 10));

        ret = new template_db_impl<map_kmer_db>(ksize, max_vars);
    }
    else
    {
        verbose_emit("vector memory (%luG) fits %luG: creating vector database",
                static_cast<unsigned long>(vec_mb >> 10),
                static_cast<unsigned long>(max_mb >> 10));

        ret = new template_db_impl<vector_kmer_db>(ksize, max_vars);
    }
 
    return std::unique_ptr<template_db>(ret);
}

std::unique_ptr<template_db>
template_db::read(std::istream& is, int max_gb, int ksize, int max_vars)
{
    std::unique_ptr<template_db> ret;

    if (is.peek() == '~')
    {
        verbose_emit("reading binary template database");

        std::string magic, nseq_label, nbases_label, ksize_label, maxvars_label, dummy;
        nseq_t nseq;
        kloc_t nbases;
        int db_ksize;
        int db_max_vars;

        is >> magic >> nseq_label >> nseq >> nbases_label >> nbases >> ksize_label >> db_ksize >> maxvars_label >> db_max_vars;
        getline(is, dummy); // consume newline

        if (magic != MAGIC || nseq_label != NSEQ_LABEL || nbases_label != NBASES_LABEL || ksize_label != KSIZE_LABEL || maxvars_label != MAXVARS_LABEL)
            raise_error("not a valid binary template file: expected header '%s %s [0-9]+ %s [0-9]+ %s [0-9]+ %s [0-9]+'",
                    MAGIC.c_str(), NSEQ_LABEL.c_str(), NBASES_LABEL.c_str(), KSIZE_LABEL.c_str(), MAXVARS_LABEL.c_str());

        if (ksize != 0 && ksize != db_ksize)
            raise_error("specified ksize %d mismatches binary template file ksize: %d",
                    ksize, db_ksize);

        if (max_vars != 0 && max_vars != db_max_vars)
            raise_error("specified max variants %d mismatches value in binary template file: %d",
                    max_vars, db_max_vars);

        ret = create_db(db_ksize, db_max_vars, max_gb);
        ret->read_binary(is, nseq);
    }
    else
    {
        verbose_emit("reading FASTA template database");

        if (ksize == 0)
            raise_error("ksize must be specified");

        if (max_vars == 0)
            raise_error("max variants must be specified");

        ret = create_db(ksize, max_vars, max_gb);
        ret->read_fasta(is);
    }

    return ret;
}


std::ostream&
template_db::write(std::ostream& os) const
{
    static const char W = ' ';

    kloc_t nbases = 0;
    for (auto n : seq_lens_)
        nbases += n;

    os << MAGIC << W << 
        NSEQ_LABEL << W << seq_ids_.size() << W << 
        NBASES_LABEL << W << nbases << W << 
        KSIZE_LABEL << W << ksize() << W <<
        MAXVARS_LABEL << W << max_vars() << std::endl;

    for (nseq_t i = 0; i != seq_ids_.size(); ++i)
        os << seq_ids_[i] << W << seq_lens_[i] /* << seq_hdrs_[i] */ << std::endl;

    write_kmer_db(os);

    return os;
}


bool
template_db::write(const std::string& filename) const
{
    std::ofstream os(filename.c_str(), std::ios_base::out|std::ios_base::binary);

    if (!os)
        return false;

    write(os);

    return (bool)os;
}

template<typename kmer_db_t>
query_result
template_db_impl<kmer_db_t>::query(const std::string& filename, double min_cov_pct, bool skip_degens) const
{
    std::istream* is = &std::cin;
    std::ifstream qry_file;

    if (!filename.empty() && filename != "-")
    {
        qry_file.open(filename.c_str(), std::ios_base::in|std::ios_base::binary);

        if (!qry_file)
            raise_error("failed to open query file: %s", filename.c_str());

        is = &qry_file;
    }

    // set up collector vectors

    std::vector<std::vector<char> > targets;
    targets.reserve(seq_lens_.size());

    for (const auto& len : seq_lens_)
        targets.push_back(std::vector<char>(len, '\0'));

    // collect the targets hits by the query

    sequence_reader qry_reader(*is);
    kmeriser k(kmer_db_.ksize(), skip_degens);
    sequence seq;

    while (qry_reader.next(seq))
    {
        k.set(seq.data.c_str(), seq.data.c_str() + seq.data.length());

        while (k.next())
        {
            for (const kloc_t& loc : kmer_db_.get_klocs(k.knum()))
            {
                nseq_t sid = loc >> 32;
                npos_t pos = loc & 0xFFFFFFFF;

                targets[sid][pos] = '\1';
            }
        }
    }

    if (is != &std::cin)
        qry_file.close();

    // produce the result

    query_result res;

    for (size_t i = 0; i != targets.size(); ++i)
    {
        npos_t hits = 0;
        npos_t len = seq_lens_[i];

        for (const char& c : targets[i]) if (c) ++hits;

        double phit = 100.0 * (double)hits / (double)len;
        if (min_cov_pct <= phit)
            res.push_back({seq_ids_[i], len, hits, phit});
    }

    return res;
}

template<typename kmer_db_t>
std::istream& 
template_db_impl<kmer_db_t>::read_binary(std::istream& is, nseq_t nseq)
{
    seq_ids_.reserve(nseq);
    seq_lens_.reserve(nseq);

    std::string seq_id;
    npos_t seq_len = 0;
    std::string seq_opt_hdr;

    while (nseq-- && is)
    {
        is >> seq_id >> seq_len;
        getline(is, seq_opt_hdr); // consume newline, seq headers ignored for now

        seq_ids_.push_back(seq_id);
        seq_lens_.push_back(seq_len);
    }

    if (!is)
        raise_error("failed to read sequence section from binary template file");

    if (seq_ids_.size() != 0)
        kmer_db_.read(is);

    return is;
}

template<typename kmer_db_t>
std::istream&
template_db_impl<kmer_db_t>::read_fasta(std::istream& is)
{
    sequence_reader reader(is, sequence_reader::fasta);
    kmerator k(kmer_db_.ksize(), max_vars_);

    sequence seq;
    nseq_t seq_cnt = 0;
    int ksize = kmer_db_.ksize();

    while (reader.next(seq))
    {
        seq_ids_.push_back(seq.id);
        seq_lens_.push_back(seq.data.length() - ksize + 1);

        k.set(seq.data.c_str(), seq.data.c_str() + seq.data.length());

        kloc_t loc = seq_cnt++;
        loc = (loc << 32) - 1;

        while (k.next())
        {
            if (!k.variant())
                ++loc;
            kmer_db_.add_kloc(k.knum(), loc);
        }
    }

    return is;
}


} // namespace khc

// vim: sts=4:sw=4:ai:si:et
