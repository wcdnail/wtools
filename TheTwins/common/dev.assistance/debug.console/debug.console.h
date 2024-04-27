#ifndef _DH_debug_console_h__
#define _DH_debug_console_h__

#include <memory>

#pragma warning(disable: 4251) // stuff `xxx` needs to have dll-interface...

#ifndef _PUBLIC
#define _PUBLIC
#endif

namespace Dh
{
	class DebugOutputListener;
	class BasicDebugConsole;

    class _PUBLIC DebugConsole
    {
    public:
        static DebugConsole& Instance(); /* throw (std::bad_alloc) */

        void ReceiveDebugOutput(bool on) const;
        void ReceiveStdOutput(bool on) const;

		void SetParameters(int cx, int cy, int align, int fontSize, char const* fontName) const;
        void SetAutoScroll(bool on);

        void Show() const;
        void Hide() const;
		void Clean() const;

        void SetTitleText(char const*) const;
        void SetTitleText(wchar_t const*) const;

        void Puts(char const*) const;
        void Puts(wchar_t const*) const;

        void AskPathAndSave() const;
        void Save(char const* filePathName) const;

        void Destroy() const;

	protected:
		DebugConsole(BasicDebugConsole*);
		~DebugConsole();

	private:
        std::auto_ptr<BasicDebugConsole> impl_;

		DebugConsole(); /* throw (std::bad_alloc) */
        DebugConsole(DebugConsole const&);
        DebugConsole& operator = (DebugConsole const&);

		friend class DebugOutputListener;
		void* GetConsoleSystemHandle() const;
    };
}

#endif // _DH_debug_console_h__
