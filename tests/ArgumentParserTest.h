/*
 * Author:  Carlos J. Cela
 *          https://github.com/cjcela/argument-parser
 *
 * License: MIT - See LICENSE file
 */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ArgumentParserTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(ArgumentParserTest);

    CPPUNIT_TEST(test_parse_vps);
    CPPUNIT_TEST(test_parse_ps);
    CPPUNIT_TEST(test_get_verb);
    CPPUNIT_TEST(test_get_string);
    CPPUNIT_TEST(test_get_bool);
    CPPUNIT_TEST(test_get_int);
    CPPUNIT_TEST(test_get_unsigned_int);
    CPPUNIT_TEST(test_get_long);
    CPPUNIT_TEST(test_get_unsigned_long);
    CPPUNIT_TEST(test_get_float);
    CPPUNIT_TEST(test_get_double);

    CPPUNIT_TEST_SUITE_END();

public:
    ArgumentParserTest();
    virtual ~ArgumentParserTest();
    void setUp();
    void tearDown();

private:
    // Unit tests
    void test_parse_vps();
    void test_parse_ps();
    void test_get_verb();
    void test_get_string();
    void test_get_bool();
    void test_get_int();
    void test_get_unsigned_int();
    void test_get_long();
    void test_get_unsigned_long();
    void test_get_float();
    void test_get_double();

    // Helper methods
    char ** split_arguments(char const * cmd, int & argc);
    ArgumentParser * create_and_parse(char const * cmd, ArgumentFormat format, bool throw_conversion_errors);
    bool create_and_parse_and_check(char const * cmd, ArgumentFormat format, bool throw_parse_errors);
};


