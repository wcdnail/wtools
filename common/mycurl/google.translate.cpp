#include "stdafx.h"
#include "google.translate.h"
#include <crash.report/exception.h>
#include <dh.tracing.h>
#include <boost/shared_ptr.hpp>
#include <json/json.h>
#include <atlconv.h>

namespace Google
{
    static const Dh::TraceCategory GoogleTt(L"GOOGLETT");

    void _ParseResp(OStringStream const& input, HttpTranslator::ResultPairs& output);

    HttpTranslator::HttpTranslator()
        : Session()
    {}

    HttpTranslator::~HttpTranslator()
    {}

    HttpTranslator::ResultPairs HttpTranslator::Translate(WidecharString const& text, char const* sourceLang, char const* targetLang) const
    {
        std::string converted = CW2A(text.c_str(), CP_UTF8);
        return Translate(converted, sourceLang, targetLang);
    }

    HttpTranslator::ResultPairs HttpTranslator::Translate(CharString const& text, char const* sourceLang, char const* targetLang) const
    {
        ResultPairs result;
        Dh::Timer reqTimer;

        boost::shared_ptr<char> escapedText(::curl_escape(text.c_str(), (int)text.length()), free);
        if (!escapedText)
            throw CrashException((text + " - can't convert it.").c_str(), CURLE_CONV_FAILED);

        OStringStream requestUrl;
        requestUrl << "http://translate.google.ru/translate_a/t?client=json"
                        "&text=" << escapedText.get()
                    << "&hl=" << targetLang
                    << "&sl=" << sourceLang
                    << "&tl=" << targetLang
                    << "&ie=UTF-8"
                        "&oe=UTF-8"
                        "&multires=1"
                        "&otf=1"
                        "&ssel=0"
                        "&tsel=0"
                        "&sc=1"
                    ;

        OStringStream tempResult;
        if (Session.Perform(requestUrl.str(), tempResult))
            _ParseResp(tempResult, result);

        DBGCPrint(GoogleTt, "%03.8f sec.\n", reqTimer.Seconds());
        return result;
    }

    static void _ParseResp(OStringStream const& input, HttpTranslator::ResultPairs& output)
    {
        CharString temp(input.str());

        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(temp, root,  false))
        {
            char tempBuffer[32] = {0};
            Dh::MakePrintable(tempBuffer, temp.c_str(), temp.length());

            CharString message = (CharString("`") + tempBuffer) + "...`\n" + reader.getFormatedErrorMessages();
            throw CrashException(message.c_str(), 1);
        }

        const Json::Value sentences = root["sentences"];
        std::string src = root["src"].asString();
        unsigned server_time = root["server_time"].asUInt();
        
        HttpTranslator::ResultPairs tempOutput;

        for (unsigned i=0; i<sentences.size(); i++)
        {
            const Json::Value obj = sentences[i];
            
            std::wstring orig = CA2W(obj["orig"].asCString(), CP_UTF8);
            std::wstring trans = CA2W(obj["trans"].asCString(), CP_UTF8);

            tempOutput[orig] = trans;

            DBGCPrint(GoogleTt, "`%s` === `%s`\n", orig.c_str(), trans.c_str());
        }

        output.swap(tempOutput);
    }
}
