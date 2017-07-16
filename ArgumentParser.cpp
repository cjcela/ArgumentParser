/*
 * Author:  Carlos J. Cela
 *          https://github.com/cjcela/ArgumentParser
 *
 * License: MIT - See LICENSE file
 */

#include <limits>
#include <cerrno>
#include <string>
#include <sstream>

#include "ArgumentParser.h"

ArgumentParser::ArgumentParser(bool throw_on_parse_error, bool throw_on_conversion_error)
: _verb(""),
  _error_message(""),
  _conversion_error(false),
  _throw_on_parse_error(throw_on_parse_error),
  _throw_on_conversion_error(throw_on_conversion_error)
{ }

ArgumentParser::~ArgumentParser()
{ }

bool ArgumentParser::error()
{
    return _conversion_error;
}

std::string ArgumentParser::get_error_message()
{
    return _error_message;
}

bool ArgumentParser::parse(int argc, char* argv[], ArgumentFormat format)
{
    _verb = "";
    _argument.clear();
    _error_message = "";
    _conversion_error = false;

    int current = 1; // Skip argv[0], which is the program name

    // Collect verb if necessary
    if(format == ArgumentFormat::VERB_PARAM_SWITCH ) {
        if(argc > 1) {
            if(!is_switch(argv[current])) {
                _verb = argv[current];
                ++current;
            } else {
                if(_throw_on_parse_error) {
                    std::stringstream msg;
                    msg << "Argument '" << argv[current] << "' is not valid; was expecting a verb, but it looks like a switch.";
                    throw std::invalid_argument(msg.str());
                }
                return false;
            }
        }
    }

    // Process switches and PV pairs
    while(current < argc) {

        if(is_switch(argv[current])) {
            auto raw_name = argv[current];
            auto name = get_stripped_switch_name(argv[current]);

            // Switch must have at least one char
            if(name.empty()) {
                std::stringstream msg;
                msg << "Argument '" << raw_name << "' is not a valid switch.";
                handle_parse_error(msg.str());
                return false;
            }

            std::string value = "";
            ++current;

            if(current < argc && !is_switch(argv[current])) {
                value = argv[current];
                ++current;
            }

            // No repeated switches allowed
            if(_argument.find(name) != _argument.end()) {
                std::stringstream msg;
                msg << "Argument '" << raw_name << "' is present multiple times.";
                handle_parse_error(msg.str());
                return false;
            }
            _argument[name] = value;

        } else {
            // No consecutive values allowed
            std::stringstream msg;
            msg << "Argument '" << argv[current] << "' is not valid; was expecting a switch, but it looks like a value.";
            handle_parse_error(msg.str());
            return false;
        }
    }
    return true;
}

bool ArgumentParser::is_switch(std::string const & token) const
{
    return (token.length() > 1 && token[0] == '-');
}

std::string ArgumentParser::get_stripped_switch_name(std::string const & token) const
{
    if(token.length() > 1 && token.substr(0,2).compare("--") == 0) {
        return token.substr(2);
    }

    if(token.length() > 0 && token[0] == '-') {
        return token.substr(1);
    }
    return token;
}

void ArgumentParser::handle_parse_error(std::string const & msg)
{
    _verb = "";
    _argument.clear();
    if(_throw_on_parse_error) {
        throw std::invalid_argument(msg);
    }
}

void ArgumentParser::handle_conversion_error(std::string const & msg)
{
    _error_message = msg;
    _conversion_error = true;

    if(_throw_on_conversion_error) {
        throw std::invalid_argument(msg);
    }
}

bool ArgumentParser::is_present(std::string const & name) const
{
    auto it = _argument.find(name);
    return (it != _argument.end()) || (name.compare(_verb) == 0);
}

std::string ArgumentParser::get_verb(std::string const & default_value)
{
    _error_message = "";
    _conversion_error = false;

    if(_verb.empty()) {
        if(default_value.empty()) {
            handle_conversion_error("Verb/Action is missing, and a default value has not been specified.");
        }
        return default_value;
    }
    return _verb;
}

std::string ArgumentParser::get_as_string(std::string const & name)
{
    _error_message = "";
    _conversion_error = false;

    auto it = _argument.find(name);
    if(it == _argument.end()) {
        std::stringstream msg;
        msg << "Argument '" << name << "' is required but is not present.";
        handle_conversion_error(msg.str());
        return "";
    }
    return it->second;
}
std::string ArgumentParser::get_as_string(std::string const & name, std::string const & default_value)
{
    _error_message = "";
    _conversion_error = false;
    auto it = _argument.find(name);
    if(it == _argument.end()) {
        return default_value;
    }
    return it->second;
}

bool ArgumentParser::case_independent_compare(std::string const & s1, std::string const & s2)
{
    if(s1.length() != s2.length()) {
        return false;
    }

    auto it = s2.begin();
    for( auto ch : s1) {
        if(*it != std::tolower(ch)) {
            return false;
        };
        ++it;
    }
    return true;
}

bool ArgumentParser::parse_bool_value(std::string const & name, std::string const & value)
{
    bool is_true =  (value.compare("1") == 0 ||
                    case_independent_compare(value, "t") ||
                    case_independent_compare(value, "y") ||
                    case_independent_compare(value, "true") ||
                    case_independent_compare(value, "yes") ||
                    case_independent_compare(value, "on"));

    bool is_false = (value.compare("0") == 0 ||
                    case_independent_compare(value, "f") ||
                    case_independent_compare(value, "n") ||
                    case_independent_compare(value, "false") ||
                    case_independent_compare(value, "no") ||
                    case_independent_compare(value, "off"));

    if(!is_true && !is_false) {
        std::stringstream msg;
        msg << "Argument '" << name << "' is boolean, and value '" << value << "' is not recognized as a valid boolean value. Try one of: 'true', 'false', 'yes', 'no', '0', '1', 'on', 'off', 't', 'f', 'y', 'n' instead.";
        handle_conversion_error(msg.str());
    }
    return is_true;;
}

bool ArgumentParser::get_as_bool(std::string const & name)
{
    auto value = get_as_string(name);
    if(value.empty()) {
        std::stringstream msg;
        msg << "Argument '" << name << "' is required, but not given.";
        handle_conversion_error(msg.str());
    }

    return parse_bool_value(name, value);
}

bool ArgumentParser::get_as_bool(std::string const & name, bool default_value)
{
    auto value = get_as_string(name, (default_value==true?"true":"false"));
    if(value.empty()) {
        return default_value;
    }
    return parse_bool_value(name, value);
}

long ArgumentParser::get_as_long(std::string const & name, long default_value, int base)
{
    auto value = get_as_string(name);
    if(value.empty()) {
        return default_value;
    }

    char *end;
    auto start = value.c_str();
    auto integral_value = std::strtol(start, &end, base);
    if(errno == 0 && end != start && *end == '\0') {
        return integral_value;
    }

    std::stringstream msg;
    msg << "Argument '" << name << "' value ('" << value << "') is not valid.";
    handle_conversion_error(msg.str());

    return default_value;
}

unsigned long ArgumentParser::get_as_unsigned_long(std::string const & name, unsigned long default_value, int base)
{
    auto value = get_as_string(name);
    if(value.empty()) {
        return default_value;
    }

    char *end;
    auto start = value.c_str();
    auto integral_value = std::strtoul(start, &end, base);
    if(errno == 0 && end != start && *end == '\0') {
        return integral_value;
    }

    std::stringstream msg;
    msg << "Argument '" << name << "' value ('" << value << "') is not valid.";
    handle_conversion_error(msg.str());

    return default_value;
}

int ArgumentParser::get_as_int(std::string const & name, int default_value, int base)
{
    auto value = get_as_long(name, default_value, base);

    // Check for over/under flow
    if(value > std::numeric_limits<int>::max() || value < std::numeric_limits<int>::min()) {
        std::stringstream msg;
        msg << "Argument '" << name << "' value ('" << value << "') is out of range.";
        handle_conversion_error(msg.str());

        return default_value;
    }
    return static_cast<int>(value);
}

unsigned int ArgumentParser::get_as_unsigned_int(std::string const & name, unsigned int default_value, int base)
{
    auto value = get_as_unsigned_long(name, default_value, base);

    // Check for over/under flow
    if(value > std::numeric_limits<unsigned int>::max() || value < std::numeric_limits<unsigned int>::min()) {
        std::stringstream msg;
        msg << "Argument '" << name << "' value ('" << value << "') is too out of range.";
        handle_conversion_error(msg.str());

        return default_value;
    }
    return static_cast<int>(value);
}

float ArgumentParser::get_as_float(std::string const & name, float default_value)
{
    auto value = get_as_string(name);
    if(value.empty()) {
        return default_value;
    }

    auto float_value = std::strtof(value.c_str(), nullptr);
    if(errno == ERANGE) {
        std::stringstream msg;
        msg << "Argument '" << name << "' value ('" << value << "') is invalid.";
        handle_conversion_error(msg.str());

        return default_value;
    }
    return float_value;
}

double ArgumentParser::get_as_double(std::string const & name, double default_value)
{
    auto value = get_as_string(name);
    if(value.empty()) {
        return default_value;
    }

    auto double_value = std::strtod(value.c_str(), nullptr);
    if(errno == ERANGE) {
        std::stringstream msg;
        msg << "Argument '" << name << "' value ('" << value << "') is invalid.";
        handle_conversion_error(msg.str());
        return default_value;
    }
    return double_value;
}
