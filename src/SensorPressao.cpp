#include "../include/SensorPressao.hpp"
#include <random>

SensorPressao::SensorPressao()
    : Sensor("PRESSAO", "bar") {
    valor = 1.0;
}

double SensorPressao::ler() {
    status = "OK";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.05, 0.05);

    valor += dist(gen);

    if (valor < 0.0) {
        valor = 0.0;
        status = "ALERTA";
    } else if (valor > 10.0) {
        valor = 10.0;
        status = "ALERTA";
    }

    return valor;
}
