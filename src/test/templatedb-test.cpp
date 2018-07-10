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

static void fill_db(template_db& db)
{
}

static bool write_db(const template_db& db)
{
    std::ofstream f(scratch_fname);
    return f.is_open() && db.write(f);
}

TEST(templatedb_test, write_empty) {

    template_db db(5, 4, 16);
    std::ofstream f(scratch_fname);

    ASSERT_TRUE(f.is_open());
    EXPECT_TRUE(db.write(f));
}

TEST(templatedb_test, read_empty) {

    std::ifstream fi(infile_empty);
    ASSERT_TRUE(fi.is_open());

    template_db dbi(5, 4, 16);
    dbi.read(fi);
    fi.close();
}

TEST(templatedb_test, write_read) {

    template_db dbo(5, 4, 16);
    fill_db(dbo);
    write_db(dbo);

    std::ifstream fi(scratch_fname);
    ASSERT_TRUE(fi.is_open());

    template_db dbi(5, 4, 16);
    dbi.read(fi);
    fi.close();

    // TODO: test same was read back
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
