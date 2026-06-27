#include "../include/EstacaoBombeamento.hpp"
#include "../include/SensorFactory.hpp"
#include "../include/SensorNivel.hpp"
#include <iostream>
#include <iomanip>


EstacaoBombeamento::EstacaoBombeamento() 
    : bomba("B-1", 50.0), valvulaAuxiliar("V-1") {}


EstacaoBombeamento::~EstacaoBombeamento() {
    for (Sensor* sensor : sensores) {
        delete sensor;
    }
    sensores.clear();
}


void EstacaoBombeamento::inicializarEstacao() {
    sensores.push_back(SensorFactory::criarSensor("NIVEL"));
    sensores.push_back(SensorFactory::criarSensor("PRESSAO"));
    sensores.push_back(SensorFactory::criarSensor("TEMPERATURA"));
    sensores.push_back(SensorFactory::criarSensor("VAZAO"));
}


void EstacaoBombeamento::atualizarSimulacao() {

    for (Sensor* sensor : sensores) {
        sensor->ler(); 
    }

    Sensor* sNivel = sensores[0];
    Sensor* sPressao = sensores[1];
    Sensor* sVazao = sensores[3];


    if (bomba.estaLigada()) {
        sVazao->setValor(50.0 + (rand() % 100 - 50) / 100.0);

        if (sNivel->getValor() < 100.0) {
            sNivel->setValor(sNivel->getValor() + 0.8);
        }

        if (!valvulaAuxiliar.estaAberta()) {
            sPressao->setValor(sPressao->getValor() + 0.3); 
        }
    } else {
        sVazao->setValor(0.0);
        if (sNivel->getValor() > 0.0) {
            sNivel->setValor(sNivel->getValor() - 0.5); // Esvazia 0.5% por segundo
        }
    }   

    if (valvulaAuxiliar.estaAberta() && sPressao->getValor() > 1.0) {
        sPressao->setValor(sPressao->getValor() - 0.6); 
    }

    
    double nivelAtual = sNivel->getValor();
    double pressaoAtual = sPressao->getValor();

    
    if (nivelAtual > 88.0 && bomba.estaLigada()) {
        bomba.desligar();
    }
   
    else if (nivelAtual < 28.0 && !bomba.estaLigada()) {
        bomba.ligar();
    }

    if (pressaoAtual > 6.8) {
        valvulaAuxiliar.abrir(); // Abre para aliviar se passar do limite da dupla 
    } else if (pressaoAtual <= 4.0) {
        valvulaAuxiliar.fechar(); // Fecha quando a pressão baixar para uma zona segura
    }
}


void EstacaoBombeamento::renderizarPainel() {
    
    std::cout << "\033[2J\033[1;1H"; 
    
    std::cout << "==========================================\n";
    std::cout << "      ESTACAO DE BOMBEAMENTO EB-59        \n";
    std::cout << "==========================================\n";
    std::cout << " STATUS DA BOMBA [" << bomba.getId() << "]: " 
              << (bomba.estaLigada() ? "LIGADA" : "DESLIGADA") << "\n";
    std::cout << " ATUADOR AUXILIAR [" << valvulaAuxiliar.getId() << "]: " 
              << (valvulaAuxiliar.estaAberta() ? "ABERTA (ALIVIO)" : "FECHADA") << "\n";
    std::cout << " VAZAO ATUAL DA BOMBA: " << bomba.getVazaoAtual() << " L/min\n";
    std::cout << "------------------------------------------\n";
    std::cout << " TELEMETRIA DOS SENSORES (COM RUIDO):\n";
    
    for (Sensor* sensor : sensores) {
        std::cout << " -> " << sensor->getTag() << ": " 
                  << std::fixed << std::setprecision(2) << sensor->getValor() 
                  << " " << sensor->getUnidade() 
                  << " [" << sensor->getStatus() << "]\n";
    }
    std::cout << "==========================================\n";
}


void EstacaoBombeamento::processarComando(const std::string& comando) {
    if (comando == "LIGAR_BOMBA") {
        bomba.ligar();
    } 
    else if (comando == "DESLIGAR_BOMBA") {
        bomba.desligar();
    } 
    else if (comando == "ABRIR_VALVULA") {
        valvulaAuxiliar.abrir();
    }
    else if (comando == "FECHAR_VALVULA") {
        valvulaAuxiliar.fechar();
    }
    else if (comando == "FORCAR_FALHA_NIVEL") {

        SensorNivel* sn = dynamic_cast<SensorNivel*>(sensores[0]);
        if (sn) sn->forcarFalhaTravado();
    } 
    else if (comando == "CORRIGIR_FALHA_NIVEL") {
        SensorNivel* sn = dynamic_cast<SensorNivel*>(sensores[0]);
        if (sn) sn->normalizarSensor();
    }
}