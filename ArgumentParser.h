/*
 * Author:  Carlos J. Cela
 *          https://github.com/cjcela/argument-parser
 *
 * License: MIT - See LICENSE file
 */

#pragma once
#include <string>
#include <unordered_map>

/*
 * Describes the expected format of arguments to be parsed.
 */
enum ArgumentFormat
{
	/*
	 *   executable VERB [[-PARAM value] | [-SWITCH]]
	 */
	VERB_PARAM_SWITCH,

	/*
	 *  executable [[-PARAM value] | [-SWITCH]]
	 */
	PARAM_SWITCH
};

/**
 *
 *
 */
class ArgumentParser
{
public:

	/**
	 * Constructor. Accepts optional parameters to define throw policy and
	 * behavior of this ArgumentParser instance.
	 *
     * @param throw_on_parse_error
	 *
	 *                         (optional, default = false)
	 *
	 *                         Defines policy regarding throwing (or not) when
	 *                         parsing command line arguments and finding an
	 *                         error. A parsing failure happens when the actual
	 *                         arguments do not conform to the requested
	 *                         ArgumentFormat in the parse() call.

	 *                         Value    Behavior on failure
	 *                         =====    ===================
	 *                         false    parse() will return false on parsing
	 *                                  failure.
	 *
	 *                         true    parse() will throw an exception of
	 *                                 type std::invalid_argument.
	 *
     * @param throw_on_conversion_error
	 *
	 *                         (optional, default = false)
	 *
     *                         Defines policy regarding throwing (or not) when
	 *                         a conversion method (methods starting with get_*)
	 *                         is called and there is an error. Examples of
	 *                         possible errors are parameters having values of
	 *                         the wrong type, or values that cause underflow or
	 *                         overflow for the requested numerical types.
     *
	 *                         Value    Behavior on failure
	 *                         =====    ===================
	 *                         false    get_*() will return the given default
	 *                                  value. If no default value was given,
	 *                                  it will return 0 for a numerical types
	 *                                  or an empty string for string types.
	 *
	 *                         true     get_*() will throw an exception of
	 *                                 type std::invalid_argument.
     */
	ArgumentParser(bool throw_on_parse_error = false, bool throw_on_conversion_error = false);

	virtual ~ArgumentParser();


    /**
     * Parse command line arguments according to requested format.
     *
     * @param argc
     * @param argv
     * @param format Command-line arguments format.
     * @return true on success, false on failure. Depending on configuration
     *         given on constructor, can throw on failure.
     */
	bool parse(int argc, char* argv[], ArgumentFormat format = PARAM_SWITCH);

    /**
     *
     * @param name
     * @return
     */
	bool is_present(std::string const & name) const;

    /**
     *
     * @param default_value
     * @return
     */
	std::string get_verb(std::string const & default_value = "");

    /**
     *
     * @param name
     * @param default_value
     * @return
     */
    std::string   get_as_string        (std::string const & name);
	std::string   get_as_string        (std::string const & name, std::string const & default_value);

    bool          get_as_bool          (std::string const & name);
	bool          get_as_bool          (std::string const & name, bool default_value);

	int           get_as_int           (std::string const & name, int default_value = 0, int base = 10);
	unsigned int  get_as_unsigned_int  (std::string const & name, unsigned int default_value = 0, int base = 10);
    long          get_as_long          (std::string const & name, long default_value = 0, int base = 10);
	unsigned long get_as_unsigned_long (std::string const & name, unsigned long default_value = 0, int base = 10);
	float         get_as_float         (std::string const & name, float default_value = 0.0);
	double        get_as_double        (std::string const & name, double default_value = 0.0);

    /**
     * Checks if the last conversion operation (any method starting with
     * get_*) had an error.
     *
     * @return true if last conversion operation  encountered an error.
     */
    bool error();
    std::string get_error_message();

private:
    // Helper methods
    bool is_switch(std::string const & token) const;
    void handle_parse_error(std::string const & msg);
    void handle_conversion_error(std::string const & msg);
    std::string get_stripped_switch_name(std::string const & token) const;
    bool parse_bool_value(std::string const & name, std::string const & value);
    bool case_independent_compare(std::string const & s1, std::string const & s2);

private:
	std::string _verb;
    std::string _error_message;
    bool mutable _conversion_error;
	bool _throw_on_parse_error;
	bool _throw_on_conversion_error;
	std::unordered_map<std::string, std::string> _argument;
};

