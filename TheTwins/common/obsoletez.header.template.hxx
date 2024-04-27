#ifndef _CF_<Header Template>_h__
#define _CF_<Header Template>_h__

namespace CF  // Common framework
{
    class SomeClass
    {
    public:
        SomeClass();
        ~SomeClass();

    private:
        SomeClass(SomeClass const&);
        SomeClass& operator = (SomeClass const&);
    };

    inline SomeClass::SomeClass()
    {}

    inline SomeClass::~SomeClass()
    {}
}

#endif // _CF_<Header Template>_h__
