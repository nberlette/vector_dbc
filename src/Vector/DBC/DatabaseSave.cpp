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
    ofs << endl;

    /* New Symbols (NS) */
    ofs << "NS_ : " << endl;
    for (std::set<std::string>::iterator newSymbol=newSymbols.begin(); newSymbol!=newSymbols.end(); ++newSymbol) {
        ofs << "\t" << *newSymbol << endl;
    }
    ofs << endl;

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
    for (std::map<std::string, Node>::iterator node=nodes.begin(); node!=nodes.end(); ++node) {
        ofs << " " << node->first;
    }
    ofs << endl;
    ofs << endl;

    /* Value Tables (VAL_TABLE) */
    for (std::map<std::string, ValueTable>::iterator valueTable=valueTables.begin(); valueTable!=valueTables.end(); ++valueTable) {
        ofs << "VAL_TABLE_ " << valueTable->second.name;
        for (std::map<unsigned int, std::string>::iterator valueDescription=valueTable->second.valueDescriptions.begin(); valueDescription!=valueTable->second.valueDescriptions.end(); ++valueDescription) {
            ofs << " " << valueDescription->first;
            ofs << " \"" << valueDescription->second << "\"";
        }
        ofs << " ;" << endl;
    }
    ofs << endl;

    /* Messages (BO) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        ofs << "BO_ " << message->second.id;
        ofs << " " << message->second.name;
        ofs << ": " << message->second.size << " ";
        if (message->second.transmitter.empty()) {
            ofs << "Vector__XXX";
        } else {
            ofs << message->second.transmitter;
        }
        ofs << endl;

        /* Signals (SG) */
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            ofs << " SG_ " << signal->second.name;
            if (signal->second.multiplexedSignal) {
                ofs << 'm';
                ofs << signal->second.multiplexerSwitchValue;
            }
            if (signal->second.multiplexorSwitch) {
                ofs << 'M';
            }
            if (signal->second.multiplexedSignal || signal->second.multiplexorSwitch) {
                ofs << ' ';
            }
            ofs << " : ";
            ofs << signal->second.startBit << '|' << signal->second.size << '@' << char(signal->second.byteOrder) << char(signal->second.valueType);
            ofs << ' ';
            ofs << '(' << signal->second.factor << ',' << signal->second.offset << ')';
            ofs << ' ';
            ofs << '[' << signal->second.minimum << '|' << signal->second.maximum << ']';
            ofs << ' ';
            ofs << '"' << signal->second.unit << '"';
            ofs << ' ';
            if (signal->second.receivers.empty()) {
                ofs << "Vector__XXX";
            } else {
                for (std::set<std::string>::iterator receiver=signal->second.receivers.begin(); receiver!=signal->second.receivers.end(); ++receiver) {
                    ofs << " " << *receiver;
                }
            }
            ofs << endl;
        }

        ofs << endl;
    }
    ofs << endl;
    ofs << endl;

    /* Message Transmitters (BO_TX_BU) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        if (!message->second.transmitters.empty()) {
            ofs << "BO_TX_BU_ " << message->second.id << " :";
            for (std::set<std::string>::iterator transmitter=message->second.transmitters.begin(); transmitter!=message->second.transmitters.end(); ++transmitter) {
                ofs << ' ' << *transmitter;
            }
            ofs << ';' << endl;
        }
    }

    /* Environment Variables (EV) */
    for (std::map<std::string, EnvironmentVariable>::iterator environmentVariable=environmentVariables.begin(); environmentVariable!=environmentVariables.end(); ++environmentVariable) {
        ofs << "EV_ " << environmentVariable->second.name << " : ";
        ofs << char(environmentVariable->second.type);
        ofs << " [";
        // minimum
        ofs << '|';
        // maximum
        ofs << "] \"";
        ofs << environmentVariable->second.unit;
        ofs << "\" ";
        ofs << environmentVariable->second.initialValue;
        ofs << ' ';
        ofs << environmentVariable->second.id;
        ofs << " DUMMY_NODE_VECTOR";
        ofs << std::hex << environmentVariable->second.accessType << std::dec;
        ofs << ' ';
        if (environmentVariable->second.accessNodes.empty()) {
            ofs << "VECTOR__XXX";
        } else {
            bool first = true;
            for (std::set<std::string>::iterator accessNode=environmentVariable->second.accessNodes.begin(); accessNode!=environmentVariable->second.accessNodes.end(); ++accessNode) {
                if (first) {
                    first = false;
                } else {
                    ofs << ',';
                }
                ofs << *accessNode;
            }
        }
        ofs << ";" << endl;
    }

    /* Environment Variables Data (ENVVAR_DATA) */
    for (std::map<std::string, EnvironmentVariable>::iterator environmentVariable=environmentVariables.begin(); environmentVariable!=environmentVariables.end(); ++environmentVariable) {
        ofs << "ENVVAR_DATA_ " << environmentVariable->second.name;
        ofs << " : " << environmentVariable->second.dataSize;
        ofs << ";" << endl;
    }

    /* Signal Types (SGTYPE, obsolete) */
    for (std::map<std::string, SignalType>::iterator signalType=signalTypes.begin(); signalType!=signalTypes.end(); ++signalType) {
        ofs << "SGTYPE_ " << signalType->second.name;
        ofs << " : " << signalType->second.size;
        ofs << '@' << char(signalType->second.byteOrder);
        ofs << ' ' << char(signalType->second.valueType);
        ofs << ' ' << signalType->second.defaultValue;
        ofs << ", " << signalType->second.valueTable;
        ofs << ';' << endl;
    }
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            if (!signal->second.type.empty()) {
                ofs << "SGTYPE_ " << message->second.id << ' ' << signal->second.name << " : " << signal->second.type << ";" << endl;
            }
        }
    }

    /* Comments (CM) */
    if (!comment.empty()) {
        ofs << "CM_ \"" << comment << "\";" << endl;
    }
    for (std::map<std::string, Node>::iterator node=nodes.begin(); node!=nodes.end(); ++node) {
        if (!node->second.comment.empty()) {
            ofs << "CM_ BU_ " << node->second.name << " \"" << node->second.comment << "\";" << endl;
        }
    }
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        if (!message->second.comment.empty()) {
            ofs << "CM_ BO_ " << message->second.id << " \"" << message->second.comment << "\";" << endl;
        }
    }
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            if (!signal->second.comment.empty()) {
                ofs << "CM_ SG_ " << message->second.id << ' ' << signal->second.name << " \"" << signal->second.comment << "\";" << endl;
            }
        }
    }
    for (std::map<std::string, EnvironmentVariable>::iterator environmentVariable=environmentVariables.begin(); environmentVariable!=environmentVariables.end(); ++environmentVariable) {
        if (!environmentVariable->second.comment.empty()) {
            ofs << "CM_ EV_ " << environmentVariable->second.name << " \"" << environmentVariable->second.comment << "\";" << endl;
        }
    }

    /* Attribute Definitions (BA_DEF) */
    for (std::map<std::string, AttributeDefinition>::iterator attributeDefinition=attributeDefinitions.begin(); attributeDefinition!=attributeDefinitions.end(); ++attributeDefinition) {
        ofs << "BA_DEF_ ";
        switch(attributeDefinition->second.objectType) {
        case AttributeDefinition::ObjectType::Database:
            break;
        case AttributeDefinition::ObjectType::Node:
            ofs << "BU_ ";
            break;
        case AttributeDefinition::ObjectType::Message:
            ofs << "BO_ ";
            break;
        case AttributeDefinition::ObjectType::Signal:
            ofs << "SG_ ";
            break;
        case AttributeDefinition::ObjectType::EnvironmentVariable:
            ofs << "EV_ ";
            break;
        }
        ofs << " \"" << attributeDefinition->second.name << "\" ";
        switch(attributeDefinition->second.valueType) {
        case AttributeValueType::Int:
            ofs << "INT ";
            ofs << attributeDefinition->second.minimumIntegerValue;
            ofs << " ";
            ofs << attributeDefinition->second.maximumIntegerValue;
            break;
        case AttributeValueType::Hex:
            ofs << "HEX ";
            ofs << attributeDefinition->second.minimumHexValue;
            ofs << " ";
            ofs << attributeDefinition->second.maximumHexValue;
            break;
        case AttributeValueType::Float:
            ofs << "FLOAT ";
            ofs << attributeDefinition->second.minimumFloatValue;
            ofs << " ";
            ofs << attributeDefinition->second.maximumFloatValue;
            break;
        case AttributeValueType::String:
            ofs << "STRING ";
            break;
        case AttributeValueType::Enum:
            ofs << "ENUM  ";
            bool first = true;
            for (std::vector<std::string>::iterator enumValue=attributeDefinition->second.enumValues.begin(); enumValue!=attributeDefinition->second.enumValues.end(); ++enumValue) {
                if (first) {
                    first = false;
                } else {
                    ofs << ',';
                }
                ofs << "\"" << *enumValue << "\"";
            }
            break;
        }
        ofs << ";" << endl;
    }

    /* Sigtype Attr List (?, obsolete) */

    /* Attribute Defaults (BA_DEF_DEF) */
    for (std::map<std::string, Attribute>::iterator attribute=attributeDefaults.begin(); attribute!=attributeDefaults.end(); ++attribute) {
        ofs << "BA_DEF_DEF_  \"" << attribute->second.name << "\" ";
        switch(attribute->second.valueType) {
        case AttributeValueType::Int:
            ofs << attribute->second.integerValue;
            break;
        case AttributeValueType::Hex:
            ofs << attribute->second.hexValue;
            break;
        case AttributeValueType::Float:
            ofs << attribute->second.floatValue;
            break;
        case AttributeValueType::String:
            ofs << '\"' << attribute->second.stringValue << '\"';
            break;
        case AttributeValueType::Enum:
            ofs << '\"' << attribute->second.stringValue << '\"';
            break;
        }
        ofs << ';' << endl;
    }

    /* Attribute Values (BA) */
    for (std::map<std::string, Attribute>::iterator attribute=attributeValues.begin(); attribute!=attributeValues.end(); ++attribute) {
        ofs << "BA_ \"" << attribute->second.name << "\" ";
        switch(attribute->second.valueType) {
        case AttributeValueType::Int:
            ofs << attribute->second.integerValue;
            break;
        case AttributeValueType::Hex:
            ofs << attribute->second.hexValue;
            break;
        case AttributeValueType::Float:
            ofs << attribute->second.floatValue;
            break;
        case AttributeValueType::String:
            ofs << '\"' << attribute->second.stringValue << '\"';
            break;
        case AttributeValueType::Enum:
            ofs << attribute->second.enumValue;
            break;
        }
        ofs << ';' << endl;
    }
    for (std::map<std::string, Node>::iterator node=nodes.begin(); node!=nodes.end(); ++node) {
        for (std::map<std::string, Attribute>::iterator attribute=node->second.attributeValues.begin(); attribute!=node->second.attributeValues.end(); ++attribute) {
            ofs << "BA_ \"" << attribute->second.name << "\" ";
            ofs << "BU_ " << node->second.name << ' ';
            switch(attribute->second.valueType) {
            case AttributeValueType::Int:
                ofs << attribute->second.integerValue;
                break;
            case AttributeValueType::Hex:
                ofs << attribute->second.hexValue;
                break;
            case AttributeValueType::Float:
                ofs << attribute->second.floatValue;
                break;
            case AttributeValueType::String:
                ofs << '\"' << attribute->second.stringValue << '\"';
                break;
            case AttributeValueType::Enum:
                ofs << attribute->second.enumValue;
                break;
            }
            ofs << ';' << endl;
        }
    }
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Attribute>::iterator attribute=message->second.attributeValues.begin(); attribute!=message->second.attributeValues.end(); ++attribute) {
            ofs << "BA_ \"" << attribute->second.name << "\" ";
            ofs << "BO_ " << message->second.id << ' ';
            switch(attribute->second.valueType) {
            case AttributeValueType::Int:
                ofs << attribute->second.integerValue;
                break;
            case AttributeValueType::Hex:
                ofs << attribute->second.hexValue;
                break;
            case AttributeValueType::Float:
                ofs << attribute->second.floatValue;
                break;
            case AttributeValueType::String:
                ofs << '\"' << attribute->second.stringValue << '\"';
                break;
            case AttributeValueType::Enum:
                ofs << attribute->second.enumValue;
                break;
            }
            ofs << ';' << endl;
        }
    }
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            for (std::map<std::string, Attribute>::iterator attribute=signal->second.attributeValues.begin(); attribute!=signal->second.attributeValues.end(); ++attribute) {
                ofs << "BA_ \"" << attribute->second.name << "\" ";
                ofs << "SG_ " << message->second.id << ' ' << signal->second.name << ' ';
                switch(attribute->second.valueType) {
                case AttributeValueType::Int:
                    ofs << attribute->second.integerValue;
                    break;
                case AttributeValueType::Hex:
                    ofs << attribute->second.hexValue;
                    break;
                case AttributeValueType::Float:
                    ofs << attribute->second.floatValue;
                    break;
                case AttributeValueType::String:
                    ofs << '\"' << attribute->second.stringValue << '\"';
                    break;
                case AttributeValueType::Enum:
                    ofs << attribute->second.enumValue;
                    break;
                }
                ofs << ';' << endl;
            }
        }
    }
    for (std::map<std::string, EnvironmentVariable>::iterator environmentVariable=environmentVariables.begin(); environmentVariable!=environmentVariables.end(); ++environmentVariable) {
        for (std::map<std::string, Attribute>::iterator attribute=environmentVariable->second.attributeValues.begin(); attribute!=environmentVariable->second.attributeValues.end(); ++attribute) {
            ofs << "BA_ \"" << attribute->second.name << "\" ";
            ofs << "EV_ " << environmentVariable->second.name << ' ';
            switch(attribute->second.valueType) {
            case AttributeValueType::Int:
                ofs << attribute->second.integerValue;
                break;
            case AttributeValueType::Hex:
                ofs << attribute->second.hexValue;
                break;
            case AttributeValueType::Float:
                ofs << attribute->second.floatValue;
                break;
            case AttributeValueType::String:
                ofs << '\"' << attribute->second.stringValue << '\"';
                break;
            case AttributeValueType::Enum:
                ofs << attribute->second.enumValue;
                break;
            }
            ofs << ';' << endl;
        }
    }

    /* Value Descriptions (VAL) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            if (!signal->second.valueDescriptions.empty()) {
                ofs << "VAL_ " << message->first << ' ' << signal->first;
                for (std::map<unsigned int, std::string>::iterator valueDescription=signal->second.valueDescriptions.begin(); valueDescription!=signal->second.valueDescriptions.end(); ++valueDescription) {
                    ofs << " " << valueDescription->first;
                    ofs << " \"" << valueDescription->second << "\"";
                }
                ofs << " ;" << endl;
            }
        }
    }
    for (std::map<std::string, EnvironmentVariable>::iterator environmentVariable=environmentVariables.begin(); environmentVariable!=environmentVariables.end(); ++environmentVariable) {
        if (!environmentVariable->second.valueDescriptions.empty()) {
            ofs << "VAL_ " << environmentVariable->first;
            for (std::map<unsigned int, std::string>::iterator valueDescription=environmentVariable->second.valueDescriptions.begin(); valueDescription!=environmentVariable->second.valueDescriptions.end(); ++valueDescription) {
                ofs << " " << valueDescription->first;
                ofs << " \"" << valueDescription->second << "\"";
            }
            ofs << " ;" << endl;
        }
    }

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filter (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            if (!signal->second.type.empty()) {
                ofs << "SGTYPE_ " << message->second.id << ' ' << signal->second.name;
                ofs << " : " << signal->second.type << ';' << endl;
            }
        }
    }

    /* Signal Groups (SIG_GROUP) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, SignalGroup>::iterator signalGroup=message->second.signalGroups.begin(); signalGroup!=message->second.signalGroups.end(); ++signalGroup) {
            ofs << "SIG_GROUP_ " << message->second.id << ' ' << signalGroup->second.name;
            ofs << ' ' << signalGroup->second.repetitions;
            bool first = true;
            for (std::set<std::string>::iterator signal=signalGroup->second.signals.begin(); signal!=signalGroup->second.signals.end(); ++signal) {
                if (first) {
                    first = false;
                } else {
                    ofs << ',';
                }
                ofs << *signal;
            }
            ofs << ';' << endl;
        }
    }

    /* Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            if (signal->second.extendedValueType != Signal::ExtendedValueType::Undefined) {
                ofs << "SIG_VALTYPE_ " << message->second.id << ' ' << signal->second.name;
                ofs << " : " << char(signal->second.extendedValueType);
                ofs << endl;
            }
        }
    }

    /* Extended Multiplexing (SG_MUL_VAL) */
    for (std::map<unsigned int, Message>::iterator message=messages.begin(); message!=messages.end(); ++message) {
        for (std::map<std::string, Signal>::iterator signal=message->second.signals.begin(); signal!=message->second.signals.end(); ++signal) {
            for (std::map<std::string, ExtendedMultiplexor>::iterator extendedMultiplexor=signal->second.extendedMultiplexors.begin(); extendedMultiplexor!=signal->second.extendedMultiplexors.end(); ++extendedMultiplexor) {
                ofs << "SG_MUL_VAL_ " << message->second.id << ' ' << signal->second.name;
                ofs << ' ' << extendedMultiplexor->second.switchName;
                bool first = true;
                for (std::set<ExtendedMultiplexor::ValueRange>::iterator valueRange=extendedMultiplexor->second.valueRanges.begin(); valueRange!=extendedMultiplexor->second.valueRanges.end(); ++valueRange) {
                    if (first) {
                        first = false;
                    } else {
                        ofs << ", ";
                    }
                    ofs << valueRange->first << '-' << valueRange->second;
                }
                ofs << ';' << endl;
            }
        }
    }

    /* close stream */
    ofs.close();

    return true;
}

bool Database::save(std::string & filename)
{
    return save(filename.c_str());
}

}
}
