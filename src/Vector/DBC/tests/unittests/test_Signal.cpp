#define BOOST_TEST_MODULE Signal
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iterator>
#include <boost/filesystem.hpp>

#include "Vector/DBC.h"

BOOST_AUTO_TEST_CASE(Signal)
{
    Vector::DBC::Signal signal;

    /* check for factor and offset */
    signal.factor = 1;
    signal.offset = 0;
    BOOST_CHECK_EQUAL(signal.rawToPhysicalValue(1.0), 1.0);
    BOOST_CHECK_EQUAL(signal.physicalToRawValue(1.0), 1.0);
    // division by zero
    signal.factor = 0;
    BOOST_CHECK_EQUAL(signal.physicalToRawValue(1.0), 0.0);

    /* check for minimum and maximum */
    // undefined
    signal.bitSize = 16;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Undefined;
    signal.valueType = Vector::DBC::ValueType::Signed;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), -32768.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 32767.0);
    signal.valueType = Vector::DBC::ValueType::Unsigned;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), 0.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 65535.0);
    // integer
    signal.bitSize = 16;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
    signal.valueType = Vector::DBC::ValueType::Signed;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), -32768.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 32767.0);
    signal.valueType = Vector::DBC::ValueType::Unsigned;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), 0.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 65535.0);
    // float
    signal.bitSize = 32;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Float;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), 3.4e-38);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 3.4e38);
    // double
    signal.bitSize = 64;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Double;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), 1.7e-308);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 1.7e308);
}
