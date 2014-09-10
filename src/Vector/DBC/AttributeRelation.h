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

#include "platform.h"

#include <map>
#include <string>

#include "Attribute.h"

#include "vector_dbc_export.h"

namespace Vector {
namespace DBC {

/**
 * Attribute Value on Relation (BA_REL)
 */
class VECTOR_DBC_EXPORT AttributeRelation : public Attribute {
public:
    AttributeRelation();

    /** Relation Type */
    enum class RelationType {
        ControlUnitEnvironmentVariable, /**< Control Unit - Env. Variable */
        NodeTxMessage, /**< Node - Tx Message */
        NodeMappedRxSignal /**< Node - Mapped Rx Signal */
    };

    /** Relation Type */
    RelationType relationType;

    /** Node Name */
    std::string nodeName;

    /** Environment Variable Name */
    std::string environmentVariableName;

    /** Message Identifier */
    unsigned int messageId;

    /** Signal Name */
    std::string signalName;

    bool operator < (const AttributeRelation & rhs) const;
};

}
}
