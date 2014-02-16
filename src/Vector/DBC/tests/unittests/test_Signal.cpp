#define BOOST_TEST_MODULE Database
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iterator>
#include <boost/filesystem.hpp>

#include "Vector/DBC/Signal.h"

BOOST_AUTO_TEST_CASE(Signal)
{
    Vector::DBC::Signal signal;

    /* normal checks */
    signal.factor = 1;
    signal.offset = 0;
    BOOST_CHECK_EQUAL(signal.rawToPhysicalValue(1.0), 1.0);
    BOOST_CHECK_EQUAL(signal.physicalToRawValue(1.0), 1.0);

    /* safety checks */
    signal.factor = 0;
    BOOST_CHECK_EQUAL(signal.physicalToRawValue(1.0), 0.0);
}
