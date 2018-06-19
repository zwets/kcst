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
static const std::string KSIZE_LABEL("ksize");

void
template_db::clear()
{
    kmer_db_ = nullptr;

    seq_ids_.clear();
    seq_lens_.clear();
}

std::istream&
template_db::read_binary(std::istream& is)
{
    std::cerr << "reading binary template database ... ";

    this->clear();

    std::string magic, nseq_label, ksize_label, dummy;
    nseq_t nseq;
    int ksize;

    is >> magic >> nseq_label >> nseq >> ksize_label >> ksize;
    getline(is, dummy); // consume newline

    if (magic != MAGIC || nseq_label != NSEQ_LABEL || ksize_label != KSIZE_LABEL)
        raise_error("not a valid binary template file: expected header '%s %s [0-9]+ %s [0-9]+'", MAGIC, NSEQ_LABEL, KSIZE_LABEL);

    if (ksize != ksize_)
        raise_error("binary template file kmer size (%d) mismatches kmer size %d", ksize, ksize_);

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

    kmer_db_ = kmer_db::read_db(is, max_mem_);

    if (kmer_db_->ksize() != ksize_)
        raise_error("programmer error 42: kmer_db kmer size %d different from khc kmer size %d", kmer_db_->ksize(), ksize_);

    std::cerr << "OK" << std::endl;

    return is;
}

std::istream&
template_db::read_fasta(std::istream& is)
{
    std::cerr << "reading FASTA template database ... ";

    this->clear();

    kmer_db_ = kmer_db::new_db(ksize_, max_mem_);

    sequence_reader reader(is, sequence_reader::fasta);
    kmerator k(ksize_, max_variants_);

    sequence seq;
    nseq_t seq_cnt = 0;

    while (reader.next(seq))
    {
        seq_ids_.push_back(seq.id);
        seq_lens_.push_back(seq.data.length() - ksize_ + 1);

        k.set(seq.data.c_str(), seq.data.c_str() + seq.data.length());

        kloc_t loc = seq_cnt++;
        loc = (loc << 32) - 1;

        do {
            if (!k.variant())
                ++loc;
            kmer_db_->add_kloc(k.knum(), loc);
        } while (k.inc());
    }

    std::cerr << "OK" << std::endl;

    return is;
}

void 
template_db::read(const std::string& filename)
{
    std::ifstream db_file;
    std::istream *is = &std::cin;

    if (filename != "-")
    {
        db_file.open(filename.c_str());

        if (!db_file)
            raise_error("cannot open file: %s", filename.c_str());

        is = &db_file;
    }

    if (is->peek() == '~')
        read_binary(*is);
    else
        read_fasta(*is);
}

std::ostream&
template_db::write(std::ostream& os)
{
    static const char W = ' ';
    os << MAGIC << W << NSEQ_LABEL << W << seq_ids_.size() << W << KSIZE_LABEL << W << ksize_ << std::endl;

    for (size_t i = 0; i != seq_ids_.size(); ++i)
        os << seq_ids_[i] << W << seq_lens_[i] /* << seq_hdrs_[i] */ << std::endl;

    if (kmer_db_)
        kmer_db_->write(os);

    return os;
}

bool
template_db::write(const std::string& filename)
{
    std::ofstream os(filename.c_str());

    if (!os)
        return false;

    write(os);

    return (bool)os;
}

query_result
template_db::query(const std::string& filename, double min_cov_pct) const
{
    std::ifstream qry_file;
    std::istream* is = &std::cin;

    if (!filename.empty() && filename != "-")
    {
        qry_file.open(filename.c_str());

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
    kmeriser k(ksize_);
    sequence seq;

    while (qry_reader.next(seq))
    {
	k.set(seq.data.c_str(), seq.data.c_str() + seq.data.length());

	do
            for (const kloc_t& loc : kmer_db_->get_klocs(k.knum()))
            {
                nseq_t sid = loc >> 32;
                npos_t pos = loc & 0xFFFFFFFF;
                
                targets[sid][pos] = '\1';
            }
        while (k.inc());
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
        if (min_cov_pct < phit)
            res.push_back({seq_ids_[i], seq_lens_[i], hits, phit});
    }

    return res;
}


} // namespace khc

// vim: sts=4:sw=4:ai:si:et
