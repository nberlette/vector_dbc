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
    bitSize(0),
    byteOrder(ByteOrder::BigEndian),
    valueType(ValueType::Unsigned),

    /* raw/physical conversion */
    factor(0.0),
    offset(0.0),
    minimumPhysicalValue(0.0),
    maximumPhysicalValue(0.0),

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

double Signal::minimumRawValue()
{
    /* calculate minimum raw value */
    double minimumRawValue;
    switch (extendedValueType) {
    case ExtendedValueType::Undefined:
    case ExtendedValueType::Integer:
        if (valueType == ValueType::Signed) {
            minimumRawValue = -(2<<(bitSize-2)); // bitSize-- because shift instead of pow
        } else {
            minimumRawValue = 0;
        }
        break;

    case ExtendedValueType::Float:
        minimumRawValue = 3.4e-38;
        break;

    case ExtendedValueType::Double:
        minimumRawValue = 1.7e-308;
        break;
    }
    return minimumRawValue;
}

double Signal::maximumRawValue()
{
    /* calculate maximum raw value */
    double maximumRawValue;
    switch (extendedValueType) {
    case ExtendedValueType::Undefined:
    case ExtendedValueType::Integer:
        if (valueType == ValueType::Signed) {
            maximumRawValue = (2<<(bitSize-2))-1; // bitSize-- because shift instead of pow
        } else {
            maximumRawValue = (2<<(bitSize-1))-1; // bitSize-- because shift instead of pow
        }
        break;

    case ExtendedValueType::Float:
        maximumRawValue = 3.4e38;
        break;

    case ExtendedValueType::Double:
        maximumRawValue = 1.7e308;
        break;
    }
    return maximumRawValue;
}

}
}
