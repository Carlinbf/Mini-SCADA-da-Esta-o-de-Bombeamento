#ifndef SENSOR_FACTORY_HPP
#define SENSOR_FACTORY_HPP

#include "Sensor.hpp"
#include <string>

class SensorFactory {
public:
    // Este método é estático (static) para podermos usá-lo sem precisar criar um objeto da fábrica.
    // Ele recebe o texto (ex: "NIVEL") e retorna um ponteiro genérico do tipo Sensor*.
    static Sensor* criarSensor(const std::string& tipo);
};

#endif