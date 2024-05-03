#pragma once

#if 0
#include <string>

namespace Cf
{
   struct Filetype
   {
      typedef unsigned Id;

      enum
      {
        Unknown = 0
      , First
      };

      struct Executable
      {
         enum
         {
           Elf = First
         , Dll, Dos, Batch, Shell, Windows
         // TODO: append more
         , Last
         };
      };

      struct Binary
      {
         enum
         {
           Object = Executable::Last
         , Raw
         // TODO: append more
         , Last
         };
      };

      struct Compressed
      {
         enum
         {
           Zip = Binary::Last
         , Rar, Bzip2, Sevenzip, Tar
         // TODO: append more
         , Last
         };
      };

      struct Microsoft
      {
         enum
         {
           Document = Compressed::Last
         , Excell, Bitmap
         // TODO: append more
         , Last
         };
      };

      struct Image
      {
         enum
         {
           Bmp = Microsoft::Bitmap
         , Png = Microsoft::Last
         , Jpeg, Psd, Gif, Xpm, Svg, Targa, Pcx
         // TODO: append more
         , Last
         };
      };

      struct Sound
      {
         enum
         {
           Mp3 = Image::Last
         , Ogg, Wav, Flac
         // TODO: append more
         , Last
         };
      };

      struct Source
      {
         enum
         {
           Cpp = Sound::Last
         , C, Header
         , Assembler, Pascal, Basic, Js, Java, Html, Css
         // TODO: append more
         , Xslt, Perl, Python
         , Last
         };
      };

      struct Misc
      {
         enum
         {
           Pdf = Source::Last
         , Rtf, Config, Text, Xml
         // TODO: append more
         , Last
         };
      };

      static char const* ToString(Id type);
      static Id GetBySignaure(std::string const& pathname);
      static Id GetByExtension(std::string const& pathname);
   };
}

#endif // 0
