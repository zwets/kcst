/* seqreader.h
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
#ifndef seqreader_h_INCLUDED
#define seqreader_h_INCLUDED

#include <iostream>
#include <string>
#include <vector>

namespace khc {


// sequence - Value object holding a single sequence
//
struct sequence {
    std::string header;  // full header of the sequence, including '>' or '@'
    std::string id;      // whatever is between '>' or '@' and the first space
    std::string data;    // the sequence data, collated into a single line
};


// sequence_reader - reads sequences off a stream
//
// This reader parses an input stream of FASTA, FASTQ, or bare sequence data,
// and returns each sequence read in reponse to calls to next().
//
// When constructed without mode setting, it autodetects the content of the
// stream in this basic way: if the first character is '>' then FASTA, else if
// it is '@' then FASTQ, otherwise it is bare sequence data.
//
// For FASTA and FASTQ the reader validates the structure of the input.  For
// each sequence it returns the full header, the sequence ID (the part of the
// header between the initial '>' or '@' and the first whitespace), and the
// collated sequence content.
//
// For bare data, all data is returned in the first call to next().  The
// sequence header is empty, and the ID is set to '(anonymous)'.
//
// The reader does not validate the content of the sequences.  It passes
// through all characters, except for whitespace which it strips in bare mode.
//
class sequence_reader {

    public:
        enum mode_t { detect, bare, fasta, fastq };

    private:
        std::istream &is_;
        std::string line_;
        int lineno_;
        mode_t mode_;

    public:
        sequence_reader(std::istream&, mode_t = detect);
        bool next(sequence&);

    protected:
        bool next_line();
        void read_bare(sequence&);
        void read_fasta(sequence&);
        void read_fastq(sequence&);
};


} // namespace khc

#endif // seqreader_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
