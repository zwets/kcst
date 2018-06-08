/* fastareader.h
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
#ifndef fastareader_h_INCLUDED
#define fastareader_h_INCLUDED

#include <iostream>
#include <string>
#include <vector>

namespace kcst {


// sequence - Value object holding a single sequence
//
struct sequence {
    std::string id;         // whatever is between '>' and the first space
    std::string header;     // the full header (defline) of the sequence
    std::string data;       // the sequence data
};


// fasta_reader - reads sequences off a stream
//
// This reader validates the structure of the input, but not the content of
// the sequences.  It passes through all characters, except whitespace which
// it strips.
//
// The reader is lenient in one special case: if the input consists of only
// sequence data (with no header lines before or after it), then this is
// accepted as an 'anonymous' sequence.
//
class fasta_reader {

    private:
        std::istream &is_;
        std::string line_;
        int lineno_;

    public:
        fasta_reader(std::istream&);
        bool next_sequence(sequence&);
};


} // namespace kcst

#endif // fastareader_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
