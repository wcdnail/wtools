#include <atlbase.h>
#include <atlstr.h>
#include <iostream>

template <size_t size>
static ATL::CStringA ExtractSerial(BYTE (&buffer)[size])
{
    ATL::CStringA result;

    size_t i = 0x34;
    size_t c = 0;
    for (; i < size, c < 25;)
    {
        result.AppendChar('0' + buffer[i]);
        ++i;
        ++c;
    }

    return result;
}

template <size_t size>
static ATL::CStringA DecodeProductKey(BYTE (&digitalProductId)[size])
{
    const int keyStartIndex = 52;
    const int keyEndIndex = keyStartIndex + 15;
    static const char digits[] = { 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'M', 'P', 'Q', 'R', 'T', 'V', 'W', 'X', 'Y', '2', '3', '4', '6', '7', '8', '9' };
    const int decodeLength = 29;
    const int decodeStringLength = 15;
    char decodedChars[decodeLength + 1] = {0};

    BYTE hexPid[keyEndIndex - keyStartIndex + 1] = {0};
    int j = 0;
    for (int i = keyStartIndex; i <= keyEndIndex; i++)
        hexPid[j++] = digitalProductId[i];

    for (int i = decodeLength - 1; i >= 0; i--)
    {
        // Every sixth char is a separator.
        if ((i + 1) % 6 == 0)
        {
            decodedChars[i] = '-';
        }
        else
        {
            // Do the actual decoding.
            int digitMapIndex = 0;
            for (int j = decodeStringLength - 1; j >= 0; j--)
            {
                unsigned byteValue = (digitMapIndex << 8) | (BYTE)hexPid[j];
                hexPid[j] = (BYTE)(byteValue / 24);
                digitMapIndex = byteValue % 24;
                decodedChars[i] = digits[digitMapIndex];
            }
        }
    }

    return ATL::CStringA(decodedChars);
} 

int main()
{
    LONG rv = 0;
    BYTE buffer[4096] = {0};
    ULONG len = _countof(buffer);
    bool underWow64 = false;

    typedef BOOL (WINAPI *IW64P)(HANDLE, PBOOL);
    IW64P IsWow64ProcessImpl = (IW64P)::GetProcAddress(::GetModuleHandle(_T("KERNEL32")), "IsWow64Process");
    if (IsWow64ProcessImpl)
    {
        BOOL dmy = FALSE;
        underWow64 = TRUE == IsWow64ProcessImpl(::GetCurrentProcess(), &dmy);
    }

    ATL::CRegKey key;
    rv = key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), KEY_READ | (underWow64 ? KEY_WOW64_64KEY : 0));
    if (ERROR_SUCCESS == rv)
    {
        rv = key.QueryBinaryValue(_T("DigitalProductId"), buffer, &len);
        if (ERROR_SUCCESS == rv)
        {
            ATL::CStringA serial = DecodeProductKey(buffer);

            char productName[256] = {0};
            DWORD type = REG_SZ;
            DWORD plen = _countof(productName);

            rv = ::RegQueryValueExA(key, "ProductName", NULL, &type, reinterpret_cast<LPBYTE>(productName), &plen);

            std::cout << "Name          : " << productName << std::endl
                      << "ID            : " << (char const*)&buffer[8] << std::endl
                      << "Serial number : " << (char const*)serial << std::endl
                      
                      << std::endl;
        }
    }

    return rv;
}
