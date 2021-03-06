/*
 * Copyright (C) 2013-2019 Tobias Lorenz.
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

#include <Vector/DBC/BitTiming.h>

namespace Vector {
namespace DBC {

std::ostream & operator<<(std::ostream & os, const BitTiming & bitTiming) {
    os << "BS_:";
    if (bitTiming.baudrate || bitTiming.btr1 || bitTiming.btr2)
        os << ' ' << bitTiming.baudrate << ':' << bitTiming.btr1 << ':' << bitTiming.btr2;
    os << endl;

    return os;
}

}
}
