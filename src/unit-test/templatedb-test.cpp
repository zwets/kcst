/* templatedb-test.cpp
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
#include <fstream>
#include <memory>
#include "templatedb.h"

using namespace khc;

namespace {

static const int ksize = 5;
static const int kbits = 2*ksize - 1;
static const int kmers = 1<<kbits; // 512

static const char infile_empty[] = "data/test.empty";
static const char infile_fasta[] = "data/test.templates";
static const char infile_binary[] = "data/test.templates.bin";
static const char scratch_fname[] = "data/test.templates.tmp";

TEST(templatedb_test, read_empty) {

    std::ifstream fi(infile_empty);
    ASSERT_TRUE(fi.is_open());

    std::unique_ptr<template_db> dbi = template_db::read(fi, 0, 5, 64);
    fi.close();
}

TEST(templatedb_test, write_empty) {

    std::ifstream fi(infile_empty);
    ASSERT_TRUE(fi.is_open());
    std::unique_ptr<template_db> db = template_db::read(fi, 0, 5, 64);

    std::ofstream f(scratch_fname);
    ASSERT_TRUE(f.is_open());
    EXPECT_TRUE(db->write(f));
}

TEST(templatedb_test, read_fasta) {

    std::ifstream fi(infile_fasta);
    ASSERT_TRUE(fi.is_open());
    std::unique_ptr<template_db> db = template_db::read(fi, 0, 5, 64);
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
