#ifndef _CF_render_conveyor_h__
#define _CF_render_conveyor_h__

#include "render.element.h"
#include <list>
#include <algorithm>

namespace CF  // Common framework
{
    class RenderConveyor
    {
    public:
        typedef RenderElement Element;
        typedef std::list<Element> Conveyor;

        RenderConveyor();
        ~RenderConveyor();

        template <class T>
        void PushBack(T const& object); /* throw (std::bad_alloc) */

        void Render() const;

    private:
        Conveyor conveyor_;

        struct OnRenderElement
        {
            void operator() (Element const& elem)
            {
                elem.Render();
            }
        };

        RenderConveyor(RenderConveyor const&);
        RenderConveyor& operator = (RenderConveyor const&);
    };

    inline RenderConveyor::~RenderConveyor()
    {}

    inline RenderConveyor::RenderConveyor()
    {}

    template <class T>
    inline void RenderConveyor::PushBack(T const& object) /* throw (std::bad_alloc) */
    {
        conveyor_.push_back(RenderElement(object));
    }

    inline void RenderConveyor::Render() const
    {
        std::for_each(conveyor_.begin(), conveyor_.end(), OnRenderElement());
    }
}

#endif // _CF_render_conveyor_h__
