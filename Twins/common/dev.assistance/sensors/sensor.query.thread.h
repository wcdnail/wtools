#pragma once

#include "sensor.h"
#include "query.thread.h"
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

namespace Assist
{
    namespace Sensors
    {
        Sensor::StatusThread& SensorStatusThread();

        class Sensor::StatusThread: boost::noncopyable
        {
        public:
            typedef std::vector<Sensor*> SensorVector;

            void AddSensor(Sensor* sensor);
            void DelSensor(Sensor* sensor);

        private:
            friend Sensor::StatusThread& SensorStatusThread();
            friend QueryThread<StatusThread>;

            QueryThread<StatusThread> qthread_;
            boost::mutex sensorsMx_;
            SensorVector sensors_;

            StatusThread();
            ~StatusThread();

            void Query();
        };
    }
}
