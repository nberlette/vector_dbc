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

#include <Vector/DBC/Network.h>

namespace Vector {
namespace DBC {

Network::Network() :
    version(),
    newSymbols(),
    bitTiming(),
    nodes(),
    valueTables(),
    messages(),
    environmentVariables(),
    signalTypes(),
    comment(),
    attributeDefinitions(),
    attributeDefaults(),
    attributeValues(),
    attributeRelationValues()
{
    /* nothing to do here */
}

std::ostream & operator<<(std::ostream & os, Network & obj)
{
    /* use english decimal points for floating numbers */
    os.imbue(std::locale("C"));

    os.precision(16);

    /* Version (VERSION) */
    // @todo

    /* New Symbols (NS) */
    // @todo

    /* Bit Timing (BS) */
    // @todo

    /* Nodes (BU) */
    // @todo

    /* Value Tables (VAL_TABLE) */
    // @todo

    /* Messages (BO) */
    // @todo

    /* Message Transmitters (BO_TX_BU) */
    // @todo

    /* Environment Variables (EV) */
    // @todo

    /* Environment Variable Data (ENVVAR_DATA) */
    // @todo

    /* Signal Types (SGTYPE, obsolete) */
    // @todo

    /* Comments (CM) */
    // @todo

    /* Attribute Definitions (BA_DEF) and Attribute Definitions at Relations (BA_DEF_REL) */
    // @todo

    /* Sigtype Attr Lists (?, obsolete) */
    // @todo

    /* Attribute Defaults (BA_DEF_DEF) and Attribute Defaults at Relations (BA_DEF_DEF_REL) */
    // @todo

    /* Attribute Values (BA) */
    // @todo

    /* Attribute Values at Relations (BA_REL) */
    // @todo

    /* Value Descriptions (VAL) */
    // @todo

    /* Category Definitions (CAT_DEF, obsolete) */
    // @todo

    /* Categories (CAT, obsolete) */
    // @todo

    /* Filters (FILTER, obsolete) */
    // @todo

    /* Signal Type Refs (SGTYPE, obsolete) */
    // @todo

    /* Signal Groups (SIG_GROUP) */
    // @todo

    /* Signal Extended Value Types (SIG_VALTYPE, obsolete) */
    // @todo

    /* Extended Multiplexors (SG_MUL_VAL) */
    // @todo

    /* close stream */
    ofs << endl;

    return os;
}

}
}
