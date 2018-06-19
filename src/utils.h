/* utils.h
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
#ifndef utils_h_INCLUDED
#define utils_h_INCLUDED

#include <iostream>

namespace kcst {

extern void raise_error(const char* t, ...);

/* Alternative using the C++ approach, without varargs, see:
 * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#-es34-dont-define-a-c-style-variadic-function
 *
void raise_error()
{
    std::cerr << std::endl;
    std::exit(1);
}

template <typename T, typename... Ts>
constexpr void error(T head, Ts... tail)
{
    std::cerr << head;
    error(tail...);
}
*/

} // namespace

#endif // utils_h_INCLUDED
       // vim: sts=4:sw=4:ai:si:et
