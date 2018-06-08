/* hitcounter.h
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
#ifndef hitcounter_h_INCLUDED
#define hitcounter_h_INCLUDED

#include <string>
#include <vector>
#include <cstdint>

namespace kcst {


// skey_t - numeric type to hold identifier of a sequence
//
typedef std::uint32_t skey_t;

// hitc_t - numeric type to hold hit counts on a sequence
//
typedef std::uint_fast32_t hitc_t;


// hit_counter - tracks a list of sequences and their hit counts
// 
class hit_counter
{
    private:
        std::vector<std::string> targets_;
        std::vector<hitc_t> hits_;

    public:
        skey_t add_target(const std::string& name);
        
        void reset();
        void count_hit(skey_t skey) { ++hits_[skey]; }

        const std::vector<std::string>& targets() const { return targets_; }
        const std::vector<hitc_t>& hits() const { return hits_; }

        std::vector< std::pair<hitc_t,std::string> > score_list() const;
};


} // namespace kcst

#endif // hitcounter_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
