//
// Copyright Artur Soloviev 2019
// soloviev.artur@gmail.com
//
// VLN.cpp
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

#include "VLN.h"

#include <numeric>

sav::VLN::VLN(std::uint64_t _initial)
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

sav::VLN::VLN()
{
	m_digits.push_back(0x00);
}

const sav::VLN sav::VLN::kVLNWhichEqualUInt64Max = sav::VLN{
	std::numeric_limits<std::uint64_t>::max()
};

std::optional<std::uint64_t> sav::VLN::ToUInt64() const
{
	if( (*this) > kVLNWhichEqualUInt64Max)
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

bool sav::VLN::operator==(const sav::VLN& _rhs) const
{
	return this->m_digits == _rhs.m_digits;
}

bool sav::VLN::operator!=(const sav::VLN& _rhs) const
{
	return !((*this) == _rhs);
}

bool sav::VLN::operator<(const sav::VLN& _rhs) const
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

bool sav::VLN::operator>(const sav::VLN& _rhs) const
{
	return !((*this) == _rhs) && !((*this) < _rhs);
}

bool sav::VLN::operator<=(const sav::VLN& _rhs) const
{
	return ((*this) < _rhs) || ((*this) == _rhs);
}

bool sav::VLN::operator>=(const sav::VLN& _rhs) const
{
	return (!((*this) < _rhs)) || ((*this) == _rhs);
}

std::uint64_t sav::VLN::UnsafeIntegerPower(std::uint64_t _base, std::uint64_t _index)
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

std::string sav::VLN::ToBase10() const
{


	return std::__cxx11::string();
}

sav::VLN::operator bool() const
{
	return m_ok;
}

sav::VLN sav::VLN::operator+(const sav::VLN& _rhs) const
{
	VLN result;
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
			if( (accumulator + this->m_digits[i]) < accumulator)
			{
				carry = 1;
			}

			// perform actual addition
			accumulator += this->m_digits[i];
		}

		if(i < _rhs.m_digits.size())
		{
			// overflow checking and set carry if needed
			if( (accumulator + _rhs.m_digits[i]) < accumulator)
			{
				carry = 1;
			}

			// perform actual addition
			accumulator += _rhs.m_digits[i];
		}

		result.m_digits.push_back(accumulator);
	}

	return result;
}

sav::VLN sav::VLN::operator-(const sav::VLN& _rhs) const
{
	VLN result;

	if( (*this) < _rhs)
	{
		result.m_ok = false;
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

sav::VLN sav::VLN::operator*(const sav::VLN& _rhs) const
{
	VLN result;

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
		VLN intermediateSum;
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
			carry = reg & static_cast<std::uint16_t>(0xFF'00);
		}

		// Accumulate intermediate sum
		result += intermediateSum;
	}

	return result;
}

sav::VLN sav::VLN::operator/(const sav::VLN& _rhs) const
{
	// TODO
	return VLN();
}

sav::VLN& sav::VLN::operator+=(const sav::VLN& _rhs)
{
	(*this) = (*this) + _rhs;

	return (*this);
}

sav::VLN& sav::VLN::operator-=(const sav::VLN& _rhs)
{
	(*this) = (*this) - _rhs;

	return (*this);
}

sav::VLN& sav::VLN::operator*=(const sav::VLN& _rhs)
{
	(*this) = (*this) * _rhs;

	return (*this);
}

sav::VLN& sav::VLN::operator/=(const sav::VLN& _rhs)
{
	(*this) = (*this) / _rhs;

	return (*this);
}

void sav::VLN::Normalize()
{
	for(int i = m_digits.size() - 1; i >= 0; i--)
	{
		if( m_digits[i] != 0x00 )
		{
			m_digits = std::vector<std::uint8_t>{m_digits.begin(), m_digits.begin() + i + 1};
			break;
		}
	}
}
