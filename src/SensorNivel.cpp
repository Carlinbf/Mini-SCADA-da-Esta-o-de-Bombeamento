#include "../include/SensorNivel.hpp"
#include <random>

SensorNivel::SensorNivel() 
    : Sensor("NIVEL", "%"), travado(false), valorTravado(0.0) {
    valor = 50.0; 
}

double SensorNivel::ler() {
    if (travado) {
        valor = valorTravado;
        status = "FALHA_TRAVADO";
    } else {
        status = "OK";

        std::random_device rd;
        std::mt19937 gen(rd());
        
        std::uniform_real_distribution<double> dist(-0.5, 0.5);
        double ruido = dist(gen);

        valor += ruido;

        if (valor > 100.0) valor = 100.0;
        if (valor < 0.0) valor = 0.0;
    }
    
    return valor;
}

void SensorNivel::forcarFalhaTravado() {
    travado = true;
    valorTravado = valor; 
    status = "FALHA_TRAVADO";
}

void SensorNivel::normalizarSensor() {
    travado = false;
    status = "OK";
}