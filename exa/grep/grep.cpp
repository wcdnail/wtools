#include "stdafx.h"
#include <dh.timer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/regex.hpp>
#include <boost/exception/diagnostic_information.hpp>

struct Grep
{
    typedef std::string String;
    typedef std::istream InputStream;
    typedef std::ifstream InputFileStream;
    typedef boost::regex_constants::syntax_option_type SyntaxOptions;

    enum Flags
    {
        UseExtendedRegEx = boost::regex_constants::extended,
        UseBasicRegEx = boost::regex_constants::basic,
        UsePerlRegEx  = boost::regex_constants::perl,
        IgnoreCase = boost::regex_constants::icase,
    };

    Grep(String const& pat, unsigned opts);

    void ProcessStream(InputStream& is, String const& filename) const;
    void ProcessFile(String const& name) const;

private:
    String Pattern;
    SyntaxOptions Options;    
    boost::regex RegExp;
    mutable boost::smatch What;
};

Grep::Grep(String const& pat, unsigned opts)
    : Pattern(pat)
    , Options((SyntaxOptions)opts)
    , RegExp()
    , What()
{
    RegExp.assign(Pattern, Options);
}

void Grep::ProcessStream(InputStream& is, String const& filename) const
{
    String line;
    while(std::getline(is, line))
    {
        bool result = boost::regex_search(line, What, RegExp);
        if (result)
        {
            std::cout << line << std::endl;
        }
    }
}

void Grep::ProcessFile(String const& name) const
{
    InputFileStream is(name.c_str());

    if (is.bad())
        std::cerr << "Unable to open file " << name << std::endl;

    ProcessStream(is, name);
}

int main(int argc, char const* argv[])
{
    Grep::String rx;

    if (argc > 1)
        rx = argv[1];

    Dh::Timer timer;

    try
    {
        Grep grep(rx, Grep::IgnoreCase | Grep::UseBasicRegEx);
        grep.ProcessStream(std::cin, "");
    }
    catch (std::exception const& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    std::cerr << "\n" << timer.Seconds() << " sec." << std::endl;
    return 0;
}
