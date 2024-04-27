#pragma once

#include <var.string/var.misc.hxx>
#include <string>
#include <deque>
#include <algorithm>
#include <iosfwd>
#include <boost/noncopyable.hpp>

struct HistoryStrings: boost::noncopyable
{
    typedef std::deque<std::wstring> Deque;
    typedef Deque::iterator iterator;
    typedef Deque::const_iterator const_iterator;
    typedef Deque::value_type value_type;

    Deque Storage;

    HistoryStrings();
    ~HistoryStrings();

    void Push(std::wstring const& item);
    std::wstring const& Pop();

    const_iterator begin() const { return Storage.begin(); }
    const_iterator end() const { return Storage.end(); }
};

inline HistoryStrings::HistoryStrings() 
    : Storage()
{}

inline HistoryStrings::~HistoryStrings() 
{}

inline void HistoryStrings::Push(std::wstring const& item)
{
    const_iterator it = std::find(Storage.begin(), Storage.end(), item);
    if (it != Storage.end())
        Storage.erase(it);

    Storage.push_front(item);
}

inline std::wstring const& HistoryStrings::Pop()
{
    Storage.pop_front();

    if (!Storage.empty())
        return Storage.front();

    static std::wstring dummy;
    return dummy;
}

template <class C, class T>
inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& s, HistoryStrings const& hs)
{
    Vs::Impl::ContainerToStream(s, hs.Storage);
    return s;
}

template <class C, class T>
inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& s, HistoryStrings& hs)
{
    Vs::Impl::ContainerFromStream(s, hs.Storage);
    return s;
}
