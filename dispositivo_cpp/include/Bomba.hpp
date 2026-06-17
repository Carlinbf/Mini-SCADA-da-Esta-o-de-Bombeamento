#ifndef BOMBA_HPP
#define BOMBA_HPP

#include <string>

class Bomba {
private:
    std::string id;
    bool ligada;
    double vazaoNominal;

public:
    Bomba(std::string idBomba, double vazao);

    void ligar();
    void desligar();

    std::string getId() const { return id; }
    bool estaLigada() const { return ligada; }
    double getVazaoNominal() const { return vazaoNominal; }
    
    double getVazaoAtual() const { return ligada ? vazaoNominal : 0.0; }
};

#endif