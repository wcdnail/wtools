#pragma once

namespace Assist
{
    namespace Sensors
    {
        class Sensor
        {
        public:
            Sensor();
            ~Sensor();

        private:

        private:
            Sensor(Sensor const&);
            Sensor& operator = (Sensor const&);
        };

        inline Sensor::Sensor()
        {
        }

        inline Sensor::~Sensor()
        {
        }
    }
}
