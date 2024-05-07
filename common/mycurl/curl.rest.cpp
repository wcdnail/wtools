#include "stdafx.h"
#include "curl.rest.h"
#include <string.hp.h>
#include <dh.tracing.h>
#include <crash.report/exception.h>
#include <curlpp/curlpp.hpp>
#include <curlpp/easy.hpp>
#include <curlpp/options.hpp>
#include <curlpp/exception.hpp>
#include <curlpp/infos.hpp>
#include <fstream>
#include <ctime>

#ifdef _WIN32
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "WLDAP32")
#endif

namespace Curl
{
    static const Dh::TraceCategory CurlApi(L"CURLAPI");
    static const bool GenerateRespFiles = Dh::DebugTrue;

#ifdef _DEBUG
    static CharString _GenFileName(char const* prefix, char const* postfix)
    {
        __time64_t lt = 0;
        ::_time64(&lt);

        struct tm ts = {0};
        ::_localtime64_s(&ts, &lt);

        char timeStr[256] = {0};
        ::strftime(timeStr, _countof(timeStr) - 1, "%d%m%Y.%H%M%S", &ts);

        OStringStream filename;
        filename << prefix << "." << timeStr << "." << postfix;

        return filename.str();
    }

    static void _FlushStream(std::ofstream& stream)
    {
        if (stream.is_open())
        {
            stream.flush();
            stream.close();
        }
    }
#endif

    class RestSession::Impl
    {
    public:
        Impl(bool genrespFiles) 
            : easy()
            , lastErrorCode(0)
            , lastErrorWhat()
            , lastErrorText()
            , ResponseStream(NULL)
            , HeaderStream()
            , cleanup() 
            , traceTimer()
#ifdef _DEBUG
            , dbgGenrespFiles(genrespFiles)
            , dbgResponseStream()
            , dbgHeaderStream()
#endif
        {}

        ~Impl() {}

        void SetTrace()
        {
            curlpp::types::DebugFunctionFunctor ftor(utilspp::BindFirst(utilspp::make_functor(this, &Impl::OnTrace), &easy));
            curlpp::options::DebugFunction func(ftor);
            easy.setOpt(func);
        }

        void SetWrite()
        {
            curlpp::types::WriteFunctionFunctor wftor(utilspp::BindFirst(utilspp::make_functor(this, &Impl::OnWrite), &easy));
            curlpp::options::WriteFunction writer(wftor);
            easy.setOpt(writer);

            curlpp::types::WriteFunctionFunctor hftor(utilspp::BindFirst(utilspp::make_functor(this, &Impl::OnWriteHeader), &easy));
            curlpp::options::HeaderFunction header(hftor);
            easy.setOpt(header);
        }

        void OnBegin(OStringStream& result)
        {
            ResponseStream = &result;
            HeaderStream.str(CharString());

#ifdef _DEBUG
            if (dbgGenrespFiles)
            {
                dbgResponseStream.open(_GenFileName("debug\\resp", "txt").c_str());
                dbgHeaderStream.open(_GenFileName("debug\\head", "txt").c_str());
            }
#endif
        }

        void OnEnd(bool success)
        {
#ifdef _DEBUG
            _FlushStream(dbgResponseStream);
            _FlushStream(dbgHeaderStream);
#endif

            if (success)
                ParseHeader();
        }

    public:
        curlpp::Easy easy;
        unsigned lastErrorCode;
        CharString lastErrorWhat;
        CharString lastErrorText;
        OStringStream* ResponseStream;
        OStringStream HeaderStream;

    private:
        curlpp::Cleanup cleanup;
        Dh::Timer traceTimer;

#ifdef _DEBUG
        bool dbgGenrespFiles;
        std::ofstream dbgResponseStream;
        std::ofstream dbgHeaderStream;
#endif

        static Dh::TraceCategory const& headerTypeCat(int id)
        {
            static const Dh::TraceCategory cats[] = 
            {
              Dh::TraceCategory(L"TEXT")        // CURLINFO_TEXT = 0
            , Dh::TraceCategory(L"HEADERIN")    // CURLINFO_HEADER_IN
            , Dh::TraceCategory(L"HEADROUT")    // CURLINFO_HEADER_OUT
            , Dh::TraceCategory(L"DATAIN")      // CURLINFO_DATA_IN
            , Dh::TraceCategory(L"DATAOUT")     // CURLINFO_DATA_OUT
            , Dh::TraceCategory(L"SSLDTAIN")    // CURLINFO_SSL_DATA_IN
            , Dh::TraceCategory(L"SSLDTOUT")    // CURLINFO_SSL_DATA_OUT
            , Dh::TraceCategory(L"UNKNOWN") 
            };

            if ((id >= 0) && (id < _countof(cats)-1))
                return cats[id];

            return cats[_countof(cats)-1];
        }

        int OnTrace(curlpp::Easy*, curl_infotype type, char const* ptr, const size_t size)
        {
            char dumpbuf[54] = {0};
            Dh::MakePrintable(dumpbuf, ptr, size);

            Dh::TCPrintf(headerTypeCat((int)type), "(%d) %03.8f %08lu `%s`\n", type, traceTimer.Seconds(), size, dumpbuf);

            traceTimer = Dh::Timer();
            return (int)size;
        };

        size_t OnWrite(curlpp::Easy*, char const* buffer, size_t size, size_t nitems)
        {
            size_t rawSize = size * nitems;

            if (ResponseStream)
                ResponseStream->write(buffer, rawSize);

#ifdef _DEBUG
            if (dbgGenrespFiles && dbgResponseStream.is_open())
                dbgResponseStream.write(buffer, rawSize);
#endif
            return rawSize;
        }

        size_t OnWriteHeader(curlpp::Easy*, char const* buffer, size_t size, size_t nitems)
        {
            size_t rawSize = size * nitems;
            HeaderStream.write(buffer, rawSize);

#ifdef _DEBUG
            if (dbgGenrespFiles && dbgHeaderStream.is_open())
                dbgHeaderStream.write(buffer, rawSize);
#endif
            return rawSize;
        }

        void ParseHeader()
        {
            CharString header(HeaderStream.str());
            DBGCPrint(CurlApi, "Header:\n%s\n", header.c_str());
        }

    public:
        void OnException(std::exception const& ex)
        {
            OnEnd(false);

            lastErrorCode = (unsigned)-1;
            lastErrorWhat = "std::exception";
            lastErrorText = ex.what();

            if (!lastErrorWhat.empty())
                Dh::TCPrintf(CurlApi, "%s\n", lastErrorWhat.c_str());
        }

        template <typename T>
        void OnException(T const& ex)
        {
            OnEnd(false);

            lastErrorCode = ex.whatCode();
            lastErrorWhat = ex.what();
            lastErrorText = ::curl_easy_strerror((CURLcode)lastErrorCode);

            if (!lastErrorWhat.empty())
                Dh::TCPrintf(CurlApi, "%s\n", lastErrorWhat.c_str());

            Dh::TCPrintf(CurlApi, "%d %s\n", lastErrorCode, lastErrorText.c_str());
        }
    };

    #define _CATCH_ALL_PASS_NEXT()                      \
        catch (curlpp::LibcurlRuntimeError const& ex)   \
        {                                               \
            impl->OnException(ex);                      \
        }                                               \
        catch (std::exception const& ex)                \
        {                                               \
            impl->OnException(ex);                      \
        }


    RestSession::RestSession()
        : impl(new Impl(GenerateRespFiles))
    {
        try
        {
            impl->SetWrite();
        }
        _CATCH_ALL_PASS_NEXT()
    }

    RestSession::~RestSession()
    {}

    void RestSession::Reset() 
    {
        impl.reset(new Impl(GenerateRespFiles)); 
    }

    bool RestSession::SetProxy(char const* address, unsigned type, char const* cred) const
    {
        try
        {
            curlpp::options::Proxy paddr(address);
            curlpp::options::ProxyAuth ptype(type);
            curlpp::options::ProxyUserPwd pcred(cred);

            impl->easy.setOpt(paddr);
            impl->easy.setOpt(ptype);
            impl->easy.setOpt(pcred);
            return true;
        }
        _CATCH_ALL_PASS_NEXT()

        return false;
    }

    bool RestSession::SetVerbose(bool verbose) const
    {
        try
        {
            curlpp::options::Verbose vb(verbose);

            impl->easy.setOpt(vb);
            return true;
        }
        _CATCH_ALL_PASS_NEXT()

        return false;
    }

    bool RestSession::SetTrace() const
    {
        try
        {
            impl->SetTrace();
            return true;
        }
        _CATCH_ALL_PASS_NEXT()

        return false;
    }

    bool RestSession::CheckConnection(CharString const& urlString) const
    {
        try
        {
            OStringStream respBuff;
            if (Perform(urlString, respBuff))
            {

                return true;
            }
        }
        catch(...)
        {}

        return false;
    }

    bool RestSession::Perform(CharString const& urlString, OStringStream& results) const
    {
        try
        {
            impl->OnBegin(results);

            curlpp::options::Url url(urlString);
            impl->easy.setOpt(url);
            impl->easy.perform();

            impl->OnEnd(true);
            return true;
        }
        _CATCH_ALL_PASS_NEXT()

        return false;
    }
}
