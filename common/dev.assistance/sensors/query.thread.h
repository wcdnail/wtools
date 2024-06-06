#pragma once

#include <wtypes.h>

namespace Assist
{
    namespace Sensors
    {
        template <class QT>
        class QueryThread
        {
        public:
            QueryThread(QT& queryRef, DWORD queryTimeout = 10);
            ~QueryThread();

        private:
            HANDLE stop_;
            boost::thread thread_;
            DWORD timeout_;
            QT& queryRef_;

            void QueryProc();
        };

        template <class QT>
        inline QueryThread<QT>::QueryThread(QT& queryRef, DWORD queryTimeout /* = 100 */)
            : stop_(::CreateEvent(NULL, FALSE, FALSE, NULL))
            , thread_(&QueryThread<QT>::QueryProc, boost::ref(*this))
            , timeout_(queryTimeout)
            , queryRef_(queryRef)
        {
        }

        template <class QT>
        inline QueryThread<QT>::~QueryThread()
        {
            ::SetEvent(stop_);
            ::CloseHandle(stop_);
        }

        template <class QT>
        inline void QueryThread<QT>::QueryProc()
        {
            for (;;)
            {
                queryRef_.Query();

                DWORD wr = ::WaitForSingleObject(stop_, timeout_);
                if (WAIT_OBJECT_0 == wr)
                    break;
            }
        }
    }
}