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
#include "kmeriser.h"

namespace {

const char  LOWER_LETTERS[] = "acgtwsmkrybdhvn";
const int   A_VAL = 0, C_VAL = 1, G_VAL = 2, T_VAL = 3;

TEST(kmeriser_test, no_ksize_zero) {
    kmeriser *r = 0;
    EXPECT_THROW(r = new kmeriser(0), std::runtime_error);
    delete r;
}

TEST(kmeriser_test, no_ksize_too_big) {
    kmeriser *r = 0;
    EXPECT_THROW(r = new kmeriser(MAX_KSIZE+1), std::runtime_error);
    delete r;
}

TEST(kmeriser_test, no_ksize_even) {
    kmeriser *r = 0;
    EXPECT_THROW(r = new kmeriser(6), std::runtime_error);
    delete r;
}

}
// vim: sts=4:sw=4:ai:si:et
// vim: sts=4:sw=4:ai:si:et
