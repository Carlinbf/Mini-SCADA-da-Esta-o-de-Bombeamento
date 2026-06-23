#ifndef VALVULA_HPP
#define VALVULA_HPP

#include <string>

class Valvula {
private:
    std::string id;
    bool aberta;

public: 

    Valvula(std::string idValvula);

    void abrir();
    void fechar();

    std::string getId() const;
    bool estaAberta() const;
};

#endif