#include "stdafx.h"
#include "cp.command.h"
#include "execution.state.h"
#include <dh.timer.h>
#include <string.utils.error.code.h>
#include <locale.helper.h>

namespace Shelltastix
{
    namespace Command
    {
        Def::String ExtractCommand(Def::String const& commandLine)
        {
            Def::String::size_type n = commandLine.find(L' ');
            return Def::String::npos == n ? commandLine : commandLine.substr(0, n);
        }

        Def::String ExtractArguments(Def::String const& commandLine)
        {
            Def::String::size_type n = commandLine.find(L' ');
            return Def::String::npos == n ? L"" : commandLine.substr(n + 1, commandLine.length() - n);
        }

        void Def::DmyCallback(String const&) 
        {}

        Def::Def(String const& name/* = L""*/, Function const& callback/* = Def::DmyCallback*/)
            : command_(name)
            , callback_(callback)
            , hint_()
        {}

        Def::Def(String const& name/* = L""*/, String const& hint/* = L""*/, Function const& callback/* = Def::DmyCallback*/)
            : command_(name)
            , callback_(callback)
            , hint_(hint)
        {}

        Def::~Def() 
        {}

        Def::String const& Def::GetName() const { return command_; }
        Def::String const& Def::GetHint() const { return hint_; }
        Def::String const& Def::GetInternalKey() const { return command_; }

        Def::Function& Def::Callback() 
        {
            return callback_;
        }

        struct AutoExecuteState
        {
            AutoExecuteState() { Execution().Start(); }
            ~AutoExecuteState() { Execution().Stop(); }
        };

        void Def::Run(String const& args) const
        {
            AutoExecuteState autoState;
            callback_(args);
        }
    }
}
