/* kmerator.cpp
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

namespace kcst {
    

kmerator::kmerator(int ksize, int max_variants)
    : pcur_(0), pend_(0), ksize_(ksize), variant_(0), max_variants_(max_variants)
{
    if (ksize < 1 || ksize > MAX_KSIZE || !(ksize & 1))
        raise_error("invalid kmer size: %d; must be an odd number in range [1,%d]", ksize, MAX_KSIZE);

    for (int i = 0; i < ksize_; ++i)
        baserators_.push_back(baserator());
}


bool
kmerator::set(const char *begin, const char *end)
{
    pcur_ = begin;
    pend_ = end - ksize_ + 1;
    variant_ = 0;
    
    if (pcur_ < pend_)
        for (int i = 0; i < ksize_; ++i)
            baserators_[i].set(pcur_[i]);

    return pcur_ < pend_;
}


bool
kmerator::inc()
{
    bool done = false;

    if (pcur_ >= pend_) // bail early if already walked off the end
        return false;

    // try bumping the baserators to the next variant at pcur_

    std::vector<baserator>::iterator p = baserators_.end();
    while (!done && p-- != baserators_.begin())
        done = p->inc();

    if (done) // done -> bumped, another variant is present for current kmer
    {
        if (++variant_ == max_variants_) // count variant and check limit
        {
            char buf[ksize_+1];
            for (int i = 0; i<ksize_; ++i)
                buf[i] = pcur_[i];
            buf[ksize_] = '\0';

            raise_error("kmer generates more than %d variants: %s", 
                    max_variants_, buf);
        }
    }
    else // not done -> the baserators have all overflown, we must move on
    {
        variant_ = 0;
        
        if ((done = ++pcur_ < pend_))   // shift one position right or fail
            for (int i = 0; i < ksize_; ++i)  // reload the baserators
                baserators_[i].set(pcur_[i]);
    }

    return done;
}


knum_t
kmerator::knum() const
{
    knum_t res = 0;

    if (!(pcur_ < pend_))
        raise_error("kmerator read attempted past right bound of sequence");

    std::vector<baserator>::const_iterator pmid = baserators_.begin() + (ksize_ / 2);

    if (!(pmid->knum() & 2)) // middle of kmer is a or c, canonical is forward strand
    {
        std::vector<baserator>::const_iterator p = baserators_.begin() - 1;

        while (++p != pmid)
            res = (res<<2) | p->knum();

        res = (res<<1) | p->knum(); // central base encoded as 1 bit: a->0, c->1
        
        while (++p != baserators_.end())
            res = (res<<2) | p->knum();
    }
    else // middle base is g or t, canonical kmer is the reverse complement
    {
        std::vector<baserator>::const_iterator p = baserators_.end();

        while (--p != pmid)
            res = (res<<2) | (p->knum() ^ 3);

        res = (res<<1) | (p->knum() ^ 3);   // center t->a=0, g->c=1
        
        while (p-- != baserators_.begin())
            res = (res<<2) | (p->knum() ^ 3);
    }

    return res;
}


std::vector<knum_t>
kmerator::knums()
{
    std::vector<knum_t> res;

    if (pcur_ < pend_) do res.push_back(knum()); while (inc());

    return res;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
