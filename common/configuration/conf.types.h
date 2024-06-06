#pragma once

#include <string>
#include <map>
#include <vector>

namespace Config
{
    class Var;
    class Section;

    typedef std::wstring String;
    typedef std::map<String, Var> VarMap;
    typedef std::vector<Section*> SectionPointeeVector;

    namespace Storage 
    { 
        enum Type 
        {
            Native,
            File 
        };
    };
}
