/* seqreader.cpp
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

#include <string>
#include <cctype>

#include "seqreader.h"
#include "utils.h"

#ifndef NO_ZLIB
#include <boost/iostreams/filter/gzip.hpp>
#endif

namespace khc {



sequence_reader::sequence_reader(std::istream &is, mode_t mode)
#ifdef NO_ZLIB
    : is_(is), lineno_(0), mode_(mode)
{
    if (is.peek() == 0x1f)
        raise_error("no decompression support");
#else
    : lineno_(0), mode_(mode)
{
    if (is.peek() == 0x1f)
    {
        verbose_emit("detected compressed input");
        is_.push(boost::iostreams::gzip_decompressor());
    }

    is_.push(is);
#endif

    if (next_line())
    {
        if (mode_ == detect)
            switch (line_[0]) {
                case '>': mode_ = fasta; break;
                case '@': mode_ = fastq; break;
                default:  mode_ = bare; break;
            }
        else if (mode == fasta && line_[0] != '>')
            raise_error("line %d: invalid FASTA, expected '>'", lineno_);
        else if (mode == fastq && line_[0] != '@')
            raise_error("line %d: invalid FASTQ, expected '@'", lineno_);
    }
}

bool
sequence_reader::next_line()
{
    line_.clear();

    while (std::getline(is_, line_))
    {
        ++lineno_;

        if (!line_.empty())
            return true;
    }

    return false;
}

bool
sequence_reader::next(sequence &seq)
{
    if (line_.empty())
        return false;

    switch (mode_) {
        case fasta: read_fasta(seq); break;
        case fastq: read_fastq(seq); break;
        case bare: read_bare(seq); break;
        default: raise_error("programmer error 42: unhandled case");
    }

    return true;
}

void
sequence_reader::read_bare(sequence &seq)
{
    static const std::string ANONYMOUS("(anonymous)");

    seq.id = ANONYMOUS;
    seq.header.clear();
    seq.data = line_;

    while (next_line()) {

        std::string::size_type pos = 0;
        while ((pos = line_.find(' ', pos)) != std::string::npos)
            line_.erase(pos, 1);

        seq.data.append(line_);
    }
}

void
sequence_reader::read_fasta(sequence &seq)
{
    seq.header = line_;

    std::string::const_iterator p = line_.begin() + 1;
    while (p != line_.end() && !std::isspace(*p))
        ++p;
    seq.id = std::string(line_, 1, p - line_.begin() - 1);

    seq.data.clear();

    while (next_line())
    {
        if (line_[0] == '>')
            break;

        seq.data.append(line_);
    }
}

void
sequence_reader::read_fastq(sequence &seq)
{
    seq.header = line_;

    std::string::const_iterator p = line_.begin() + 1;
    while (p != line_.end() && !std::isspace(*p))
        ++p;
    seq.id = std::string(line_, 1, p - line_.begin() - 1);

    if (!next_line()) 
        raise_error("line %d: invalid fastq, expected base sequence", lineno_);

    seq.data = line_;

    if (!next_line() || line_[0] != '+')
        raise_error("line %d: invalid fastq, line should start with '+'", lineno_);

    if (!next_line())
        raise_error("line %d: invalid fastq, line with phred scores expected", lineno_);

    if (next_line() && line_[0] != '@')
        raise_error("line %d: invalid fastq, header line should start with '@'", lineno_);
}


} // namespace khc

// vim: sts=4:sw=4:ai:si:et
