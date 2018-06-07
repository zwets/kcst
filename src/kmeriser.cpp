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

#include "kmeriser.h"
#include "utils.h"

static const knum A = 0;
static const knum C = 1;
static const knum G = 2;
static const knum T = 3;
static const knum X = -1;

static const knum BASE_VALUES[] = { A, X, C, X, X, X, G, X, X, X, X, X, X, X, X, X, X, X, X, T };


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
    return ++pcur_ < pend_;
}


static int
base_value(int c)
{
    int o = c - 'a';

    if (o < 0 || o > 19)
    {
        o = c - 'A';            // try between 'A' and 'T'
        if (o < 0 || o > 19)
            raise_error("invalid base: %c; must be one of [acgtACGT]", c);
    }

    int v = BASE_VALUES[o];

    if (v == X)
        raise_error("invalid base: %c; must be one of [acgtACGT]", c);

    return v;
}


inline int
comp_base_value(int c)
{
    return base_value(c) ^ 3;
}


knum
kmeriser::get() const
{
    knum res = 0;

    if (!(pcur_ < pend_))
        raise_error("kmeriser read attempted past right bound of sequence");

    const char *pmid = pcur_ + (ksize_ / 2);

    if (!(base_value(*pmid) & 2))  // a or c, so encode forward strand
    {
        const char *p = pcur_-1;

        while (++p < pmid)
            res = (res << 2) | base_value(*p);

        res = (res << 1) | base_value(*p) & 1;        // a->0, c->1

        while (++p < pcur_ + ksize_)
            res = (res << 2) | base_value(*p);
    }
    else
    {
        const char *p = pcur_ + ksize_;

        while (--p > pmid)
            res = (res << 2) | comp_base_value(*p);

        res = (res << 1) | comp_base_value(*p) & 1;   // t->a->0, g->c->1

        while (--p >= pcur_)
            res = (res << 2) | comp_base_value(*p);
    }

    return res;
}


std::vector<knum>
kmeriser::get_all()
{
    std::vector<knum> res;

    if (pcur_ < pend_) do res.push_back(get()); while (inc());

    return res;
}

// vim: sts=4:sw=4:ai:si:et
