#include "../include/Bomba.hpp"

Bomba::Bomba(std::string idBomba, double vazao)
    : id(idBomba), ligada(false), vazaoNominal(vazao) {}

void Bomba::ligar() {
    ligada = true;
}

void Bomba::desligar() {
    ligada = false;
