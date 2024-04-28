#include "stdafx.h"

namespace std
{
    exception::~exception()
    {
        _Tidy();
    }

    exception::exception(char const* const& message)
        : _Mywhat(message)
        , _Mydofree(false)
    {}

    exception::exception(char const* const& message, int)
        : _Mywhat(message)
        , _Mydofree(false)
    {}

    exception::exception(exception const& rhs)
        : _Mywhat(NULL)
        , _Mydofree(false)
    {
        *this = rhs;
    }

    exception& exception::operator = (exception const& rhs)
    {
        if (this != &rhs)
        {
            _Tidy();

            if (rhs._Mydofree)
                _Copy_str(rhs._Mywhat);
            else
                _Mywhat = rhs._Mywhat;
        }

        return *this;
    }

    void exception::_Copy_str(char const* source)
    {
        if (source != NULL)
        {
            const size_t bs = ::strlen(source) + 1;

            _Mywhat = static_cast<char *>(malloc(bs));

            if (_Mywhat != NULL)
            {
                ::strcpy_s(const_cast<char *>(_Mywhat), bs, source);
                _Mydofree = true;
            }
        }
    }

    void exception::_Tidy()
    {
        if (_Mydofree)
        {
            free(const_cast<char *>(_Mywhat));
        }

        _Mywhat = NULL;
        _Mydofree = false;
    }

    char const* exception::what() const
    {
        return _Mywhat;
    }
}
