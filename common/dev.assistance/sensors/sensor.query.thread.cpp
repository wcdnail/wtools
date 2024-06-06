#include "stdafx.h"
#include "sensor.query.thread.h"

namespace Assist
{
    namespace Sensors
    {
        Sensor::StatusThread::StatusThread() 
            : qthread_(*this, 100) 
            , sensorsMx_()
            , sensors_()
        {
        }

        Sensor::StatusThread::~StatusThread() 
        {
        }

        void Sensor::StatusThread::AddSensor(Sensor* sensor)
        {
            boost::mutex::scoped_lock lk(sensorsMx_);
            sensors_.push_back(sensor);
        }

        void Sensor::StatusThread::DelSensor(Sensor* sensor)
        {
            boost::mutex::scoped_lock lk(sensorsMx_);
            SensorVector::iterator it = std::find(sensors_.begin(), sensors_.end(), sensor);
            if (it != sensors_.end())
                sensors_.erase(it);
        }

        void Sensor::StatusThread::Query() 
        {
            if (sensors_.empty())
                return ;

            boost::mutex::scoped_lock lk(sensorsMx_);
            Sensor* lsensorPtr = NULL;
            for (SensorVector::iterator it = sensors_.begin(); it != sensors_.end(); ++it)
            {
                lsensorPtr = (*it);
                lsensorPtr->Query();
            }

            if (NULL != lsensorPtr)
                lsensorPtr->Invalidate();
        }

        Sensor::StatusThread& SensorStatusThread()
        {
            static Sensor::StatusThread inst;
            return inst;
        }
    }
}
