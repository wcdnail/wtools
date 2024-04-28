#pragma once

#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace Config
{
    class Var;
    class Section;

    typedef boost::noncopyable Noncopyable;
    typedef std::wstring String;
    typedef std::map<String, Var> VarMap;
    typedef std::vector<Section*> SectionPointeeVector;

    namespace Storage 
    { 
        enum Type 
        {
          Native
        , File 
        };
    };
}
