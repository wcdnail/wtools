#pragma once

#include "cp.command.h"
#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>

namespace Shelltastix
{
    namespace Console
    {
        class View;
    }

    namespace Commands
    {
        class Internals: boost::noncopyable
        {
        public:
            typedef Command::Def Def;
            typedef Def::String String;
            typedef boost::unordered_map<String, Def> CommandMap;

            static Internals& Instance();

            void Add(Def const& command);
            bool Run(Def const& command, String const& commandLine);
            void ShowHelp(Console::View& console) const;

        private:
            CommandMap map_;
            String::size_type maxLen_;

        private:
            Internals();
            ~Internals();
        };
    }

    namespace Command
    {
        inline Commands::Internals& Internals()
        {
            return Commands::Internals::Instance();
        }
    }
}
