#define BOOST_TEST_MODULE Message
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "Vector/DBC.h"

#define SUCCEEDED(code) ((int)(code) >= 0)
#define WARNED(code) ((int)(code) & 0x40000000)
#define FAILED(code) ((int)(code) < 0)

void statusCallback(Vector::DBC::Network & /* network */, Vector::DBC::Status status)
{
    if (FAILED(status)) {
        std::cerr << "Error: 0x" << std::hex << (int) status << std::endl;
    } else if (WARNED(status)) {
        std::cout << "Warning: 0x" << std::hex << (int) status << std::endl;
    } else if (SUCCEEDED(status)) {
        std::cout << "Success: 0x" << std::hex << (int) status << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(File)
{
    Vector::DBC::Network network;
    Vector::DBC::File file;
    file.setStatusCallback(&statusCallback);

    /* load database file */
    boost::filesystem::path infile(CMAKE_CURRENT_SOURCE_DIR "/data/Database.dbc");
    std::string infilename = infile.string();
    std::cout << "Input file: " << infilename << std::endl;
    BOOST_REQUIRE(file.load(network, infilename) == Vector::DBC::Status::Ok);

    /* define message data */
    std::vector<uint8_t> messageData;
    messageData.push_back(0x00);
    messageData.push_back(0x00);
    messageData.push_back(0x00);
    messageData.push_back(0x00);
    messageData.push_back(0x00);
    messageData.push_back(0x00);
    messageData.push_back(0x00);
    messageData.push_back(0x00);

    /* extract signal */
    Vector::DBC::Message & message = network.messages[0xC0000000];
    Vector::DBC::Signal & signal = message.signals["Signal_8_Motorola_Unsigned"];
    uint64_t rawValue = signal.extract(messageData);
    BOOST_CHECK_EQUAL(rawValue, 0x00000000);
}
