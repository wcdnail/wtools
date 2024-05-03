#pragma once

namespace Slist
{
    struct slist
    {
        int data;
        slist* next;
    };

    inline slist* reverse(slist* head)
    {
        slist* next = NULL;
        slist* result = head;

        while (head)
        {
            slist* prev = head->next;
            head->next = next;
            result = head;
            next = head;
            head = prev;
        }

        return result;
    }

    inline void puts(slist const* head)
    {
        while (head)
        {
            ATLTRACE("%d ", head->data);
            head = head->next;
        }

        ATLTRACE("\n");
    }

    inline void test()
    {
        slist l9 = { 0, NULL };
        slist l8 = { 9, &l9 };
        slist l7 = { 8, &l8 };
        slist l6 = { 7, &l7 };
        slist l5 = { 6, &l6 };
        slist l4 = { 5, &l5 };
        slist l3 = { 4, &l4 };
        slist l2 = { 3, &l3 };
        slist l1 = { 2, &l2 };
        slist l0 = { 1, &l1 };

        puts(&l0);
        puts(reverse(&l0));
    }
}
