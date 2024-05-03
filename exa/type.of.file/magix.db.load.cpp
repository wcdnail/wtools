#include "stdafx.h"
#include "magix.db.load.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

namespace Filetype
{
   DbLoader::~DbLoader()
   {}

   DbLoader::DbLoader()
   {}

   void DbLoader::ParseDirectory(wchar_t const* initialdir)
   {
      using namespace boost::filesystem;

      path p(initialdir);

      recursive_directory_iterator end;
      for (recursive_directory_iterator it(p); it != end; ++it)
         LoadNParseText(it->path().c_str());
   }

   void DbLoader::LoadNParseText(wchar_t const* filename)
   {
      std::wcout << L"Loading: `" << filename << "`";

      std::ifstream in(filename);
      in.unsetf(std::ios::skipws);

      std::string content;
      std::copy(std::istream_iterator<char>(in)
              , std::istream_iterator<char>()
              , std::back_inserter(content));

      std::wcout << L"\nContent:\n\n" << content.c_str();

      ParseText(content);

      std::wcout << std::endl;
   }

   void DbLoader::ParseText(std::string const& content)
   {
   }
}
