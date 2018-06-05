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

// Overall typedef for a number storing an encoded kmer
//
typedef long knum;

// Generates the base(s) for a single character, e.g. y -> c t = 1 3.
//
class baserator
{
    private:
        const int *vals_;
        int pos_;

    public:
        baserator(char c) { set(c); }
        void set(char c);
        bool inc();
        int val() const { return vals_[pos_]; }
};

// Generates all kmers for the sequence between begin and end.
// Takes into account letters coding for multiple bases, and generates
// all kmers for these.  E.g. 2mers for aaw -> 0 
//
class kmerator
{
    private:
        const char* pcur_;
        const char* pend_;
        int ksize_;
        std::vector<baserator> baserators_;

    public:
        kmerator(const char* begin, const char* end, int ksize);
        bool inc();
        knum val() const;
};

// Generates all kmers for the sequence between begin and end.
// Accepts only the proper base letters: a, c, g, t.
//
class kmeriser
{
    private:
        const char* pcur_;
        const char* pend_;
        int ksize_;

    public:
        kmeriser(const char* begin, const char* end, int ksize);
        bool inc();
        knum val() const;
};

#endif // kmeriser_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
