# ArgumentParser
ArgumentParser is a helper C++ class that parses command line arguments.

ArgumentParser requires C++ 11 and has no external dependencies.

Project has been written using Netbeans 8.0.2 IDE

## Using ArgumentParser
To use ArgumentParser, just copy ArgumentParser.h and ArgumentParser.cpp
files to your project.

See additional information below and in source comments and tests.

## Features
* Easy to use
* Supports default values
* Reads and validates std::string, bool, (unsigned) int, (unsigned)
  long, float, and double types
* Error handling is configurable to use with and without exceptions
* Can parse command line arguments using either:
  *  Parameters-Switches syntax (e.g "executable -switch
     -param value ...").
  *  Verb-Parameters-Switches syntax (e.g "executable verb -switch
     -param value ..."). Verb must be first argument, but pv pairs
     and switches can go in any order afterwards.

## Quick use example
```c++
int main(int argc, char** argv)
{
    // Create object and parse
    auto ap = ArgumentParser();
    if(!ap.parse(argc, argv))
        return 1;

    // Get values
    bool print = ap.is_present("print");     // Switch
    int  reps  = ap.get_as_int("reps", 100); // Parameter-Value pair with default value

    for(int i = 0; i < reps; i++) {
        if(print)
            std::cout << "Iteration #" << i << std::endl;
    }

    return 0;
}
```

The example snippet above handles default values, correctly parsing command lines like:

> myprogram <executable-name>
> myprogram -reps 50
> myprogram -print
> myprogram -reps 50 -print


## Parsing values
Since it is not always possible to tell the intended type from an argument
value passed as a string, internally, parsing of argument values happens in
two steps:

   * First, the parse() method parses all parameter-value pairs as string values
   * Then, whenever one of the conversion methods (the ones starting with 'get_*')
is called, the internal string value is converted and validated to the
requested type.

### Command line argument syntax
ArgumentParser supports two formats, depending on ArgumentFormat value passed to the parse() method:

##### executable VERB [[-PARAM value] | [-SWITCH]]
      Used mostly by command-line tools than perform a variety of functions.

##### executable [[-PARAM value] | [-SWITCH]]
      Most common format used by all executables.

### Conversion methods
See ArgumentParser.h, methods starting with "get_as_".

### Note on bool parameters
Bool parameters accept the following values in the command line (case
insensitive):

Logical value | Argument string value
-------------|--------------------------
true | true, t, yes, y, on, 1
false | false, f, no, n, off, 0

## Error checking and handling
The following methods are available:

Method name | Description
-----------|---------------------
bool parse(int argc, char* argv[], ArgumentFormat format = PARAM_SWITCH) | Returns true is no errors occurred during parsing, false otherwise
bool is_present(std::string const & name) const | Checks if a switch or parameter is present.
bool error() | Checks if the last get_* method called had any errors
ArgumentParser(bool throw_on_parse_error = false, bool throw_on_conversion_error = false) | (constructor) Enable/disable exception throwing for parsing and conversion operations.

### Error checking without using exceptions
Checking after getting value:

```c++
int main(int argc, char** argv)
{
    auto ap = ArgumentParser();
    if(!ap.parse(argc, argv)) {
        std::cerr << ap.get_error_message() << std::endl;
        return 1;
    }

    std::string file_name  = ap.get_as_string("filename");
    if(ap.error()) {
        std::cerr << ap.get_error_message() << std::endl;
        return 2;
    }
    ...
}
```

Checking if required parameter is present in command line:

```c++
int main(int argc, char** argv)
{
    auto ap = ArgumentParser();
    if(!ap.parse(argc, argv)) {
        std::cerr << ap.get_error_message() << std::endl;
        return 1;
    }

    if(!ap.is_present("maxsize") {
        std::cerr << "Parameter 'maxsize' is required!" << std::endl;
    }
    ...
}
```

### Error checking using exceptions
The ArgumentParser constructor has parameters that set the throwing policy for
the instance. Throwing policy for parsing errors (parse() method) and
conversion errors (any of the get_* methods) can be individually controlled.
In all error cases (other than the unlikely out of memory), the exception type
thrown is std::invalid_argument.

```c++
int main(int argc, char** argv)
{
    std::string file_name;

    auto ap = ArgumentParser(true, true);

    try {
        ap.parse(argc, argv));
        file_name = ap.get_as_string("filename");

    } catch(std::invalid_argument ex) {
        std::cerr << ex.what() << std::endl;
    }
    ...
}
```


Copyright (c) 2017 Carlos J. Cela
