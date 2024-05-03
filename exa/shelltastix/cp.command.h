#pragma once

#include <string>
#include <boost/function.hpp>

namespace Shelltastix
{
    namespace Command
    {
        class Def
        {
        public:
            typedef std::wstring String;
            typedef boost::function<void(String const&)> Function;

            Def(String const& name = L"", Function const& callback = Def::DmyCallback);
            Def(String const& name, String const& hint, Function const& callback = Def::DmyCallback);
            ~Def();

            String const& GetName() const;
            String const& GetHint() const;
            String const& GetInternalKey() const;

            Function& Callback();
            void Run(String const& args) const;
            
            static void DmyCallback(String const&);

        private:
            String command_;
            Function callback_;
            String hint_;
        };

        Def::String ExtractCommand(Def::String const& commandLine);
        Def::String ExtractArguments(Def::String const& commandLine);
    }
}
