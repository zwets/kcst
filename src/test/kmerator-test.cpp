/* kmerator-test.cpp
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

#include <stdexcept>
#include <list>
#include <gtest/gtest.h>
#include "kmerise.h"

using namespace kcst;

namespace {

const char  LOWER_LETTERS[] = "acgtwsmkrybdhvn";
const char  UPPER_LETTERS[] = "ACGTWSMKRYBDHVN";
const int   A_VAL = 0, C_VAL = 1, G_VAL = 2, T_VAL = 3;

TEST(kmerator_test, no_ksize_zero) {
    kmerator *r = 0;
    EXPECT_THROW(r = new kmerator(0), std::runtime_error);
    delete r;
}

TEST(kmerator_test, no_ksize_too_big) {
    kmerator *r = 0;
    EXPECT_THROW(r = new kmerator(kmerator::max_ksize+1), std::runtime_error);
    delete r;
}

TEST(kmerator_test, no_ksize_even) {
    kmerator *r = 0;
    EXPECT_THROW(r = new kmerator(6), std::runtime_error);
    delete r;
}

TEST(kmerator_test, ksize_1) {
    kmerator r(1);
    char seq[] = "a";
    r.set(seq, seq+strlen(seq));
    EXPECT_EQ(A_VAL,r.knum());
    EXPECT_FALSE(r.inc());
}

TEST(kmerator_test, reverse_1) {
    kmerator r(1);
    char seq[] = "t";
    r.set(seq, seq+strlen(seq));
    EXPECT_EQ(A_VAL,r.knum());    // Note t read on rev strand, so A_VAL
    EXPECT_FALSE(r.inc());
}

TEST(kmerator_test, reverse_long) {
    kmerator r1(7), r2(7);
    char seq[] = "acgattagcgatagggt";
    char rev[] = "accctatcgctaatcgt";
    r1.set(seq, seq+strlen(seq));
    r2.set(rev, rev+strlen(rev));
    std::list<knum_t> v1, v2;
    do {
        v1.push_back(r1.knum());
        v2.push_front(r2.knum());
    } while (r1.inc() && r2.inc());
    EXPECT_EQ(v1, v2);
}

TEST(kmerator_test, empty_seq) {
    kmerator r(1);
    char seq[] = "";
    r.set(seq, seq+strlen(seq));
    EXPECT_THROW(r.knum(),std::runtime_error);
}

TEST(kmerator_test, set_past_end) {
    kmerator r(3);
    char seq[] = "acg";
    EXPECT_FALSE(r.set(seq+1, seq+3));
    EXPECT_FALSE(r.inc());
    EXPECT_THROW(r.knum(), std::runtime_error);
}

TEST(kmerator_test, read_past_end) {
    kmerator r(3);
    char seq[] = "acg";
    EXPECT_TRUE(r.set(seq, seq+3));
    EXPECT_FALSE(r.inc());
    EXPECT_THROW(r.knum(), std::runtime_error);
}

TEST(kmerator_test, knums_expires) {
    kmerator r(3);
    char seq[] = "cgtatatgca";
    r.set(seq,seq+strlen(seq));
    r.knums();
    EXPECT_FALSE(r.inc());
}

TEST(kmerator_test, ksize_3) {
    kmerator r(3);
    char seq[] = "acgtca";
    r.set(seq, seq+strlen(seq));
    EXPECT_EQ(6,r.knum()); // acg -> 00110
    EXPECT_TRUE(r.inc());
    EXPECT_EQ(6,r.knum()); // cgt -> acg -> 00110
    EXPECT_TRUE(r.inc());
    EXPECT_EQ(17,r.knum()); // gtc -> gac -> 10001
    EXPECT_TRUE(r.inc());
    EXPECT_EQ(28,r.knum()); // tca -> 11100
    EXPECT_FALSE(r.inc());
}

TEST(kmerator_test, move_halfway) {
    kmerator r(3);
    char seq[] = "acgtca";
    r.set(seq, seq+6);
    EXPECT_EQ(6,r.knum()); // acg -> 00110
    EXPECT_TRUE(r.inc());
    r.set(seq+2, seq+6);
    EXPECT_EQ(17,r.knum()); // gtc -> gac -> 10001
    EXPECT_TRUE(r.inc());
    EXPECT_EQ(28,r.knum()); // tca -> 11100
    EXPECT_FALSE(r.inc());
}

TEST(kmerator_test, variant_limit) {
    kmerator r(3);
    char seq[] = "abcdg";   // variants 1x3x1x3 = 9 is too many
    r.set(seq, seq+5);
    EXPECT_THROW(r.knums(),std::runtime_error);
}

TEST(kmerator_test, set_limit) {
    kmerator r(3,1);
    char seq[] = "acsct";   // variants 2 is too many
    r.set(seq, seq+5);
    EXPECT_THROW(r.knums(),std::runtime_error);
}

TEST(kmerator_test, no_limit) {
    kmerator r(3,0);
    char seq[] = "nnnnn";
    r.set(seq, seq+5);
    EXPECT_EQ(64+64+64,r.knums().size());
}

TEST(kmerator_test, full_alphabet) {
    kmerator r(7,0);
    char seq[] = "abcdghkmnrstvwy";
    EXPECT_TRUE(r.set(seq,seq+sizeof(seq)-1));
    while (r.inc())
	EXPECT_TRUE(r.knum() < 0x2000); // tttcttt -> 1 1111 1111 1111
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
