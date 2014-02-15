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

void Database::chomp(std::string & line)
{
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
        valueTable.name = valueTableName;
        ValueDescriptions & valueDescriptions = valueTable.valueDescriptions;
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
        signal.name = signalName;

        signal.multiplexedSignal = (m[2] == 'm');
        std::string multiplexerSwitchValue = m[3];
        if (multiplexerSwitchValue.empty()) {
            signal.multiplexerSwitchValue = 0;
        } else {
            signal.multiplexerSwitchValue = stoul(multiplexerSwitchValue);
        }
        signal.multiplexorSwitch = (m[4] == 'M');

        signal.startBit = stoul(m[5]);
        signal.size = stoul(m[6]);
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

        signal.factor = stod(m[9]);
        signal.offset = stod(m[10]);

        signal.minimum = stod(m[11]);
        signal.maximum = stod(m[12]);

        signal.unit = m[13];

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

    std::cerr << line << std::endl;
}

/* Messages (BO) */
void Database::readMessage(std::ifstream & ifs, std::string & line)
{
    smatch m;
    regex re("^BO_ ([[:digit:]]+) ([[:alnum:]_-]+) ?: ([[:digit:]]+) ([[:alnum:]_-]+)$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        Message & message =  messages[messageId];
        message.id = messageId;
        message.name = m[2];
        message.size = stoul(m[3]);
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
        std::istringstream iss(m[2]);
        while(iss.good()) {
            std::string transmitter;
            iss >> transmitter;
            message.transmitters.insert(transmitter);
        }
        return;
    }

    std::cerr << line << std::endl;
}

/* Environment Variables (EV) */
void Database::readEnvironmentVariable(std::string & line)
{
    smatch m;
    regex re("^EV_ ([[:alnum:]_-]+) : ([[:digit:]]+) .([[:digit:].+-]+),([[:digit:].+-]+). \"(.*)\" ([[:digit:].+-]+) ([[:digit:]]+) DUMMY_NODE_VECTOR([[:digit:]]+) (.+) ;$");
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        EnvironmentVariable & environmentVariable = environmentVariables[envVarName];
        environmentVariable.name = envVarName;
        std::string envVarType = m[2];
        switch(envVarType.front()) {
        case '0':
            environmentVariable.type = EnvironmentVariable::Type::Integer;
            break;
        case '1':
            environmentVariable.type = EnvironmentVariable::Type::Float;
            break;
        case '2':
            environmentVariable.type = EnvironmentVariable::Type::String;
            break;
        default:
            std::cerr << line << std::endl;
        }
        environmentVariable.minimum = stod(m[3]);
        environmentVariable.maximum = stod(m[4]);
        environmentVariable.unit = m[5];
        environmentVariable.initialValue = stod(m[6]);
        environmentVariable.id = stoul(m[7]);
        environmentVariable.accessType = stoul(m[8], nullptr, 16);
        std::istringstream iss(m[9]);
        while(iss.good()) {
            std::string accessNode;
            std::getline(iss, accessNode, ',');
            environmentVariable.accessNodes.insert(accessNode);
        }
        return;
    }

    std::cerr << line << std::endl;
}

/* Environment Variables Data (ENVVAR_DATA) */
void Database::readEnvironmentVariableData(std::string & line)
{
    smatch m;
    regex re("^ENVVAR_DATA_ ([[:alnum:]_-]+) : ([[:digit:]]+);$");
    if (regex_search(line, m, re)) {
        std::string envVarName = m[1];
        EnvironmentVariable & environmentVariable = environmentVariables[envVarName];
        environmentVariable.name = envVarName;
        environmentVariable.dataSize = stoul(m[2]);
        return;
    }

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
        signalType.name = signalTypeName;
        signalType.size = stoul(mST[2]);
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
        signalType.factor = stod(mST[5]);
        signalType.offset = stod(mST[6]);
        signalType.minimum = stod(mST[7]);
        signalType.maximum = stod(mST[8]);
        signalType.unit = mST[9];
        signalType.defaultValue = stod(mST[10]);
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

    std::cerr << line << std::endl;
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
        return;
    }

    // for messages (BO)
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
        return;
    }

    // for signals (SG)
    smatch mSG;
    regex reSG("^CM_ SG_ ([[:digit:]]+) ([[:alnum:]_-]+) \"(.+)\";$");
    if (regex_search(line, mSG, reSG)) {
        unsigned int messageId = stoul(mBO[1]);
        std::string signalName = mSG[2];
        std::string comment2 = mSG[3];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        messages[messageId].signals[signalName].comment = comment2;
        return;
    }

    // for environment variables (EV)
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
        return;
    }

    // for database
    smatch m;
    regex re("^CM_ \"(.*)\";$");
    if (regex_search(line, m, re)) {
        std::string comment2 = m[1];
        while(!lineBreaks.empty()) {
            comment2.insert(lineBreaks.top(), "\r\n");
            lineBreaks.pop();
        }
        comment = comment2;
        return;
    }

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
        if (objectType == "") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Database;
        } else
        if (objectType == "BU_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Node;
        } else
        if (objectType == "BO_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Message;
        } else
        if (objectType == "SG_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::Signal;
        } else
        if (objectType == "EV_") {
            attributeDefinition.objectType = AttributeDefinition::ObjectType::EnvironmentVariable;
        } else {
            std::cerr << line << std::endl;
        }

        if (attributeValueType == "INT") {
            attributeDefinition.valueType = AttributeValueType::Int;
            std::string value;
            iss >> value;
            attributeDefinition.minimumIntegerValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumIntegerValue = stoul(value);
        } else
        if (attributeValueType == "HEX") {
            attributeDefinition.valueType = AttributeValueType::Hex;
            std::string value;
            iss >> value;
            attributeDefinition.minimumHexValue = stoul(value);
            iss >> value;
            attributeDefinition.maximumHexValue = stoul(value);
        } else
        if (attributeValueType == "FLOAT") {
            attributeDefinition.valueType = AttributeValueType::Float;
            std::string value;
            iss >> value;
            attributeDefinition.minimumFloatValue = stod(value);
            iss >> value;
            attributeDefinition.maximumFloatValue = stod(value);
        } else
        if (attributeValueType == "STRING") {
            attributeDefinition.valueType = AttributeValueType::String;
        } else
        if (attributeValueType == "ENUM") {
            attributeDefinition.valueType = AttributeValueType::Enum;
            while(iss.good()) {
                std::string value;
                std::getline(iss, value, ',');
                value.erase(0, 1);
                value.pop_back();
                attributeDefinition.enumValues.push_back(value);
            }
        } else {
            std::cerr << line << std::endl;
        }
        return;
    }

    std::cerr << line << std::endl;
}

/* Sigtype Attr List (?, obsolete) */

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
        attributeDefault.name = attributeName;
        attributeDefault.valueType = attributeDefinition.valueType;
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

    std::cerr << line << std::endl;
}

/* Attribute Values (BA) */
void Database::readAttributeValue(std::string & line)
{
    // for nodes (BU)
    smatch mBU;
    regex reBU("^BA_ \"([[:alnum:]_-]+)\" BU_ ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, mBU, reBU)) {
        std::string attributeName = mBU[1];
        std::string nodeName = mBU[2];
        std::string attributeValue = mBU[3];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = nodes[nodeName].attributeValues[attributeName];
        attribute.name = attributeName;
        attribute.valueType = attributeDefinition.valueType;
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
        return;
    }

    // for messages (BO)
    smatch mBO;
    regex reBO("^BA_ \"([[:alnum:]_-]+)\" BO_ ([[:digit:]]+) (.+);$");
    if (regex_search(line, mBO, reBO)) {
        std::string attributeName = mBO[1];
        unsigned int messageId = stoul(mBO[2]);
        std::string attributeValue = mBO[3];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = messages[messageId].attributeValues[attributeName];
        attribute.name = attributeName;
        attribute.valueType = attributeDefinition.valueType;
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
        return;
    }

    // for signals (SG)
    smatch mSG;
    regex reSG("^BA_ \"([[:alnum:]_-]+)\" SG_ ([[:digit:]]+) ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, mSG, reSG)) {
        std::string attributeName = mSG[1];
        unsigned int messageId = stoul(mSG[2]);
        std::string signalName = mSG[3];
        std::string attributeValue = mSG[4];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = messages[messageId].signals[signalName].attributeValues[attributeName];
        attribute.name = attributeName;
        attribute.valueType = attributeDefinition.valueType;
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
        return;
    }

    // for environment variables (EV)
    smatch mEV;
    regex reEV("^BA_ \"([[:alnum:]_-]+)\" EV_ ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, mEV, reEV)) {
        std::string attributeName = mEV[1];
        std::string envVarName = mEV[2];
        std::string attributeValue = mEV[3];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = environmentVariables[envVarName].attributeValues[attributeName];
        attribute.name = attributeName;
        attribute.valueType = attributeDefinition.valueType;
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
        return;
    }

    // for database
    smatch m;
    regex re("^BA_ \"([[:alnum:]_-]+)\" (.+);$");
    if (regex_search(line, m, re)) {
        std::string attributeName = m[1];
        std::string attributeValue = m[2];
        AttributeDefinition & attributeDefinition = attributeDefinitions[attributeName];
        Attribute & attribute = attributeValues[attributeName];
        attribute.name = attributeName;
        attribute.valueType = attributeDefinition.valueType;
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
        return;
    }

    std::cerr << line << std::endl;
}

/* Value Descriptions (VAL) */
void Database::readValueDescription(std::string & line)
{
    // for signal
    smatch mS;
    regex reSig("^VAL_ ([[:digit:]]+) ([[:alnum:]_-]+) (.*) ;$");
    if (regex_search(line, mS, reSig)) {
        unsigned int messageId = stoul(mS[1]);
        std::string signalName = mS[2];
        ValueDescriptions & valueDescriptions = messages[messageId].signals[signalName].valueDescriptions;
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
        return;
    }

    // for environment variable
    smatch mEV;
    regex reEnvVar("^VAL_ ([[:alnum:]_-]+) (.*) ;$");
    if (regex_search(line, mEV, reEnvVar)) {
        std::string envVarName = mEV[1];
        ValueDescriptions & valueDescriptions = environmentVariables[envVarName].valueDescriptions;
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
        return;
    }

    std::cerr << line << std::endl;
}

/* Category Definitions (?, obsolete) */

/* Categories (?, obsolete) */

/* Filter (?, obsolete) */

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
        signalGroup.messageId = messageId;
        signalGroup.name = signalGroupName;
        signalGroup.repetitions = stoul(m[3]);
        std::istringstream iss(m[4]);
        while (iss.good()) {
            std::string signalName;
            iss >> signalName;
            signalGroup.signals.insert(signalName);
        }
    }
    std::cerr << line << std::endl;
}

/* Signal Extended Value Type (SIG_VALTYPE, obsolete) */
void Database::readSignalExtendedValueType(std::string & line)
{
    smatch m;
    regex re("^SIG_VALTYPE_ ([[:digit:]]+) ([[:alnum:]_-]+) : ([[:digit:]]);$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string signalName = m[2];
        unsigned int signalExtendedValueType = stoul(m[3]);
        switch(signalExtendedValueType) {
        case 0:
            messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Integer;
            break;
        case 1:
            messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Float;
            break;
        case 2:
            messages[messageId].signals[signalName].extendedValueType = Signal::ExtendedValueType::Double;
            break;
        default:
            std::cerr << line << std::endl;
        }
        return;
    }

    std::cerr << line << std::endl;
}

/* Extended Multiplexing (SG_MUL_VAL) */
void Database::readExtendedMultiplexor(std::string & line)
{
    smatch m;
    regex re("^SG_MUL_VAL_ ([[:digit:]]+) ([[:alnum:]_-]+) ([[:alnum:]_-]+) (.+);$");
    if (regex_search(line, m, re)) {
        unsigned int messageId = stoul(m[1]);
        std::string multiplexedSignalName = m[2];
        std::string switchName = m[3];
        ExtendedMultiplexor & extendedMultiplexor = messages[messageId].signals[multiplexedSignalName].extendedMultiplexors[switchName];
        extendedMultiplexor.switchName = switchName;
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

    std::cerr << line << std::endl;
}

bool Database::load(const char * filename)
{
    std::ifstream ifs;

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
        regex re("^([[:alnum:]_]+) ");
        if (regex_search(line, m, re)) {
            std::string name = m[1];

            /* Version (VERSION) */
            if (name == "VERSION") {
                readVersion(line);
                goto next;
            }

            /* New Symbols (NS) */
            if (name == "NS_") {
                readNewSymbols(ifs, line);
                goto next;
            }

            /* Bit Timing (BS) */
            if (name == "BS_") {
                readBitTiming(line);
                goto next;
            }

            /* Nodes (BU) */
            if (name == "BU_") {
                readNodes(line);
                goto next;
            }

            /* Value Tables (VAL_TABLE) */
            if (name == "VAL_TABLE_") {
                readValueTable(line);
                goto next;
            }

            /* Messages (BO) */
            if (name == "BO_") {
                readMessage(ifs, line);
                goto next;
            }

            /* Message Transmitters (BO_TX_BU) */
            if (name == "BO_TX_BU_") {
                readMessageTransmitter(line);
                goto next;
            }

            /* Environment Variables (EV) */
            if (name == "EV_") {
                readEnvironmentVariable(line);
                goto next;
            }

            /* Environment Variables Data (ENVVAR_DATA) */
            if (name == "ENVVAR_DATA_") {
                readEnvironmentVariableData(line);
                goto next;
            }

            /* Signal Types (SGTYPE, obsolete) */
            if (name == "SGTYPE_") {
                readSignalType(line);
                goto next;
            }

            /* Comments (CM) */
            if (name == "CM_") {
                readComment(ifs, line);
                goto next;
            }

            /* Attribute Definitions (BA_DEF) */
            if (name == "BA_DEF_") {
                readAttributeDefinition(line);
                goto next;
            }

            /* Sigtype Attr List (?, obsolete) */

            /* Attribute Defaults (BA_DEF_DEF) */
            if (name == "BA_DEF_DEF_") {
                readAttributeDefault(line);
                goto next;
            }

            /* Attribute Values (BA) */
            if (name == "BA_") {
                readAttributeValue(line);
                goto next;
            }

            /* Value Descriptions (VAL) */
            if (name == "VAL_") {
                readValueDescription(line);
                goto next;
            }

            /* Category Definitions (CAT_DEF, obsolete) */

            /* Categories (CAT, obsolete) */

            /* Filter (FILTER, obsolete) */

            /* Signal Type Refs (SGTYPE, obsolete) */
            // see above readSignalType

            /* Signal Groups (SIG_GROUP) */
            if (name == "SIG_GROUP_") {
                readSignalGroup(line);
                goto next;
            }

            /* Signal Extended Value Type (SIG_VALTYPE, obsolete) */
            if (name == "SIG_VALTYPE_") {
                readSignalExtendedValueType(line);
                goto next;
            }

            /* Extended Multiplexing (SG_MUL_VAL) */
            if (name == "SG_MUL_VAL_") {
                readExtendedMultiplexor(line);
                goto next;
            }

            /* not supported */
            std::cerr << name << " not supported" << std::endl;
            next: ;
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
