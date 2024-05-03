#pragma once

#include <string>

namespace Filetype
{
   class DbLoader
   {
   public:
      ~DbLoader();
      DbLoader();

      void ParseDirectory(wchar_t const* initialdir);

   private:
      void LoadNParseText(wchar_t const* filename);
      void ParseText(std::string const& content);
   };
}
