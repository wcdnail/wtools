#pragma once

namespace Ref
{
    template <class Archive>
    inline void List::Serialize(Archive& ar)
    {
        for (iterator it = begin(); it != end(); ++it) {
            ar & it->second;
        }
    }
}
