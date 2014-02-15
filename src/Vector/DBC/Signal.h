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

#pragma once

#include "config.h"

#include <map>
#include <set>
#include <string>

#include "Attribute.h"
#include "ByteOrder.h"
#include "ExtendedMultiplexor.h"
#include "ValueDescriptions.h"
#include "ValueType.h"

#include "vector_dbc_export.h"

namespace Vector {
namespace DBC {

/**
 * Signal (SG)
 */
class VECTOR_DBC_EXPORT Signal
{
public:
    Signal();

    /** Name */
    std::string name;

    /** Multiplexed Signal (m) */
    bool multiplexedSignal; // m

    /** Multiplexer Switch Value */
    unsigned int multiplexerSwitchValue;

    /** Multiplexor Switch (M) */
    bool multiplexorSwitch; // M

    /** Start Bit */
    unsigned int startBit;

    /** Size */
    unsigned int size;

    /** Byte Order */
    ByteOrder byteOrder;

    /** Value Type */
    ValueType valueType;

    /** Factor */
    double factor;

    /** Offset */
    double offset;

    /** Minimun */
    double minimum;

    /** Maximum */
    double maximum;

    /** Unit */
    std::string unit;

    /** Receivers */
    std::set<std::string> receivers;

    /**
     * @brief Convert from Raw to Physical Value
     * @param[in] physicalValue Physical Value
     * @return Raw Value
     *
     * Converts a value from raw to physical representation.
     */
    double rawToPhysicalValue(double physicalValue);

    /**
     * @brief Convert from Physical to Raw Value
     * @param[in] rawValue Raw Value
     * @return Physical Value
     *
     * Converts a value from physical to raw representation.
     */
    double physicalToRawValue(double rawValue);

    /** Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    enum class ExtendedValueType : char {
        Undefined = ' ',
        Integer = '0',
        Float = '1',
        Double = '2'
    };

    /** Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    ExtendedValueType extendedValueType;

    /** Value Descriptions (VAL) */
    ValueDescriptions valueDescriptions;

    /** Signal Type Refs (SGTYPE, obsolete) */
    std::string type;

    /** Comment (CM) */
    std::string comment;

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues;

    /** Extended Multiplexors (SG_MUL_VAL) */
    std::map<std::string, ExtendedMultiplexor> extendedMultiplexors;
};

}
}
