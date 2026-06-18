#ifndef SENSOR_NIVEL_HPP
#define SENSOR_NIVEL_HPP

#include "Sensor.hpp"

class SensorNivel : public Sensor {
private:
    bool travado;         
    double valorTravado;  

public:
    SensorNivel();

    double ler() override;

    void forcarFalhaTravado();
    void normalizarSensor();
};

#endif