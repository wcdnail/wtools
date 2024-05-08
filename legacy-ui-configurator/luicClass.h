#pragma once

enum EClass_Index : int
{
    /*                    */ CLASS_Invalid = -1,
    /*                    */ CLASS_Count
};

struct Class
{
    ~Class();
    Class();

    int& operator[](int index);
    int operator[](int index) const;

private:
    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};
