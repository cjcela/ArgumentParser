/*
 * Author:  Carlos J. Cela
 *          https://github.com/cjcela/ArgumentParser
 *
 * License: MIT - See LICENSE file
 */

#include <iostream>
#include <cstring>

#include "../ArgumentParser.h"
#include "ArgumentParserTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION(ArgumentParserTest);

ArgumentParserTest::ArgumentParserTest()
{
}

ArgumentParserTest::~ArgumentParserTest()
{
}

void ArgumentParserTest::setUp()
{
}

void ArgumentParserTest::tearDown()
{
}

char ** ArgumentParserTest::split_arguments(char const * cmd, int & argc)
{
    // Assumes trimmed ASCIIZ string, tokens separated by single spaces,
    // at least one token present, no embedded double quotes.
    argc = 0;

    // Count number of tokens in command
    auto it = cmd;
    while(*it != 0) {
        if(*it == ' ') {
            ++argc;
        }
        ++it;
    }
    ++argc;

    // Create argument array and cmd writable copy
    auto argv = new char *[argc];

    char *buffer = new char[strlen(cmd)+1];
    strcpy(buffer, cmd);

    // Populate argument array
    int i = 0;
    char * pch;
    pch = strtok(buffer, " ");
    while (pch != nullptr)
    {
        argv[i] = new char[strlen(pch) + 1];
        strcpy(argv[i], pch);
        pch = strtok (nullptr, " ");
        ++i;
    }
    delete [] buffer;
    return argv;
}

ArgumentParser * ArgumentParserTest::create_and_parse(char const * cmd, ArgumentFormat format, bool throw_conversion_errors)
{
    int argc;
    char ** argv = split_arguments(cmd, argc);

    ArgumentParser *ap = new ArgumentParser(true, throw_conversion_errors);
    ap->parse(argc, argv, format);
    delete [] *argv;
    delete argv;

    return ap;
}

bool ArgumentParserTest::create_and_parse_and_check(char const * cmd, ArgumentFormat format, bool throw_parse_errors)
{
    int argc;
    char ** argv = split_arguments(cmd, argc);

    ArgumentParser ap(throw_parse_errors);
    bool result = ap.parse(argc, argv, format);
    delete [] *argv;
    delete argv;

    return result;
}

void ArgumentParserTest::test_parse_vps()
{
    // Correct cases
    CPPUNIT_ASSERT_MESSAGE("Case 1", create_and_parse_and_check("tool", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 2", create_and_parse_and_check("tool verb", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 3", create_and_parse_and_check("tool verb -switch", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 4", create_and_parse_and_check("tool verb -option value", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 5", create_and_parse_and_check("tool verb -switch --option value", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 6", create_and_parse_and_check("tool verb -option value --switch", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 7", create_and_parse_and_check("tool verb --switch", ArgumentFormat::VERB_PARAM_SWITCH, false));

    // Incorrect cases / No-throw
    CPPUNIT_ASSERT_MESSAGE("Case 8", !create_and_parse_and_check("tool -switch", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 9", !create_and_parse_and_check("tool -switch value1 value2", ArgumentFormat::VERB_PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 10", !create_and_parse_and_check("tool verb value", ArgumentFormat::VERB_PARAM_SWITCH, false));

    // Incorrect cases / Throw
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 11", create_and_parse_and_check("tool -switch", ArgumentFormat::VERB_PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 12", create_and_parse_and_check("tool -switch value1 value2", ArgumentFormat::VERB_PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 13", create_and_parse_and_check("tool verb value", ArgumentFormat::VERB_PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 14", create_and_parse_and_check("tool verb -", ArgumentFormat::VERB_PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 15", create_and_parse_and_check("tool verb -switch --", ArgumentFormat::VERB_PARAM_SWITCH, true), std::invalid_argument);
}

void ArgumentParserTest::test_parse_ps()
{
    // Correct cases
    CPPUNIT_ASSERT_MESSAGE("Case 1", create_and_parse_and_check("tool", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 3", create_and_parse_and_check("tool -switch", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 4", create_and_parse_and_check("tool -option value", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 5", create_and_parse_and_check("tool -switch -option value", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 6", create_and_parse_and_check("tool -option value --switch", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 7", create_and_parse_and_check("tool --switch", ArgumentFormat::PARAM_SWITCH, false));

    // Incorrect cases / No-throw
    CPPUNIT_ASSERT_MESSAGE("Case 8", !create_and_parse_and_check("tool verb -switch", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 9", !create_and_parse_and_check("tool -switch value1 value2", ArgumentFormat::PARAM_SWITCH, false));
    CPPUNIT_ASSERT_MESSAGE("Case 10", !create_and_parse_and_check("tool -option value1 value2", ArgumentFormat::PARAM_SWITCH, false));

    // Incorrect cases / Throw
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 11", create_and_parse_and_check("tool verb -switch", ArgumentFormat::PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 12", create_and_parse_and_check("tool -switch value1 value2", ArgumentFormat::PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 13", create_and_parse_and_check("tool option value1 value2", ArgumentFormat::PARAM_SWITCH, true), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 14", create_and_parse_and_check("tool -option value1 -option value2", ArgumentFormat::PARAM_SWITCH, true), std::invalid_argument);
}

void ArgumentParserTest::test_get_verb()
{
    // Correct
    auto ap = create_and_parse("tool verb", ArgumentFormat::VERB_PARAM_SWITCH, false);
    CPPUNIT_ASSERT_MESSAGE("Case 1:1", ap->is_present("verb"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:2", ap->get_verb().compare("verb")==0);
    CPPUNIT_ASSERT_MESSAGE("Case 1:3", ap->get_verb("default").compare("verb")==0);
    CPPUNIT_ASSERT_MESSAGE("Case 1:4", !ap->error());
    delete ap;

    // Missing verb
    ap = create_and_parse("tool", ArgumentFormat::VERB_PARAM_SWITCH, false);
    CPPUNIT_ASSERT_MESSAGE("Case 2:1", !ap->is_present("verb"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:2", ap->get_verb().empty());
    CPPUNIT_ASSERT_MESSAGE("Case 2:3", ap->error());
    CPPUNIT_ASSERT_MESSAGE("Case 2:4", ap->get_verb("default").compare("default")==0);
    CPPUNIT_ASSERT_MESSAGE("Case 2:5", !ap->error());
    delete ap;

    // Incorrect / throwing
    ap = create_and_parse("tool", ArgumentFormat::VERB_PARAM_SWITCH, true);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:1", ap->get_verb(), std::invalid_argument);
    CPPUNIT_ASSERT_MESSAGE("Case 3:2",ap->error());
    delete ap;
}

void ArgumentParserTest::test_get_string()
{
    // Correct
    auto ap = create_and_parse("tool -s1 test -s2", ArgumentFormat::PARAM_SWITCH, false);
    CPPUNIT_ASSERT_MESSAGE("Case 1:1", ap->get_as_string("s1").compare("test") == 0);
    CPPUNIT_ASSERT_MESSAGE("Case 1:3", !ap->error());
    CPPUNIT_ASSERT_MESSAGE("Case 1:4", ap->get_as_string("s2").empty());
    CPPUNIT_ASSERT_MESSAGE("Case 1:5", !ap->error());
    CPPUNIT_ASSERT_MESSAGE("Case 1:6", ap->get_as_string("sx","test").compare("test") == 0);
    CPPUNIT_ASSERT_MESSAGE("Case 1:7", !ap->error());

    // Incorrect
    ap->get_as_string("sx");
    CPPUNIT_ASSERT_MESSAGE("Case 2:1", ap->error());
    delete ap;

    // Incorrect + throw
    ap = create_and_parse("tool -s1 test -s2 test -s3", ArgumentFormat::PARAM_SWITCH, true);
    CPPUNIT_ASSERT_MESSAGE("Case 3:1", !ap->error());
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:2", ap->get_as_string("sx"), std::invalid_argument);
    CPPUNIT_ASSERT_MESSAGE("Case 3:3", ap->error());
    delete ap;
}

void ArgumentParserTest::test_get_bool()
{
    // Correct
    auto ap = create_and_parse("tool -b1 1 -b2 yes -b3 Yes -b4 ON -b5 T -b6 true -b7 y", ArgumentFormat::PARAM_SWITCH, true);
    CPPUNIT_ASSERT_MESSAGE("Case 1:1", ap->get_as_bool("b1"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:2", ap->get_as_bool("b2"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:3", ap->get_as_bool("b3"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:4", ap->get_as_bool("b4"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:5", ap->get_as_bool("b5"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:6", ap->get_as_bool("b6"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:7", ap->get_as_bool("b7"));
    CPPUNIT_ASSERT_MESSAGE("Case 1:8", ap->get_as_bool("bx", true));
    CPPUNIT_ASSERT_MESSAGE("Case 1:9", !ap->error());
    delete ap;

    ap = create_and_parse("tool -b1 0 -b2 no -b3 NO -b4 off -b5 F -b6 false -b7 n", ArgumentFormat::PARAM_SWITCH, true);
    CPPUNIT_ASSERT_MESSAGE("Case 2:1", !ap->get_as_bool("b1"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:2", !ap->get_as_bool("b2"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:3", !ap->get_as_bool("b3"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:4", !ap->get_as_bool("b4"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:5", !ap->get_as_bool("b5"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:6", !ap->get_as_bool("b6"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:7", !ap->get_as_bool("b7"));
    CPPUNIT_ASSERT_MESSAGE("Case 2:8", !ap->get_as_bool("bx", false));
    delete ap;

    // Incorrect + throw
    ap = create_and_parse("tool -b1 10 -b2 maybe -b3 x -b4", ArgumentFormat::PARAM_SWITCH, true);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:1", ap->get_as_bool("b1"), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:2", ap->get_as_bool("b2"), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:3", ap->get_as_bool("b3"), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:4", ap->get_as_bool("b4"), std::invalid_argument);
    CPPUNIT_ASSERT_THROW_MESSAGE("Case 3:5", ap->get_as_bool("bx"), std::invalid_argument);
    delete ap;
}

void ArgumentParserTest::test_get_int()
{

}

void ArgumentParserTest::test_get_unsigned_int()
{

}

void ArgumentParserTest::test_get_long()
{

}

void ArgumentParserTest::test_get_unsigned_long()
{

}

void ArgumentParserTest::test_get_float()
{

}

void ArgumentParserTest::test_get_double()
{

}

