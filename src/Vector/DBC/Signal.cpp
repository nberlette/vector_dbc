/*
 * Copyright (C) 2013 Tobias Lorenz.
 * Contact: tobias.lorenz@gmx.net
 *
 * This file is part of Tobias Lorenz's Toolkit.
 *
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * GNU General Public License 3.0 Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 */

#include <algorithm>
#include <cmath>

#include "Signal.h"

namespace Vector {
namespace DBC {

Signal::Signal() :
    name(),

    /* multiplexer indicator */
    multiplexedSignal(false),
    multiplexerSwitchValue(0),
    multiplexorSwitch(false),

    /* position */
    startBit(0),
    size(0),
    byteOrder(ByteOrder::BigEndian),
    valueType(ValueType::Unsigned),

    /* raw/physical conversion */
    factor(0.0),
    offset(0.0),
    minimum(0.0),
    maximum(0.0),

    /* unit */
    unit(),

    /* receivers */
    receivers(),

    /* value type and description */
    extendedValueType(Signal::ExtendedValueType::Undefined),
    valueDescriptions(),
    type(),

    /* comments and attributes */
    comment(),
    attributeValues(),

    /* extended multiplexors */
    extendedMultiplexors()
{
    /* nothing to do here */
}

double Signal::rawToPhysicalValue(double rawValue)
{
    /* physicalValue = rawValue * factor + offset */
    return rawValue * factor + offset;
}

double Signal::physicalToRawValue(double physicalValue)
{
    /* safety check */
    if (factor == 0)
        return 0;

    /* rawValue = (physicalValue - offset) / factor */
    return (physicalValue - offset) / factor;
}

double Signal::getMinimumPhysicalValue()
{
    /* if this is not set to auto-calculation, return minimum */
    if ((minimum != 0.0) && (maximum != 0.0)) {
        return maximum;
    }

    /* calculate minimum */
    switch (extendedValueType) {
    case ExtendedValueType::Undefined:
    case ExtendedValueType::Integer:
        if (valueType == ValueType::Signed) {
            return -(2<<(size-1));
        } else {
            return 0;
        }
        break;

    case ExtendedValueType::Float:
        // precision: 7 digits
        return 3.4 * std::pow(10, -38);
        break;

    case ExtendedValueType::Double:
        // precision: 15 digits
        return 1.7 * std::pow(10, -308);
        break;
    }
}

double Signal::getMaximumPhysicalValue()
{
    /* if this is not set to auto-calculation, return maximum */
    if ((minimum != 0.0) && (maximum != 0.0)) {
        return maximum;
    }

    /* calculate maximum */
    switch (extendedValueType) {
    case ExtendedValueType::Undefined:
    case ExtendedValueType::Integer:
        if (valueType == ValueType::Signed) {
            return (2<<(size-1))-1;
        } else {
            return (2<< size   )-1;
        }
        break;

    case ExtendedValueType::Float:
        // precision: 7 digits
        return 3.4 * std::pow(10, 38);
        break;

    case ExtendedValueType::Double:
        // precision: 15 digits
        return 1.7 * std::pow(10, 308);
        break;
    }
}

}
}
