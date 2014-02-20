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

#include "Database.h"

#include <clocale>
#include <fstream>
#include <string>

namespace Vector {
namespace DBC {

#if __cplusplus < 201103L
#define constexpr static const
#endif

constexpr char endl[] = "\r\n";

bool Database::save(const char * filename)
{
    std::ofstream ofs;

    std::setlocale(LC_ALL, "C");

    /* open stream */
    ofs.open(filename, std::ofstream::out | std::ofstream::trunc);
    if (!ofs.is_open()) {
        return false;
    }
    ofs.precision(16);

    /* Version (VERSION) */
    ofs << "VERSION \"" << version << "\"" << endl;
    ofs << endl;

    /* New Symbols (NS) */
    if (newSymbols.size() > 0) {
        ofs << endl;
        ofs << "NS_ : " << endl;
        for (auto newSymbol : newSymbols) {
            ofs << "\t" << newSymbol << endl;
        }
        ofs << endl;
    }

    /* Bit Timing (BS) */
    ofs << "BS_:";
    if (bitTiming.baudrate || bitTiming.btr1 || bitTiming.btr2) {
        ofs << ' ' << bitTiming.baudrate;
        ofs << ':' << bitTiming.btr1;
        ofs << ':' << bitTiming.btr2;
        ofs << ';';
    }
    ofs << endl;
    ofs << endl;

    /* Nodes (BU) */
    ofs << "BU_:";
    for (auto node : nodes) {
        ofs << " " << node.second.name;
    }
    ofs << endl;

    /* Value Tables (VAL_TABLE) */
    for (auto valueTable : valueTables) {
        ofs << "VAL_TABLE_ " << valueTable.second.name;
        for (auto valueDescription : valueTable.second.valueDescriptions) {
            ofs << " " << valueDescription.first;
            ofs << " \"" << valueDescription.second << "\"";
        }
        ofs << " ;" << endl;
    }
    ofs << endl;
    ofs << endl;

    /* Messages (BO) */
    for (auto message : messages) {
        ofs << "BO_ " << message.second.id;
        ofs << " " << message.second.name;
        ofs << ": " << message.second.size << " ";
        if (message.second.transmitter.empty()) {
            ofs << "Vector__XXX";
        } else {
            ofs << message.second.transmitter;
        }
        ofs << endl;

        /* Signals (SG) */
        for (auto signal : message.second.signals) {
            ofs << " SG_ " << signal.second.name;
            if (signal.second.multiplexedSignal) {
                ofs << 'm';
                ofs << signal.second.multiplexerSwitchValue;
            }
            if (signal.second.multiplexorSwitch) {
                ofs << 'M';
            }
            if (signal.second.multiplexedSignal || signal.second.multiplexorSwitch) {
                ofs << ' ';
            }
            ofs << " : ";
            ofs << signal.second.startBit << '|' << signal.second.size << '@' << char(signal.second.byteOrder) << char(signal.second.valueType);
            ofs << ' ';
            ofs << '(' << signal.second.factor << ',' << signal.second.offset << ')';
            ofs << ' ';
            ofs << '[' << signal.second.minimum << '|' << signal.second.maximum << ']';
            ofs << ' ';
            ofs << '"' << signal.second.unit << '"';
            ofs << ' ';
            if (signal.second.receivers.empty()) {
                ofs << "Vector__XXX";
            } else {
                for (auto receiver : signal.second.receivers) {
                    ofs << " " << receiver;
                }
            }
            ofs << endl;
        }

        ofs << endl;
    }

    /* Message Transmitters (BO_TX_BU) */
    for (auto message : messages) {
        if (!message.second.transmitters.empty()) {
            ofs << "BO_TX_BU_ " << message.second.id << " :";
            for (auto transmitter : message.second.transmitters) {
                ofs << ' ' << transmitter;
            }
            ofs << ';' << endl;
        }
    }
    ofs << endl;

    /* Environment Variables (EV) */
    for (auto environmentVariable : environmentVariables) {
        ofs << endl;
        ofs << "EV_ " << environmentVariable.second.name << ": ";
        switch (environmentVariable.second.type) {
        case EnvironmentVariable::Type::Integer:
        case EnvironmentVariable::Type::String:
        case EnvironmentVariable::Type::Data:
            ofs << '0';
            break;
        case EnvironmentVariable::Type::Float:
            ofs << '1';
            break;
        }
        ofs << " [";
        ofs << environmentVariable.second.minimum;
        ofs << '|';
        ofs << environmentVariable.second.maximum;
        ofs << "] \"";
        ofs << environmentVariable.second.unit;
        ofs << "\" ";
        ofs << environmentVariable.second.initialValue;
        ofs << ' ';
        ofs << environmentVariable.second.id;
        ofs << " DUMMY_NODE_VECTOR";
        ofs << std::hex;
        if (environmentVariable.second.type == EnvironmentVariable::Type::String) {
            ofs << ((unsigned int)(environmentVariable.second.accessType) | 0x8000);
        } else {
            ofs << (unsigned int)(environmentVariable.second.accessType);
        }
        ofs << std::dec;
        ofs << ' ';
        if (environmentVariable.second.accessNodes.empty()) {
            ofs << "VECTOR__XXX";
        } else {
            bool first = true;
            for (auto accessNode : environmentVariable.second.accessNodes) {
                if (first) {
                    first = false;
                } else {
                    ofs << ',';
                }
                ofs << accessNode;
            }
        }
        ofs << ";" << endl;
    }

    /* Environment Variables Data (ENVVAR_DATA) */
    for (auto environmentVariable : environmentVariables) {
        if (environmentVariable.second.type == EnvironmentVariable::Type::Data) {
            ofs << "ENVVAR_DATA_ " << environmentVariable.second.name;
            ofs << ": " << environmentVariable.second.dataSize;
            ofs << ";" << endl;
        }
    }
    ofs << endl; // this might go below SGTYPE

    /* Signal Types (SGTYPE, obsolete) */
    for (auto signalType : signalTypes) {
        ofs << "SGTYPE_ " << signalType.second.name;
        ofs << " : " << signalType.second.size;
        ofs << '@' << char(signalType.second.byteOrder);
        ofs << ' ' << char(signalType.second.valueType);
        ofs << ' ' << signalType.second.defaultValue;
        ofs << ", " << signalType.second.valueTable;
        ofs << ';' << endl;
    }
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (!signal.second.type.empty()) {
                ofs << "SGTYPE_ " << message.second.id << ' ' << signal.second.name << " : " << signal.second.type << ";" << endl;
            }
        }
    }

    /* Comments (CM) */
    if (!comment.empty()) {
        ofs << "CM_ \"" << comment << "\";" << endl;
    }
    for (auto node : nodes) {
        if (!node.second.comment.empty()) {
            ofs << "CM_ BU_ " << node.second.name << " \"" << node.second.comment << "\";" << endl;
        }
    }
    for (auto message : messages) {
        if (!message.second.comment.empty()) {
            ofs << "CM_ BO_ " << message.second.id << " \"" << message.second.comment << "\";" << endl;
        }
    }
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (!signal.second.comment.empty()) {
                ofs << "CM_ SG_ " << message.second.id << ' ' << signal.second.name << " \"" << signal.second.comment << "\";" << endl;
            }
        }
    }
    for (auto environmentVariable : environmentVariables) {
        if (!environmentVariable.second.comment.empty()) {
            ofs << "CM_ EV_ " << environmentVariable.second.name << " \"" << environmentVariable.second.comment << "\";" << endl;
        }
    }

    /* Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
    for (auto attributeDefinition : attributeDefinitions) {
        switch(attributeDefinition.second.objectType) {
        case AttributeDefinition::ObjectType::Network:
            ofs << "BA_DEF_ ";
            break;
        case AttributeDefinition::ObjectType::Node:
            ofs << "BA_DEF_ BU_ ";
            break;
        case AttributeDefinition::ObjectType::Message:
            ofs << "BA_DEF_ BO_ ";
            break;
        case AttributeDefinition::ObjectType::Signal:
            ofs << "BA_DEF_ SG_ ";
            break;
        case AttributeDefinition::ObjectType::EnvironmentVariable:
            ofs << "BA_DEF_ EV_ ";
            break;
        case AttributeDefinition::ObjectType::ControlUnitEnvironmentVariable:
            ofs << "BA_DEF_REL_ BU_EV_REL_ ";
            break;
        case AttributeDefinition::ObjectType::NodeTxMessage:
            ofs << "BA_DEF_REL_ BU_BO_REL_ ";
            break;
        case AttributeDefinition::ObjectType::NodeMappedRxSignal:
            ofs << "BA_DEF_REL_ BU_SG_REL_ ";
            break;
        }
        ofs << " \"" << attributeDefinition.second.name << "\" ";
        switch(attributeDefinition.second.valueType) {
        case AttributeValueType::Int:
            ofs << "INT ";
            ofs << attributeDefinition.second.minimumIntegerValue;
            ofs << " ";
            ofs << attributeDefinition.second.maximumIntegerValue;
            break;
        case AttributeValueType::Hex:
            ofs << "HEX ";
            ofs << attributeDefinition.second.minimumHexValue;
            ofs << " ";
            ofs << attributeDefinition.second.maximumHexValue;
            break;
        case AttributeValueType::Float:
            ofs << "FLOAT ";
            ofs << attributeDefinition.second.minimumFloatValue;
            ofs << " ";
            ofs << attributeDefinition.second.maximumFloatValue;
            break;
        case AttributeValueType::String:
            ofs << "STRING ";
            break;
        case AttributeValueType::Enum:
            ofs << "ENUM  ";
            bool first = true;
            for (auto enumValue : attributeDefinition.second.enumValues) {
                if (first) {
                    first = false;
                } else {
                    ofs << ',';
                }
                ofs << "\"" << enumValue << "\"";
            }
            break;
        }
        ofs << ";" << endl;
    }

    /* Sigtype Attr List (?, obsolete) */

    /* Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
    for (auto attribute : attributeDefaults) {
        AttributeDefinition attributeDefinition = attributeDefinitions[attribute.second.name];
        switch(attributeDefinition.objectType) {
        case AttributeDefinition::ObjectType::Network:
        case AttributeDefinition::ObjectType::Node:
        case AttributeDefinition::ObjectType::Message:
        case AttributeDefinition::ObjectType::Signal:
        case AttributeDefinition::ObjectType::EnvironmentVariable:
            ofs << "BA_DEF_DEF_";
            break;
        case AttributeDefinition::ObjectType::ControlUnitEnvironmentVariable:
        case AttributeDefinition::ObjectType::NodeTxMessage:
        case AttributeDefinition::ObjectType::NodeMappedRxSignal:
            ofs << "BA_DEF_DEF_REL_";
            break;
        }
        ofs << "  \"" << attribute.second.name << "\" ";
        switch(attribute.second.valueType) {
        case AttributeValueType::Int:
            ofs << attribute.second.integerValue;
            break;
        case AttributeValueType::Hex:
            ofs << attribute.second.hexValue;
            break;
        case AttributeValueType::Float:
            ofs << attribute.second.floatValue;
            break;
        case AttributeValueType::String:
            ofs << '"' << attribute.second.stringValue << '"';
            break;
        case AttributeValueType::Enum:
            ofs << '"' << attribute.second.stringValue << '"';
            break;
        }
        ofs << ';' << endl;
    }

    /* Attribute Values (BA) for Network */
    for (auto attribute : attributeValues) {
        ofs << "BA_ \"" << attribute.second.name << "\" ";
        switch(attribute.second.valueType) {
        case AttributeValueType::Int:
            ofs << attribute.second.integerValue;
            break;
        case AttributeValueType::Hex:
            ofs << attribute.second.hexValue;
            break;
        case AttributeValueType::Float:
            ofs << attribute.second.floatValue;
            break;
        case AttributeValueType::String:
            ofs << '"' << attribute.second.stringValue << '"';
            break;
        case AttributeValueType::Enum:
            ofs << attribute.second.enumValue;
            break;
        }
        ofs << ';' << endl;
    }

    /* Attribute Values (BA) for Nodes (BU) */
    for (auto node : nodes) {
        for (auto attribute : node.second.attributeValues) {
            ofs << "BA_ \"" << attribute.second.name << "\" ";
            ofs << "BU_ " << node.second.name << ' ';
            switch(attribute.second.valueType) {
            case AttributeValueType::Int:
                ofs << attribute.second.integerValue;
                break;
            case AttributeValueType::Hex:
                ofs << attribute.second.hexValue;
                break;
            case AttributeValueType::Float:
                ofs << attribute.second.floatValue;
                break;
            case AttributeValueType::String:
                ofs << '"' << attribute.second.stringValue << '"';
                break;
            case AttributeValueType::Enum:
                ofs << attribute.second.enumValue;
                break;
            }
            ofs << ';' << endl;
        }
    }

    /* Attribute Values (BA) for Messages (BO) */
    for (auto message : messages) {
        for (auto attribute : message.second.attributeValues) {
            ofs << "BA_ \"" << attribute.second.name << "\" ";
            ofs << "BO_ " << message.second.id << ' ';
            switch(attribute.second.valueType) {
            case AttributeValueType::Int:
                ofs << attribute.second.integerValue;
                break;
            case AttributeValueType::Hex:
                ofs << attribute.second.hexValue;
                break;
            case AttributeValueType::Float:
                ofs << attribute.second.floatValue;
                break;
            case AttributeValueType::String:
                ofs << '"' << attribute.second.stringValue << '"';
                break;
            case AttributeValueType::Enum:
                ofs << attribute.second.enumValue;
                break;
            }
            ofs << ';' << endl;
        }
    }

    /* Attribute Values (BA) for Signals (SG) */
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            for (auto attribute : signal.second.attributeValues) {
                ofs << "BA_ \"" << attribute.second.name << "\" ";
                ofs << "SG_ " << message.second.id << ' ' << signal.second.name << ' ';
                switch(attribute.second.valueType) {
                case AttributeValueType::Int:
                    ofs << attribute.second.integerValue;
                    break;
                case AttributeValueType::Hex:
                    ofs << attribute.second.hexValue;
                    break;
                case AttributeValueType::Float:
                    ofs << attribute.second.floatValue;
                    break;
                case AttributeValueType::String:
                    ofs << '"' << attribute.second.stringValue << '"';
                    break;
                case AttributeValueType::Enum:
                    ofs << attribute.second.enumValue;
                    break;
                }
                ofs << ';' << endl;
            }
        }
    }

    /* Attribute Values (BA) for Environment Variables (EV) */
    for (auto environmentVariable : environmentVariables) {
        for (auto attribute : environmentVariable.second.attributeValues) {
            ofs << "BA_ \"" << attribute.second.name << "\" ";
            ofs << "EV_ " << environmentVariable.second.name << ' ';
            switch(attribute.second.valueType) {
            case AttributeValueType::Int:
                ofs << attribute.second.integerValue;
                break;
            case AttributeValueType::Hex:
                ofs << attribute.second.hexValue;
                break;
            case AttributeValueType::Float:
                ofs << attribute.second.floatValue;
                break;
            case AttributeValueType::String:
                ofs << '"' << attribute.second.stringValue << '"';
                break;
            case AttributeValueType::Enum:
                ofs << attribute.second.enumValue;
                break;
            }
            ofs << ';' << endl;
        }
    }

    /* Attribute Values for Relations (BA_REL) for relation  */
    for (auto attributeRelation : attributeRelationValues) {
        ofs << "BA_REL_ \"" << attributeRelation.name << "\" ";
        switch(attributeRelation.relationType) {
        case AttributeRelation::RelationType::ControlUnitEnvironmentVariable:
            ofs << "BU_EV_REL_ ";
            ofs << attributeRelation.nodeName;
            ofs << ' ';
            ofs << attributeRelation.environmentVariableName;
            break;
        case AttributeRelation::RelationType::NodeTxMessage:
            ofs << "BU_BO_REL_ ";
            ofs << attributeRelation.nodeName;
            ofs << ' ';
            ofs << attributeRelation.messageId;
            break;
        case AttributeRelation::RelationType::NodeMappedRxSignal:
            ofs << "BU_SG_REL_ ";
            ofs << attributeRelation.nodeName;
            ofs << " SG_ ";
            ofs << attributeRelation.messageId;
            ofs << ' ';
            ofs << attributeRelation.signalName;
            break;
        }
        ofs << ' ';
        switch(attributeRelation.valueType) {
        case AttributeValueType::Int:
            ofs << attributeRelation.integerValue;
            break;
        case AttributeValueType::Hex:
            ofs << attributeRelation.hexValue;
            break;
        case AttributeValueType::Float:
            ofs << attributeRelation.floatValue;
            break;
        case AttributeValueType::String:
            ofs << '"' << attributeRelation.stringValue << '"';
            break;
        case AttributeValueType::Enum:
            ofs << attributeRelation.enumValue;
            break;
        }
        ofs << ';' << endl;
    }

    /* Value Descriptions (VAL) */
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (!signal.second.valueDescriptions.empty()) {
                ofs << "VAL_ " << message.second.id << ' ' << signal.second.name;
                for (auto valueDescription : signal.second.valueDescriptions) {
                    ofs << " " << valueDescription.first;
                    ofs << " \"" << valueDescription.second << "\"";
                }
                ofs << " ;" << endl;
            }
        }
    }
    for (auto environmentVariable : environmentVariables) {
        if (!environmentVariable.second.valueDescriptions.empty()) {
            ofs << "VAL_ " << environmentVariable.second.name;
            for (auto valueDescription : environmentVariable.second.valueDescriptions) {
                ofs << " " << valueDescription.first;
                ofs << " \"" << valueDescription.second << "\"";
            }
            ofs << " ;" << endl;
        }
    }

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filter (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (!signal.second.type.empty()) {
                ofs << "SGTYPE_ " << message.second.id << ' ' << signal.second.name;
                ofs << " : " << signal.second.type << ';' << endl;
            }
        }
    }

    /* Signal Groups (SIG_GROUP) */
    for (auto message : messages) {
        for (auto signalGroup : message.second.signalGroups) {
            ofs << "SIG_GROUP_ " << message.second.id << ' ' << signalGroup.second.name;
            ofs << ' ' << signalGroup.second.repetitions;
            bool first = true;
            for (auto signal : signalGroup.second.signals) {
                if (first) {
                    first = false;
                } else {
                    ofs << ',';
                }
                ofs << signal;
            }
            ofs << ';' << endl;
        }
    }

    /* Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (signal.second.extendedValueType  !=  Signal::ExtendedValueType::Undefined) {
                ofs << "SIG_VALTYPE_ " << message.second.id << ' ' << signal.second.name;
                ofs << " : " << char(signal.second.extendedValueType);
                ofs << endl;
            }
        }
    }

    /* Extended Multiplexing (SG_MUL_VAL) */
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            for (auto extendedMultiplexor : signal.second.extendedMultiplexors) {
                ofs << "SG_MUL_VAL_ " << message.second.id << ' ' << signal.second.name;
                ofs << ' ' << extendedMultiplexor.second.switchName;
                bool first = true;
                for (auto valueRange : extendedMultiplexor.second.valueRanges) {
                    if (first) {
                        first = false;
                    } else {
                        ofs << ", ";
                    }
                    ofs << valueRange.first << '-' << valueRange.second;
                }
                ofs << ';' << endl;
            }
        }
    }

    /* close stream */
    ofs << endl;
    ofs.close();

    return true;
}

bool Database::save(std::string & filename)
{
    return save(filename.c_str());
}

}
}
