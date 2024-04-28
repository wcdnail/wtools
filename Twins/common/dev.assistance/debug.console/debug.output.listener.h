#ifndef _DH_debug_output_listener_h__
#define _DH_debug_output_listener_h__

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <deque>

namespace Dh
{
	class DebugConsole;

	class DebugOutputListener: boost::noncopyable
	{
	public:
		DebugOutputListener(DebugConsole const& owner);
		~DebugOutputListener();

		bool Start();
		bool Stop();

	private:
		DebugConsole const& owner_;
		HRESULT rv_;
		HANDLE ackEvent_;
		HANDLE readyEvent_;
		HANDLE sharedFile_;
		PVOID sharedMem_;
		HANDLE routine_;
		unsigned routineId_;
		HANDLE threadStarted_;
		HANDLE threadStop_;

		bool Init();
		static void FreeResources(HANDLE& ackEvent, HANDLE& readyEvent, HANDLE& sharedFile, PVOID& sharedMem);
		static unsigned __stdcall CaptureProc(void*);
		void CaptureProc();
	};
}

#endif // _DH_debug_output_listener_h__
