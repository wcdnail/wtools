#ifndef _CF_trect_h__
#define _CF_trect_h__

#include "tpoint.h"

namespace cf // Common framework
{
    struct put_at
    {
        enum
        {
            left    = 0x00,
            top     = 0x01,
            right   = 0x02,
            bottom  = 0x04,
            xcenter = 0x08,
            ycenter = 0x10,
            fill    = 0x20,
            center  = xcenter | ycenter
        };

        static bool in(unsigned long long flag, unsigned long long val)
        {
            return (flag & val) != 0;
        }
    };

	template <class T>
	class rect
	{
	public:
		template <class Y>
		rect(rect<Y> const& other)
			: start_(other.start())
			, size_(other.size()) 
		{}

		rect(): start_(0, 0), size_(0, 0) {}

		rect(T a, T b, T c, T d): start_(a, b), size_(c, d) {}

		rect(T c, T d)
			: start_(0, 0)
			, size_(c, d)
		{
		}

		rect(point<T> const& a, point<T> const& b): start_(a), size_(b) {}
		rect(point<T> const& a, T c, T d): start_(a), size_(c, d) {}
		rect(T a, T b, point<T> const& s): start_(a, b), size_(s) {}

		rect(rect<T> const& it, rect<T> const& external, unsigned long long how)
			: start_(it.start())
			, size_(it.size())
		{
			put_into(external, how);
		}

		rect(T c, T d, rect<T> const& external, unsigned long long how)
			: start_(0, 0)
			, size_(c, d)
		{
			put_into(external, how);
		}

		~rect() {}

		void reset(T a, T b, T c, T d)
		{
			start_.reset(a, b);
			size_.reset(c, d);
		}

		void put_into(rect<T> const& external, unsigned long long how)
		{
			if (put_at::in(how, put_at::fill))
			{
				*this = external;
			}
			else
			{
				T x = external.left();
				T y = external.top();

				if (put_at::in(how, put_at::right))
					x = external.right() - width();

				if (put_at::in(how, put_at::bottom))
					y = external.bottom() - height();

				if (put_at::in(how, put_at::xcenter))
					x += (external.width() - width())/2;

				if (put_at::in(how, put_at::ycenter))
					y += (external.height() - height())/2;

				start_.reset(x, y);
			}
		}

		void shrink(T x, T y)
		{
			start_.set_x(start_.x() + x);
			start_.set_y(start_.y() + y);
			size_.set_x(size_.x() - x * 2);
			size_.set_y(size_.y() - y * 2);
		}

		template <class Y>
		rect<T> intersection(rect<Y> const& arg)
		{
			return rect<T>(
				(start_.x() > (T)arg.start_.x() ? start_.x() : (T)arg.start_.x()), 
				(start_.y() > (T)arg.start_.y() ? start_.y() : (T)arg.start_.y()), 
				( size_.x() <  (T)arg.size_.x() ?  size_.x() : (T)arg.size_.x()), 
				( size_.y() <  (T)arg.size_.y() ?  size_.y() : (T)arg.size_.y())
				);
		}

		T set_left(T val) { return start_.set_x(val); }
		T set_top(T val) { return start_.set_y(val); }
		T set_right(T val) { return size_.set_x(val - start_.x()); }
		T set_bottom(T val) { return size_.set_y(val - start_.y()); }
		T set_width(T val) { return size_.set_x(val); }
		T set_height(T val) { return size_.set_y(val); }

		T& left() { return start_.x(); }
		T& top() { return start_.y(); }
		T& width() { return size_.x(); }
		T& height() { return size_.y(); }

		T const& left() const { return start_.x(); }
		T const& top() const { return start_.y(); }
		T const& width() const { return size_.x(); }
		T const& height() const { return size_.y(); }

		point<T>& start() { return start_; }
		point<T>& size() { return size_; }

		point<T> const& start() const { return start_; }
		point<T> const& size() const { return size_; }

		T right() const { return start_.x() + size_.x(); }
		T bottom() const { return start_.y() + size_.y(); }

		template <class Y>
		rect& operator += (rect<Y> const& op)
		{
#pragma message(_TODO("Implementation"))
			return *this;
		}

		template <class Y>
		rect& operator -= (rect<Y> const& op)
		{
#pragma message(_TODO("Implementation"))
			return *this;
		}

	private:
		point<T> start_;
		point<T> size_;
	};

	template <class X, class Y>
	inline point<X> operator + (point<X> const& x, point<Y> const& y)
	{
		point<X> result(x);
		result += y;
		return result;
	}

	template <class X, class Y>
	inline point<X> operator - (point<X> const& x, point<Y> const& y)
	{
		point<X> result(x);
		result -= y;
		return result;
	}

	template <class X, class Y>
	inline rect<X> operator + (rect<X> const& x, rect<Y> const& y)
	{
		rect<X> result(x);
		result += y;
		return result;
	}

	template <class X, class Y>
	inline rect<X> operator - (rect<X> const& x, rect<Y> const& y)
	{
		rect<X> result(x);
		result -= y;
		return result;
	}
}

#endif // _CF_trect_h__
