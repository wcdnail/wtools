#include "stdafx.h"
#include "crt.system.allocator.h"
#include "crt.defs.h"

// TODO: __type_info_root_node mutex
__type_info_node __type_info_root_node = {0};

type_info::type_info(const type_info& rhs) {}
type_info& type_info::operator=(const type_info& rhs) { return *this; }
type_info::~type_info() { _Type_info_dtor(this); }
char const* type_info::raw_name() const { return _M_d_name; }
char const* type_info::name(__type_info_node* __ptype_info_node) const { return _Name_base(this, __ptype_info_node); }

bool type_info::operator == (type_info const& rhs) const
{
    return 0 == ::strcmp(_M_d_name + 1, rhs._M_d_name + 1);
}

bool type_info::operator != (type_info const& rhs) const
{
    return !(this->operator == (rhs));
}

int type_info::before(const type_info& rhs) const
{
    return ::strcmp(rhs._M_d_name + 1, _M_d_name + 1) > 0;
}

void type_info::_Type_info_dtor(type_info* self)
{
    if (self->_M_data != NULL) 
    {
        // TODO: __type_info_root_node mutex lock
        for (__type_info_node* node = __type_info_root_node._Next, *tmp = &__type_info_root_node; node != NULL; node = tmp)
        {
            if(node->_MemPtr == self->_M_data) 
            {
                tmp->_Next = node->_Next;
                MinCrt::RawHeap::_MemFree(node, 0);
                break;
            }

            tmp = node;
        }

        MinCrt::RawHeap::_MemFree(self->_M_data, 0);
        self->_M_data = NULL;
    }
}

static void* UndecorateName(char*, char const* dname, int, unsigned short)
{
    char uname[1024];
    DWORD ulen = MinCrt::Crt().GetDbgHelper().UndecorateSymbolName(dname, uname, _countof(uname), 0);

    if (ulen)
    {
        char* retval = (char*)MinCrt::RawHeap::_MemAlloc(ulen + 1);
        ::strcpy_s(retval, ulen, uname);
        return (void*)retval;
    }

    return NULL;
}

char const* type_info::_Name_base(type_info const* self, __type_info_node* root)
{
    if (self->_M_data == NULL) 
    {
        void *undname = NULL;
        if ((undname = UndecorateName(NULL, (self->_M_d_name) + 1, 0, 0)) == NULL)
            return NULL;

        size_t len = 0;
        for (len = ::strlen((char*)undname); len-->0 && ((char*)undname)[len] == ' ';) 
            ((char *)undname)[len] = '\0';

        if (self->_M_data == NULL) 
        {
            // TODO: __type_info_root_node mutex lock
            __type_info_node *node = (__type_info_node *)MinCrt::RawHeap::_MemAlloc(sizeof(__type_info_node));
            if (node != NULL) 
            {
                if ((((type_info *)self)->_M_data = MinCrt::RawHeap::_MemAlloc(len + 2)) != NULL) 
                {
                    ::strcpy_s((char *)((type_info *)self)->_M_data, len+2, (char *)undname);
                    node->_MemPtr = self->_M_data;

                    node->_Next = root->_Next;
                    root->_Next = node;
                }
                else 
                {
                    MinCrt::RawHeap::_MemFree(node, 0);
                }
            }
        }

        MinCrt::RawHeap::_MemFree(undname, 0);
    }

    return (char const*)self->_M_data;
}

void __clean_type_info_names_internal(__type_info_node* root)
{
    for (__type_info_node* node = root->_Next, *tmp = NULL; node != NULL; node = tmp)
    {
        tmp = node->_Next;

        MinCrt::RawHeap::_MemFree(node->_MemPtr, 0);
        MinCrt::RawHeap::_MemFree(node, 0);
    }
}

extern "C" void __clean_type_info_names()
{
    // TODO: __type_info_root_node mutex lock
    __clean_type_info_names_internal(&__type_info_root_node);
}
