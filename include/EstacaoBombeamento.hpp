#ifndef ESTACAO_BOMBEAMENTO_HPP
#define ESTACAO_BOMBEAMENTO_HPP

#include "Sensor.hpp"
#include "Bomba.hpp"
#include "Valvula.hpp"
#include <vector>
#include <string>

class EstacaoBombeamento {
private:
    std::vector<Sensor*> sensores; 
    Bomba bomba;                   
    Valvula valvulaAuxiliar;    

public:

    EstacaoBombeamento();
    ~EstacaoBombeamento();

    void inicializarEstacao();
    void atualizarSimulacao();
    void renderizarPainel();
    void processarComando(const std::string& comando);
};

#endif