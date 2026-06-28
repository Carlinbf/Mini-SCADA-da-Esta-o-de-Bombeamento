#include "../include/SensorVazao.hpp"
#include <random>

SensorVazao::SensorVazao() : Sensor("VAZAO", "L/min") {
    valor = 15.0; 
}

double SensorVazao::ler() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.3, 0.3);
    
    valor += dist(gen);
    if (valor < 0.0) valor = 0.0;
    
    return valor;
}