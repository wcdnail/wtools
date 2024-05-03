#include "stdafx.h"
#include "run.external.h"
#include "console.view.h"
#include "filesystem.h"
#include <dh.tracing.h>
#include <dh.timer.h>
#include <locale.helper.h>

#define BOOST_PROCESS_WINDOWS_USE_NAMED_PIPE
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp> 
#include <boost/process.hpp>
#include <boost/array.hpp> 
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <atlstr.h>
#include <atlconv.h>

namespace Shelltastix
{
    namespace std_redirect
    {
#if defined(BOOST_POSIX_API) 

        static const bool native_posix = true;

        typedef boost::asio::posix::stream_descriptor stream_handle; 
        typedef boost::process::context proc_context;
        typedef boost::process::child child_process;

#elif defined(BOOST_WINDOWS_API) 

        static const bool native_posix = false;

        typedef boost::asio::windows::stream_handle stream_handle;
        typedef boost::process::win32_context proc_context;
        typedef boost::process::win32_child child_process;

#endif 

        class capture_impl: boost::noncopyable
        {
        public:
            typedef boost::array<char, 4096> IoBuffer;

            capture_impl(Console::View& con);
            ~capture_impl();

            void run(boost::process::child& proc);

        private:
            boost::asio::io_service io_; 
            stream_handle handle_;
            IoBuffer buffer_; 
            Console::View& con_;

            void start();
            void on_finish(boost::system::error_code const& ec, std::size_t transferred);
        };

        capture_impl::capture_impl(Console::View& con)
            : io_() 
            , handle_(io_)
            , buffer_()
            , con_(con)
        {}

        capture_impl::~capture_impl()
        {}

        void capture_impl::run(boost::process::child& proc)
        {
            boost::process::pistream &is = proc.get_stdout(); 
            handle_.assign(is.handle().release()); 

            start();
            io_.run(); 
        }

        void capture_impl::start()
        {
            handle_.async_read_some(boost::asio::buffer(buffer_), 
                boost::bind(&capture_impl::on_finish, this
                          , boost::asio::placeholders::error
                          , boost::asio::placeholders::bytes_transferred
                           )); 
        }

        void capture_impl::on_finish(boost::system::error_code const& ec, std::size_t transferred)
        {
            if (!ec) 
            { 
                CStringA buffer(buffer_.data(), (int)transferred);  // TODO: get display codepage
                buffer.OemToAnsi();                                 // TODO: format to display codepage
                CStringW to_display(buffer);
                con_.WriteRaw((PCWSTR)to_display);

                start();
            } 
        }
    }

    bool RunExternal(Console::View& con, Filesystem& fs, std::wstring const& name, std::wstring const& arguments)
    {
        Dh::Timer timer;
        HRESULT hr = 0;
        std::wstring cpath = fs.ResolveExecutablePath(name, std_redirect::native_posix);
        if (cpath.empty())
        {
            con.WriteLine(_LS("'%s' - Неизвестная команда."), name.c_str());
            return false;
        }

        try
        {
            std::string pathname(CW2A(cpath.c_str()).m_psz);

            std::vector<std::string> vargs; 
            vargs.push_back(CW2A(arguments.c_str()).m_psz);

            std_redirect::proc_context ctx; 
            ctx.stdout_behavior = boost::process::capture_stream();
            ctx.environment = boost::process::self::get_environment(); 

#ifdef BOOST_WINDOWS_API
            STARTUPINFOA startup = {0};
            startup.cb = sizeof(startup);

            // FIXME: Показывать гуйню или нет - ВОТ в чем вопрос!
            startup.dwFlags |= STARTF_USESHOWWINDOW;
            startup.wShowWindow = SW_SHOWMINIMIZED;

            ctx.startupinfo = &startup;
            std_redirect::child_process proc = boost::process::win32_launch(pathname, vargs, ctx); 
#else
            std_redirect::child_process proc = boost::process::launch(pathname, vargs, ctx); 
#endif

            std_redirect::capture_impl capturer(con);
            capturer.run(proc);
            con.NewLine();

#ifdef BOOST_WINDOWS_API
            DWORD exitcode = 0;
            if (::GetExitCodeProcess(proc.get_handle(), &exitcode))
                hr = (HRESULT)exitcode;
#endif
        }
        catch (boost::exception const& ex)
        {
            hr = ::GetLastError();
            con.WriteLine("%S: \n%s", _LS("Внутренняя ошибка"), boost::diagnostic_information(ex).c_str());
        }
        catch (std::exception const& ex)
        {
            hr = ::GetLastError();
            con.WriteLine("%S: '%s'", _LS("Внутренняя ошибка"), ex.what());
        }

        fs.WriteResult(_LS("запуск"), cpath, timer, hr);
        return 0 == hr;
    }
}
