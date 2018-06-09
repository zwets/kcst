/* kmeriser.cpp
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

#include "kmerise.h"
#include "utils.h"

namespace kcst {


static const knum_t A = 0;
static const knum_t C = 1;
static const knum_t G = 2;
static const knum_t T = 3;
static const knum_t X = -1;

static const knum_t BASE_VALUES[] = { A, X, C, X, X, X, G, X, X, X, X, X, X, X, X, X, X, X, X, T };


kmeriser::kmeriser(int ksize)
    : pcur_(0), pend_(0), ksize_(ksize)
{
    if (ksize < 1 || ksize > MAX_KSIZE || !(ksize & 1))
        raise_error("invalid kmer size: %d; must be an odd number in range [1,%d]", ksize, MAX_KSIZE);
}


bool
kmeriser::set(const char *begin, const char *end)
{
    pcur_ = begin;
    pend_ = end - ksize_ + 1;

    return pcur_ < pend_;
}


bool
kmeriser::inc()
{
    return pcur_ < pend_ && ++pcur_ < pend_;
}


static knum_t
base_value(char c)
{
    int o = c - 'a';

    if (o < 0 || o > 19)
    {
        o = c - 'A';            // try between 'A' and 'T'
        if (o < 0 || o > 19)
            raise_error("invalid base: %c; must be one of [acgtACGT]", c);
    }

    knum_t v = BASE_VALUES[o];

    if (v == X)
        raise_error("invalid base: %c; must be one of [acgtACGT]", c);

    return v;
}


knum_t
kmeriser::knum() const
{
    knum_t res = 0;

    if (!(pcur_ < pend_))
        raise_error("kmeriser read attempted past right bound of sequence");

    const char *pmid = pcur_ + (ksize_ / 2);
    const char *pstop = pcur_ + ksize_;

    if (!(base_value(*pmid) & 2))  // middle base is a or c, canonical is forward strand
    {
        const char *p = pcur_ - 1;

        while (++p != pmid)
            res = (res<<2) | base_value(*p);

        res = (res<<1) | base_value(*p); // central base encoded as 1 bit: a->0, c->1

        while (++p != pstop)
            res = (res<<2) | base_value(*p);
    }
    else // middle base is g or t, canonical kmer is the reverse complement
    {
        const char *p = pstop;

        while (--p != pmid)
            res = (res<<2) | (base_value(*p) ^ 3); // xor with 3 is complementary base

        res = (res<<1) | (base_value(*p) ^ 3); // t->a->0, g->c->1

        while (p-- != pcur_)
            res = (res<<2) | (base_value(*p) ^ 3);
    }

    return res;
}


std::vector<knum_t>
kmeriser::knums()
{
    std::vector<knum_t> res;

    if (pcur_ < pend_) do res.push_back(knum()); while (inc());

    return res;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
