#pragma once

#include <unordered_map>
#include <string>
#include <set>

struct CFontDef;

using     FontStr = std::wstring;
using     FontMap = std::unordered_map<FontStr, CFontDef>;
using  FontStrSet = std::set<FontStr>;

struct CFontDef
{
    FontStr   m_sFullName = {};
    FontStrSet m_sCharset = {};

    static HRESULT LoadAll(FontMap& target);
};
