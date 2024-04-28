#ifndef _CF_tpoint_h__
#define _CF_tpoint_h__

namespace cf // Common framework
{
    template <class T>
    class point
    {
    public:
        template <class y>
        point(point<y> const& other)
            : x_((T)other.x()), y_((T)other.y())
        {}

        point(): x_(0), y_(0) {}
        point(T a, T b): x_(a), y_(b) {}

        ~point() {}

        void reset(T a, T b)
        {
            x_ = a, y_ = b;
        }

        T set_x(T val) 
        { 
            T rv = x_; 
            x_ = val;
            return rv;
        }

        T set_y(T val) 
        { 
            T rv = y_; 
            y_ = val;
            return rv;
        }

        T& x() { return x_; }
        T& y() { return y_; }

        T const& x() const { return x_; }
        T const& y() const { return y_; }

        template <class y>
        point& operator += (point<y> const& op)
        {
            x_ += op.x_;
            y_ += op.y_;
            return *this;
        }

        template <class y>
        point& operator -= (point<y> const& op)
        {
            x_ -= op.x_;
            y_ -= op.y_;
            return *this;
        }

    private:
        T x_;
        T y_;
    };
}

#endif // _CF_tpoint_h__
