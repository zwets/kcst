/* kmeriser-test.cpp
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

#include <fstream>
#include <gtest/gtest.h>
#include "seqreader.h"

using namespace khc;

namespace {

static const char *fasta_fname = "data/test.fasta";
static const char *fastq_fname = "data/test.fastq";
static const char *bare_fname = "data/test.bare";

TEST(seqreader_test, read_fasta) {

    std::ifstream f;
    f.open(fasta_fname);
    sequence_reader r(f, sequence_reader::fasta);
    sequence s;

    EXPECT_TRUE(r.next(s));
    EXPECT_EQ(std::string("1"), s.id);
    EXPECT_EQ(std::string(">1 First Sequence"), s.header);
    EXPECT_EQ(std::string("ABC"), s.data);
    EXPECT_TRUE(r.next(s));
    EXPECT_EQ(std::string("2"), s.id);
    EXPECT_EQ(std::string(">2 Second Sequence"), s.header);
    EXPECT_EQ(std::string("DEF"), s.data);
    EXPECT_FALSE(r.next(s));
}

TEST(seqreader_test, read_fastq) {

    std::ifstream f;
    f.open(fastq_fname);
    sequence_reader r(f, sequence_reader::fastq);
    sequence s;

    EXPECT_TRUE(r.next(s));
    EXPECT_EQ(std::string("1"), s.id);
    EXPECT_EQ(std::string("@1 First FASTQ Stanza"), s.header);
    EXPECT_EQ(std::string("ABCABCABCABC"), s.data);
    EXPECT_TRUE(r.next(s));
    EXPECT_EQ(std::string("2"), s.id);
    EXPECT_EQ(std::string("@2 Second FASTQ Stanza"), s.header);
    EXPECT_EQ(std::string("DEFDEFDEFDEF"), s.data);
    EXPECT_FALSE(r.next(s));
}

TEST(seqreader_test, read_bare) {

    std::ifstream f;
    f.open(bare_fname);
    sequence_reader r(f, sequence_reader::bare);
    sequence s;

    EXPECT_TRUE(r.next(s));
    EXPECT_EQ(std::string("(anonymous)"), s.id);
    EXPECT_TRUE(s.header.empty());
    EXPECT_EQ(std::string("BAREData!"), s.data);
    EXPECT_FALSE(r.next(s));
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
