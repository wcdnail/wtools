#include "stdafx.h"
#include <mycurl/google.translate.h>
#include <exceptions.any.h>
#include <dh.tracing.h>
#include <curl/curl.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    try
    {
        Google::HttpTranslator translator;
        Curl::RestSession& session = translator.GetSession();

        //session.SetProxy("gdcproxy.russia.local:3128", CURLAUTH_NTLM, "RUSSIA\\NikonovM:wiperLine2");
        session.SetProxy("gdcproxy.russia.local:3128", CURLAUTH_NTLM, "RUSSIA\\NikonovM:jfdsklgjsf");

#ifdef _DEBUG
        session.SetVerbose(true);
        session.SetTrace();
#endif
        if (!session.CheckConnection("http://google.com/"))
            throw CrashException("http://google.com/ - check connection failed", 5);
    
        Google::HttpTranslator::ResultPairs result;

        result = translator.Translate("Ei-aktiivinen", "fi", "ru");
        //result = translator.Translate("Poistettu", "fi", "ru");
        //result = translator.Translate("Salasana", "fi", "ru");
        //result = translator.Translate("Valitse yhdistettävät vuorot", "fi", "ru");

    }
    catch (...)
    {
        Cf::HandleAnyException(Cf::HandleAnyExceptionDefaults());
    }

    return 0;
}
