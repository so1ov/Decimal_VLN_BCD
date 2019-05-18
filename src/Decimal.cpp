//
// Copyright Artur Soloviev 2019
// soloviev.artur@gmail.com
//
// Decimal.cpp
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

#include "Decimal.h"

#include "DecimalIntegerDivisionResult.h"

#include <numeric>
#include <algorithm>

sav::Decimal::Decimal(std::uint64_t _initial)
{
	for(;;)
	{
		m_digits.push_back(_initial % kBase256);
		_initial /= kBase256;

		if(_initial == 0)
		{
			break;
		}
	}
}

sav::Decimal::Decimal()
{
	m_digits.push_back(0x00);
}

sav::Decimal::Decimal(const std::string& _fromString)
{
	SetFromString(_fromString);
}

const sav::Decimal sav::Decimal::kDecimalWhichEqualUInt64Max = sav::Decimal{
	std::numeric_limits<std::uint64_t>::max()
};

const sav::Decimal sav::Decimal::kDecimalWhichEqualBase10ToAmplifyFrom = sav::Decimal{
	sav::Decimal::kBase10
};

std::optional<std::uint64_t> sav::Decimal::ToUInt64() const
{
	if( (*this) > kDecimalWhichEqualUInt64Max)
	{
		return std::nullopt;
	}

	std::uint64_t result = 0;

	// m_digits == 0x02 0x00 0x01 // 65538
	// result = 0x02 * 256^0 + 0x00 * 256^1 + 0x01 * 256^2 == 2 + 0 + 65536 == 65538
	for(int i = 0; i < m_digits.size(); i++)
	{
		result += m_digits[i] * UnsafeIntegerPower(kBase256, i);
	}

	return std::optional<std::uint64_t>{result};
}

bool sav::Decimal::operator==(const sav::Decimal& _rhs) const
{
	return this->m_digits == _rhs.m_digits;
}

bool sav::Decimal::operator!=(const sav::Decimal& _rhs) const
{
	return !((*this) == _rhs);
}

bool sav::Decimal::operator<(const sav::Decimal& _rhs) const
{
	if(this->m_digits.size() > _rhs.m_digits.size())
	{
		return false;
	}

	for(int i = this->m_digits.size() - 1; i >= 0; i--)
	{
		if(i > _rhs.m_digits.size() - 1)
		{
			continue;
		}

		if(this->m_digits[i] == _rhs.m_digits[i])
		{
			continue;
		}

		return this->m_digits[i] < _rhs.m_digits[i];
	}

	// Unreachable
	return false;
}

bool sav::Decimal::operator>(const sav::Decimal& _rhs) const
{
	return !((*this) == _rhs) && !((*this) < _rhs);
}

bool sav::Decimal::operator<=(const sav::Decimal& _rhs) const
{
	return ((*this) < _rhs) || ((*this) == _rhs);
}

bool sav::Decimal::operator>=(const sav::Decimal& _rhs) const
{
	return (!((*this) < _rhs)) || ((*this) == _rhs);
}

std::uint64_t sav::Decimal::UnsafeIntegerPower(std::uint64_t _base, std::uint64_t _index)
{
	if(_index == 0)
	{
		return 1;
	}

	if(_base == 0)
	{
		throw;
	}

	std::uint64_t result = 0;

	std::uint64_t accum = 1;
	for(std::uint64_t i = 0; i < _index; i++)
	{
		accum *= _base;
	}

	result += accum;

	return result;
}

std::string sav::Decimal::ToString() const
{
	if(this->EqualsZero())
	{
		return "0";
	}

	auto intermediateResult = std::optional<DecimalIntegerDivisionResult>();
	intermediateResult->Quotient = (*this);
	std::string result;

	do
	{
		intermediateResult = intermediateResult->Quotient / Decimal{kBase10};
		result += std::to_string(intermediateResult->Remainder.ToUInt64().value());
	}while(!intermediateResult->Quotient.EqualsZero());

	std::reverse(result.begin(), result.end());

	return result;
}

sav::Decimal::operator bool() const
{
	return m_status == DecimalStatus::Ok;
}

sav::Decimal sav::Decimal::operator+(const sav::Decimal& _rhs) const
{
	Decimal result;
	result.m_digits.clear();

	std::uint8_t carry = 0;
	std::uint8_t accumulator = 0;

	for(int i = 0; i < this->m_digits.size() || i < _rhs.m_digits.size(); i++)
	{
		// reset accumulator from previous addition
		accumulator = 0;

		// add and reset carry from previous addition
		if( (accumulator + carry) < accumulator)
		{
			accumulator += carry;
			// cock carry again in case of overflow
			carry = 1;
		}
		else
		{
			accumulator += carry;
			carry = 0;
		}

		if(i < this->m_digits.size())
		{
			// overflow checking and set carry if needed
			if( (accumulator + static_cast<int>(this->m_digits[i])) > std::numeric_limits<std::uint8_t>::max())
			{
				carry = 1;
			}

			// perform actual addition
			accumulator += this->m_digits[i];
		}

		if(i < _rhs.m_digits.size())
		{
			// overflow checking and set carry if needed
			if( (accumulator + static_cast<int>(_rhs.m_digits[i])) > std::numeric_limits<std::uint8_t>::max())
			{
				carry = 1;
			}

			// perform actual addition
			accumulator += _rhs.m_digits[i];
		}

		result.m_digits.push_back(accumulator);
	}

	if(carry)
	{
		result.m_digits.push_back(0x01);
	}

	return result;
}

sav::Decimal sav::Decimal::operator-(const sav::Decimal& _rhs) const
{
	Decimal result;

	if( (*this) < _rhs)
	{
		result.m_status = DecimalStatus::Error_Underflow;
		return result;
	}

	if( (*this) == _rhs)
	{
		return result;
	}

	// Prepare for the actual arbitraty-based subtraction.
	result = (*this);

	std::uint8_t carry = 0;

	for(int i = 0; i < result.m_digits.size(); i++)
	{
		// subtract and reset carry from previous subtraction
		if(result.m_digits[i] < carry)
		{
			result.m_digits[i] -= carry;
			// cock carry again in case of underflow
			carry = 1;
		}
		else
		{
			result.m_digits[i] -= carry;
			carry = 0;
		}

		if(i < _rhs.m_digits.size())
		{
			// underflow checking and set carry if needed
			if(result.m_digits[i] < _rhs.m_digits[i])
			{
				carry = 1;
			}

			// perform actual subtraction
			result.m_digits[i] -= _rhs.m_digits[i];
		}
	}

	result.Normalize();

	return result;
}

sav::Decimal sav::Decimal::operator*(const sav::Decimal& _rhs) const
{
	Decimal result;

	// if one of multipliers equal to 0
	if( (this->m_digits.size() == 1 && this->m_digits[0] == 0x00) ||
		(_rhs.m_digits.size() == 1 && _rhs.m_digits[0] == 0x00) )
	{
		// return 0;
		return result;
	}

	// if one of multipliers equal to 1, return the other one
	//
	if(this->m_digits.size() == 1 && this->m_digits[0] == 0x01)
	{
		return _rhs;
	}
	//
	if(_rhs.m_digits.size() == 1 && _rhs.m_digits[0] == 0x01)
	{
		return (*this);
	}

	// Perform an actual multiplication
	std::uint8_t carry = 0;
	for(int currentRhsDigit = 0; currentRhsDigit < _rhs.m_digits.size(); currentRhsDigit++)
	{
		Decimal intermediateSum;
		intermediateSum.m_digits.clear();

		// 1234 * 567 =
		// 1234 * 7 * 10^0 + 1234 * 6 * 10^1 + 1234 * 5 * 10^2 =
		// __8638 + <- intermediate sum 0 (0th digit of the second multiplier: 0 digits to complete) : --
		// _74040 + <- intermediate sum 1 (1th digit of the second multiplier: 1 digits to complete) : 0
		// 123900   <- intermediate sum 2 (2th digit of the second multiplier: 2 digits to complete) : 00
		//
		// index of 10 is a count of digits to complete for each next intermediate sum
		// (notice it's actually base256, not base10)
		for(int i = 0; i < currentRhsDigit; i++)
		{
			intermediateSum.m_digits.push_back(0x00);
		}

		for(int currentThisDigit = 0; currentThisDigit < this->m_digits.size(); currentThisDigit++)
		{
			// 1234 *
			//	567
			//	Step 1: 1234 * 7 ,
			//		Step 1.1 : 4 * 7, reg = 4 * 7 = 28, current digit = 8, carry = 2
			//		Step 1.2 : 3 * 7, reg = 3 * 7 = 21, current digit = carry (which is still 2) + 1 = 3, new carry = 2
			//		...
			std::uint16_t reg =  _rhs.m_digits[currentRhsDigit] * this->m_digits[currentThisDigit];
			intermediateSum.m_digits.push_back(carry + static_cast<std::uint8_t>(reg & static_cast<std::uint8_t>(0xFF)));
			carry = (reg & static_cast<std::uint16_t>(0xFF'00)) >> (sizeof(std::uint8_t) * CHAR_BIT * 1);
		}

		// If carry occured in the most significant digit
		if(carry != 0x00)
		{
			intermediateSum.m_digits.push_back(carry);
		}

		// Accumulate intermediate sum
		result += intermediateSum;
	}

	return result;
}

std::optional<sav::DecimalIntegerDivisionResult> sav::Decimal::operator/(const sav::Decimal& _rhs) const
{
	if(_rhs.m_digits.size() == 1 && _rhs.m_digits[0] == 0x00)
	{
		return std::nullopt;
	}

	DecimalIntegerDivisionResult result;

	if((*this) < _rhs)
	{
		result.Remainder = (*this);
		return {result};
	}

	Decimal mutableThis = (*this);
	int currentFrameEnd = mutableThis.m_digits.size();
	int currentFrameBegin = currentFrameEnd - _rhs.m_digits.size();
	int loanedDigits = 0;
	Decimal currentFrame;

	bool first = true;
	for(;;)
	{
		if(currentFrameBegin - loanedDigits < 0)
		{
			if(loanedDigits != 0)
			{
				result.Quotient.AmplifyInBase256(loanedDigits - 1);
				result.Quotient.Normalize();

				currentFrameBegin -= loanedDigits;
				loanedDigits = 0;
			}

			break;
		}

		currentFrame.m_digits =
			std::vector<std::uint8_t>{
				mutableThis.m_digits.begin() + currentFrameBegin - loanedDigits,
				mutableThis.m_digits.begin() + currentFrameEnd
			};

		currentFrame.Normalize();

		if(currentFrame < _rhs)
		{
			loanedDigits++;
			continue;
		}

		//

		if(loanedDigits != 0)
		{
			result.Quotient.AmplifyInBase256(loanedDigits);
			result.Quotient.Normalize();

			currentFrameBegin -= loanedDigits;
			loanedDigits = 0;
		}

		while(currentFrame >= _rhs)
		{
			currentFrame -= _rhs;
			result.Quotient++;
		}

		for(int i = currentFrameBegin; i < currentFrameEnd; i++)
		{
			if(i - currentFrameBegin >= currentFrame.m_digits.size())
			{
				mutableThis.m_digits[i] = 0x00;
			}
			else
			{
				mutableThis.m_digits[i] = currentFrame.m_digits[i - currentFrameBegin];
			}
		}

		if(currentFrame.EqualsZero())
		{
			currentFrameEnd = currentFrameBegin;
			loanedDigits += _rhs.m_digits.size();
		}

		first = false;
	}

	result.Remainder = currentFrame;
	result.Remainder.Normalize();

	return {result};
}

sav::Decimal& sav::Decimal::operator+=(const sav::Decimal& _rhs)
{
	(*this) = (*this) + _rhs;

	return (*this);
}

sav::Decimal& sav::Decimal::operator-=(const sav::Decimal& _rhs)
{
	(*this) = (*this) - _rhs;

	return (*this);
}

sav::Decimal& sav::Decimal::operator*=(const sav::Decimal& _rhs)
{
	(*this) = (*this) * _rhs;

	return (*this);
}

void sav::Decimal::Normalize()
{
	for(int i = m_digits.size() - 1; i >= 0; i--)
	{
		if( m_digits[i] != 0x00 )
		{
			m_digits = std::vector<std::uint8_t>{m_digits.begin(), m_digits.begin() + i + 1};
			return;
		}
	}

	// If internal array was like 0x00 0x00 0x00 ... 0x00
	m_digits.clear();
	m_digits.push_back(0x00);
}

bool sav::Decimal::EqualsZero() const
{
	if(m_digits.size() == 1 && m_digits[0] == 0x00)
	{
		return true;
	}

	return false;
}

bool sav::Decimal::CompareFrames(const sav::Decimal& _lhs, int _lhsFrame, const sav::Decimal& _rhs)
{
	return _lhs.m_digits[_lhsFrame] > _rhs.m_digits[_lhsFrame];
}

sav::Decimal& sav::Decimal::operator++(int)
{
	for(int i = 0; i < m_digits.size(); i++)
	{
		if(m_digits[i] != std::numeric_limits<std::uint8_t>::max())
		{
			m_digits[i]++;
			return (*this);
		}
		else
		{
			m_digits[i]++;
		}
	}

	// 9999 -> 10000
	std::fill(m_digits.begin(), m_digits.end(), 0x00);
	m_digits.push_back(0x01);

	return (*this);
}

sav::Decimal& sav::Decimal::operator--(int)
{
	for(int i = 0; i < m_digits.size(); i++)
	{
		if(m_digits[i] != std::numeric_limits<std::uint8_t>::min())
		{
			m_digits[i]--;
			return (*this);
		}
		else
		{
			m_digits[i]--;
		}
	}

	return (*this);
}

sav::Decimal& sav::Decimal::AmplifyInBase256(int _digits)
{
	for(int i = 0; i < _digits; i++)
	{
		m_digits.insert(m_digits.begin(), 0x00);
	}

	return (*this);
}

sav::Decimal& sav::Decimal::AmplifyInBase10(int _digits)
{
	for(int i = 0; i < _digits; i++)
	{
		this->operator*=(kDecimalWhichEqualBase10ToAmplifyFrom);
	}

	return (*this);
}

sav::DecimalStatus sav::Decimal::SetFromString(const std::string& _fromString)
{
	m_digits.clear();
	m_digits.push_back(0x00);

	int currentParseIndexFromEnd = 0;
	while(currentParseIndexFromEnd < _fromString.size())
	{
		auto fu = sav::Decimal{
			static_cast<std::uint64_t>(
				std::stoi(
					std::string{
						_fromString.rbegin() + currentParseIndexFromEnd,
						_fromString.rbegin() + currentParseIndexFromEnd + 1
					}
				)
			)
		}.AmplifyInBase10(currentParseIndexFromEnd);

		this->operator+=(
			fu
		);

		currentParseIndexFromEnd++;
	}

	return DecimalStatus::Ok;
}

sav::DecimalStatus sav::Decimal::CheckParseStringCoherency(const std::string& _stringToCheck)
{
	return DecimalStatus::Error_Underflow;
}