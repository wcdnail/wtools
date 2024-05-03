#include "../win.target.2k.h"
#include "../win.dll.h"
#include "../report.bugs.h"
#include "../error.seh.h"

namespace 
{
    class Hint
    {
    public:
        Hint(char const* hint): hint_(hint) 
        {
            OutputDebugStringA(__FUNCSIG__);
            OutputDebugStringA(": ");
            OutputDebugStringA(hint_); 
            OutputDebugStringA("\n");
        }

        ~Hint() 
        { 
            OutputDebugStringA(__FUNCSIG__);
            OutputDebugStringA(": ");
            OutputDebugStringA(hint_); 
            OutputDebugStringA("\n");
        }
    private:
        char const* hint_;
    };

    int main_ex(int argc, char *argv[]) 
    {
        Hint hint1("Object 1");

        try
        {
            Hint hint2("Object 2");
            Hint hint3("Object 3");

            BOOST_THROW_EXCEPTION(Exception("Simple boost exception"));
            *((char*)(127)) = 'A';
            throw std::logic_error("Simple logic error");
            BOOST_THROW_EXCEPTION(std::logic_error("Simple logic error (boost)"));
        }
        catch (Exception const& ex) 
        {
            puts(ex.Dump());
            ShowBugReport(ex);
        }
        catch (std::exception const& ex) 
        {
            puts(ex.what());
            ShowBugReport(ex);
        }

        return 0;
    }

	LONG WINAPI SEH_flt(EXCEPTION_POINTERS *info) 
	{
        SEHRecord seh(info);
        puts(seh.Info());
		ShowBugReport(seh);
		return EXCEPTION_EXECUTE_HANDLER;
	}
}

int main(int argc, char *argv[]) 
{
    int rv = 0;
    __try 
    {
        rv = main_ex(argc, argv);
    }
    __except(SEH_flt(GetExceptionInformation()))
    {}
    return rv;
}
