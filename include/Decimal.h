//
// Copyright Artur Soloviev 2019
// soloviev.artur@gmail.com
//
// Decimal.h
//
// This file is part of Decimal_VLN_BCD.
//
// Decimal_VLN_BCD is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Decimal_VLN_BCD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Decimal_VLN_BCD.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef DECIMAL_VLN_BCD_DECIMAL_H
#define DECIMAL_VLN_BCD_DECIMAL_H

#include "DecimalStatus.h"

#include <vector>
#include <limits>
#include <cstdint>
#include <optional>
#include <utility>
#include <memory>

namespace sav
{
	class DecimalIntegerDivisionResult;

	class Decimal
	{
		public:
			// Constructor for an initial unsigned 64-bit value.
			explicit Decimal(std::uint64_t _initial);

			// Constructor for an empty value.
			explicit Decimal();

			std::optional<std::uint64_t> ToUInt64() const;

			// Returns false if Decimal integrity has been violated (e.g. divided by zero), true otherwise
			explicit operator bool() const;

			// Comparison operators
			bool operator==(const Decimal& _rhs) const;
			bool operator!=(const Decimal& _rhs) const;

			bool operator<(const Decimal& _rhs) const;
			bool operator>(const Decimal& _rhs) const;

			bool operator<=(const Decimal& _rhs) const;
			bool operator>=(const Decimal& _rhs) const;

			// Immutable arithmetic operators.
			Decimal operator+(const Decimal& _rhs) const;
			Decimal operator-(const Decimal& _rhs) const;
			Decimal operator*(const Decimal& _rhs) const;
			// in the result pair, first is quotient (integer part) and second is remainder
			std::optional<DecimalIntegerDivisionResult> operator/(const Decimal& _rhs) const;

			// Mutable arithmetic operators (implementation depends on the immutable ones).
			Decimal& operator+=(const Decimal& _rhs);
			Decimal& operator-=(const Decimal& _rhs);
			Decimal& operator*=(const Decimal& _rhs);
			// Unable to perform in-place division without remainder loss. Use operator/ .
			void operator/=(const Decimal& _rhs) = delete;

			// Autonomous mutable arithmetic operators
			Decimal& operator++(int);
			Decimal& operator--(int);

		protected:
			enum
			{
				kBase10 = 10,
				kBase256 = std::numeric_limits<std::uint8_t>::max() + 1
			};

			std::vector<std::uint8_t> m_digits;

			DecimalStatus m_status = DecimalStatus::Ok;

			static const Decimal kDecimalWhichEqualUInt64Max;

			/**
			 * UnsafeIntegerPower - perform integer exponentiation without overflow checks.
			 * For internal use in ToUInt64() function.
			 * @param _base
			 * @param _index
			 * @return strictly integer result
			 */
			static std::uint64_t UnsafeIntegerPower(std::uint64_t _base, std::uint64_t _index);

			/**
			 * ToString - convert stored decimal value to a base10 string, e.g. "1234".
			 * @return value as base10
			 */
			std::string ToString() const;

			/**
			 * Normalize - remove unsignificant zeros.
			 * (e.g. for base10 : 0001023 -> 1023)
			 */
			void Normalize();

			/**
			 * EqualsZero - return true if stored value equals zero, false otherwise.
			 */
			bool EqualsZero() const;

			/**
			 * CompareFrames - internal division utility function.
			 * Unsafe and does not perform any checks!
			 * Compares current division frame.
			 * @param _lhs
			 * @param _rhs, must be normalized (must not contain unsignificant zeros)
			 * @return true if _lhs frame greater or equal than entire _rhs, false otherwise.
			 *
			 * @example
			 * 1224 | 12
			 * 12___| 102
			 * 	 24
			 * 	 24
			 * 	  0
			 *
			 * The first frame in 1224 is 12, the second is 02, and the third is 24.
			 * The first result is true, so we can perform intermediate subtraction,
			 * the second is false, so multiply result by 10,
			 * the third is true, so perform additional subtraction.
			 *
			 */
			static bool CompareFrames(const Decimal& _lhs, int _lhsFrame, const Decimal& _rhs);

			/**
			 * Amplify - Amplify decimal value by
			 * @param _digits
			 * @example 0xFF-> Amplify(1) -> 0x00'FF (little-endian)
			 */
			void Amplify(int _digits);
	};
}

#endif //DECIMAL_VLN_BCD_DECIMAL_H
