#include "../include/Valvula.hpp"

// O construtor inicializa a válvula com o ID e começa FECHADA (false)
Valvula::Valvula(std::string idValvula) : id(idValvula), aberta(false) {}

// Abre a válvula para o alívio de pressão
void Valvula::abrir() {
    aberta = true;
}

// Fecha a válvula quando o sistema normaliza
void Valvula::fechar() {
    aberta = false;
}

// Retorna o identificador da válvula
std::string Valvula::getId() const {
    return id;
}

// Retorna se a válvula está aberta ou fechada
bool Valvula::estaAberta() const {
    return aberta;
}