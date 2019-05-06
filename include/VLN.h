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
			// Constructor for an initial unsigned 64-bit value.
			explicit VLN(std::uint64_t _initial);

			// Constructor for an empty value.
			explicit VLN();

			std::optional<std::uint64_t> ToUInt64() const;

			// Returns false if VLN integrity has been violated (e.g. divided by zero), true otherwise
			explicit operator bool() const;

			// Comparison operators
			bool operator==(const VLN& _rhs) const;
			bool operator!=(const VLN& _rhs) const;

			bool operator<(const VLN& _rhs) const;
			bool operator>(const VLN& _rhs) const;

			bool operator<=(const VLN& _rhs) const;
			bool operator>=(const VLN& _rhs) const;

			// Immutable arithmetic operators.
			VLN operator+(const VLN& _rhs) const;
			VLN operator-(const VLN& _rhs) const;
			VLN operator*(const VLN& _rhs) const;
			VLN operator/(const VLN& _rhs) const;

			// Mutable arithmetic operators (implementation depends on the immutable ones).
			VLN& operator+=(const VLN& _rhs);
			VLN& operator-=(const VLN& _rhs);
			VLN& operator*=(const VLN& _rhs);
			VLN& operator/=(const VLN& _rhs);

		protected:
			enum
			{
				kBase10 = 10,
				kBase256 = std::numeric_limits<std::uint8_t>::max() + 1
			};

			std::vector<std::uint8_t> m_digits;

			// Becomes false on invalid operations
			// (such as division by zero, subtraction by greater value (which is senseless for unsigned),
			// so operator bool returns false.
			bool m_ok = true;

			static const VLN kVLNWhichEqualUInt64Max;

			/**
			 * UnsafeIntegerPower - perform integer exponentiation without overflow checks.
			 * For internal use in ToUInt64() function.
			 * @param _base
			 * @param _index
			 * @return strictly integer result
			 */
			static std::uint64_t UnsafeIntegerPower(std::uint64_t _base, std::uint64_t _index);

			std::string ToBase10() const;
	};
}

#endif //VLN_VLN_H
