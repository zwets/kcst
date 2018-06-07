/* test-int-sizes.cpp
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

#include <iostream>
#include <cstdint>

int main(int argc, char *argv[]) 
{
    std::cout
        << "sizeof(char) = " << sizeof(char) << std::endl
        << "sizeof(short) = " << sizeof(short) << std::endl
        << "sizeof(int) = " << sizeof(int) << std::endl
        << "sizeof(long) = " << sizeof(long) << std::endl
        << "sizeof(long long) = " << sizeof(long long) << std::endl
        << std::endl
	<< "sizeof(int8_t) = " << sizeof(std::int8_t) <<std::endl
	<< "sizeof(int16_t) = " << sizeof(std::int16_t) << std::endl
	<< "sizeof(int32_t) = " << sizeof(std::int32_t) << std::endl
	<< "sizeof(int64_t) = " << sizeof(std::int64_t) << std::endl
	<< std::endl
	<< "sizeof(int_fast8_t) = " << sizeof(std::int_fast8_t) << std::endl
	<< "sizeof(int_fast16_t) = " << sizeof(std::int_fast16_t) << std::endl
	<< "sizeof(int_fast32_t) = " << sizeof(std::int_fast32_t) << std::endl
	<< "sizeof(int_fast64_t) = " << sizeof(std::int_fast64_t) << std::endl
	<< "sizeof(int_least8_t) = " << sizeof(std::int_least8_t) << std::endl
	<< "sizeof(int_least16_t) = " << sizeof(std::int_least16_t) << std::endl
	<< "sizeof(int_least32_t) = " << sizeof(std::int_least32_t) << std::endl
	<< "sizeof(int_least64_t) = " << sizeof(std::int_least64_t) << std::endl
	<< std::endl << std::endl
        << "sizeof(unsigned char) = " << sizeof(unsigned char) << std::endl
        << "sizeof(unsigned short) = " << sizeof(unsigned short) << std::endl
        << "sizeof(unsigned int) = " << sizeof(unsigned int) << std::endl
        << "sizeof(unsigned long) = " << sizeof(unsigned long) << std::endl
        << "sizeof(unsigned long long) = " << sizeof(unsigned long long) << std::endl
        << std::endl
	<< "sizeof(uint8_t) = " << sizeof(std::uint8_t) << std::endl
	<< "sizeof(uint16_t) = " << sizeof(std::uint16_t) << std::endl
	<< "sizeof(uint32_t) = " << sizeof(std::uint32_t) << std::endl
	<< "sizeof(uint64_t) = " << sizeof(std::uint64_t) << std::endl
	<< std::endl
	<< "sizeof(uint_fast8_t) = " << sizeof(std::uint_fast8_t) << std::endl
	<< "sizeof(uint_fast16_t) = " << sizeof(std::uint_fast16_t) << std::endl
	<< "sizeof(uint_fast32_t) = " << sizeof(std::uint_fast32_t) << std::endl
	<< "sizeof(uint_fast64_t) = " << sizeof(std::uint_fast64_t) << std::endl
	<< "sizeof(uint_least8_t) = " << sizeof(std::uint_least8_t) << std::endl
	<< "sizeof(uint_least16_t) = " << sizeof(std::uint_least16_t) << std::endl
	<< "sizeof(uint_least32_t) = " << sizeof(std::uint_least32_t) << std::endl
	<< "sizeof(uint_least64_t) = " << sizeof(std::uint_least64_t) << std::endl;
}

// vim: sts=4:sw=4:ai:si:et
