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

#ifndef DECIMAL_VLN_BCD_DECIMALINTEGERDIVISIONRESULT_H
#define DECIMAL_VLN_BCD_DECIMALINTEGERDIVISIONRESULT_H

#include "Decimal.h"

namespace sav
{
	/**
	 * @class DecimalIntegerDivisionResult
	 * Division result class - quotient, remainder and division status.
	 */
	class DecimalIntegerDivisionResult
	{
		friend class Decimal;

	public:
		// Returns true on coherent division, false otherwise (for example, if divided by zero)
		explicit operator bool() const noexcept;

		// Quotient (integer part of the division result)
		Decimal Quotient;

		// Remainder
		Decimal Remainder;

	protected:
		DecimalStatus m_divisionStatus = DecimalStatus::Ok;
	};
}

#endif //DECIMAL_VLN_BCD_DECIMALINTEGERDIVISIONRESULT_H
