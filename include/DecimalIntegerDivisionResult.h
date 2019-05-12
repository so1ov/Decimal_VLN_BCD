//
// DecimalIntegerDivisionResult.h
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

#ifndef DECIMAL_VLN_BCD_DECIMALINTEGERDIVISIONRESULT_H
#define DECIMAL_VLN_BCD_DECIMALINTEGERDIVISIONRESULT_H

#include "Decimal.h"

namespace sav
{
	/**
	 * @struct DecimalIntegerDivisionResult
	 * Division result struct - quotient and remainder.
	 */
	struct DecimalIntegerDivisionResult
	{
		// Quotient (integer part of the division result)
		Decimal Quotient;

		// Remainder
		Decimal Remainder;
	};
}

#endif //DECIMAL_VLN_BCD_DECIMALINTEGERDIVISIONRESULT_H
