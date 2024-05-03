#include "stdafx.h"
#if 0
#include "type.of.file.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>

namespace Cf
{
   char const* Filetype::ToString(Id type)
   {
      #define Stringize(X) \
         case X: return #X

      switch (type)
      {
      // Executables
      Stringize(Executable::Elf);
      Stringize(Executable::Dll);
      Stringize(Executable::Dos);
      Stringize(Executable::Batch);
      Stringize(Executable::Shell);
      Stringize(Executable::Windows);

      // Binary
      Stringize(Binary::Object);
      Stringize(Binary::Raw);

      // Compressed
      Stringize(Compressed::Zip);
      Stringize(Compressed::Rar);
      Stringize(Compressed::Bzip2);
      Stringize(Compressed::Sevenzip);
      Stringize(Compressed::Tar);

      // Microsoft
      Stringize(Microsoft::Document);
      Stringize(Microsoft::Excell);
      Stringize(Microsoft::Bitmap);

      // Images
      Stringize(Image::Png);
      Stringize(Image::Jpeg);
      Stringize(Image::Psd);
      Stringize(Image::Gif);
      Stringize(Image::Xpm);
      Stringize(Image::Svg);
      Stringize(Image::Targa);
      Stringize(Image::Pcx);

      // Sounds
      Stringize(Sound::Mp3);
      Stringize(Sound::Ogg);
      Stringize(Sound::Wav);

      // Sources
      Stringize(Source::Cpp);
      Stringize(Source::C);
      Stringize(Source::Header);
      Stringize(Source::Assembler);
      Stringize(Source::Pascal);
      Stringize(Source::Basic);
      Stringize(Source::Js);
      Stringize(Source::Java);
      Stringize(Source::Html);
      Stringize(Source::Css);
      Stringize(Source::Xslt);
      Stringize(Source::Perl);
      Stringize(Source::Python);

      // Misc
      Stringize(Misc::Pdf);
      Stringize(Misc::Rtf);
      Stringize(Misc::Config);
      Stringize(Misc::Text);
      Stringize(Misc::Xml);
      }

      return "";

      #undef Stringize
   }

   Filetype::Id Filetype::GetBySignaure(std::string const& pathname)
   {
      Id rv = Unknown;

      union
      {
         char buffer[512];
         uint16_t sign16;
         uint32_t sign32;
      };

      {
         std::ifstream input(pathname.c_str());
         if (input.is_open())
            input.readsome(buffer, sizeof(buffer));
         else
            return rv;
      }

      // attempting to get file type from 4 byte-signature...
      switch (sign32)
      {
      case 0x464c457f: rv = Executable::Elf; break;
      case 0x04034b50: rv = Compressed::Zip; break;
      case 0x21726152: rv = Compressed::Rar; break;
      case 0x5f786170: rv = Compressed::Tar; break;
      case 0xe011cfd0: rv = Microsoft::Document; break;
      case 0x474e5089: rv = Image::Png; break;
      case 0xe0ffd8ff: rv = Image::Jpeg; break;
      case 0x53504238: rv = Image::Psd; break;
      case 0x38464947: rv = Image::Gif; break;
      case 0x46445025: rv = Misc::Pdf; break;
      }

      // can't get file type from 4 byte-signature...
      if (Unknown == rv)
      {
         // attempting to get file type from 3 byte-signature...
         switch (sign32 & 0xffffff)
         {
         case 0x334449: rv = Sound::Mp3; break;
         }
      }

      // can't get file type from 3 byte-signature...
      if (Unknown == rv)
      {
         // attempting to get file type from 2 byte-signature...
         switch (sign16)
         {
         case 0x5a42: rv = Compressed::Bzip2; break;
         case 0x5a4d: rv = Executable::Windows; break;
         case 0x4d42: rv = Microsoft::Bitmap; break;
         }
      }

      return rv;
   }

   Filetype::Id Filetype::GetByExtension(std::string const& pathname)
   {
      Id rv = Unknown;

      std::string::size_type dot = pathname.rfind('.');
      if (std::string::npos == dot)
         return rv;

      char const* ext = &pathname[++dot];

      uint32_t mask = 0xffffffff;
      size_t len = pathname.length() - dot;
      for (size_t i=len; i<sizeof(mask); i++)
         mask >>= 8;

      uint32_t ext32 = (*((uint32_t*)&ext[0])) & mask;
      switch (ext32)
      {
      case 0x004c4c44:                                      // DLL
      case 0x0058434f:                                      // OCX
      case 0x00004f53: rv = Executable::Dll;         break; // SO
      case 0x00455845: rv = Executable::Windows;     break; // EXE
      case 0x004d4f43: rv = Executable::Dos;         break; // COM

      case 0x0000004f:                                      // O
      case 0x004a424f: rv = Binary::Object;          break; // OBJ
      case 0x00584548:                                      // HEX
      case 0x00544144:                                      // DAT
      case 0x00574152: rv = Binary::Raw;             break; // RAW

      case 0x0050495a: rv = Compressed::Zip;         break; // ZIP
      case 0x00524152: rv = Compressed::Rar;         break; // RAR
      case 0x50495a42:                                      // BZIP2
      case 0x00325a42: rv = Compressed::Bzip2;       break; // BZ2
      case 0x00005a37:                                      // 7Z
      case 0x50495a37: rv = Compressed::Sevenzip;    break; // 7ZIP
      case 0x00524154: rv = Compressed::Tar;         break; // TAR

      case 0x00434f44:                                      // DOC
      case 0x58434f44: rv = Microsoft::Document;     break; // DOCX
      case 0x004c5358: rv = Microsoft::Excell;       break; // XSL
      case 0x00504d42: rv = Microsoft::Bitmap;       break; // BMP

      case 0x00474e50: rv = Image::Png;              break; // PNG
      case 0x0047504a:                                      // JPG
      case 0x4745504a:                                      // JPEG
      case 0x4649464a: rv = Image::Jpeg;             break; // JFIF
      case 0x00445350: rv = Image::Psd;              break; // PSD
      case 0x00464947: rv = Image::Gif;              break; // GIF
      case 0x004d5058: rv = Image::Xpm;              break; // XPM
      case 0x00475653: rv = Image::Svg;              break; // SVG

      case 0x0033504d: rv = Sound::Mp3;              break; // MP3
      case 0x0047474f: rv = Sound::Ogg;              break; // OGG
      case 0x00564157: rv = Sound::Wav;              break; // WAV

      case 0x00505043:                                      // CPP
      case 0x00004343:                                      // CC
      case 0x00585843:                                      // CXX
      case 0x004c4e49: rv = Source::Cpp;             break; // INL
      case 0x00000043: rv = Source::C;               break; // C
      case 0x00000048:                                      // H
      case 0x00505048: rv = Source::Header;          break; // HPP
      case 0x004d5341:                                      // ASM
      case 0x00000053:                                      // S
      case 0x00000041: rv = Source::Assembler;       break; // A
      case 0x00534150: rv = Source::Pascal;          break; // PAS
      case 0x00534142:                                      // BAS
      case 0x00004256: rv = Source::Basic;           break; // VB
      case 0x0000534a: rv = Source::Js;              break; // JS
      case 0x4156414a: rv = Source::Java;            break; // JAVA
      case 0x4c4d5448:                                      // HTML
      case 0x004d5448: rv = Source::Html;            break; // HTM
      case 0x00535343: rv = Source::Css;             break; // CSS
      case 0x544c5358: rv = Source::Xslt;            break; // XSLT
      case 0x00004c50:                                      // PL
      case 0x00004d50: rv = Source::Perl;            break; // PM
      case 0x00005950: rv = Source::Python;          break; // PY

      case 0x00464450: rv = Misc::Pdf;               break; // PDF
      case 0x00465452: rv = Misc::Rtf;               break; // RTF
      case 0x00494e49:                                      // INI
      case 0x464e4f43:                                      // CONF
      case 0x00464e43: rv = Misc::Config;            break; // CNF
      case 0x00545854:                                      // TXT
      case 0x00435341: rv = Misc::Text;              break; // ASC
      case 0x004c4d58: rv = Misc::Xml;               break; // XML
      }

      if (Unknown == rv)
         std::cout << pathname.c_str() << ": " << ext << " (" << std::hex << ext32 << ")\n";
         
      return rv;
   }
}
#endif
