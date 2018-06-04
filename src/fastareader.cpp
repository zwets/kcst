/* fastareader.cpp
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
#include <stdexcept>

#include "fastareader.h"
#include "utils.h"

fasta_reader::fasta_reader(std::istream &is)
    : is_(is), lineno_(0)
{
    while (line_.empty() && std::getline(is_,line_)) lineno_++;
}

bool
fasta_reader::next_sequence(sequence &seq)
{
    if (line_.empty())
    {
        seq.id.clear();
        seq.header.clear();
        seq.data.clear();

        return false;
    }

    if (line_[0] != '>')
        raise_error("line %d: sequence header does not start with '>'", lineno_);

    seq.header = line_;

    std::string::const_iterator p = line_.begin();
    while (++p != line_.end() && !std::isspace(*p)) /* skip to space or end */;
    seq.id = std::string(line_, 1, p - line_.begin() - 1);

    seq.data.clear();

    line_.clear();

    while (std::getline(is_,line_))
    {
        lineno_++;

        if (line_.empty())
            continue;

        if (line_[0] == '>')
            break;

        seq.data.append(line_);

        line_.clear();
    }

    return true;
}

// vim: sts=4:sw=4:ai:si:et
