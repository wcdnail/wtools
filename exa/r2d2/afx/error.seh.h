#ifndef aw_error_seh_h__
#define aw_error_seh_h__

#include <string>
#include "use.windows.h"
#include "report.bugs.h"

class SEHRecord
{
public:
    SEHRecord(EXCEPTION_POINTERS *info) throw()
        : code_(info->ExceptionRecord->ExceptionCode)
    {
    }

    char const* Info() const throw()
    {
        try
        {
            std::stringstream temp;

            temp << CodeString(code_) 
                 << " (0x" << std::hex << code_ << ").\r\n"
                 << DescribeCodeString(code_);

            info_ = temp.str();
        }
        catch(...) {}

        return info_.c_str(); 
    }

private:
    static char const* CodeString(DWORD code)
    {
        switch (code)
        {
        case EXCEPTION_ACCESS_VIOLATION         : return "Access violation";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    : return "Array bounds exceeded";
        case EXCEPTION_BREAKPOINT               : return "Breakpoint";
        case EXCEPTION_DATATYPE_MISALIGNMENT    : return "Data type misalignment";
        case EXCEPTION_FLT_DENORMAL_OPERAND     : return "Float denormal operand";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO       : return "Float divide by zero";
        case EXCEPTION_FLT_INEXACT_RESULT       : return "Float inexact result";
        case EXCEPTION_FLT_INVALID_OPERATION    : return "Float invalid operation";
        case EXCEPTION_FLT_OVERFLOW             : return "Float overflow";
        case EXCEPTION_FLT_STACK_CHECK          : return "Float stack_check";
        case EXCEPTION_FLT_UNDERFLOW            : return "Float underflow";
        case EXCEPTION_ILLEGAL_INSTRUCTION      : return "Illegal instruction";
        case EXCEPTION_IN_PAGE_ERROR            : return "In page_error";
        case EXCEPTION_INT_DIVIDE_BY_ZERO       : return "Integer divide by zero";
        case EXCEPTION_INT_OVERFLOW             : return "Integer overflow";
        case EXCEPTION_INVALID_DISPOSITION      : return "Invalid disposition";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION : return "Noncontinuable exception";
        case EXCEPTION_PRIV_INSTRUCTION         : return "Privilege instruction";
        case EXCEPTION_SINGLE_STEP              : return "Single step";
        case EXCEPTION_STACK_OVERFLOW           : return "Stack overflow";
        }

        return "Unknown exception.";
    }

    static char const* DescribeCodeString(DWORD code)
    {
        switch (code)
        {
        case EXCEPTION_ACCESS_VIOLATION         : return "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    : return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
        case EXCEPTION_BREAKPOINT               : return "A breakpoint was encountered.";
        case EXCEPTION_DATATYPE_MISALIGNMENT    : return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
        case EXCEPTION_FLT_DENORMAL_OPERAND     : return "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO       : return "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
        case EXCEPTION_FLT_INEXACT_RESULT       : return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
        case EXCEPTION_FLT_INVALID_OPERATION    : return "This exception represents any floating-point exception not included in this list.";
        case EXCEPTION_FLT_OVERFLOW             : return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
        case EXCEPTION_FLT_STACK_CHECK          : return "The stack overflowed or underflowed as the result of a floating-point operation.";
        case EXCEPTION_FLT_UNDERFLOW            : return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
        case EXCEPTION_ILLEGAL_INSTRUCTION      : return "The thread tried to execute an invalid instruction.";
        case EXCEPTION_IN_PAGE_ERROR            : return "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.";
        case EXCEPTION_INT_DIVIDE_BY_ZERO       : return "The thread tried to divide an integer value by an integer divisor of zero.";
        case EXCEPTION_INT_OVERFLOW             : return "The result of an integer operation caused a carry out of the most significant bit of the result.";
        case EXCEPTION_INVALID_DISPOSITION      : return "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION : return "The thread tried to continue execution after a noncontinuable exception occurred.";
        case EXCEPTION_PRIV_INSTRUCTION         : return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
        case EXCEPTION_SINGLE_STEP              : return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
        case EXCEPTION_STACK_OVERFLOW           : return "The thread used up its stack.";
        }

        return "Invalid exception code.";
    }

    DWORD code_;
    mutable std::string info_;
};

template <>
char const* BugReport<SEHRecord>::GetMessage() const { return info_.Info(); }

#endif // aw_error_seh_h__
