#include "../include/SensorTemperatura.hpp"
#include <random>

SensorTemperatura::SensorTemperatura() : Sensor("TEMPERATURA", "C") {
    valor = 25.0; // Temperatura ambiente base
}

double SensorTemperatura::ler() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.1, 0.1); // Ruído térmico
    
    valor += dist(gen);
    
    return valor;
}