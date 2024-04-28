#pragma once

#include "command.h"
#include <boost/noncopyable.hpp>
#include <map>
#include <string>

namespace Twins
{
    namespace Command
    {
        class Manager: boost::noncopyable
        {
        public:
            typedef Definition::Handler CommandHandler;
            typedef std::map<int, Definition> CommandMap;

            static Manager& Instance();

            void Initialize();

            void AddCommand(int id, unsigned keyId, std::wstring const& name, CommandHandler const& callback = Definition::DummyCallback);
            Definition const& GetCommand(int id) const;

            void OnKeyDown(unsigned code, unsigned flags) const;

        private:
            CommandMap map_;

        private:
            Manager();
            ~Manager();
        };
    }

    inline Command::Manager& Commands() { return Command::Manager::Instance(); }
}
