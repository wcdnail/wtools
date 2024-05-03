#pragma once

#include <xmemory>
#include <boost/noncopyable.hpp>

namespace WTL { class CMessageLoop; }

namespace Sqlbro
{
    class AppWindow: boost::noncopyable
    {
    public:
        AppWindow(CMessageLoop& loop);
        ~AppWindow();

        void OnRun();

    private:
        class Implementation;
        std::auto_ptr<Implementation> Impl;
    };
}
