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

class ArithmeticTests
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

TEST_F(ArithmeticTests, Addition_WithoutCarry)
{
	m_decimal1 = DecimalTestWrapper{"10"};
	m_decimal2 = DecimalTestWrapper{"20"};

	auto result = m_decimal1 + m_decimal2;
	ASSERT_EQ(result, sav::Decimal{"30"});
}

TEST_F(ArithmeticTests, Addition_WithCarry)
{
	m_decimal1 = DecimalTestWrapper{"256"};
	m_decimal2 = DecimalTestWrapper{"2"};

	auto result = m_decimal1 + m_decimal2;
	ASSERT_EQ(result, sav::Decimal{"258"});
}

TEST_F(ArithmeticTests, Addition_ZeroByZero)
{
	m_decimal1 = DecimalTestWrapper{"0"};
	m_decimal2 = DecimalTestWrapper{"0"};

	auto result = m_decimal1 + m_decimal2;
	ASSERT_EQ(result, sav::Decimal{"0"});
}

TEST_F(ArithmeticTests, Addition_ToZero)
{
	m_decimal1 = DecimalTestWrapper{"0"};
	m_decimal2 = DecimalTestWrapper{"20"};

	auto result = m_decimal1 + m_decimal2;
	ASSERT_EQ(result, sav::Decimal{"20"});
}

TEST_F(ArithmeticTests, Addition_ByZero)
{
	m_decimal1 = DecimalTestWrapper{"10"};
	m_decimal2 = DecimalTestWrapper{"0"};

	auto result = m_decimal1 + m_decimal2;
	ASSERT_EQ(result, sav::Decimal{"10"});
}

TEST_F(ArithmeticTests, DivideByZero)
{
	m_decimal1.SetFromString("1");
	m_decimal2.SetFromString("0");

	auto result = m_decimal1 / m_decimal2;
	ASSERT_FALSE(result);
}

TEST_F(ArithmeticTests, DivideLess)
{
	m_decimal1 = DecimalTestWrapper{"10"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{});
	ASSERT_EQ(result->Remainder, m_decimal1);
}

TEST_F(ArithmeticTests, DivideEqual)
{
	m_decimal1 = DecimalTestWrapper{"15"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"1"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"0"});
}

TEST_F(ArithmeticTests, DivideGreater)
{
	m_decimal1 = DecimalTestWrapper{"16"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"1"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"1"});
}

TEST_F(ArithmeticTests, DivideGreaterByExp1WithoutRemainder)
{
	m_decimal1 = DecimalTestWrapper{"1500"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"100"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"0"});
}

TEST_F(ArithmeticTests, DivideGreaterByExp1WithRemainder)
{
	m_decimal1 = DecimalTestWrapper{"1501"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"100"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"1"});
}

TEST_F(ArithmeticTests, DivideGreaterBy10thExp2WithoutRemainder)
{
	m_decimal1 = DecimalTestWrapper{"15000"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"1000"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"0"});
}

TEST_F(ArithmeticTests, DivideGreaterBy10thExp2WithRemainder)
{
	m_decimal1 = DecimalTestWrapper{"15001"};
	m_decimal2 = DecimalTestWrapper{"15"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"1000"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"1"});
}

TEST_F(ArithmeticTests, DivideWithCarryAndWithRemainder)
{
	m_decimal1 = DecimalTestWrapper{"100000"};
	m_decimal2 = DecimalTestWrapper{"120"};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{"833"});
	ASSERT_EQ(result->Remainder, sav::Decimal{"40"});
}

TEST_F(ArithmeticTests, DivideBinaryGreaterByBinaryExp1WithoutRemainder)
{
	// Assume this platform has int max value greater than divident
	int divisor = 15;
	int divident = divisor * (std::numeric_limits<std::uint8_t>::max() + 1);

	m_decimal1 = DecimalTestWrapper{std::to_string(divident)};
	m_decimal2 = DecimalTestWrapper{std::to_string(divisor)};

	auto result = m_decimal1 / m_decimal2;
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->Quotient, sav::Decimal{std::to_string(divident / divisor)});
	ASSERT_EQ(result->Remainder, sav::Decimal{std::to_string(divident % divisor)});
}

class VATTests
	:	public ::testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}

	sav::Decimal k10 = sav::Decimal{"10"};
	sav::Decimal k20 = sav::Decimal{"20"};
	sav::Decimal k110 = sav::Decimal{"110"};
	sav::Decimal k120 = sav::Decimal{"120"};
};

TEST_F(VATTests, CalculateVATForOnePosition)
{
	// One product, which costs 10000 copecks = 100 roubles. VAT 20 included.
	sav::Decimal position = sav::Decimal{"10000"};

	// calculating VAT
	sav::Decimal intermediate = position * k20;
	auto result = intermediate.DivideAndRoundInBase10(k120);

	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result.value(), sav::Decimal{"1667"});
}

TEST_F(VATTests, CalculateVATForEntireTwoPositions)
{
	// One product x2, which costs 5000 copecks * 2 = 10000 copecks  = 100 roubles. VAT 20 included.
	sav::Decimal position1 = sav::Decimal{"5000"};
	sav::Decimal position2 = sav::Decimal{"5000"};

	// calculating VAT for position1
	sav::Decimal intermediate1 = position1 * k20;
	auto result1 = intermediate1.DivideAndRoundInBase10(k120);
	ASSERT_TRUE(result1.has_value());

	// calculating VAT for position2
	sav::Decimal intermediate2 = position2 * k20;
	auto result2 = intermediate2.DivideAndRoundInBase10(k120);
	ASSERT_TRUE(result2.has_value());

	auto total = result1.value() + result2.value();
	ASSERT_EQ(total, sav::Decimal{"1666"});
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}