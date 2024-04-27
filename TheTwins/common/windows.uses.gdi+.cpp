#include "stdafx.h"
#include "windows.uses.gdi+.h"
#include "string.utils.error.code.h"
#include <stdexcept>
#include <sstream>
#include <gdiplus.h>

#pragma comment(lib, "GDIPLUS")

namespace Initialize
{
    GdiPlus::GdiPlus(int requiredver)
        : Id(0)
    {
        Gdiplus::GdiplusStartupInput input;
        input.GdiplusVersion = requiredver;

        Gdiplus::Status rv = Gdiplus::GdiplusStartup(&Id, &input, NULL);
        if (Gdiplus::Ok != rv)
        {
            std::stringstream message;

            message << "Initialization ERROR\nGdiplus::GdiplusStartup failed - " 
                    << rv << " - " << GdiPlus::StatusString(rv);

            throw std::runtime_error(message.str());
        }
    }

    GdiPlus::~GdiPlus() /* throw() */
    {
        Gdiplus::GdiplusShutdown(Id);
    }

    char const* GdiPlus::StatusString(int status)
    {
        switch (status)
        {
        case Gdiplus::Ok                         : return "No errors";
        case Gdiplus::GenericError               : return "Generic error";
        case Gdiplus::InvalidParameter           : return "Invalid parameter";
        case Gdiplus::OutOfMemory                : return "Out of memory";
        case Gdiplus::ObjectBusy                 : return "Object busy";
        case Gdiplus::InsufficientBuffer         : return "Insufficient buffer";
        case Gdiplus::NotImplemented             : return "Not implemented";
        case Gdiplus::Win32Error                 : return "Win32 error";
        case Gdiplus::WrongState                 : return "Wrong state";
        case Gdiplus::Aborted                    : return "Aborted";
        case Gdiplus::FileNotFound               : return "File not found";
        case Gdiplus::ValueOverflow              : return "Value overflow";
        case Gdiplus::AccessDenied               : return "Access denied";
        case Gdiplus::UnknownImageFormat         : return "Unknown image format";
        case Gdiplus::FontFamilyNotFound         : return "Font family not found";
        case Gdiplus::FontStyleNotFound          : return "Font style not found";
        case Gdiplus::NotTrueTypeFont            : return "Not TRUETYPE font";
        case Gdiplus::UnsupportedGdiplusVersion  : return "Unsupported GDI+ version";
        case Gdiplus::GdiplusNotInitialized      : return "GDI+ not initialized";
        case Gdiplus::PropertyNotFound           : return "Property not found";
        case Gdiplus::PropertyNotSupported       : return "Property not supported";
#if (GDIPVER >= 0x0110)
        case Gdiplus::ProfileNotFound            : return "Profile not found";
#endif
        }

        return "";
    }
}
