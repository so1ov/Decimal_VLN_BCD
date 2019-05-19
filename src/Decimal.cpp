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

#include "Decimal.h"

#include "DecimalIntegerDivisionResult.h"

#include <numeric>
#include <algorithm>

sav::Decimal::Decimal(unsigned int _initial)
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

const sav::Decimal sav::Decimal::kDecimalWhichEqualUnsignedIntMax = sav::Decimal{
	std::numeric_limits<unsigned int>::max()
};

const sav::Decimal sav::Decimal::kDecimalWhichEqualBase10 = sav::Decimal{
	sav::Decimal::kBase10
};

std::optional<unsigned int> sav::Decimal::ToUInt() const
{
	if( (*this) > kDecimalWhichEqualUnsignedIntMax)
	{
		return std::nullopt;
	}

	unsigned int result = 0;

	// m_digits == 0x02 0x00 0x01 // 65538
	// result = 0x02 * 256^0 + 0x00 * 256^1 + 0x01 * 256^2 == 2 + 0 + 65536 == 65538
	for(int i = 0; i < m_digits.size(); i++)
	{
		result += m_digits[i] * UnsafeIntegerPower(kBase256, i);
	}

	return std::optional<unsigned int>{result};
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

unsigned int sav::Decimal::UnsafeIntegerPower(unsigned int _base, unsigned int _index)
{
	if(_index == 0)
	{
		return 1;
	}

	if(_base == 0)
	{
		throw;
	}

	unsigned int result = 0;

	unsigned int accum = 1;
	for(unsigned int i = 0; i < _index; i++)
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
		intermediateResult = intermediateResult->Quotient / kDecimalWhichEqualBase10;
		result += std::to_string(intermediateResult->Remainder.ToUInt().value());
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

sav::DecimalIntegerDivisionResult sav::Decimal::operator/(const sav::Decimal& _rhs) const
{
	DecimalIntegerDivisionResult result;

	if(_rhs.m_digits.size() == 1 && _rhs.m_digits[0] == 0x00)
	{
		result.m_divisionStatus = DecimalStatus::Error_Underflow;
		return result;
	}

	if((*this) < _rhs)
	{
		result.Remainder = (*this);
		return result;
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

	return result;
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

	(*this).m_status = DecimalStatus::Error_Underflow;
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
		this->operator*=(kDecimalWhichEqualBase10);
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
		this->operator+=(
			sav::Decimal{
				static_cast<unsigned int>(
					std::stoi(
						std::string{
							_fromString.rbegin() + currentParseIndexFromEnd,
							_fromString.rbegin() + currentParseIndexFromEnd + 1
						}
					)
				)
			}.AmplifyInBase10(currentParseIndexFromEnd)
		);

		currentParseIndexFromEnd++;
	}

	return DecimalStatus::Ok;
}

sav::Decimal sav::Decimal::DivideAndRoundInBase10(const sav::Decimal& _divisor) const
{
	sav::Decimal result;

	auto divisionResult = (*this) / _divisor;
	if(!divisionResult)
	{
		// Seems to be division by zero, nothing to round.
		result.m_status = divisionResult.m_divisionStatus;
		return result;
	}

	// Division occured without remainder, nothing to round.
	if(divisionResult.Remainder.EqualsZero())
	{
		result = divisionResult.Quotient;
		return result;
	}

	/**
	 * To properly round in base10 using 4/5 rule, determine the most significant digit in the rounded part.
	 * Example:
	 * 10000 / 6 = 1666.666...
	 *                  ^ We need the first digit after .
	 *                  (6 > 5) -> (1666.666... -> 1667)
	 */
	auto intermediate = divisionResult.Remainder / kDecimalWhichEqualBase10;
	auto firstDigitToCompleteRounding = intermediate.Quotient.ToUInt();
	if(!firstDigitToCompleteRounding)
	{
		throw;
	}
	if(firstDigitToCompleteRounding.value() >= 5)
	{
		divisionResult.Quotient++;
	}

	result = divisionResult.Quotient;
	return result;
}
