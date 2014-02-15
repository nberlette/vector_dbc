#define BOOST_TEST_MODULE Database
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iterator>
#include <boost/filesystem.hpp>

#include "Vector/DBC/Database.h"

BOOST_AUTO_TEST_CASE(Database)
{
    Vector::DBC::Database database;

    /* load database */
    boost::filesystem::path infile(CMAKE_CURRENT_SOURCE_DIR "/data/Database.dbc");
    BOOST_REQUIRE(database.load(infile.c_str()));

    /* create output directory */
    boost::filesystem::path outdir(CMAKE_CURRENT_BINARY_DIR "/data/");
    if (!exists(outdir)) {
        BOOST_REQUIRE(create_directory(outdir));
    }

    /* save database */
    boost::filesystem::path outfile(CMAKE_CURRENT_BINARY_DIR "/data/Database.dbc");
    BOOST_REQUIRE(database.save(outfile.c_str()));

    /* loaded and saved file should be equivalent */
    std::ifstream ifs1(infile.c_str());
    std::ifstream ifs2(outfile.c_str());
    std::istream_iterator<char> b1(ifs1), e1;
    std::istream_iterator<char> b2(ifs2), e2;
    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
