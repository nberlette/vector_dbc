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

/* this is the standard line ending for this file format */
#ifdef _WINDOWS
#define endl std::endl;
#else
// std::endl under linux would just generate a "\n"
constexpr char endl[] = "\r\n";
#endif

/* Version (VERSION) */
void Database::writeVersion(std::ofstream & ofs)
{
    ofs << "VERSION \"" << version << "\"" << endl;
    ofs << endl;
}

/* New Symbols (NS) */
void Database::writeNewSymbols(std::ofstream & ofs)
{
    if (newSymbols.size() > 0) {
        ofs << endl;
        ofs << "NS_ : " << endl;
        for (auto newSymbol : newSymbols) {
            ofs << "\t" << newSymbol << endl;
        }
        ofs << endl;
    }
}

/* Bit Timing (BS) */
void Database::writeBitTiming(std::ofstream & ofs)
{
    ofs << "BS_:";
    if (bitTiming.baudrate || bitTiming.btr1 || bitTiming.btr2) {
        ofs << ' ' << bitTiming.baudrate;
        ofs << ':' << bitTiming.btr1;
        ofs << ':' << bitTiming.btr2;
        ofs << ';';
    }
    ofs << endl;
    ofs << endl;
}

/* Nodes (BU) */
void Database::writeNodes(std::ofstream & ofs)
{
    ofs << "BU_:";
    for (auto node : nodes) {
        ofs << " " << node.second.name;
    }
    ofs << endl;
}

/* Value Tables (VAL_TABLE) */
void Database::writeValueTables(std::ofstream & ofs)
{
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
}

/* Signals (SG) */
void Database::writeSignals(std::ofstream & ofs, Message & message)
{
    for (auto signal : message.signals) {
        /* Name */
        ofs << " SG_ " << signal.second.name << ' ';

        /* Multiplexed Signal, Multiplexor Switch/Signal */
        if (signal.second.multiplexedSignal) {
            ofs << 'm' << signal.second.multiplexerSwitchValue;
        }
        if (signal.second.multiplexorSwitch) {
            ofs << 'M';
        }
        if (signal.second.multiplexedSignal || signal.second.multiplexorSwitch) {
            ofs << ' ';
        }
        ofs << ": ";

        /* Start Bit, Size, Byte Order, Value Type */
        ofs << signal.second.startBit << '|' << signal.second.size << '@' << char(signal.second.byteOrder) << char(signal.second.valueType);

        /* Factor, Offset */
        ofs << " (" << signal.second.factor << ',' << signal.second.offset << ')';

        /* Minimum, Maximum */
        ofs << " [" << signal.second.minimum << '|' << signal.second.maximum << ']';

        /* Unit */
        ofs << " \"" << signal.second.unit << "\" ";

        /* Receivers */
        if (signal.second.receivers.empty()) {
            ofs << "Vector__XXX";
        } else {
            for (auto receiver : signal.second.receivers) {
                ofs << " " << receiver;
            }
        }
        ofs << endl;
    }
}

/* Messages (BO) */
void Database::writeMessages(std::ofstream & ofs)
{
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
        writeSignals(ofs, message.second);

        ofs << endl;
    }
}

/* Message Transmitters (BO_TX_BU) */
void Database::writeMessageTransmitters(std::ofstream & ofs)
{
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
}

/* Environment Variables (EV) */
void Database::writeEnvironmentVariables(std::ofstream & ofs)
{
    for (auto environmentVariable : environmentVariables) {
        ofs << endl;
        ofs << "EV_ " << environmentVariable.second.name << ": ";

        /* Type */
        switch (environmentVariable.second.type) {
        // Integer, String, Data
        case EnvironmentVariable::Type::Integer:
        case EnvironmentVariable::Type::String:
        case EnvironmentVariable::Type::Data:
            ofs << '0';
            break;
        // Float
        case EnvironmentVariable::Type::Float:
            ofs << '1';
            break;
        }

        /* Minimum, Maximum */
        ofs << " [";
        ofs << environmentVariable.second.minimum;
        ofs << '|';
        ofs << environmentVariable.second.maximum;
        ofs << ']';

        /* Unit */
        ofs << " \"";
        ofs << environmentVariable.second.unit;
        ofs << "\" ";

        /* Initial Value */
        ofs << environmentVariable.second.initialValue;
        ofs << ' ';

        /* ID */
        ofs << environmentVariable.second.id;

        /* Access Type */
        ofs << " DUMMY_NODE_VECTOR";
        ofs << std::hex;
        if (environmentVariable.second.type == EnvironmentVariable::Type::String) {
            ofs << ((unsigned int)(environmentVariable.second.accessType) | 0x8000);
        } else {
            ofs << (unsigned int)(environmentVariable.second.accessType);
        }
        ofs << std::dec;
        ofs << ' ';

        /* Access Nodes */
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
}

/* Environment Variables Data (ENVVAR_DATA) */
void Database::writeEnvironmentVariableData(std::ofstream & ofs)
{
    for (auto environmentVariable : environmentVariables) {
        if (environmentVariable.second.type == EnvironmentVariable::Type::Data) {
            ofs << "ENVVAR_DATA_ " << environmentVariable.second.name;
            ofs << ": " << environmentVariable.second.dataSize;
            ofs << ";" << endl;
        }
    }
    ofs << endl; // this might go below SGTYPE
}

/* Signal Types (SGTYPE, obsolete) */
void Database::writeSignalTypes(std::ofstream & ofs)
{
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
}

/* Comments (CM) for Networks */
void Database::writeCommentsNetworks(std::ofstream & ofs)
{
    if (!comment.empty()) {
        ofs << "CM_ \"" << comment << "\";" << endl;
    }
}

/* Comments (CM) for Node (BU) */
void Database::writeCommentsNodes(std::ofstream & ofs)
{
    for (auto node : nodes) {
        if (!node.second.comment.empty()) {
            ofs << "CM_ BU_ " << node.second.name << " \"" << node.second.comment << "\";" << endl;
        }
    }
}

/* Comments (CM) for Message (BO) */
void Database::writeCommentsMessages(std::ofstream & ofs)
{
    for (auto message : messages) {
        if (!message.second.comment.empty()) {
            ofs << "CM_ BO_ " << message.second.id << " \"" << message.second.comment << "\";" << endl;
        }
    }
}

/* Comments (CM) for Signal (SG) */
void Database::writeCommentsSignals(std::ofstream & ofs)
{
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (!signal.second.comment.empty()) {
                ofs << "CM_ SG_ " << message.second.id << ' ' << signal.second.name << " \"" << signal.second.comment << "\";" << endl;
            }
        }
    }
}

/* Comments (CM) for Environment Variable (EV) */
void Database::writeCommentsEnvironmentVariables(std::ofstream & ofs)
{
    for (auto environmentVariable : environmentVariables) {
        if (!environmentVariable.second.comment.empty()) {
            ofs << "CM_ EV_ " << environmentVariable.second.name << " \"" << environmentVariable.second.comment << "\";" << endl;
        }
    }
}

/* Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
void Database::writeAttributeDefinitions(std::ofstream & ofs)
{
    for (auto attributeDefinition : attributeDefinitions) {
        /* Object Type */
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

        /* Name */
        ofs << " \"" << attributeDefinition.second.name << "\" ";

        /* Value Type */
        switch(attributeDefinition.second.valueType) {
        // integer
        case AttributeValueType::Int:
            ofs << "INT ";
            ofs << attributeDefinition.second.minimumIntegerValue;
            ofs << " ";
            ofs << attributeDefinition.second.maximumIntegerValue;
            break;

        // hexadecimal
        case AttributeValueType::Hex:
            ofs << "HEX ";
            ofs << attributeDefinition.second.minimumHexValue;
            ofs << " ";
            ofs << attributeDefinition.second.maximumHexValue;
            break;

        // float
        case AttributeValueType::Float:
            ofs << "FLOAT ";
            ofs << attributeDefinition.second.minimumFloatValue;
            ofs << " ";
            ofs << attributeDefinition.second.maximumFloatValue;
            break;

        // string
        case AttributeValueType::String:
            ofs << "STRING ";
            break;

        // enumeration
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
}

/* Sigtype Attr Lists (?, obsolete) */

/* Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
void Database::writeAttributeDefaults(std::ofstream & ofs)
{
    for (auto attribute : attributeDefaults) {
        AttributeDefinition attributeDefinition = attributeDefinitions[attribute.second.name];

        /* Object Type */
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

        /* Name */
        ofs << "  \"" << attribute.second.name << "\" ";

        /* Value */
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
}

/* Attribute Values (BA) for Network */
void Database::writeAttributeValuesNetworks(std::ofstream & ofs)
{
    for (auto attribute : attributeValues) {
        /* Name */
        ofs << "BA_ \"" << attribute.second.name << "\" ";

        /* Value */
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

/* Attribute Values (BA) for Nodes (BU) */
void Database::writeAttributeValuesNodes(std::ofstream & ofs)
{
    for (auto node : nodes) {
        for (auto attribute : node.second.attributeValues) {
            /* Name */
            ofs << "BA_ \"" << attribute.second.name << "\" ";

            /* Node Name */
            ofs << "BU_ " << node.second.name << ' ';

            /* Value */
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

/* Attribute Values (BA) for Messages (BO) */
void Database::writeAttributeValuesMessages(std::ofstream & ofs)
{
    for (auto message : messages) {
        for (auto attribute : message.second.attributeValues) {
            /* Name */
            ofs << "BA_ \"" << attribute.second.name << "\" ";

            /* Message Name */
            ofs << "BO_ " << message.second.id << ' ';

            /* Value */
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

/* Attribute Values (BA) for Signals (SG) */
void Database::writeAttributeValuesSignals(std::ofstream & ofs)
{
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            for (auto attribute : signal.second.attributeValues) {
                /* Name */
                ofs << "BA_ \"" << attribute.second.name << "\" ";

                /* Message Identifier, Signal Name */
                ofs << "SG_ " << message.second.id << ' ' << signal.second.name << ' ';

                /* Value */
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
}

/* Attribute Values (BA) for Environment Variables (EV) */
void Database::writeAttributeValuesEnvironmentVariables(std::ofstream & ofs)
{
    for (auto environmentVariable : environmentVariables) {
        for (auto attribute : environmentVariable.second.attributeValues) {
            /* Name */
            ofs << "BA_ \"" << attribute.second.name << "\" ";

            /* Environment Variable Name */
            ofs << "EV_ " << environmentVariable.second.name << ' ';

            /* Value */
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

/* Attribute Values at Relations (BA_REL)  */
void Database::writeAttributeRelationValues(std::ofstream & ofs)
{
    for (auto attributeRelation : attributeRelationValues) {
        /* Name */
        ofs << "BA_REL_ \"" << attributeRelation.name << "\" ";

        /* Relation Type */
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

        /* Value Type */
        switch(attributeRelation.valueType) {
        // integer
        case AttributeValueType::Int:
            ofs << attributeRelation.integerValue;
            break;

        // hexadecimal
        case AttributeValueType::Hex:
            ofs << attributeRelation.hexValue;
            break;

        // float
        case AttributeValueType::Float:
            ofs << attributeRelation.floatValue;
            break;

        // string
        case AttributeValueType::String:
            ofs << '"' << attributeRelation.stringValue << '"';
            break;

        // enumeration
        case AttributeValueType::Enum:
            ofs << attributeRelation.enumValue;
            break;
        }
        ofs << ';' << endl;
    }
}

/* Value Descriptions (VAL) for Signals (SG) */
void Database::writeValueDescriptionsSignals(std::ofstream & ofs)
{
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
}

/* Value Descriptions (VAL) for Environment Variables (EV) */
void Database::writeValueDescriptionsEnvironmentVariables(std::ofstream & ofs)
{
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
}

/* Category Definitions (CAT_DEF, obsolete) */

/* Categories (CAT, obsolete) */

/* Filters (FILTER, obsolete) */

/* Signal Type Refs (SGTYPE, obsolete) */

/* Signal Groups (SIG_GROUP) */
void Database::writeSignalGroups(std::ofstream & ofs)
{
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
}

/* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
void Database::writeSignalExtendedValueTypes(std::ofstream & ofs)
{
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            if (signal.second.extendedValueType  !=  Signal::ExtendedValueType::Undefined) {
                ofs << "SIG_VALTYPE_ " << message.second.id << ' ' << signal.second.name;
                ofs << " : " << char(signal.second.extendedValueType);
                ofs << endl;
            }
        }
    }
}

/* Extended Multiplexors (SG_MUL_VAL) */
void Database::writeExtendedMultiplexors(std::ofstream & ofs)
{
    for (auto message : messages) {
        for (auto signal : message.second.signals) {
            for (auto extendedMultiplexor : signal.second.extendedMultiplexors) {
                /* Identifier, Name */
                ofs << "SG_MUL_VAL_ " << message.second.id << ' ' << signal.second.name;

                /* Switch Name */
                ofs << ' ' << extendedMultiplexor.second.switchName;

                /* Value Ranges */
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
}

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
    writeVersion(ofs);

    /* New Symbols (NS) */
    writeNewSymbols(ofs);

    /* Bit Timing (BS) */
    writeBitTiming(ofs);

    /* Nodes (BU) */
    writeNodes(ofs);

    /* Value Tables (VAL_TABLE) */
    writeValueTables(ofs);

    /* Messages (BO) */
    writeMessages(ofs);

    /* Message Transmitters (BO_TX_BU) */
    writeMessageTransmitters(ofs);

    /* Environment Variables (EV) */
    writeEnvironmentVariables(ofs);

    /* Environment Variable Data (ENVVAR_DATA) */
    writeEnvironmentVariableData(ofs);

    /* Signal Types (SGTYPE, obsolete) */
    writeSignalTypes(ofs);

    /* Comments (CM) */
    writeCommentsNetworks(ofs);
    writeCommentsNodes(ofs);
    writeCommentsMessages(ofs);
    writeCommentsSignals(ofs);
    writeCommentsEnvironmentVariables(ofs);

    /* Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
    writeAttributeDefinitions(ofs);

    /* Sigtype Attr Lists (?, obsolete) */

    /* Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
    writeAttributeDefaults(ofs);

    /* Attribute Values (BA) */
    writeAttributeValuesNetworks(ofs);
    writeAttributeValuesNodes(ofs);
    writeAttributeValuesMessages(ofs);
    writeAttributeValuesSignals(ofs);
    writeAttributeValuesEnvironmentVariables(ofs);

    /* Attribute Values at Relations (BA_REL) */
    writeAttributeRelationValues(ofs);

    /* Value Descriptions (VAL) */
    writeValueDescriptionsSignals(ofs);
    writeValueDescriptionsEnvironmentVariables(ofs);

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filters (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */

    /* Signal Groups (SIG_GROUP) */
    writeSignalGroups(ofs);

    /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    writeSignalExtendedValueTypes(ofs);

    /* Extended Multiplexors (SG_MUL_VAL) */
    writeExtendedMultiplexors(ofs);

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
