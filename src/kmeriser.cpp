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

namespace khc {


static const knum_t A = 0;
static const knum_t C = 1;
static const knum_t G = 2;
static const knum_t T = 3;
static const knum_t X = -1;

//                                    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
static const knum_t BASE_VALUES[] = { A, X, C, X, X, X, G, X, X, X, X, X, X, X, X, X, X, X, X, T, X, X, X, X, X, X };
static const int DEGEN_BASES[]    = { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 };


static knum_t
base_value(char c)
{
    int o = c - 'A';

    if (o < 0 || o > 19) {      // outside 'A'..'T'
        o = c - 'a';            // try 'a'..'t' smallcaps
        if (o < 0 || o > 19)
            raise_error("invalid base: %c; must be one of [acgtACGT]", c);
    }

    knum_t v = BASE_VALUES[o];

    if (v == X)
        raise_error("invalid base: %c; must be one of [acgtACGT]", c);

    return v;
}


static bool
is_degen_base(char c)
{
    int o = c - 'A';

    if (o < 0 || o > 25) {      // outside 'A'..'Z'
        o = c - 'a';            // try 'a'..'z' smallcaps
        if (o < 0 || o > 25)    // outside either range then
            return false;       // by definition not a degenerate base 
    }

    return DEGEN_BASES[o];
}


kmeriser::kmeriser(int ksize, bool skip_degens)
    : pcur_(0), pend_(0), ksize_(ksize), skip_degens_(skip_degens)
{
    if (ksize < 1 || ksize > max_ksize || !(ksize & 1))
        raise_error("invalid kmer size: %d; must be an odd number in range [1,%d]", ksize, max_ksize);
}


void
kmeriser::set(const char *begin, const char *end)
{
    pcur_ = begin - 1;          // one before start of first kmer
    pend_ = end - ksize_ + 1;   // one beyond start of last kmer

    // if we skip_degens, then bump pcur to one before first kmer not containing a degen base
    // note: ignore invalid bases as these will error out in any case in knum()

    if (skip_degens_)
    {
        while (++pcur_ < pend_)
        {
            const char *p = pcur_ + ksize_;

            while (p-- != pcur_) 
                if (is_degen_base(*p))
                {
                    pcur_ = p;
                    break;
                }
        
            // post condition: p == pcur and is degen, or p < pcur and pcur starts good kmer

            if (p != pcur_)
                break;
        }

        // post condition: pcur on start of non-degen kmer, or pcur on or beyond pend

        --pcur_;  // first next() will advance it (possibly onto pend and yield false)
    }
}


bool
kmeriser::next()
{
    if (skip_degens_)
    {
        while (++pcur_ < pend_ && is_degen_base(pcur_[ksize_-1]))
            pcur_ += ksize_ - 1;
        return pcur_ < pend_;
    }
    else
        return ++pcur_ < pend_;
}



knum_t
kmeriser::knum() const
{
    knum_t res = 0;

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

    while (next()) 
        res.push_back(knum());

    return res;
}


} // namespace khc

// vim: sts=4:sw=4:ai:si:et
