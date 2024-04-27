#include "stdafx.h"
#include "command.h"
#include "command.key.ids.h"
#include "keyboard.h"

namespace Twins
{
    namespace Command
    {
        void Definition::DummyCallback() {}

        Definition::Definition(int id, unsigned key, std::wstring const& name, Handler const& callback)
            : Id(id)
            , Name(name)
            , Key(key)
            , Callback(callback)
        {}

        Definition::~Definition()
        {}

        std::wstring Definition::GetKeyIdName() const
        {
            Keyboard::KeyState temp(Key & KeyId::KeyMaks, 0, KeyId::IsShift(Key), KeyId::IsAlt(Key), KeyId::IsCtrl(Key));
            return temp.KeyIdName();
        }
    }
}
