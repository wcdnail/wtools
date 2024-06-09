#pragma once

#include <unordered_map>
#include <cinttypes>
#include <string>
#include <set>

struct CFontDef;

using      FontStr = std::wstring;
using      FontMap = std::unordered_map<FontStr, CFontDef>;
using  CharsetPair = std::pair<int, FontStr>;
using  FontCharset = std::set<CharsetPair>;

struct CFontDef
{
    FontStr    m_sFullName = {};
    FontCharset m_sCharset = {};
    LONG        m_tmHeight = {0};

    static HRESULT LoadAll(FontMap& target);
};
