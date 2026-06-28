#ifndef SENSOR_PRESSAO_HPP
#define SENSOR_PRESSAO_HPP

#include "Sensor.hpp"

class SensorPressao : public Sensor {
public:
    SensorPressao();
    double ler() override;
};

#endif
