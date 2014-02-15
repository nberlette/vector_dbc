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

#include <map>
#include <set>
#include <string>

#include "Attribute.h"
#include "AttributeDefinition.h"
#include "BitTiming.h"
#include "EnvironmentVariable.h"
#include "Message.h"
#include "Node.h"
#include "SignalType.h"
#include "ValueDescriptions.h"
#include "ValueTable.h"

namespace Vector {
namespace DBC {

/**
 * Database
 */
class Database
{
public:
    Database();

    /**
     * @brief Load Database
     * @param[in] filename File name
     * @return True if successful
     *
     * Loads database.
     */
    bool load(const char * filename);

    /**
     * @brief Load Database
     * @param[in] filename File Name
     * @return True if successful
     *
     * Loads database.
     */
    bool load(std::string & filename);

    /**
     * @brief Save Database
     * @param[in] filename File Name
     * @return True if successful
     *
     * Saves database.
     */
    bool save(const char * filename);

    /**
     * @brief Save Database
     * @param[in] filename File Name
     * @return True if successful
     *
     * Saves database.
     */
    bool save(std::string & filename);

    /** Version (VERSION) */
    std::string version;

    /** New Symbols (NS) */
    std::set<std::string> newSymbols;

    /** Bit Timing (BS) */
    BitTiming bitTiming;

    /** Nodes (BU) */
    std::map<std::string, Node> nodes;

    /** Value Tables (VAL_TABLE) */
    std::map<std::string, ValueTable> valueTables;

    /** Messages (BO) */
    std::map<unsigned int, Message> messages;

    /* Message Transmitters (BO_TX_BU) */
    // moved to Message (BO)

    /** Environment Variables (EV) */
    std::map<std::string, EnvironmentVariable> environmentVariables;

    /* Environment Variables Data (ENVVAR_DATA) */
    // moved to Environment Variables (EV)

    /** Signal Types (SGTYPE, obsolete) */
    std::map<std::string, SignalType> signalTypes;

    /** Comments (CM) */
    std::string comment; // for database
    // moved to Node (BU) for nodes
    // moved to Message (BO) for messages
    // moved to Signal (SG) for signals
    // moved to Environment Variable (EV) for environment variables

    /** Attribute Definitions (BA_DEF) */
    std::map<std::string, AttributeDefinition> attributeDefinitions;

    /* Sigtype Attr List (?, obsolete) */

    /** Attribute Defaults (BA_DEF_DEF) */
    std::map<std::string, Attribute> attributeDefaults;

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues; // for database
    // moved to Node (BU) for nodes
    // moved to Message (BO) for messages
    // moved to Signal (SG) for signals
    // moved to Environment Variable (EV) for environment variables

    /* Value Descriptions (VAL) */
    // moved to Signals (BO) for signals
    // moved to EnvironmentVariable (EV) for environment variables

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filter (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    // moved to Signal (SG)

    /* Signal Groups (SIG_GROUP) */
    // moved to Message (BO)

    /* Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    // moved to Signal (SG)

    /* Extended Multiplexing */
    // moved to Signal (SG)

private:
    /* remove windows line endings */
    void chomp(std::string & line);

    /* Version (VERSION) */
    void readVersion(std::string & line);

    /* New Symbols (NS) */
    void readNewSymbols(std::ifstream & ifs, std::string & line);

    /* Bit Timing (BS) */
    void readBitTiming(std::string & line);

    /* Nodes (BU) */
    void readNodes(std::string & line);

    /* Value Tables (VAL_TABLE) */
    void readValueTable(std::string & line);

    /* Signals (SG) */
    void readSignal(Message & message, std::string & line);

    /* Messages (BO) */
    void readMessage(std::ifstream & ifs, std::string & line);

    /* Message Transmitters (BO_TX_BU) */
    void readMessageTransmitter(std::string & line);

    /* Environment Variables (EV) */
    void readEnvironmentVariable(std::string & line);

    /* Environment Variables Data (ENVVAR_DATA) */
    void readEnvironmentVariableData(std::string & line);

    /* Signal Types (SGTYPE, obsolete) */
    void readSignalType(std::string & line);

    /* Comments (CM) */
    void readComment(std::ifstream & ifs, std::string & line);

    /* Attribute Definitions (BA_DEF) */
    void readAttributeDefinition(std::string & line);

    /* Sigtype Attr List (?, obsolete) */

    /* Attribute Defaults (BA_DEF_DEF) */
    void readAttributeDefault(std::string & line);

    /* Attribute Values (BA) */
    void readAttributeValue(std::string & line);

    /* Value Descriptions (VAL) */
    void readValueDescription(std::string & line);

    /* Category Definitions (?, obsolete) */

    /* Categories (?, obsolete) */

    /* Filter (?, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    // see above readSignalType

    /* Signal Groups (SIG_GROUP) */
    void readSignalGroup(std::string & line);

    /* Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    void readSignalExtendedValueType(std::string & line);

    /* Extended Multiplexing (SG_MUL_VAL) */
    void readExtendedMultiplexor(std::string & line);
};

}
}
