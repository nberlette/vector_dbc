/*
 * Copyright (C) 2013-2019 Tobias Lorenz.
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

#include <Vector/DBC/platform.h>

#include <cstdint>
#include <string>
#include <vector>

#include <Vector/DBC/vector_dbc_export.h>

namespace Vector {
namespace DBC {

/** Attribute Value Type */
class VECTOR_DBC_EXPORT AttributeValueType {
public:
    AttributeValueType();

    /** Type */
    enum class Type {
        /** Integer */
        Int,

        /** Hex */
        Hex,

        /** Float */
        Float,

        /** String */
        String,

        /** Enum */
        Enum
    };

    /** @copydoc Type */
    Type type;

    /** Value Union */
    union {
        struct {
            int32_t minimum; /**< Min Value of type AttributeValueType::Int */
            int32_t maximum; /**< Min Value of type AttributeValueType::Int */
        } integerValue;

        struct {
            int32_t minimum; /**< Min Value of type AttributeValueType::Hex */
            int32_t maximum; /**< Max Value of type AttributeValueType::Hex */
        } hexValue;

        struct {
            double minimum; /**< Min Value of type AttributeValueType::Float */
            double maximum; /**< Max Value of type AttributeValueType::Float */
        } floatValue;

        // std::string has no default
    };

    /** Values of type AttributeValueType::Enum */
    std::vector<std::string> enumValues;
};

}
}
