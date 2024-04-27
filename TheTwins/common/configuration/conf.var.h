#pragma once

#include "conf.types.h"

namespace Config
{
    class Var
    {
    public:
        ~Var();

        Var(Var const& rhs);
        Var& operator = (Var const& rhs);
        void Swap(Var& rhs);

        template <class T>
        Var& operator = (T& x);

    private:
        friend class Section;

        template <class T>
        Var(T& x);

        template <class T>
        T& Get();

    private:
        class IRefference: Noncopyable
        {
        protected:
            IRefference();
            virtual ~IRefference();
        };

        template <class T>
        class Refference: public IRefference
        {
        public:
            T& Ref_;
            Refference(T& object): Ref_(object) {}
        };

        boost::shared_ptr<IRefference> RefPointee_;
    };

    template <class T>
    inline Var::Var(T& x): RefPointee_(new Refference<T>(x)) {}

    inline Var::Var(Var const& rhs): RefPointee_(rhs.RefPointee_) {}
    inline void Var::Swap(Var& rhs) { RefPointee_.swap(rhs.RefPointee_); }

    inline Var& Var::operator = (Var const& rhs)
    {
        Var temp(rhs);
        Swap(temp);
        return *this;
    }

    template <class T>
    inline Var& Var::operator = (T& x)
    {
        RefPointee_.reset(new Refference<T>(x));
        return *this;
    }

    template <class T>
    inline T& Var::Get() { return static_cast<Refference<T>*>(RefPointee_.get())->Ref_; }
}
