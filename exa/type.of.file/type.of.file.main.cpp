#include "stdafx.h"
#include "magix.db.load.h"

int main(int argc, char const* argv[])
{
   Filetype::DbLoader loader;
   loader.ParseDirectory(L"temp.db");

#if 0
   using namespace boost::filesystem3;

   path p(argc > 1 ? argv[1] : current_path());

   directory_iterator it(p);
   directory_iterator end;

   for (; it != end; ++it)
   {
      path c = it->path();
      std::string filename = c.filename().string();

      //Cf::Filetype::Id byExt = Cf::Filetype::GetByExtension(filename);
      //Cf::Filetype::Id bySgn = Cf::Filetype::GetBySignaure(filename);

      std::cout << std::setw(40)
                  << filename.c_str() << ":"
                  //<< " " << Cf::Filetype::ToString(byExt) << " (" << byExt << ")"
                  //<< " " << Cf::Filetype::ToString(bySgn) << " (" << bySgn << ")"
                  << std::endl;
   }
#endif

   return 0;
}
