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

#include <gtest/gtest.h>
#include "kmerdb.h"

using namespace kcst;

namespace {

static const int ksize = 5;
static const int kbits = 2*ksize - 1;
static const int kmers = 1<<kbits; // 512

TEST(mapdb_test, empty_db) {
    map_kmer_db db;

    for (int i=0; i < kmers; ++i) {
        EXPECT_TRUE(db.get_klocs(i).empty());
    }
}

TEST(mapdb_test, single_at_start) {
    map_kmer_db db;

    db.add_kloc(0,42);
    ASSERT_EQ(1, db.get_klocs(0).size());
    EXPECT_EQ(42, db.get_klocs(0)[0]);
}

TEST(mapdb_test, multi_at_start) {
    map_kmer_db db;

    db.add_kloc(0,42);
    db.add_kloc(0,99);
    ASSERT_EQ(2, db.get_klocs(0).size());
    EXPECT_EQ(42, db.get_klocs(0)[0]);
    EXPECT_EQ(99, db.get_klocs(0)[1]);
}

TEST(mapdb_test, single_at_end) {
    map_kmer_db db;

    db.add_kloc(kmers-1,42);
    ASSERT_EQ(1, db.get_klocs(kmers-1).size());
    EXPECT_EQ(42, db.get_klocs(kmers-1)[0]);
}

TEST(mapdb_test, multi_at_end) {
    map_kmer_db db;

    db.add_kloc(kmers-1,42);
    db.add_kloc(kmers-1,99);
    ASSERT_EQ(2, db.get_klocs(kmers-1).size());
    EXPECT_EQ(42, db.get_klocs(kmers-1)[0]);
    EXPECT_EQ(99, db.get_klocs(kmers-1)[1]);
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
