#ifndef SENSOR_TEMPERATURA_HPP
#define SENSOR_TEMPERATURA_HPP

#include "Sensor.hpp"

class SensorTemperatura : public Sensor {
public:
    SensorTemperatura();
    double ler() override;
};

#endif