#ifndef VALVULA_HPP
#define VALVULA_HPP

#include <string>

class Valvula {
private:
    std::string id;
    bool aberta;

public:
    // Construtor
    Valvula(std::string idValvula);

    // Métodos de atuação
    void abrir();
    void fechar();

    // Métodos de acesso (Getters)
    std::string getId() const;
    bool estaAberta() const;
};

#endif