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

#include <stdexcept>
#include <list>
#include <gtest/gtest.h>
#include "kmerise.h"

using namespace kcst;

namespace {

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

TEST(kmeriser_test, ksize_1) {
    kmeriser r(1);
    char seq[] = "a";
    r.set(seq, seq+strlen(seq));
    EXPECT_EQ(A_VAL,r.knum());
    EXPECT_FALSE(r.inc());
}

TEST(kmeriser_test, reverse_1) {
    kmeriser r(1);
    char seq[] = "t";
    r.set(seq, seq+strlen(seq));
    EXPECT_EQ(A_VAL,r.knum());    // Note t read on rev strand, so A_VAL
    EXPECT_FALSE(r.inc());
}

TEST(kmeriser_test, reverse_long) {
    kmeriser r1(7), r2(7);
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

TEST(kmeriser_test, empty_seq) {
    kmeriser r(1);
    char seq[] = "";
    EXPECT_FALSE(r.set(seq, seq+strlen(seq)));
    EXPECT_THROW(r.knum(),std::runtime_error);
}

TEST(kmeriser_test, set_past_end) {
    kmeriser r(3);
    char seq[] = "acg";
    EXPECT_FALSE(r.set(seq+1, seq+3));
    EXPECT_FALSE(r.inc());
    EXPECT_THROW(r.knum(), std::runtime_error);
}

TEST(kmeriser_test, read_past_end) {
    kmeriser r(3);
    char seq[] = "acg";
    EXPECT_TRUE(r.set(seq, seq+3));
    EXPECT_FALSE(r.inc());
    EXPECT_THROW(r.knum(), std::runtime_error);
}

TEST(kmeriser_test, knums_expires) {
    kmeriser r(3);
    char seq[] = "cgtatatgca";
    r.set(seq,seq+strlen(seq));
    r.knums();
    EXPECT_FALSE(r.inc());
}

TEST(kmeriser_test, ksize_3) {
    kmeriser r(3);
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

TEST(kmeriser_test, move_halfway) {
    kmeriser r(3);
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

TEST(kmeriser_test, same_as_ator) {
    kmeriser ki(5); kmerator ka(5);
    char seq[] = "acgtaaccggttagacatgtacgggattaatag";
    ki.set(seq, seq+sizeof(seq)-1);
    ka.set(seq, seq+sizeof(seq)-1);
    do {
        EXPECT_EQ(ki.knum(), ka.knum());
    } while (ki.inc() && ka.inc());
    EXPECT_FALSE(ki.inc());
    EXPECT_FALSE(ka.inc());
}


} // namespace
// vim: sts=4:sw=4:ai:si:et
