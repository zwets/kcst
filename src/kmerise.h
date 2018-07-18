/* kmerise.h
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
#ifndef kmerise_h_INCLUDED
#define kmerise_h_INCLUDED

#include <vector>
#include <cstdint>


// ABOUT KMERISING
//
// This unit defines two classes (kmerator, kmeriser) to 'kmerise' sequences,
// plus a helper (baserator) to generate all bases for a "degenerate base".
//
// Class kmeriser kmerises sequences containing only proper bases (acgtACGT),
// whereas kmerator additionally accepts degenerate bases (e.g. N, R, and Y),
// and generates all matching knums for a kmer containing degenerate bases.
//
// The 'knum_t' representation of a kmer is a number that encodes a sequence
// of bases by using 2 bits for every base (a=b00, c=b01, g=b10, t=b11), 
// except the middle base, which is encoded as a single bit: a=b0, c=b1.
// There is always a middle base, because we enforce ksize to be odd.
//
// Why the single bit for the middle base, and how do we encode g or t?  The
// trick is that if the middle base is g or t, then we encode the reverse
// complement of the sequence.  This way we have a 'canonical encoding', so
// need to match one way only, while also needing half the storage.


namespace khc {

// knum_t - the number type used to store an encoded kmer.
//
// Counterpart of kmer_t in kmerdb.h, but named differently to keep decoupled.
// Relying on compiler to warn when converting between different sized types.
//
typedef std::uint64_t knum_t;


// Generator for the knums for a single extended base character.
//
// For the four proper bases (ACGT), knum() generates just the single letter,
// len() is 1, and inc() is false on the first call.  For extended bases, each
// knum() followed by inc() generates one of the len() alternatives in turn.
//
// E.g.: after set('y'), len() returns 2, knum() returns 0 ('a'), inc() returns
// true; then knum() returns 3 ('t'), inc() returns false, and it has rolled
// over to the initial situation.
//
class baserator
{
    private:
        const knum_t *vals_;
        int pos_;
        int end_;

    public:
        void set(char c);
        bool inc();
        void dec() { --pos_; }
        int len() const { return end_; }
        knum_t knum() const { return vals_[pos_]; }
};


// Generator for all knums for a sequence of characters.
//
// Iterates a ksize window over sequence [begin,end).  Method next() advances
// or returns false, knum() returns the knum at the current location, knums()
// all knums in the range.
//
// If the current window has extended characters (aka "degenerate bases"), then
// next() produces each variant in turn, before advancing the window to the next
// position in the sequence.
//
// Note how this blows up quickly: a 15-mer 'NN...NN' generates a billion kmers
// (and matches the entire kmer space).  To protect against this, set max_vars.
// A kmer generating more than max_vars will then raise an error.
//
class kmerator
{
    public:
        static const int max_ksize = 4*sizeof(knum_t) - 1;

    private:
        std::vector<baserator> baserators_;
        const char* pcur_;
        const char* pend_;
        int ksize_;
        int variant_;
        int max_variants_;

        void init_baserators();

    public:
        kmerator(int ksize, int max_variants = 0);

        void set(const char *begin, const char *end);
        bool next();
        int variant() const { return variant_; }
        knum_t knum() const;
        std::vector<knum_t> knums();
};


// Generator for all knums for a sequence of bases.
//
// Identical to kmerator(begin, end, 1), but faster, as this generator does
// not handle sequences with variants, i.e. accepts only the four proper bases.
//
// When constructor argument skip_degens is true, the class will silently skip
// kmers with degenerate bases.  By default (false) it raises an error.
//
class kmeriser
{
    public:
        static const int max_ksize = 4*sizeof(knum_t) - 1;

    private:
        const char* pcur_;
        const char* pend_;
        int ksize_;
        bool skip_degens_;

    public:
        kmeriser(int ksize, bool skip_degens = false);

        void set(const char *begin, const char *end);
        bool next();
        knum_t knum() const;
        std::vector<knum_t> knums();
};


} // namespace khc

#endif // kmerise_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
