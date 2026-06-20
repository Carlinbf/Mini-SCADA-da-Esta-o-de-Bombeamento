#ifndef SENSOR_VAZAO_HPP
#define SENSOR_VAZAO_HPP

#include "Sensor.hpp"

class SensorVazao : public Sensor {
public:
    SensorVazao();
    double ler() override;
};

#endif