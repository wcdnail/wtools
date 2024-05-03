#include "stdafx.h"
#include "atl.std.string.adapter.h"
#include <string>
#include <sstream>
#include <vector>
#include <stlport/string>
#include <stlport/sstream>
#include <clocale>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include "measure.stuff.h"

#define _SIMPLE_CONCAT          0
#define _DYNRAND_CONCAT         0
#define _COMP_CONCAT            0
#define _SIMPLE_DYN_COMP_TESTS  0

static const int IterationCount = 38;

typedef std::wstring                StdString;
typedef AtlStd::Adapter<wchar_t>    AtlString;
typedef stlport::wstring            StpString;

template <typename StringT>
static inline stlport::string _ToString(typename StringT const& rhs) 
{
    return stlport::string(CW2A(rhs.c_str())); 
}

template <typename StringT>
static inline typename StringT A_plus_B(typename StringT::value_type const* astr, typename StringT::value_type const* bstr)
{
    typename StringT a = astr;
    typename StringT b = bstr;

    return a + b;
}

template <typename StringT>
static inline typename StringT A_plus_B(typename StringT& a, typename StringT::value_type const* astr
                                      , typename StringT& b, typename StringT::value_type const* bstr
                                      )
{
    a = astr;
    b = bstr;

    return a + b;
}

template <typename StringT, typename StreamT>
static inline typename StringT Comp_TestSs(typename StreamT& stream, typename StringT::value_type const* someText)
{
    typedef typename StringT::value_type CharT;

    stream << (CharT)' ' << (long double)3.14159265358979323846 
           << (CharT)' ' << (double)3.14159265358979323846 
           << (CharT)' ' << (float)3.14159265358979323846 
           << (CharT)' ' << (unsigned int)6782436
           << (CharT)' ' << (unsigned short)1024
           << (CharT)' ' << (unsigned char)76
           << (CharT)' ' << someText
           ;

    return stream.str();
}

static inline AtlString Comp_TestFt(wchar_t const* someText)
{
    AtlString temp;

    temp.Format(L" %.12f %.8f %.4f %d %d %d `%s`"
        , (long double)3.14159265358979323846 
        , (double)3.14159265358979323846 
        , (float)3.14159265358979323846 
        , (unsigned int)6782436
        , (unsigned short)1024
        , (unsigned char)76
        , someText
        );

    return temp;
}

static CStringW _GenString()
{
    int l = 8 + ::rand() % 24;
    CStringW rv; //(L' ', l);

    for (int i=0; i<l; i++)
        rv.AppendChar((L' ' + 1) + ::rand() % 20);

    rv.AppendChar(L' ');
    return rv;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ::setlocale(LC_ALL, "");
    ::srand(::GetTickCount());

#if _SIMPLE_CONCAT
    {
        _measure_output("simple_concat");
        _measure_puts("STL\tATL\tSTLPORT");
        for (int i=0; i <= IterationCount; i++)
        {
            _measure3(i
                    , A_plus_B<StdString>(L"String #1 ", L"String #2")
                    , A_plus_B<AtlString>(L"String #1 ", L"String #2")
                    , A_plus_B<StpString>(L"String #1 ", L"String #2")
                    );
        }
        _measure_done();
    }
#endif

#if _DYNRAND_CONCAT
    {
        std::vector<boost::shared_ptr<StdString> > stdv;
        std::vector<boost::shared_ptr<AtlString> > atlv;
        std::vector<boost::shared_ptr<StpString> > stpv;

        _measure_output("dynamic_concat");
        _measure_puts("STL\tATL\tSTLPORT");

        for (int i = 0; i <= IterationCount; i++)
        {
            boost::shared_ptr<StdString> stda(new StdString()); stdv.push_back(stda);
            boost::shared_ptr<StdString> stdb(new StdString()); stdv.push_back(stdb);
            boost::shared_ptr<AtlString> atla(new AtlString()); atlv.push_back(atla);
            boost::shared_ptr<AtlString> atlb(new AtlString()); atlv.push_back(atla);
            boost::shared_ptr<StpString> stpa(new StpString()); stpv.push_back(stpa);
            boost::shared_ptr<StpString> stpb(new StpString()); stpv.push_back(stpa);

            CStringW rawa = _GenString();
            CStringW rawb = _GenString();

            _measure3(i
                    , A_plus_B<StdString>(*stda, rawa, *stdb, rawb)
                    , A_plus_B<AtlString>(*atla, rawa, *atlb, rawb)
                    , A_plus_B<StpString>(*stpa, rawa, *stpb, rawb)
                    );
        }

        _measure_done();
    }
#endif

#if _COMP_CONCAT
    {
        _measure_output("comp_concat");
        _measure_puts("STL\tATL\tSTLPORT");

        for (int i = 0; i <= IterationCount; i++)
        {
            std::wstringstream st1;
            stlport::wstringstream st2;

            CStringW someText = _GenString();

            _measure3(i
                    , Comp_TestSs<StdString>(st1, someText)
                    , Comp_TestFt(someText)
                    , Comp_TestSs<StpString>(st2, someText)
                    );
        }
    }
#endif    

#if _SIMPLE_DYN_COMP_TESTS
    {
        _measure_output("simple_dyn_comp_tests");
        _measure_puts("simple A+B STL\tsimple A+B ATL\tsimple A+B STLPORT\t"
                      "dynamic A+B STL\tdynamic A+B ATL\tdynamic A+B STLPORT\t"
                      "operator << STL\tformat ATL\toperator << STLPORT"
                      );

        std::vector<boost::shared_ptr<StdString> > stdv;
        std::vector<boost::shared_ptr<AtlString> > atlv;
        std::vector<boost::shared_ptr<StpString> > stpv;

        for (int i=0; i <= IterationCount; i++)
        {
            boost::shared_ptr<StdString> stda(new StdString()); stdv.push_back(stda);
            boost::shared_ptr<StdString> stdb(new StdString()); stdv.push_back(stdb);
            boost::shared_ptr<AtlString> atla(new AtlString()); atlv.push_back(atla);
            boost::shared_ptr<AtlString> atlb(new AtlString()); atlv.push_back(atla);
            boost::shared_ptr<StpString> stpa(new StpString()); stpv.push_back(stpa);
            boost::shared_ptr<StpString> stpb(new StpString()); stpv.push_back(stpa);

            std::wstringstream st1;
            stlport::wstringstream st2;

            CStringW rawa = _GenString();
            CStringW rawb = _GenString();
            CStringW someText = _GenString();

            _measure9(i
                , A_plus_B<StdString>(L"String #1 ", L"String #2")
                , A_plus_B<AtlString>(L"String #1 ", L"String #2")
                , A_plus_B<StpString>(L"String #1 ", L"String #2")
                , A_plus_B<StdString>(*stda, rawa, *stdb, rawb)
                , A_plus_B<AtlString>(*atla, rawa, *atlb, rawb)
                , A_plus_B<StpString>(*stpa, rawa, *stpb, rawb)
                , Comp_TestSs<StdString>(st1, someText)
                , Comp_TestFt(someText)
                , Comp_TestSs<StpString>(st2, someText)
                );
        }
        _measure_done();
    }
#endif
    return 0;
}
