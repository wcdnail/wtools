#pragma once

#include "ref.serialization.h"
#include "ref.std.streaming.h"
#include <string>
#include <iosfwd>
#include <string.h>
#include <wingdi.h>

namespace Ref
{
    class Font: public LOGFONT
              , public Serializable<Font>
    {
    public:
        Font();
        Font(int size, std::basic_string<TCHAR> const& faceName);

        template <class A>
        void Serialize(A& ar);

    private:
        void Zeroing();
    };

    inline Font::Font()
    {
        Zeroing();
    }

    inline Font::Font(int size, std::basic_string<TCHAR> const& facename)
    {
        Zeroing();

        lfHeight = size;
        facename._Copy_s(lfFaceName, sizeof(lfFaceName), _countof(lfFaceName)-1);
    }

    inline void Font::Zeroing()
    {
        lfHeight         = 0;
        lfWidth          = 0;
        lfEscapement     = 0;
        lfOrientation    = 0;
        lfWeight         = 0;
        lfItalic         = 0;
        lfUnderline      = 0;
        lfStrikeOut      = 0;
        lfCharSet        = 0;
        lfOutPrecision   = 0;
        lfClipPrecision  = 0;
        lfQuality        = 0;
        lfPitchAndFamily = 0;
        ::memset(&lfFaceName, 0, sizeof(lfFaceName));
    }

    template <class A>
    inline void Font::Serialize(A& ar)
    {
        std::basic_string<TCHAR> facename(lfFaceName);
        ar & boost::serialization::make_nvp("Facename", facename);
        facename._Copy_s(lfFaceName, sizeof(lfFaceName), _countof(lfFaceName)-1);

        ar & boost::serialization::make_nvp("Height", lfHeight);
        ar & boost::serialization::make_nvp("Width", lfWidth);
        ar & boost::serialization::make_nvp("Escapement", lfEscapement);
        ar & boost::serialization::make_nvp("Orientation", lfOrientation);
        ar & boost::serialization::make_nvp("Weight", lfWeight);
        ar & boost::serialization::make_nvp("Italic", lfItalic);
        ar & boost::serialization::make_nvp("Underline", lfUnderline);
        ar & boost::serialization::make_nvp("StrikeOut", lfStrikeOut);
        ar & boost::serialization::make_nvp("CharSet", lfCharSet);
        ar & boost::serialization::make_nvp("OutPrecision", lfOutPrecision);
        ar & boost::serialization::make_nvp("ClipPrecision", lfClipPrecision);
        ar & boost::serialization::make_nvp("Quality", lfQuality);
        ar & boost::serialization::make_nvp("PitchAndFamily", lfPitchAndFamily);
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Font const& font)
    {
        std::basic_string<TCHAR> facename(font.lfFaceName);

        stream << facename
               << (C)'-' << font.lfHeight
               << (C)'-' << font.lfWidth
               << (C)'-' << font.lfEscapement
               << (C)'-' << font.lfOrientation
               << (C)'-' << font.lfWeight
               << (C)'-' << (int)font.lfItalic
               << (C)'-' << (int)font.lfUnderline
               << (C)'-' << (int)font.lfStrikeOut
               << (C)'-' << (int)font.lfCharSet
               << (C)'-' << (int)font.lfOutPrecision
               << (C)'-' << (int)font.lfClipPrecision
               << (C)'-' << (int)font.lfQuality
               << (C)'-' << (int)font.lfPitchAndFamily
               ;

        return stream;
    }
}
