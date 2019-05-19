// MIT License
//
// Copyright (c) 2019 Artur Soloviev (soloviev.artur@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
			// Constructor for an initial unsigned value.
			explicit Decimal(unsigned int _initial);

			// Constructor from string in base10, e.g. "1234".
			explicit Decimal(const std::string& _fromString);

			// Constructor for an empty value.
			explicit Decimal();

			DecimalStatus SetFromString(const std::string& _fromString);

			std::optional<unsigned int> ToUInt() const;

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

			/**
			 * DivideAndRoundInBase10 - divide and round in base10 using 4/5 rule.
			 * @param _divisor
			 * @return
			 */
			std::optional<Decimal> DivideAndRoundInBase10(const Decimal& _divisor) const;

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

			static const Decimal kDecimalWhichEqualUnsignedIntMax;

			static const Decimal kDecimalWhichEqualBase10;

			/**
			 * UnsafeIntegerPower - perform integer exponentiation without overflow checks.
			 * For internal use in ToUInt() function.
			 * @param _base
			 * @param _index
			 * @return strictly integer result
			 */
			static unsigned int UnsafeIntegerPower(unsigned int _base, unsigned int _index);

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
			 * AmplifyInBase256 - Amplify decimal value by
			 * @param _digits in base256
			 * @example 0xFF-> Amplify(1) -> 0x00'FF (little-endian)
			 */
			Decimal& AmplifyInBase256(int _digits);

			/**
			 * AmplifyInBase10 - Amplify decimal value by
			 * @param _digits in base10
		 	* @example 0xFF (256) -> Amplify(1) -> 0x00'0A (2560) (little-endian)
			 */
			Decimal& AmplifyInBase10(int _digits);

			/**
			 * TODO IMPLEMENT
			 * Internal procedure to perform exception-safe parsing without wrapping std::stoi.
			 * @param _stringToCheck
			 * @return DecimalStatus::Ok if Decimal can be constructed correctly from this string, otherwise appropriate error.
			 */
			static DecimalStatus CheckParseStringCoherency(const std::string& _stringToCheck);
	};
}

#endif //DECIMAL_VLN_BCD_DECIMAL_H
