#include "stdafx.h"
#include "cp.internal.commands.h"
#include "console.view.h"
#include <locale.helper.h>
#include <atlstr.h>

namespace Shelltastix
{
    namespace Commands
    {
        Internals& Internals::Instance()
        {
            static Internals instance;
            return instance;
        }

        Internals::Internals()
            : map_()
            , maxLen_(0)
        {}

        Internals::~Internals() 
        {}

        void Internals::Add(Def const& command)
        {
            String const& key = command.GetInternalKey();

            CommandMap::const_iterator it = map_.find(key);
            if (it != map_.end())
            {
                // TODO: Warning - command already present;
            }

            map_[key] = command;

            // Для вывода справки:
            if (maxLen_ < command.GetName().length())
                maxLen_ = command.GetName().length();
        }

        bool Internals::Run(Def const& command, String const& commandLine)
        {
            CommandMap::const_iterator it = map_.find(command.GetInternalKey());
            if (it != map_.end())
            {
                it->second.Run(commandLine);
                return true;
            }

            return false;
        }

        void Internals::ShowHelp(Console::View& Console) const
        {
            CStringW helpLineFormat;
            helpLineFormat.Format(L"%%%ds - %%s", -((int)maxLen_));

            for (CommandMap::const_iterator it = map_.begin(); it != map_.end(); ++it)
            {
                Command::Def const& def = it->second;
                Console.WriteLine((PCWSTR)helpLineFormat, def.GetName().c_str(), Locale.String[def.GetHint()].c_str());
            }
        }
    }
}
