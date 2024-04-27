#pragma once

#include "command.ids.h"
#include <string>
#include <functional>

namespace Twins
{
    namespace Command
    {
        struct Definition
        {
            using Handler = std::function<void(void)>;

            int Id;
            std::wstring Name;
            unsigned Key;
            Handler Callback;

            Definition(int id = Ids::Dummy, unsigned key = 0xffffffff, std::wstring const& name = L"", Handler const& callback = Definition::DummyCallback);
            ~Definition();

            std::wstring GetKeyIdName() const;

            static void DummyCallback();
        };
    }
}
