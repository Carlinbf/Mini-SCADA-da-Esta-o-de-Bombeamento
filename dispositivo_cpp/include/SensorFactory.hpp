#ifndef SENSOR_FACTORY_HPP
#define SENSOR_FACTORY_HPP

#include "Sensor.hpp"
#include <string>

class SensorFactory {
public:
    
    static Sensor* criarSensor(const std::string& tipo);
};

#endif