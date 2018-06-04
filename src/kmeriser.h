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

// generates the sequence of bases for a single character, until !inc()
//
class baserator
{
    private:
        const int *vals_;
        int pos_;

    public:
        void set(char c);
        bool inc();
        int val() const { return vals_[pos_]; }
};

// generates (possibly multiple) kmers from a ksize sequence, terminating -1
//
class kmerator
{
    private:
        std::vector<baserator> baserators_;

    public:
        kmerator(int k);
        void set(const char*);
        bool inc();
        int val() const;
};

// generates all kmers from a vector of characters, terminating with -1
//
class kmeriser 
{
    private:
        const char* pcur_;
        const char* pend_;
        kmerator kmerator_;

    public:
        kmeriser(const char *begin, const char *end, int ksize);
        bool inc();
        int val() const;
};

#endif // kmeriser_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
