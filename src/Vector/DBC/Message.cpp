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

#include "Message.h"

namespace Vector {
namespace DBC {

Message::Message() :
    id(0),
    name(),
    size(0),
    transmitter(),
    signals(),
    transmitters(),
    signalGroups(),
    comment(),
    attributeValues()
{
    /* nothing to do here */
}

std::uint64_t Message::extractSignal(std::vector<std::uint8_t> & data, Signal & signal)
{
    std::uint64_t retVal = 0;
    unsigned int size = signal.bitSize;

    /* copy bits */
    if (signal.byteOrder == ByteOrder::BigEndian) {
        /* start with MSB */
        unsigned int srcBit = signal.startBit;
        unsigned int dstBit = size - 1;
        while(size > 0) {
            /* copy bit */
            if (data[srcBit/8] & (1<<(srcBit%8))) {
                retVal |= (1<<dstBit);
            }

            /* calculate next position */
            if ((srcBit % 8) == 0) {
                srcBit += 15;
            } else {
                --srcBit;
            }
            --dstBit;
            --size;
        }
    } else {
        /* start with LSB */
        unsigned int srcBit = signal.startBit;
        unsigned int dstBit = 0;
        while(size > 0) {
            /* copy bit */
            if (data[srcBit/8] & (1<<(srcBit%8))) {
                retVal |= (1<<dstBit);
            }

            /* calculate next position */
            ++srcBit;
            ++dstBit;
            --size;
        }
    }

    /* if signed, then fill all bits above MSB with 1 */
    if (signal.valueType == ValueType::Signed) {
        uint64_t msb = (retVal >> (size - 1)) & 1;
        if (msb) {
            for (unsigned int i = size; i < 8*sizeof(retVal); ++i) {
                retVal |= (1<<i);
            }
        }
    }

    return retVal;
}

}
}
