#pragma once

#include "sensor.h"
#include <atltypes.h>
#include <atlgdi.h>
#include <vector>
#include <memory>

namespace Assist
{
    namespace Sensors
    {
        class Facility
        {
        public:
            typedef std::vector<SensorPtr> SensorVector;

            Facility(); /* throw(std::bad_alloc) */
            ~Facility();

            bool Prepare(HWND ownerWindow, CRect const& rc, UINT controlId);

            void Invalidate();

            template <class T>
            T& AddSensor(); /* throw(std::bad_alloc) */

        private:
            class Container;
            friend class Container;

            std::auto_ptr<Container> containerPtr_;
            SensorVector sensors_;
            COLORREF backColor_;
            CFont font_;

            template <class T>
            T& InitAndInsert(T* newSensor); /* throw(std::bad_alloc) */

            void DoPaint(CDCHandle dc, CRect const& rc) const;

        private:
            Facility(Facility const&);
            Facility& operator = (Facility const&);
        };

        template <class T>
        inline T& Facility::AddSensor() /* throw(std::bad_alloc) */
        {
            return InitAndInsert(new T(*this));
        }

        template <class T>
        inline T& Facility::InitAndInsert(T* newSensor) /* throw(std::bad_alloc) */
        {
            sensors_.push_back(SensorPtr(newSensor));
            return (T&)*(sensors_[sensors_.size()-1]);
        }
    }
}
