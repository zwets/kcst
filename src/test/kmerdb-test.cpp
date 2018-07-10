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
#include <memory>
#include <fstream>
#include "kmerdb.h"

using namespace khc;

namespace {

static const int ksize = 5;
static const int kbits = 2*ksize - 1;
static const int kmers = 1<<kbits; // 512

static const char scratch_fname[] = "testfile.tmp";

static void fill_db(kmer_db& db)
{
    db.add_kloc(42, 1);
    db.add_kloc(42, 2);
    db.add_kloc(42, 3);
    db.add_kloc(20, 4);
    db.add_kloc(0, 5);
    db.add_kloc(1, 5);
    db.add_kloc(13, 6);
    db.add_kloc((1<<5)-1, 9);
}

static bool write_db(const kmer_db* db)
{
    std::ofstream f(scratch_fname);
    return f.is_open() && db->write(f);
}

TEST(read_write_test, write_empty) {

    vector_kmer_db db(5);
    std::ofstream f(scratch_fname);

    ASSERT_TRUE(f.is_open());
    EXPECT_TRUE(db.write(f));
    f.close();
}

TEST(read_write_test, read_empty) {

    vector_kmer_db dbo(5);
    ASSERT_TRUE(write_db(&dbo));

    std::ifstream fi(scratch_fname);
    ASSERT_TRUE(fi.is_open());

    std::unique_ptr<kmer_db> dbi(kmer_db::read_db(fi));
    fi.close();

    ASSERT_NE(nullptr, dbi);
    EXPECT_EQ(5, dbi->ksize());

    for (size_t i = 0; i < (1<<5); ++i)
        EXPECT_EQ(0, dbi->get_klocs(i).size());
}

TEST(read_write_test, vector_db_rw) {

    vector_kmer_db db0(5);
    fill_db(db0);
    write_db(&db0);

    std::ifstream fi(scratch_fname);
    ASSERT_TRUE(fi.is_open());

    std::unique_ptr<kmer_db> db1(kmer_db::read_db(fi, 1, kmer_db::db_type::vector));
    fi.close();

    ASSERT_NE(nullptr, db1);
    EXPECT_EQ(5, db1->ksize());

    for (kmer_t i = 0; i < (1<<5); ++i)
    {
        std::vector<kloc_t> v0 = db0.get_klocs(i);
        std::vector<kloc_t> v1 = db1->get_klocs(i);

        EXPECT_EQ(v0.size(), v1.size());
        for (size_t j = 0; j < v0.size(); ++j)
            EXPECT_EQ(v0.at(j), v1.at(j));
    }
}

TEST(read_write_test, map_db_rw) {

    map_kmer_db db0(5);
    fill_db(db0);
    write_db(&db0);

    std::ifstream fi(scratch_fname);
    ASSERT_TRUE(fi.is_open());

    std::unique_ptr<kmer_db> db1(kmer_db::read_db(fi,1,kmer_db::db_type::map));
    fi.close();

    ASSERT_NE(nullptr, db1);
    EXPECT_EQ(5, db1->ksize());

    for (kmer_t i = 0; i < (1<<5); ++i)
    {
        std::vector<kloc_t> v0 = db0.get_klocs(i);
        std::vector<kloc_t> v1 = db1->get_klocs(i);

        EXPECT_EQ(v0.size(), v1.size());
        for (size_t j = 0; j < v0.size(); ++j)
            EXPECT_EQ(v0.at(j), v1.at(j));
    }
}

TEST(read_write_test, vec_to_map) {

    vector_kmer_db db0(5);
    fill_db(db0);
    write_db(&db0);

    std::ifstream fi(scratch_fname);
    ASSERT_TRUE(fi.is_open());
    std::unique_ptr<kmer_db> db1(kmer_db::read_db(fi,1,kmer_db::db_type::map));
    fi.close();

    ASSERT_NE(nullptr, db1);
    EXPECT_EQ(5, db1->ksize());

    for (kmer_t i = 0; i < (1<<5); ++i)
    {
        std::vector<kloc_t> v0 = db0.get_klocs(i);
        std::vector<kloc_t> v1 = db1->get_klocs(i);

        EXPECT_EQ(v0.size(), v1.size());
        for (size_t j = 0; j < v0.size(); ++j)
            EXPECT_EQ(v0.at(j), v1.at(j));
    }
}

TEST(read_write_test, map_to_vec) {

    map_kmer_db db0(5);
    fill_db(db0);
    write_db(&db0);

    std::ifstream fi(scratch_fname);
    ASSERT_TRUE(fi.is_open());

    std::unique_ptr<kmer_db> db1(kmer_db::read_db(fi,1,kmer_db::db_type::vector));
    fi.close();

    ASSERT_NE(nullptr, db1);
    EXPECT_EQ(5, db1->ksize());

    for (kmer_t i = 0; i < (1<<5); ++i)
    {
        std::vector<kloc_t> v0 = db0.get_klocs(i);
        std::vector<kloc_t> v1 = db1->get_klocs(i);

        EXPECT_EQ(v0.size(), v1.size());
        for (size_t j = 0; j < v0.size(); ++j)
            EXPECT_EQ(v0.at(j), v1.at(j));
    }
}

} // namespace
// vim: sts=4:sw=4:ai:si:et
