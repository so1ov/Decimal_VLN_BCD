//
// Copyright Artur Soloviev 2019
// soloviev.artur@gmail.com
//
// tests.cpp
//
// This file is part of Decimal Toolkit.
//
// Decimal Toolkit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Decimal Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Decimal Toolkit.  If not, see <https://www.gnu.org/licenses/>.
//

#include <Decimal.h>

#include "DecimalIntegerDivisionResult.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

class DecimalTestWrapper
	:	public sav::Decimal
{
public:
	DecimalTestWrapper()
		:	sav::Decimal()
	{

	}

	DecimalTestWrapper(std::uint64_t _initialUint64t)
		:	sav::Decimal(_initialUint64t)
	{

	}

	DecimalTestWrapper(const std::string& _fromString)
		:	sav::Decimal(_fromString)
	{

	}

	const std::vector<std::uint8_t>& GetInternalDigitsVector()
	{
		return m_digits;
	}
};

class DecimalDivisionTests
	:	public ::testing::Test
{
public:
	void SetUp()
	{
		m_decimal1 = DecimalTestWrapper{};
		m_decimal2 = DecimalTestWrapper{};
	}

	void TearDown()
	{

	}

	DecimalTestWrapper m_decimal1;
	DecimalTestWrapper m_decimal2;
};

TEST_F(DecimalDivisionTests, DivideByZero)
{
	m_decimal1.SetFromString("1");
	m_decimal2.SetFromString("0");

	auto result = m_decimal1 / m_decimal2;
	ASSERT_FALSE(result);
}

TEST_F(DecimalDivisionTests, DivideLess)
{
	m_decimal1 = DecimalTestWrapper{"10"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{});
	ASSERT_EQ(result->Remainder, m_decimal1);
}

TEST_F(DecimalDivisionTests, DivideEqual)
{
	m_decimal1 = DecimalTestWrapper{"15"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"1"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"0"});
}

TEST_F(DecimalDivisionTests, DivideGreater)
{
	m_decimal1 = DecimalTestWrapper{"16"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"1"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"1"});
}

TEST_F(DecimalDivisionTests, DivideGreaterByExp1WithoutRemainder)
{
	m_decimal1 = DecimalTestWrapper{"1500"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"100"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"0"});
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}