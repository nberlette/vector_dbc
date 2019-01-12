#define BOOST_TEST_MODULE File
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <boost/filesystem.hpp>

#include <Vector/DBC.h>

BOOST_AUTO_TEST_CASE(NewParser)
{
    Vector::DBC::Network network;

    /* load database file */
    boost::filesystem::path infile(CMAKE_CURRENT_SOURCE_DIR "/data/Database.dbc");

    /* put in own namespace to see that File constructs and destructs correctly */
    {
        std::ifstream file(infile.string());
        Vector::DBC::Network network;
        file >> network;
        BOOST_REQUIRE(network.successfullyParsed);
    }
}

BOOST_AUTO_TEST_CASE(File)
{
    Vector::DBC::Network network;

    /* load database file */
    boost::filesystem::path infile(CMAKE_CURRENT_SOURCE_DIR "/data/Database.dbc");

    /* put in own namespace to see that File constructs and destructs correctly */
    {
        Vector::DBC::File file;
        std::string infilename = infile.string();
        BOOST_REQUIRE(file.load(network, infilename) == Vector::DBC::Status::Ok);
    }

    /* create output directory */
    boost::filesystem::path outdir(CMAKE_CURRENT_BINARY_DIR "/data/");
    if (!exists(outdir)) {
        BOOST_REQUIRE(create_directory(outdir));
    }

    /* save database file */
    boost::filesystem::path outfile(CMAKE_CURRENT_BINARY_DIR "/data/Database.dbc");

    /* put in own namespace to see that File constructs and destructs correctly */
    {
        Vector::DBC::File file;
        std::string outfilename = outfile.string();
        BOOST_REQUIRE(file.save(network, outfilename) == Vector::DBC::Status::Ok);
    }

    /* loaded and saved file should be equivalent */
    std::ifstream ifs1(infile.c_str());
    std::ifstream ifs2(outfile.c_str());
    std::istream_iterator<char> b1(ifs1), e1;
    std::istream_iterator<char> b2(ifs2), e2;
    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
