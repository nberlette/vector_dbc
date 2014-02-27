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
#include <iostream> // for std::cerr
#include <fstream>
#include <sstream>
#include <stack>
#include <string>

#ifdef USE_CPP11_REGEX
#include <regex>
#define smatch       std::smatch
#define regex        std::regex
#define regex_search std::regex_search
#else
#include <boost/regex.hpp>
#define smatch       boost::smatch
#define regex        boost::regex
#define regex_search boost::regex_search
#endif

namespace Vector {
namespace DBC {

/* Removes windows/unix/mac line endings. */
void Database::chomp(std::string & line)
{
    /* don't do anything if line is empty */
    if (line.length() == 0) {
        return;
    }

    /* remove trailing \r and \n characters */
    for(;;) {
        char back = line.back();
        if ((back == '\r') || (back == '\n')) {
            line.pop_back();
        } else {
            return;
        }
    }
}

/* Version (VERSION) */
void Database::readVersion(std::string & line)
{
    smatch m;
    regex re("^VERSION \"(.*)\"$");
    if (regex_search(line, m, re)) {
        version = m[1];
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* New Symbols (NS) */
void Database::readNewSymbols(std::ifstream & ifs, std::string & line)
{
    while(ifs.good()) {
        std::getline(ifs, line);
        chomp(line);
        if (line.empty())
            return;

        smatch m;
        regex re("^[[:space:]]+([[:alnum:]_]+)$");
        if (regex_search(line, m, re)) {
            newSymbols.insert(m[1]);
        }
    }
}

/* Bit Timing (BS) */
void Database::readBitTiming(std::string & line)
{
    if (line == "BS_:") {
        // all ok
        return;
    } else {
        smatch m;
        regex re("^BS_: ([[:digit:]]+):([[:digit:]]+):([[:digit:]]+);$");
        if (regex_search(line, m, re)) {
            bitTiming.baudrate = stoul(m[1]);
            bitTiming.btr1 = stoul(m[2]);
            bitTiming.btr2 = stoul(m[2]);
            return;
        }
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Nodes (BU) */
void Database::readNodes(std::string & line)
{
    std::istringstream iss(line);
    std::string node;

    /* skip BU_: */
    iss >> node;

    while (iss.good()) {
        iss >> node;
        nodes[node].name = node;
    }
}

/* Value Tables (VAL_TABLE) */
void Database::readValueTable(std::string & line)
{
    smatch m;
    regex re("^VAL_TABLE_ ([[:alnum:]_-]+) (.*) ;$");
    if (regex_search(line, m, re)) {
        std::string valueTableName = m[1];
        ValueTable & valueTable = valueTables[valueTableName];

        /* Name */
        valueTable.name = valueTableName;
        ValueDescriptions & valueDescriptions = valueTable.valueDescriptions;

        /* Value Description Pairs */
        std::istringstream iss(m[2]);
        while (iss.good()) {
            std::string value;
            iss >> value;
            std::string description;
            iss >> description;
            while (description.back() != '"') {
                std::string nextStr;
                iss >> nextStr;
                description += " ";
                description += nextStr;
            }
            description.erase(0, 1);
            description.pop_back();
            valueDescriptions[stoul(value)] = description;
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Signals (SG) */
void Database::readSignal(Message & message, std::string & line)
{
    smatch m;
    regex re("^ SG_ ([[:alnum:]_-]+) ([m]*)([[:digit:]]*)([M]*): ([[:digit:]]+).([[:digit:]]+)@([01])([+-]) .([[:digit:].+-]+),([[:digit:].+-]+). .([[:digit:].+-]+).([[:digit:].+-]+). \"(.*)\" +(.*)$");
    if (regex_search(line, m, re)) {
        std::string signalName = m[1];
        Signal & signal = message.signals[signalName];

        /* Name */
        signal.name = signalName;

        /* Multiplexed Signal */
        signal.multiplexedSignal = (m[2] == 'm');

        /* Multiplexer Switch Value */
        std::string multiplexerSwitchValue = m[3];
        if (multiplexerSwitchValue.empty()) {
            signal.multiplexerSwitchValue = 0;
        } else {
            signal.multiplexerSwitchValue = stoul(multiplexerSwitchValue);
        }

        /* Multiplexor Switch */
        signal.multiplexorSwitch = (m[4] == 'M');

        /* Start Bit */
        signal.startBit = stoul(m[5]);

        /* Size */
        signal.size = stoul(m[6]);

        /* Byte Order */
        std::string byteOrder = m[7];
        switch(byteOrder.front()) {
        case '0':
            signal.byteOrder = ByteOrder::BigEndian;
            break;
        case '1':
            signal.byteOrder = ByteOrder::LittleEndian;
            break;
        default:
            std::cerr << line << std::endl;
        }

        /* Value Type */
        std::string valueType = m[8];
        switch(valueType.front()) {
        case '+':
            signal.valueType = ValueType::Unsigned;
            break;
        case '-':
            signal.valueType = ValueType::Signed;
            break;
        default:
            std::cerr << line << std::endl;
        }

        /* Factor, Offset */
        signal.factor = stod(m[9]);
        signal.offset = stod(m[10]);

        /* Minimum, Maximum */
        signal.minimum = stod(m[11]);
        signal.maximum = stod(m[12]);

        /* Unit */
        signal.unit = m[13];

        /* Receivers */
        std::istringstream iss(m[14]);
        while (iss.good()) {
            std::string node;
            iss >> node;
            if (node != "Vector__XXX") {
                signal.receivers.insert(node);
            }
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Messages (BO) */
void Database::readMessage(std::ifstream & ifs, std::string & line)
{
    smatch m;
    regex re("^BO_ ([[:digit:]]+) ([[:alnum:]_-]+) ?: ([[:digit:]]+) ([[:alnum:]_-]+)$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        Message & message = messages[messageId];

        /* Identifier */
        message.id = messageId;

        /* Name */
        message.name = m[2];

        /* Size */
        message.size = stoul(m[3]);

        /* Transmitter */
        std::string transmitter = m[4];
        if (transmitter != "Vector__XXX") {
            message.transmitter = transmitter;
        }

        /* Signals (SG) */
        while(ifs.good()) {
            std::getline(ifs, line);
            chomp(line);
            if (line.empty())
                return;

            readSignal(message, line);
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Message Transmitters (BO_TX_BU) */
void Database::readMessageTransmitter(std::string & line)
{
    smatch m;
    regex re("^BO_TX_BU_ ([[:digit:]]+) : (.+);$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        Message & message = messages[messageId];

        /* Message Identifier */
        message.id = messageId;

        /* Transmitters */
        std::istringstream iss(m[2]);
        while(iss.good()) {
            std::string transmitter;
            iss >> transmitter;
            message.transmitters.insert(transmitter);
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Environment Variables (EV) */
void Database::readEnvironmentVariable(std::string & line)
{
    smatch m;
    regex re("^EV_ ([[:alnum:]_-]+) ?: ([[:digit:]]+) \\[([[:digit:].+-]+)\\|([[:digit:].+-]+)\\] \"(.*)\" ([[:digit:].+-]+) ([[:digit:]]+) DUMMY_NODE_VECTOR([[:digit:]]+) (.+) ?;$");
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        EnvironmentVariable & environmentVariable = environmentVariables[envVarName];

        /* Name */
        environmentVariable.name = envVarName;

        /* Type */
        std::string envVarType = m[2];
        switch(envVarType.front()) {
        case '0':
            environmentVariable.type = EnvironmentVariable::Type::Integer;
            break;
        case '1':
            environmentVariable.type = EnvironmentVariable::Type::Float;
            break;
        default:
            std::cerr << line << std::endl;
        }

        /* Minimum, Maximum */
        environmentVariable.minimum = stod(m[3]);
        environmentVariable.maximum = stod(m[4]);

        /* Unit */
        environmentVariable.unit = m[5];

        /* Initial Value */
        environmentVariable.initialValue = stod(m[6]);

        /* Identifier */
        environmentVariable.id = stoul(m[7]);

        /* Access Type */
        unsigned int accessType = stoul(m[8], nullptr, 16);
        if (accessType & 0x8000) {
            accessType &= ~0x8000;
            environmentVariable.type = EnvironmentVariable::Type::String;
        }
        switch(accessType) {
        case 0:
            environmentVariable.accessType = EnvironmentVariable::AccessType::Unrestricted;
            break;
        case 1:
            environmentVariable.accessType = EnvironmentVariable::AccessType::Read;
            break;
        case 2:
            environmentVariable.accessType = EnvironmentVariable::AccessType::Write;
            break;
        case 3:
            environmentVariable.accessType = EnvironmentVariable::AccessType::ReadWrite;
            break;
        default:
            std::cerr << line << std::endl;
        }

        /* Access Nodes */
        std::istringstream iss(m[9]);
        while(iss.good()) {
            std::string accessNode;
            std::getline(iss, accessNode, ',');
            environmentVariable.accessNodes.insert(accessNode);
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Environment Variable Data (ENVVAR_DATA) */
void Database::readEnvironmentVariableData(std::string & line)
{
    smatch m;
    regex re("^ENVVAR_DATA_ ([[:alnum:]_-]+) ?: ([[:digit:]]+);$");
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        EnvironmentVariable & environmentVariable = environmentVariables[envVarName];

        /* Name */
        environmentVariable.name = envVarName;

        /* Type */
        environmentVariable.type = EnvironmentVariable::Type::Data;

        /* Data Size */
        environmentVariable.dataSize = stoul(m[2]);
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Signal Types (SGTYPE, obsolete) */
void Database::readSignalType(std::string & line)
{
    // Signal Type
    smatch mST;
    regex reST("^SGTYPE_ ([[:alnum:]_-]+) : ([[:digit:]]+)@([01])([+-]) ([[:digit:]]+).([[:digit:]]+)@([01])([+-]) .([[:digit:].+-]+),([[:digit:].+-]+). \"(.*)\" , ([[:alnum:]_-]+);$");
    if (regex_search(line, mST, reST)) {
        std::string signalTypeName = mST[1];
        SignalType & signalType = signalTypes[signalTypeName];

        /* Name */
        signalType.name = signalTypeName;

        /* Size */
        signalType.size = stoul(mST[2]);

        /* Byte Order */
        std::string byteOrder = mST[3];
        switch(byteOrder.front()) {
        case '0':
            signalType.byteOrder = ByteOrder::BigEndian;
            break;
        case '1':
            signalType.byteOrder = ByteOrder::LittleEndian;
            break;
        default:
            std::cerr << line << std::endl;
        }

        /* Value Type */
        std::string valueType = mST[4];
        switch(valueType.front()) {
        case '+':
            signalType.valueType = ValueType::Unsigned;
            break;
        case '-':
            signalType.valueType = ValueType::Signed;
            break;
        default:
            std::cerr << line << std::endl;
        }

        /* Factor, Offset */
        signalType.factor = stod(mST[5]);
        signalType.offset = stod(mST[6]);

        /* Minimum, Maximum */
        signalType.minimum = stod(mST[7]);
        signalType.maximum = stod(mST[8]);

        /* Unit */
        signalType.unit = mST[9];

        /* Default Value */
        signalType.defaultValue = stod(mST[10]);

        /* Value Table */
        signalType.valueTable = mST[11];
        return;
    }

    // Signal Type Ref
    smatch mSTR;
    regex reSTR("^SGTYPE_ ([[:digit:]]+) ([[:alnum:]_-]+) : ([[:alnum:]_-]+);$");
    if (regex_search(line, mSTR, reSTR)) {
        unsigned int messageId = stoul(mSTR[1]);
        std::string signalName = mSTR[2];
        std::string signalTypeName = mSTR[3];
        messages[messageId].signals[signalName].type = signalTypeName;
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Comments (CM) for Networks */
bool Database::readCommentNetwork(std::stack<size_t> & lineBreaks, std::string & line)
{
    smatch m;
    regex re("^CM_ \"(.*)\";$");
    if (regex_search(line, m, re)) {
        std::string comment2 = m[1];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        comment = comment2;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Nodes (BU) */
bool Database::readCommentNode(std::stack<size_t> & lineBreaks, std::string & line)
{
    smatch mBU;
    regex reBU("^CM_ BU_ ([[:alnum:]_-]+) \"(.+)\";$");
    if (regex_search(line, mBU, reBU)) {
        std::string nodeName = mBU[1];
        std::string comment2 = mBU[2];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        nodes[nodeName].comment = comment2;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Messages (BO) */
bool Database::readCommentMessage(std::stack<size_t> & lineBreaks, std::string & line)
{
    smatch mBO;
    regex reBO("^CM_ BO_ ([[:digit:]]+) \"(.+)\";$");
    if (regex_search(line, mBO, reBO)) {
        unsigned int messageId = stoul(mBO[1]);
        std::string comment2 = mBO[2];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        messages[messageId].comment = comment2;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Signals (SG) */
bool Database::readCommentSignal(std::stack<size_t> & lineBreaks, std::string & line)
{
    smatch mSG;
    regex reSG("^CM_ SG_ ([[:digit:]]+) ([[:alnum:]_-]+) \"(.+)\";$");
    if (regex_search(line, mSG, reSG)) {
        unsigned int messageId = stoul(mSG[1]);
        std::string signalName = mSG[2];
        std::string comment2 = mSG[3];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        messages[messageId].signals[signalName].comment = comment2;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) for Environment Variables (EV) */
bool Database::readCommentEnvironmentVariable(std::stack<size_t> & lineBreaks, std::string & line)
{
    smatch mEV;
    regex reEV("^CM_ EV_ ([[:alnum:]_-]+) \"(.+)\";$");
    if (regex_search(line, mEV, reEV)) {
        std::string envVarName = mEV[1];
        std::string comment2 = mEV[2];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        environmentVariables[envVarName].comment = comment2;
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Comments (CM) */
void Database::readComment(std::ifstream & ifs, std::string & line)
{
    /* support multi-line comments */
    std::string suffix("\";");
    size_t firstCommentCharPos = line.find('"') + 1;
    std::stack<size_t> lineBreaks;
    while (line.rfind(suffix) != (line.size() - suffix.size())) {
        std::string nextLine;
        std::getline(ifs, nextLine);
        chomp(nextLine);
        lineBreaks.push(line.size() - firstCommentCharPos);
        line += nextLine;
    }

    // for nodes (BU)
    if (readCommentNode(lineBreaks, line)) {
        return;
    }

    // for messages (BO)
    if (readCommentMessage(lineBreaks, line)) {
        return;
    }

    // for signals (SG)
    if (readCommentSignal(lineBreaks, line)) {
        return;
    }

    // for environment variables (EV)
    if (readCommentEnvironmentVariable(lineBreaks, line)) {
        return;
    }

    // for network
    if (readCommentNetwork(lineBreaks, line)) {
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Attribute Definitions (BA_DEF) */
void Database::readAttributeDefinition(std::string & line)
{
    smatch m;
    regex re("^BA_DEF_ ([[:alnum:]_]*) +\"([[:alnum:]_-]+)\" ([[:alnum:]_]+) +(.*);$");
    if (regex_search(line, m, re)) {
        std::string objectType = m[1];
        std::string attributeName = m[2];
        std::string attributeValueType = m[3];
        std::istringstream iss(m[4]);
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        attributeDefinition.name = attributeName;

        // for network
        if (objectType == "") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Network;
        } else
        // for node
        if (objectType == "BU_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Node;
        } else
        // for message
        if (objectType == "BO_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Message;
        } else
        // for signal
        if (objectType == "SG_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Signal;
        } else
        // for environment variable
        if (objectType == "EV_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::EnvironmentVariable;
        } else
        // format doesn't match
        {
            std::cerr << line << std::endl;
        }

        // for integer
        if (attributeValueType == "INT") {
            attributeDefinition.valueType = AttributeValueType::Int;
            std::string value;
            iss >> value;
            attributeDefinition.minimumIntegerValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumIntegerValue = stoul(value);
        } else
        // for hexadecimal
        if (attributeValueType == "HEX") {
            attributeDefinition.valueType = AttributeValueType::Hex;
            std::string value;
            iss >> value;
            attributeDefinition.minimumHexValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumHexValue = stoul(value);
        } else
        // for float
        if (attributeValueType == "FLOAT") {
            attributeDefinition.valueType = AttributeValueType::Float;
            std::string value;
            iss >> value;
            attributeDefinition.minimumFloatValue = stod(value);
            iss >> value;
            attributeDefinition.maximumFloatValue = stod(value);
        } else
        // for string
        if (attributeValueType == "STRING") {
            attributeDefinition.valueType = AttributeValueType::String;
        } else
        // for enumeration
        if (attributeValueType == "ENUM") {
            attributeDefinition.valueType = AttributeValueType::Enum;
            while(iss.good()) {
                std::string value;
                std::getline(iss, value, ',');
                value.erase(0, 1);
                value.pop_back();
                attributeDefinition.enumValues.push_back(value);
            }
        } else
        // format doesn't match
        {
            std::cerr << line << std::endl;
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Attribute Definitions at Relations (BA_DEF_REL) */
void Database::readAttributeDefinitionRelation(std::string & line)
{
    smatch m;
    regex re("^BA_DEF_REL_ ([[:alnum:]_]*) +\"([[:alnum:]_-]+)\" ([[:alnum:]_]+) +(.*);$");
    if (regex_search(line, m, re)) {
        std::string objectType = m[1];
        std::string attributeName = m[2];
        std::string attributeValueType = m[3];
        std::istringstream iss(m[4]);
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        attributeDefinition.name = attributeName;

        // Control Unit - Env. Variable
        if (objectType == "BU_EV_REL_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::ControlUnitEnvironmentVariable;
        } else

        // Node - Tx Message
        if (objectType == "BU_BO_REL_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::NodeTxMessage;
        } else

        // Node - Mapped Rx Signal
        if (objectType == "BU_SG_REL_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::NodeMappedRxSignal;
        } else
        // format doesn't match
        {
            std::cerr << line << std::endl;
        }

        // Integer
        if (attributeValueType == "INT") {
            attributeDefinition.valueType = AttributeValueType::Int;
            std::string value;
            iss >> value;
            attributeDefinition.minimumIntegerValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumIntegerValue = stoul(value);
        } else

        // Hexadecimal
        if (attributeValueType == "HEX") {
            attributeDefinition.valueType = AttributeValueType::Hex;
            std::string value;
            iss >> value;
            attributeDefinition.minimumHexValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumHexValue = stoul(value);
        } else

        // Float
        if (attributeValueType == "FLOAT") {
            attributeDefinition.valueType = AttributeValueType::Float;
            std::string value;
            iss >> value;
            attributeDefinition.minimumFloatValue = stod(value);
            iss >> value;
            attributeDefinition.maximumFloatValue = stod(value);
        } else

        // String
        if (attributeValueType == "STRING") {
            attributeDefinition.valueType = AttributeValueType::String;
        } else

        // Enumeration
        if (attributeValueType == "ENUM") {
            attributeDefinition.valueType = AttributeValueType::Enum;
            while(iss.good()) {
                std::string value;
                std::getline(iss, value, ',');
                value.erase(0, 1);
                value.pop_back();
                attributeDefinition.enumValues.push_back(value);
            }
        } else
        // format doesn't match
        {
            std::cerr << line << std::endl;
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Sigtype Attr Lists (?, obsolete) */

/* Attribute Defaults (BA_DEF_DEF) */
void Database::readAttributeDefault(std::string & line)
{
    smatch m;
    regex re("^BA_DEF_DEF_ +\"([[:alnum:]_-]+)\" (.+);$");
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attributeDefault = attributeDefaults[attributeName];

        /* Name */
        attributeDefault.name = attributeName;

        /* Value Type */
        attributeDefault.valueType = attributeDefinition.valueType;

        /* Value */
        attributeDefault.stringValue = attributeValue;
        switch(attributeDefinition.valueType) {
        case AttributeValueType::Int:
            attributeDefault.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attributeDefault.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attributeDefault.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attributeDefault.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attributeDefault.stringValue = attributeValue;
            break;
        default:
            std::cerr << line << std::endl;
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Attribute Defaults at Relations (BA_DEF_DEF_REL) */
void Database::readAttributeDefaultRelation(std::string & line)
{
    smatch m;
    regex re("^BA_DEF_DEF_REL_ +\"([[:alnum:]_-]+)\" (.+);$");
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attributeDefault = attributeDefaults[attributeName];

        /* Name */
        attributeDefault.name = attributeName;

        /* Value Type */
        attributeDefault.valueType = attributeDefinition.valueType;

        /* Value */
        attributeDefault.stringValue = attributeValue;
        switch(attributeDefinition.valueType) {
        case AttributeValueType::Int:
            attributeDefault.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attributeDefault.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attributeDefault.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attributeDefault.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attributeDefault.stringValue = attributeValue;
            break;
        default:
            std::cerr << line << std::endl;
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/** Attribute Values (BA) for Network */
bool Database::readAttributeValueNetwork(std::string & line)
{
    smatch m;
    regex re("^BA_ \"([[:alnum:]_-]+)\" (.+);$");
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attribute.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        default:
            std::cerr << line << std::endl;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/** Attribute Values (BA) for Node (BU) */
bool Database::readAttributeValueNode(std::string & line)
{
    smatch mBU;
    regex reBU("^BA_ \"([[:alnum:]_-]+)\" BU_ ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, mBU, reBU)) {
        std::string attributeName = mBU[1];
        std::string nodeName = mBU[2];
        std::string attributeValue = mBU[3];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        attributeDefinition.name = attributeName;
        Node & node = nodes[nodeName];
        node.name = nodeName;
        Attribute & attribute = node.attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attribute.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        default:
            std::cerr << line << std::endl;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/** Attribute Values (BA) for Message (BO) */
bool Database::readAttributeValueMessage(std::string & line)
{
    smatch mBO;
    regex reBO("^BA_ \"([[:alnum:]_-]+)\" BO_ ([[:digit:]]+) (.+);$");
    if (regex_search(line, mBO, reBO)) {
        std::string attributeName = mBO[1];
        unsigned int messageId = stoul(mBO[2]);
        std::string attributeValue = mBO[3];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = messages[messageId].attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attribute.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        default:
            std::cerr << line << std::endl;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/** Attribute Values (BA) for Signal (SG) */
bool Database::readAttributeValueSignal(std::string & line)
{
    smatch mSG;
    regex reSG("^BA_ \"([[:alnum:]_-]+)\" SG_ ([[:digit:]]+) ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, mSG, reSG)) {
        std::string attributeName = mSG[1];
        unsigned int messageId = stoul(mSG[2]);
        std::string signalName = mSG[3];
        std::string attributeValue = mSG[4];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = messages[messageId].signals[signalName].attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attribute.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        default:
            std::cerr << line << std::endl;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/** Attribute Values (BA) for Environment Variable (EV) */
bool Database::readAttributeValueEnvironmentVariable(std::string & line)
{
    smatch mEV;
    regex reEV("^BA_ \"([[:alnum:]_-]+)\" EV_ ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, mEV, reEV)) {
        std::string attributeName = mEV[1];
        std::string envVarName = mEV[2];
        std::string attributeValue = mEV[3];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = environmentVariables[envVarName].attributeValues[attributeName];

        /* Name */
        attribute.name = attributeName;

        /* Value Type */
        attribute.valueType = attributeDefinition.valueType;

        /* Value */
        switch(attribute.valueType) {
        case AttributeValueType::Int:
            attribute.integerValue = stoul(attributeValue);
            break;
        case AttributeValueType::Hex:
            attribute.hexValue = stoul(attributeValue);
            break;
        case AttributeValueType::Float:
            attribute.floatValue = stod(attributeValue);
            break;
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attribute.stringValue = attributeValue;
            break;
        case AttributeValueType::Enum:
            attribute.enumValue = stoul(attributeValue);
            break;
        default:
            std::cerr << line << std::endl;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Attribute Values (BA) */
void Database::readAttributeValue(std::string & line)
{
    // for nodes (BU)
    if (readAttributeValueNode(line)) {
        return;
    }

    // for messages (BO)
    if (readAttributeValueMessage(line)) {
        return;
    }

    // for signals (SG)
    if (readAttributeValueSignal(line)) {
        return;
    }

    // for environment variables (EV)
    if (readAttributeValueEnvironmentVariable(line)) {
        return;
    }

    // for database
    if (readAttributeValueNetwork(line)) {
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Attribute Values at Relations (BA_REL) */
void Database::readAttributeRelationValue(std::string & line)
{
    bool found = false;
    AttributeRelation attributeRelation;
    std::string attributeValue;

    // for relation "Control Unit - Env. Variable" (BU_EV_REL)
    smatch mBU_EV_REL;
    regex reBU_EV_REL("^BA_REL_ \"([[:alnum:]_-]+)\" BU_EV_REL_ ([[:alnum:]_-]+) ([[:alnum:]_-]+) (.+);$");
    if (!found && regex_search(line, mBU_EV_REL, reBU_EV_REL)) {
        attributeRelation.name = mBU_EV_REL[1];
        attributeRelation.nodeName = mBU_EV_REL[2];
        attributeRelation.environmentVariableName = mBU_EV_REL[3];
        attributeValue = mBU_EV_REL[4];
        found = true;
    }

    // for relation "Node - Tx Message" (BU_BO_REL)
    smatch mBU_BO_REL;
    regex reBU_BO_REL("^BA_REL_ \"([[:alnum:]_-]+)\" BU_BO_REL_ ([[:alnum:]_-]+) ([[:digit:]]+) (.+);$");
    if (!found && regex_search(line, mBU_BO_REL, reBU_BO_REL)) {
        attributeRelation.name = mBU_BO_REL[1];
        attributeRelation.nodeName = mBU_BO_REL[2];
        attributeRelation.messageId = stoul(mBU_BO_REL[3]);
        attributeValue = mBU_BO_REL[4];
        found = true;
    }

    // for relation "Node - Mapped Rx Signal" (BU_SG_REL)
    smatch mBU_SG_REL;
    regex reBU_SG_REL("^BA_REL_ \"([[:alnum:]_-]+)\" BU_SG_REL_ ([[:alnum:]_-]+) SG_ ([[:digit:]]+) ([[:alnum:]_-]+) (.+);$");
    if (!found && regex_search(line, mBU_SG_REL, reBU_SG_REL)) {
        attributeRelation.name = mBU_SG_REL[1];
        attributeRelation.nodeName = mBU_SG_REL[2];
        attributeRelation.messageId = stoul(mBU_SG_REL[3]);
        attributeRelation.signalName = mBU_SG_REL[4];
        attributeValue = mBU_SG_REL[5];
        found = true;
    }

    if (found) {
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeRelation.name];
        attributeRelation.relationType = AttributeRelation::RelationType::ControlUnitEnvironmentVariable;
        attributeRelation.valueType = attributeDefinition.valueType;
        switch(attributeRelation.valueType) {
        // Integer
        case AttributeValueType::Int:
            attributeRelation.integerValue = stoul(attributeValue);
            break;
        // Hexadecimal
        case AttributeValueType::Hex:
            attributeRelation.hexValue = stoul(attributeValue);
            break;
        // Float
        case AttributeValueType::Float:
            attributeRelation.floatValue = stod(attributeValue);
            break;
        // String
        case AttributeValueType::String:
            attributeValue.erase(0, 1);
            attributeValue.pop_back();
            attributeRelation.stringValue = attributeValue;
            break;

        // Enumeration
        case AttributeValueType::Enum:
            attributeRelation.enumValue = stoul(attributeValue);
            break;
        default:
            std::cerr << line << std::endl;
        }
        attributeRelationValues.insert(attributeRelation);
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Value Descriptions (VAL) for Signals (SG) */
bool Database::readValueDescriptionSignal(std::string & line)
{
    smatch mS;
    regex reSig("^VAL_ ([[:digit:]]+) ([[:alnum:]_-]+) (.*) ;$");
    if (regex_search(line, mS, reSig)) {
        unsigned int messageId = stoul(mS[1]);
        std::string signalName = mS[2];
        ValueDescriptions & valueDescriptions = messages[messageId].signals[signalName].valueDescriptions;

        /* Value Description Pairs */
        std::istringstream iss(mS[3]);
        while (iss.good()) {
            std::string value;
            iss >> value;
            std::string description;
            iss >> description;
            while (description.back() != '"') {
                std::string nextStr;
                iss >> nextStr;
                description += " ";
                description += nextStr;
            }
            description.erase(0, 1);
            description.pop_back();
            valueDescriptions[stoul(value)] = description;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Value Descriptions (VAL) for Environment Variables (EV) */
bool Database::readValueDescriptionEnvironmentVariable(std::string & line)
{
    smatch mEV;
    regex reEnvVar("^VAL_ ([[:alnum:]_-]+) (.*) ;$");
    if (regex_search(line, mEV, reEnvVar)) {
        std::string envVarName = mEV[1];
        ValueDescriptions & valueDescriptions = environmentVariables[envVarName].valueDescriptions;

        /* Value Description Pairs */
        std::istringstream iss(mEV[2]);
        while (iss.good()) {
            std::string value;
            iss >> value;
            std::string description;
            iss >> description;
            while (description.back() != '"') {
                std::string nextStr;
                iss >> nextStr;
                description += " ";
                description += nextStr;
            }
            description.erase(0, 1);
            description.pop_back();
            valueDescriptions[stoul(value)] = description;
        }
        return true;
    }

    /* format doesn't match */
    return false;
}

/* Value Descriptions (VAL) */
void Database::readValueDescription(std::string & line)
{
    // for signal
    if (readValueDescriptionSignal(line)) {
        return;
    }

    // for environment variable
    if (readValueDescriptionEnvironmentVariable(line)) {
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Category Definitions (?, obsolete) */

/* Categories (?, obsolete) */

/* Filters (?, obsolete) */

/* Signal Type Refs (SGTYPE, obsolete) */
// see above readSignalType

/* Signal Groups (SIG_GROUP) */
void Database::readSignalGroup(std::string & line)
{
    smatch m;
    regex re("^SIG_GROUP_ ([[:digit:]]+) ([[:alnum:]_-]+) ([[:digit:]]) : (.+);$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string signalGroupName = m[2];
        SignalGroup & signalGroup = messages[messageId].signalGroups[signalGroupName];

        /* Message Identifier */
        signalGroup.messageId = messageId;

        /* Name */
        signalGroup.name = signalGroupName;

        /* Repetitions */
        signalGroup.repetitions = stoul(m[3]);

        /* Signals */
        std::istringstream iss(m[4]);
        while (iss.good()) {
            std::string signalName;
            iss >> signalName;
            signalGroup.signals.insert(signalName);
        }
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
void Database::readSignalExtendedValueType(std::string & line)
{
    smatch m;
    regex re("^SIG_VALTYPE_ ([[:digit:]]+) ([[:alnum:]_-]+) : ([[:digit:]]);$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string signalName = m[2];

        /* Extended Value Type */
        unsigned int signalExtendedValueType = stoul(m[3]);
        switch(signalExtendedValueType) {
        // Integer
        case 0:
            messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Integer;
            break;
        // Float
        case 1:
            messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Float;
            break;
        // Double
        case 2:
            messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Double;
            break;
        default:
            std::cerr << line << std::endl;
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

/* Extended Multiplexors (SG_MUL_VAL) */
void Database::readExtendedMultiplexor(std::string & line)
{
    smatch m;
    regex re("^SG_MUL_VAL_ ([[:digit:]]+) ([[:alnum:]_-]+) ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string multiplexedSignalName = m[2];
        std::string switchName = m[3];
        ExtendedMultiplexor & extendedMultiplexor = messages[messageId].signals[multiplexedSignalName].extendedMultiplexors[switchName];

        /* Switch Name */
        extendedMultiplexor.switchName = switchName;

        /* Multiplexor Value Range */
        std::istringstream iss(m[4]);
        while(iss.good()) {
            std::string multiplexorValueRange;
            std::getline(iss, multiplexorValueRange, ',');

            std::istringstream iss2(multiplexorValueRange);
            std::string minValue;
            std::getline(iss2, minValue, '-');
            std::string maxValue;
            std::getline(iss2, maxValue, '-');
            ExtendedMultiplexor::ValueRange valueRange;
            valueRange.first = stoul(minValue);
            valueRange.second = stoul(maxValue);
            extendedMultiplexor.valueRanges.insert(valueRange);
        }
        return;
    }

    /* format doesn't match */
    std::cerr << line << std::endl;
}

bool Database::load(const char * filename)
{
    std::ifstream ifs;

    /* use english decimal points for floating numbers */
    std::setlocale(LC_ALL, "C");

    /* open stream */
    ifs.open(filename, std::ifstream::in);
    if (!ifs.is_open()) {
        return false;
    }

    /* parse stream */
    while(ifs.good()) {
        std::string line;
        std::getline(ifs, line);
        chomp(line);

        smatch m;
        regex re("^([[:alnum:]_]+):? ");
        if (regex_search(line, m, re)) {
            std::string name = m[1];

            /* Version (VERSION) */
            if (name == "VERSION") {
                readVersion(line);
            } else

            /* New Symbols (NS) */
            if (name == "NS_") {
                readNewSymbols(ifs, line);
            } else

            /* Bit Timing (BS) */
            if (name == "BS_") {
                readBitTiming(line);
            } else

            /* Nodes (BU) */
            if (name == "BU_") {
                readNodes(line);
            } else

            /* Value Tables (VAL_TABLE) */
            if (name == "VAL_TABLE_") {
                readValueTable(line);
            } else

            /* Messages (BO) */
            if (name == "BO_") {
                readMessage(ifs, line);
            } else

            /* Message Transmitters (BO_TX_BU) */
            if (name == "BO_TX_BU_") {
                readMessageTransmitter(line);
            } else

            /* Environment Variables (EV) */
            if (name == "EV_") {
                readEnvironmentVariable(line);
            } else

            /* Environment Variable Data (ENVVAR_DATA) */
            if (name == "ENVVAR_DATA_") {
                readEnvironmentVariableData(line);
            } else

            /* Signal Types (SGTYPE, obsolete) */
            if (name == "SGTYPE_") {
                readSignalType(line);
            } else

            /* Comments (CM) */
            if (name == "CM_") {
                readComment(ifs, line);
            } else

            /* Attribute Definitions (BA_DEF) */
            if (name == "BA_DEF_") {
                readAttributeDefinition(line);
            } else

            /* Attribute Definitions at Relations (BA_DEF_REL) */
            if (name == "BA_DEF_REL_") {
                readAttributeDefinitionRelation(line);
            } else

            /* Sigtype Attr Lists (?, obsolete) */

            /* Attribute Defaults (BA_DEF_DEF) */
            if (name == "BA_DEF_DEF_") {
                readAttributeDefault(line);
            } else

            /* Attribute Defaults at Relations (BA_DEF_DEF_REL) */
            if (name == "BA_DEF_DEF_REL_") {
                readAttributeDefaultRelation(line);
            } else

            /* Attribute Values (BA) */
            if (name == "BA_") {
                readAttributeValue(line);
            } else

            /* Attribute Values at Relations (BA_REL) */
            if (name == "BA_REL_") {
                readAttributeRelationValue(line);
            } else

            /* Value Descriptions (VAL) */
            if (name == "VAL_") {
                readValueDescription(line);
            } else

            /* Category Definitions (CAT_DEF, obsolete) */

            /* Categories (CAT, obsolete) */

            /* Filters (FILTER, obsolete) */

            /* Signal Type Refs (SGTYPE, obsolete) */
            // see above readSignalType

            /* Signal Groups (SIG_GROUP) */
            if (name == "SIG_GROUP_") {
                readSignalGroup(line);
            } else

            /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
            if (name == "SIG_VALTYPE_") {
                readSignalExtendedValueType(line);
            } else

            /* Extended Multiplexors (SG_MUL_VAL) */
            if (name == "SG_MUL_VAL_") {
                readExtendedMultiplexor(line);
            } else

            /* not supported */
            {
                std::cerr << name << " not supported" << std::endl;
            }
        }
    }

    /* close stream */
    ifs.close();
    return true;
}

bool Database::load(std::string & filename)
{
    return load(filename.c_str());
}

}
}
