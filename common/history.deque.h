#pragma once

#include <reflection/ref.std.h>
#include <string.hp.h>
#include <boost/noncopyable.hpp>
#include <deque>
#include <algorithm>
#ifdef _WIN32
#include <atlstr.h>
#include <atlctrls.h>
#endif 

struct HistoryDeque: boost::noncopyable
{
    typedef WidecharString String;
    typedef std::deque<String> Deque;
    typedef Ref::Container<Deque> Reflection;
    typedef Deque::iterator iterator;
    typedef Deque::const_iterator const_iterator;
    typedef Deque::value_type value_type;

    Reflection Storage;

    HistoryDeque();
    ~HistoryDeque();

    void Push(String const& item);
    String const& Pop();

    const_iterator begin() const { return Storage.begin(); }
    const_iterator end() const { return Storage.end(); }

    template <class Archive>
    void Serialize(Archive& archive);

#ifdef _WIN32
    void LoadFrom(WTL::CComboBox& combo);
    int LoadTo(WTL::CComboBox& combo) const;
#endif 
};

inline HistoryDeque::HistoryDeque() 
    : Storage()
{}

inline HistoryDeque::~HistoryDeque() 
{}

inline void HistoryDeque::Push(String const& item)
{
    const_iterator it = std::find(Storage.begin(), Storage.end(), item);
    if (it != Storage.end())
        Storage.erase(it);

    Storage.push_front(item);
}

inline HistoryDeque::String const& HistoryDeque::Pop()
{
    Storage.pop_front();

    if (!Storage.empty())
        return Storage.front();

    static String dmyString;
    return dmyString;
}

template <class Archive>
inline void HistoryDeque::Serialize(Archive& archive)
{
    archive & Storage;
}

#ifdef _WIN32
inline void HistoryDeque::LoadFrom(WTL::CComboBox& combo)
{
    HistoryDeque temp;

    int count = combo.GetCount();
    for (int i=0; i<count; i++)
    {
        CString text;
        if (combo.GetLBText(i, text) > 0)
            temp.Push((PCWSTR)text);
    }

    Storage.swap(temp.Storage);
}

inline int HistoryDeque::LoadTo(WTL::CComboBox& combo) const
{
    int last = LB_ERR;

    CString text;
    combo.GetWindowText(text);

    combo.ResetContent();
    for (const_iterator it = begin(); it != end(); ++it)
    {
        last = combo.AddString(it->c_str());
    }

    combo.SetWindowText(text);
    return last;
}
#endif
