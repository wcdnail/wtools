#pragma once

#include <wcdafx.api.h>
#include <3rd-party/inipp.h>
#include <fstream>

struct CScheme;

using     IniParser = inipp::Ini<char>;
using     IniString = IniParser::String;
using        IniMap = IniParser::Section;
using    InutStream = std::basic_ifstream<char>;
using SectionLoader = std::function<bool(CScheme& scheme, IniMap& iniFields)>;

using     IniParserW = inipp::Ini<wchar_t>;
using     IniStringW = IniParserW::String;
using        IniMapW = IniParserW::Section;
using    InutFStream = std::fstream;
