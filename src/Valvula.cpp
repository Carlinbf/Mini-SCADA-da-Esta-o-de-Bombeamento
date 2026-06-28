#include "../include/Valvula.hpp"

Valvula::Valvula(std::string idValvula) : id(idValvula), aberta(false) {}

void Valvula::abrir() {
    aberta = true;
}

void Valvula::fechar() {
    aberta = false;
}

std::string Valvula::getId() const {
    return id;
}

bool Valvula::estaAberta() const {
    return aberta;
}