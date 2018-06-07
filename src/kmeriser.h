/* kmeriser.h
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
#ifndef kmeriser_h_INCLUDED
#define kmeriser_h_INCLUDED

#include <vector>
#include <cstdint>


// knum - typedef for the number type used to store an encoded kmer,
//
typedef std::uint64_t knum;

// MAX_KSIZE is the longest possible kmer size (must be odd)
//
const int MAX_KSIZE = 4 * sizeof(knum) - 1;  // largest odd number


// Generator for the knums for a single extended base character.
//
// For the four proper bases (ACGT), val() returns just the single letter;
// len() is 1, and inc() returns false on the first call.  For extended bases,
// val() followed by inc() generates each of the len() alternatives in turn.
//
// E.g.: y -> c,t -> 1,3; n -> a,c,g,t -> 0,1,2,3.
//
class baserator
{
    private:
        const knum *vals_;
        int pos_;
        int end_;

    public:
        void set(char c);
        bool inc();
        int len() const { return end_; }
        knum val() const { return vals_[pos_]; }
};


// Generator for all knums for a sequence of characters.
//
// Iterates a ksize kmer window over the character sequence [begin,end).
// get() returns the knum representation at the current location, inc()
// advances to the next or returns false.
// If the current window has extended characters (that stand for multiple
// bases), then each variant is generated in turn before advancing to the
// next position in the sequence.
//
// For instance: abc -> {acc,agc,atc}, and nnn -> {aaa,aac,...60...,ttg,ttt}.
// Note how this blows up quickly: a 15-mer 'NN...NN' generates a billion
// kmers, and is useless as it is hit by everything.  Hence by default the
// limit max_variants (per kmer) is DEFAULT_MAX_VARIANTS (4), adjustable
// via the constructor, pass 0 to set no limit.
//
class kmerator
{
    public:
        static const int DEFAULT_MAX_VARIANTS = 4;

    private:
        std::vector<baserator> baserators_;
        const char* pcur_;
        const char* pend_;
        int ksize_;
        int variant_;
        int max_variants_;

    public:
        kmerator(int ksize, int max_variants = DEFAULT_MAX_VARIANTS);

        bool set(const char *begin, const char *end);
        bool inc();
        int variant() const;
        knum get() const;
        std::vector<knum> get_all();
};


// Generator for all knums for a sequence of bases.
//
// Identical to kmerator(begin, end, ksize, 1), but faster, as this generator
// handles only sequences consisting of proper bases.
//
class kmeriser
{
    private:
        const char* pcur_;
        const char* pend_;
        int ksize_;

    public:
        kmeriser(int ksize);

        bool set(const char *begin, const char *end);
        bool inc();
        knum get() const;
        std::vector<knum> get_all();
};

#endif // kmeriser_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
