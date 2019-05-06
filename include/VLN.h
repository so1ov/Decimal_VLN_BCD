//
// Copyright Artur Soloviev 2019
// soloviev.artur@gmail.com
//
// VLN.h
//
// This file is part of VLN Toolkit.
//
// VLN Toolkit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// VLN Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VLN Toolkit.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef VLN_VLN_H
#define VLN_VLN_H

#include <vector>
#include <limits>
#include <cstdint>
#include <optional>

namespace sav
{
	class VLN
	{
		public:
			// Constructor for initial unsigned 64-bit value.
			VLN(std::uint64_t _initial);

			std::optional<std::uint64_t> ToUInt64() const;

			bool operator==(const VLN& _rhs) const;
			bool operator!=(const VLN& _rhs) const;

			bool operator<(const VLN& _rhs) const;
			bool operator>(const VLN& _rhs) const;

			bool operator<=(const VLN& _rhs) const;
			bool operator>=(const VLN& _rhs) const;

		protected:
			enum
			{
				kBaseTen = 10,
				kBase256 = std::numeric_limits<std::uint8_t>::max() + 1
			};

			std::vector<std::uint8_t> m_digits;

			static const VLN kVLNWhichEqualUInt64Max;

			/**
			 * UnsafeIntegerPower - perform integer exponentiation without overflow checks.
			 * For internal use in ToUInt64() function.
			 * @param _base
			 * @param _index
			 * @return strictly integer result
			 */
			static std::uint64_t UnsafeIntegerPower(std::uint64_t _base, std::uint64_t _index);
	};
}

#endif //VLN_VLN_H
