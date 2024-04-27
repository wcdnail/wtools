#ifndef _CF_render_element_h__
#define _CF_render_element_h__

namespace CF  // Common framework
{
    class RenderElement
    {
    public:
        template <class T>
        RenderElement(T const& source); /* throw (std::bad_alloc) */
        ~RenderElement();

        RenderElement(RenderElement const&);
        RenderElement& operator = (RenderElement const&);

        void Render() const;

    private:
        struct IRef
        {
            virtual ~IRef();
            virtual void Render() const = 0;
        };

        template <class T>
        struct Ref: public IRef
        {
            Ref(T const& object);
            virtual void Render() const;

            T const& ref_;
        };

        std::shared_ptr<IRef> ref_;
    };

    inline RenderElement::IRef::~IRef()
    {}

    inline void RenderElement::IRef::Render() const
    {}

    template <class T>
    inline RenderElement::Ref<T>::Ref(T const& object)
        : ref_(object)
    {}

    template <class T>
    inline void RenderElement::Ref<T>::Render() const
    {
        ref_.Render();
    }

    inline RenderElement::~RenderElement() 
    {}

    template <class T>
    inline RenderElement::RenderElement(T const& source) /* throw (std::bad_alloc) */
        : ref_(new Ref<T>(source))
    {}

    inline RenderElement::RenderElement(RenderElement const& rhs)
        : ref_(rhs.ref_)
    {}

    inline RenderElement& RenderElement::operator = (RenderElement const& rhs)
    {
        ref_ = rhs.ref_;
        return *this;
    }

    inline void RenderElement::Render() const
    {
        ref_->Render();
    }
}

#endif // _CF_render_element_h__
