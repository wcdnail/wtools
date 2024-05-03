#include "stdafx.h"
#include "test.frame.h"
#include <dh.tracing.h>
#include <info/runtime.information.h>

CAppModule _Module;

int Run(PTSTR, int show)
{
    Dh::Printf("%s\n", Runtime::Info().SimpleReport().c_str());

    CMessageLoop loop;
    _Module.AddMessageLoop(&loop);

    int rv = ERROR_CANCELLED;
    Test::Frame testFrame;
    if (NULL != testFrame.Create(NULL))
    {
        testFrame.ShowWindow(show);
        rv = loop.Run();
    }

    _Module.RemoveMessageLoop();
    return rv;
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, PSTR, int show)
{
    int rv = -1;

    __try
    {
        HRESULT hr = ::OleInitialize(NULL);
        if (FAILED(hr))
        {
            ::ShowErrorBox(hr, L"OleInitialize");
            return (int)hr;
        }
    
        ::DefWindowProc(NULL, 0, 0, 0L);
        AtlInitCommonControls(ICC_BAR_CLASSES);
    
        hr = _Module.Init(NULL, inst);
        if (FAILED(hr))
            ::ShowErrorBox(hr, L"CAppModule.Init");
        else
            rv = ::Run(NULL, show);
    }
    __finally
    {
        _Module.Term();
        ::OleUninitialize();
    }

    return rv;
}

/*
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    using namespace Fv;

#if 0
  generic_category :                        | system_category : 
  0 - No error                              |  0 - The operation completed successfully
  1 - Operation not permitted               |  1 - Incorrect function
  2 - No such file or directory             |  2 - The system cannot find the file specified
  3 - No such process                       |  3 - The system cannot find the path specified
  4 - Interrupted function call             |  4 - The system cannot open the file
  5 - Input/output error                    |  5 - Access is denied
  6 - No such device or address             |  6 - The handle is invalid
  7 - Arg list too long                     |  7 - The storage control blocks were destroyed
  8 - Exec format error                     |  8 - Not enough storage is available to process this command
  9 - Bad file descriptor                   |  9 - The storage control block address is invalid
 10 - No child processes                    |  10 - The environment is incorrect
 11 - Resource temporarily unavailable      |  11 - An attempt was made to load a program with an incorrect format
 12 - Not enough space                      |  12 - The access code is invalid
 13 - Permission denied                     |  13 - The data is invalid
 14 - Bad address                           |  14 - Not enough storage is available to complete this operation
 15 - Unknown error                         |  15 - The system cannot find the drive specified
 16 - Resource device                       |  16 - The directory cannot be removed
 17 - File exists                           |  17 - The system cannot move the file to a different disk drive
 18 - Improper link                         |  18 - There are no more files
 19 - No such device                        |  19 - The media is write protected
 20 - Not a directory                       |  20 - The system cannot find the device specified
 21 - Is a directory                        |  21 - The device is not ready
 22 - Invalid argument                      |  22 - The device does not recognize the command
 23 - Too many open files in system         |  23 - Data error (cyclic redundancy check)
 24 - Too many open files                   |  24 - The program issued a command but the command length is incorrect
 25 - Inappropriate I/O control operation   |  25 - The drive cannot locate a specific area or track on the disk
 26 - Unknown error                         |  26 - The specified disk or diskette cannot be accessed
 27 - File too large                        |  27 - The drive cannot find the sector requested
 28 - No space left on device               |  28 - The printer is out of paper
 29 - Invalid seek                          |  29 - The system cannot write to the specified device
 30 - Read-only file system                 |  30 - The system cannot read from the specified device
 31 - Too many links                        |  31 - A device attached to the system is not functioning
 32 - Broken pipe                           |  32 - The process cannot access the file because it is being used by another process
 33 - Domain error                          |  33 - The process cannot access the file because another process has locked a portion of the file
 34 - Result too large                      |  34 - The wrong diskette is in the drive.
 35 - Unknown error                         |  Insert %2 (Volume Serial Number: %3) into drive %1
 36 - Resource deadlock avoided             |  35 - Unknown error
 37 - Unknown error                         |  36 - Too many files opened for sharing
 38 - Filename too long                     |  37 - Unknown error
 39 - No locks available                    |  38 - Reached the end of the file
 40 - Function not implemented              |  39 - The disk is full
 41 - Directory not empty                   | 
 42 - Illegal byte sequence                 | 
#endif // 0


#if 0
    {
        ErrorCode ec(0, boost::system::system_category());

        for (int i=0; i<50; i++)
        {
            ec.assign(i, ec.category());
            Dh::Printf("%3d - %s\n", i, ec.message().c_str());
        }
    }
#endif

    return 0;
}
*/

