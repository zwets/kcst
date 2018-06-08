/* hitcounter.cpp
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

#include <algorithm>

#include "hitcounter.h"
#include "utils.h"

namespace kcst {


skey_t
hit_counter::add_target(const std::string& name)
{
    targets_.push_back(name);
    hits_.push_back(0);
    return hits_.size();
}


void
hit_counter::reset()
{
    std::vector<hitc_t>::iterator p = hits_.begin();
    while (p != hits_.end())
    {
        *p++ = 0;
    }
}


typedef std::pair<hitc_t,std::string> hit_rec;

std::vector<hit_rec>
hit_counter::score_list() const
{
    std::vector<hit_rec> res;

    std::vector<std::string>::const_iterator p_tgt = targets_.begin();
    std::vector<hitc_t>::const_iterator p_hit = hits_.begin();

    while (p_tgt != targets_.end())
        res.push_back(hit_rec(*p_hit++, *p_tgt++));

    std::sort(res.begin(), res.end());

    return res;
}


} // namespace kcst

// vim: sts=4:sw=4:ai:si:et
