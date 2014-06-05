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

#include <list>
#include <map>
#include <stack>
#include <string>

#include "Attribute.h"
#include "AttributeDefinition.h"
#include "AttributeRelation.h"
#include "BitTiming.h"
#include "EnvironmentVariable.h"
#include "Message.h"
#include "Node.h"
#include "SignalType.h"
#include "Status.h"
#include "ValueDescriptions.h"
#include "ValueTable.h"

#include "vector_dbc_export.h"

namespace Vector {
namespace DBC {

/**
 * File
 */
class VECTOR_DBC_EXPORT File
{
public:
    File();

    /**
     * @brief Load database file
     * @param[in] filename File name
     * @return Status code
     *
     * Loads database file.
     */
    Status load(const char * filename);

    /**
     * @brief Load database file
     * @param[in] filename File Name
     * @return Status code
     *
     * Loads database file.
     */
    Status load(std::string & filename);

    /**
     * @brief Save database file
     * @param[in] filename File Name
     * @return Status code
     *
     * Saves database file.
     */
    Status save(const char * filename);

    /**
     * @brief Save database file
     * @param[in] filename File Name
     * @return Status code
     *
     * Saves database file.
     */
    Status save(std::string & filename);

    /**
     * Progress Callback function type
     */
    typedef void (*ProgressCallback)(float numerator, float denominator);

    /**
     * @brief Set Progress Callback function
     * @param[in] function Progress Callback function
     *
     * Set the callback function to get progress information.
     * On load this is the file position, so can be used for debugging also.
     */
    void setProgressCallback(ProgressCallback function);

    /**
     * Status Callback function type
     */
    typedef void (*StatusCallback)(Status status);

    /**
     * @brief Set Status Callback function
     * @param[in] function Status Callback function
     *
     * Set the callback function to get status information.
     * Even if the database loads fine, it might step over some warnings that can only be seen here.
     */
    void setStatusCallback(StatusCallback function);

    /** Version (VERSION) */
    std::string version;

    /** New Symbols (NS) */
    std::list<std::string> newSymbols;

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
    std::string comment; // for network
    // moved to Node (BU) for nodes
    // moved to Message (BO) for messages
    // moved to Signal (SG) for signals
    // moved to Environment Variable (EV) for environment variables

    /**
     * Attribute Definitions (BA_DEF) and
     * Attribute Definitions for Relations (BA_DEF_REL)
     */
    std::map<std::string, AttributeDefinition> attributeDefinitions;

    /* Sigtype Attr List (?, obsolete) */

    /**
     * Attribute Defaults (BA_DEF_DEF) and
     * Attribute Defaults for Relations (BA_DEF_DEF_REL)
     */
    std::map<std::string, Attribute> attributeDefaults;

    /** Attribute Values (BA) */
    std::map<std::string, Attribute> attributeValues; // for network
    // moved to Node (BU) for nodes
    // moved to Message (BO) for messages
    // moved to Signal (SG) for signals
    // moved to Environment Variable (EV) for environment variables

    /** Attribute Values on Relations (BA_REF) */
    std::set<AttributeRelation> attributeRelationValues;

    /* Value Descriptions (VAL) */
    // moved to Signals (BO) for signals
    // moved to EnvironmentVariable (EV) for environment variables

    /* Category Definitions (CAT_DEF, obsolete) */

    /* Categories (CAT, obsolete) */

    /* Filters (FILTER, obsolete) */

    /* Signal Type Refs (SGTYPE, obsolete) */
    // moved to Signal (SG)

    /* Signal Groups (SIG_GROUP) */
    // moved to Message (BO)

    /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    // moved to Signal (SG)

    /* Extended Multiplexors (SG_MUL_VAL) */
    // moved to Signal (SG)

private:
    /** Progress Callback function */
    ProgressCallback progressCallback;

    /** Status Callback function */
    StatusCallback statusCallback;

    /** Remove windows/unix/mac line endings */
    void chomp(std::string & line);

    /** stod with C locale */
    double stod(const std::string & str);

    /** Read Version (VERSION) */
    void readVersion(std::string & line);

    /** Write Version (VERSION) */
    void writeVersion(std::ofstream & ofs);

    /** Read New Symbols (NS) */
    void readNewSymbols(std::ifstream & ifs, std::string & line);

    /** Write New Symbols (NS) */
    void writeNewSymbols(std::ofstream & ofs);

    /** Read Bit Timing (BS) */
    void readBitTiming(std::string & line);

    /** Write Bit Timing (BS) */
    void writeBitTiming(std::ofstream & ofs);

    /** Read Nodes (BU) */
    void readNodes(std::string & line);

    /** Write Nodes (BU) */
    void writeNodes(std::ofstream & ofs);

    /** Read Value Table (VAL_TABLE) */
    void readValueTable(std::string & line);

    /** Write Value Tables (VAL_TABLE) */
    void writeValueTables(std::ofstream & ofs);

    /** Read Signal (SG) */
    void readSignal(Message & message, std::string & line);

    /* Write Signals (SG) */
    void writeSignals(std::ofstream & ofs, Message & message);

    /** Read Message (BO) */
    void readMessage(std::ifstream & ifs, std::string & line);

    /** Write Messages (BO) */
    void writeMessages(std::ofstream & ofs);

    /** Read Message Transmitter (BO_TX_BU) */
    void readMessageTransmitter(std::string & line);

    /** Write Message Transmitters (BO_TX_BU) */
    void writeMessageTransmitters(std::ofstream & ofs);

    /** Read Environment Variable (EV) */
    void readEnvironmentVariable(std::string & line);

    /** Write Environment Variables (EV) */
    void writeEnvironmentVariables(std::ofstream & ofs);

    /** Read Environment Variable Data (ENVVAR_DATA) */
    void readEnvironmentVariableData(std::string & line);

    /** Write Environment Variable Data (ENVVAR_DATA) */
    void writeEnvironmentVariableData(std::ofstream & ofs);

    /** Read Signal Type (SGTYPE, obsolete) */
    void readSignalType(std::string & line);

    /** Write Signal Types (SGTYPE, obsolete) */
    void writeSignalTypes(std::ofstream & ofs);

    /** Read Comment (CM) for Network */
    bool readCommentNetwork(std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Node (BU) */
    bool readCommentNode(std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Message (BO) */
    bool readCommentMessage(std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Signal (SG) */
    bool readCommentSignal(std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) for Environment Variable (EV) */
    bool readCommentEnvironmentVariable(std::stack<std::size_t> & lineBreaks, std::string & line);

    /** Read Comment (CM) */
    void readComment(std::ifstream & ifs, std::string & line);

    /** Write Comments (CM) for Networks */
    void writeCommentsNetworks(std::ofstream & ofs);

    /** Write Comments (CM) for Node (BU) */
    void writeCommentsNodes(std::ofstream & ofs);

    /** Write Comments (CM) for Message (BO) */
    void writeCommentsMessages(std::ofstream & ofs);

    /** Write Comments (CM) for Signal (SG) */
    void writeCommentsSignals(std::ofstream & ofs);

    /** Write Comments (CM) for Environment Variable (EV) */
    void writeCommentsEnvironmentVariables(std::ofstream & ofs);

    /** Write Comments (CM) */
    void writeComments(std::ofstream & ofs);

    /** Read Attribute Definition (BA_DEF) */
    void readAttributeDefinition(std::string & line);

    /** Read Attribute Definition at Relation (BA_DEF_REL) */
    void readAttributeDefinitionRelation(std::string & line);

    /** Write Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
    void writeAttributeDefinitions(std::ofstream & ofs);

    /* Read Sigtype Attr List (?, obsolete) */

    /* Write Sigtype Attr Lists (?, obsolete) */

    /** Read Attribute Default (BA_DEF_DEF) */
    void readAttributeDefault(std::string & line);

    /** Read Attribute Default at Relation (BA_DEF_DEF_REL) */
    void readAttributeDefaultRelation(std::string & line);

    /** Write Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
    void writeAttributeDefaults(std::ofstream & ofs);

    /** Read Attribute Value (BA) for Network */
    bool readAttributeValueNetwork(std::string & line);

    /** Read Attribute Value (BA) for Node (BU) */
    bool readAttributeValueNode(std::string & line);

    /** Read Attribute Value (BA) for Message (BO) */
    bool readAttributeValueMessage(std::string & line);

    /** Read Attribute Value (BA) for Signal (SG) */
    bool readAttributeValueSignal(std::string & line);

    /** Read Attribute Value (BA) for Environment Variable (EV) */
    bool readAttributeValueEnvironmentVariable(std::string & line);

    /** Read Attribute Value (BA) */
    void readAttributeValue(std::string & line);

    /** Write Attribute Values (BA) for Networks */
    void writeAttributeValuesNetworks(std::ofstream & ofs);

    /** Write Attribute Values (BA) for Nodes (BU) */
    void writeAttributeValuesNodes(std::ofstream & ofs);

    /** Write Attribute Values (BA) for Messages (BO) */
    void writeAttributeValuesMessages(std::ofstream & ofs);

    /** Write Attribute Values (BA) for Signals (SG) */
    void writeAttributeValuesSignals(std::ofstream & ofs);

    /** Write Attribute Values (BA) for Environment Variables (EV) */
    void writeAttributeValuesEnvironmentVariables(std::ofstream & ofs);

    /** Read Attribute Value at Relation (BA_REL) */
    void readAttributeRelationValue(std::string & line);

    /** Write Attribute Values at Relations (BA_REL) */
    void writeAttributeRelationValues(std::ofstream & ofs);

    /* Read Value Description (VAL) for Signal (SG) */
    bool readValueDescriptionSignal(std::string & line);

    /* Read Value Description (VAL) for Environment Variable (EV) */
    bool readValueDescriptionEnvironmentVariable(std::string & line);

    /** Read Value Description (VAL) */
    void readValueDescription(std::string & line);

    /* Write Value Descriptions (VAL) for Signals (SG) */
    void writeValueDescriptionsSignals(std::ofstream & ofs);

    /* Write Value Descriptions (VAL) for Environment Variables (EV) */
    void writeValueDescriptionsEnvironmentVariables(std::ofstream & ofs);

    /* Read Category Definition (?, obsolete) */

    /* Write Category Definitions (?, obsolete) */

    /* Read Category (?, obsolete) */

    /* Write Categories (?, obsolete) */

    /* Read Filter (?, obsolete) */

    /* Write Filters (?, obsolete) */

    /* Read Signal Type Ref (SGTYPE, obsolete) */
    // see above readSignalType

    /* Write Signal Type Refs (SGTYPE, obsolete) */
    // see above writeSignalTypes

    /** Read Signal Group (SIG_GROUP) */
    void readSignalGroup(std::string & line);

    /** Write Signal Groups (SIG_GROUP) */
    void writeSignalGroups(std::ofstream & ofs);

    /** Read Signal Extended Value Type (SIG_VALTYPE, obsolete) */
    void readSignalExtendedValueType(std::string & line);

    /** Write Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    void writeSignalExtendedValueTypes(std::ofstream & ofs);

    /** Read Extended Multiplexor (SG_MUL_VAL) */
    void readExtendedMultiplexor(std::string & line);

    /** Write Extended Multiplexors (SG_MUL_VAL) */
    void writeExtendedMultiplexors(std::ofstream & ofs);
};

}
}
