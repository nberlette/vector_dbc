#define BOOST_TEST_MODULE Regression
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <boost/filesystem.hpp>

#include "Vector/DBC.h"

#define SUCCEEDED(code) ((int)(code) >= 0)
#define WARNED(code) ((int)(code) & 0x40000000)
#define FAILED(code) ((int)(code) < 0)

void statusCallback(Vector::DBC::Network & /*network*/, Vector::DBC::Status status)
{
    if (FAILED(status)) {
        std::cerr << "Error: 0x" << std::hex << (int) status << std::endl;
    } else if (WARNED(status)) {
        std::cout << "Warning: 0x" << std::hex << (int) status << std::endl;
    } else if (SUCCEEDED(status)) {
        std::cout << "Success: 0x" << std::hex << (int) status << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(Regression)
{
    Vector::DBC::Network network;
    Vector::DBC::File file;
    file.setStatusCallback(&statusCallback);

    /* load database file */
    boost::filesystem::path infile(CMAKE_CURRENT_SOURCE_DIR "/data/Regression.dbc");
    std::string infilename = infile.string();
    std::cout << "Input file: " << infilename << std::endl;
    BOOST_REQUIRE(file.load(network, infilename) == Vector::DBC::Status::Ok);

    /*
     * The DBC file contains two messages that are separated by a line with one space.
     * In previous versions this wasn't interpreted as message separator.
     * Hence the signals of the second message were appended to the first message.
     * chomp was modified to also chop away spaces and tabs.
     */
    Vector::DBC::Message & message = network.messages[1];
    BOOST_REQUIRE(message.id == 1);
    BOOST_CHECK(message.name == "Message_1");
    Vector::DBC::Signal & signal = message.signals["Signal_1"];
    BOOST_CHECK(signal.name == "Signal_1");
    message = network.messages[2];
    BOOST_REQUIRE(message.id == 2);
    BOOST_CHECK(message.name == "Message_2");
    signal = message.signals["Signal_2"];
    BOOST_CHECK(signal.name == "Signal_2");

    /*
     * In Value Descriptions a description with leading space let to undefined behavior.
     * Example: 1 " Error B" 0 " Error A"
     * The algorithm to evaluate these value/description pairs was rewritten.
     */
    message = network.messages[2];
    signal = message.signals["Signal_2"];
    Vector::DBC::ValueDescriptions & valueDescriptions = signal.valueDescriptions;
    BOOST_CHECK(valueDescriptions[0] == " Error A");
    BOOST_CHECK(valueDescriptions[1] == " Error B");
}
