/* baserator-test.cpp
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
#include <gtest/gtest.h>
#include "kmerise.h"

using namespace khc;

namespace {

const char  LOWER_LETTERS[] = "acgtwsmkrybdhvn";
const char  UPPER_LETTERS[] = "ACGTWSMKRYBDHVN";
const int   A_VAL = 0, C_VAL = 1, G_VAL = 2, T_VAL = 3;

TEST(baserator_test, base_a) {
    baserator b;
    b.set('a');
    EXPECT_EQ(1, b.len());
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_c) {
    baserator b;
    b.set('c');
    EXPECT_EQ(1, b.len());
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_g) {
    baserator b;
    b.set('g');
    EXPECT_EQ(1, b.len());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_t) {
    baserator b;
    b.set('t');
    EXPECT_EQ(1, b.len());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_w) {
    baserator b;
    b.set('w');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_s) {
    baserator b;
    b.set('s');
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_m) {
    baserator b;
    b.set('m');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_k) {
    baserator b;
    b.set('k');
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_r) {
    baserator b;
    b.set('r');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_y) {
    baserator b;
    b.set('y');
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_b) {
    baserator b;
    b.set('b');
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_d) {
    baserator b;
    b.set('d');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_h) {
    baserator b;
    b.set('h');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_v) {
    baserator b;
    b.set('v');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, base_n) {
    baserator b;
    b.set('n');
    EXPECT_EQ(A_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(C_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(G_VAL, b.knum());
    EXPECT_TRUE(b.inc());
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

TEST(baserator_test, capital_letters) {
    baserator b1, b2;
    bool x1, x2;
    for (size_t i = 0; i<sizeof(LOWER_LETTERS)/sizeof(LOWER_LETTERS[0])-1; ++i) {
        b1.set(LOWER_LETTERS[i]);
        b2.set(UPPER_LETTERS[i]);
        do {
            EXPECT_EQ(b1.knum(), b2.knum());
            x1 = b1.inc();
            x2 = b2.inc();
        } while (x1 || x2);
        EXPECT_FALSE(x1 || x2);
    }
}

TEST(baserator_test, invalid_letters) {
    baserator b;

    //EXPECT_THROW(b.set('e'), std::runtime_error);
    //EXPECT_THROW(b.set('u'), std::runtime_error);
    //EXPECT_THROW(b.set('z'), std::runtime_error);
    //EXPECT_THROW(b.set('0'), std::runtime_error);

    EXPECT_DEATH(b.set('e'), ".*");
    EXPECT_DEATH(b.set('u'), ".*");
    EXPECT_DEATH(b.set('z'), ".*");
    EXPECT_DEATH(b.set('0'), ".*");
}

TEST(baserator_test, rollaround) {
    baserator b;

    b.set('a');
    EXPECT_FALSE(b.inc());
    EXPECT_EQ(A_VAL, b.knum());
}

TEST(baserator_test, change_halfway) {
    baserator b;

    b.set('n');
    b.inc();
    EXPECT_EQ(C_VAL, b.knum());
    b.set('t');
    EXPECT_EQ(T_VAL, b.knum());
    EXPECT_FALSE(b.inc());
}

}

// vim: sts=4:sw=4:ai:si:et
