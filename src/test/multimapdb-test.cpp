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

using namespace khc;

namespace {

TEST(multimapdb_test, empty_db) {
    multimap_kmer_db db;

    multimap_kmer_db::iter_pair pp = db.get_klocs();
    EXPECT_EQ(pp.first, pp.second);
}

TEST(multimapdb_test, single_at_start) {
    multimap_kmer_db db;

    db.add_kloc(0,42);

    multimap_kmer_db::iter_pair pp = db.get_klocs(0);
    multimap_kmer_db::iter p0 = pp.first;
    multimap_kmer_db::iter p1 = pp.second;

    ASSERT_NE(p0,p1);
    EXPECT_EQ(0, p0->first);
    EXPECT_EQ(42, p0->second);
}

TEST(multimapdb_test, get_multi) {
    multimap_kmer_db db;

    db.add_kloc(5,55);
    db.add_kloc(6,66);
    db.add_kloc(6,666);
    db.add_kloc(9,99);
    
    multimap_kmer_db::iter_pair pp = db.get_klocs(6);
    multimap_kmer_db::iter p0 = pp.first;
    multimap_kmer_db::iter p1 = pp.second;

    ASSERT_NE(p0,p1);
    EXPECT_EQ(6, p0->first);
    EXPECT_EQ(66, p0++->second);
    ASSERT_NE(p0,p1);
    EXPECT_EQ(6, p0->first);
    EXPECT_EQ(666, p0++->second);
    EXPECT_EQ(p0,p1);
}

TEST(multimapdb_test, get_all) {
    multimap_kmer_db db;

    db.add_kloc(5,55);
    db.add_kloc(6,66);
    db.add_kloc(6,666);
    db.add_kloc(9,99);
    
    multimap_kmer_db::iter_pair pp = db.get_klocs();
    multimap_kmer_db::iter p0 = pp.first;
    multimap_kmer_db::iter p1 = pp.second;

    ASSERT_NE(p0,p1);
    EXPECT_EQ(5, p0->first);
    EXPECT_EQ(55, p0++->second);
    ASSERT_NE(p0,p1);
    EXPECT_EQ(6, p0->first);
    EXPECT_EQ(66, p0++->second);
    ASSERT_NE(p0,p1);
    EXPECT_EQ(6, p0->first);
    EXPECT_EQ(666, p0++->second);
    ASSERT_NE(p0,p1);
    EXPECT_EQ(9, p0->first);
    EXPECT_EQ(99, p0++->second);
    EXPECT_EQ(p0,p1);
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
