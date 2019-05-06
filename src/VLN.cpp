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
	// result = 0x02 * 256^0 + 0x00 * 256^1 + 0x01 * 256^2 == 2 + 0 + 65536 = 65538
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

bool sav::VLN::operator<(const sav::VLN &_rhs) const
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

bool sav::VLN::operator<=(const sav::VLN &_rhs) const
{
	return ((*this) < _rhs) || ((*this) == _rhs);
}

bool sav::VLN::operator>=(const sav::VLN &_rhs) const
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
