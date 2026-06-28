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
        sNivel->setStatus("ALTO");
        if(bomba.estaLigada()) {
            bomba.desligar();
        }
    }
   
    else if (nivelAtual < 28.0 && !bomba.estaLigada()) {
        sNivel->setStatus("BAIXO");
        if(!bomba.estaLigada()) {
            bomba.ligar();
        }
    }
    else{
        sNivel->setStatus("OK");
    }

    if (pressaoAtual > 6.8) {
        sPressao->setStatus("SOBREPRESSAO");
        valvulaAuxiliar.abrir();
    } else if (pressaoAtual <= 4.0) {
        if (pressaoAtual> 5.5){
            sPressao->setStatus("ALTO");
        }else{
            sPressao->setStatus("OK");
        }
        valvulaAuxiliar.fechar(); 
    }
    else{
        if (valvulaAuxiliar.estaAberta()) {
            sPressao->setStatus("ALIVIO");
        } else {
            sPressao->setStatus("OK");
        }
    }
    if (bomba.estaLigada()) {
        sVazao->setStatus("OK");
    } else {
        sVazao->setStatus("ZERADA");
    }
}

void EstacaoBombeamento::renderizarPainel() {
    // Identifica os sensores para extrair os dados do contrato
    Sensor* sNivel = sensores[0];
    Sensor* sPressao = sensores[1];
    Sensor* sVazao = sensores[3];

    // Envia os dados estruturados em formato JSON Lines (uma linha por objeto JSON)
    // Atende aos campos obrigatórios: tag, valor, unidade, status (timestamp gerado no Python)
    std::cout << "{\"tag\": \"NI-59\", \"valor\": " << sNivel->getValor() 
              << ", \"unidade\": \"%\", \"status\": \"" << sNivel->getStatus() << "\"}\n";

    std::cout << "{\"tag\": \"PI-59\", \"valor\": " << sPressao->getValor() 
              << ", \"unidade\": \"bar\", \"status\": \"" << sPressao->getStatus() << "\"}\n";

    std::cout << "{\"tag\": \"FI-59\", \"valor\": " << sVazao->getValor() 
              << ", \"unidade\": \"L/min\", \"status\": \"" << sVazao->getStatus() << "\"}\n";

    // Envia o estado dos atuadores para o supervisor mapear
    std::cout << "{\"tag\": \"B-1\", \"valor\": " << (bomba.estaLigada() ? 1 : 0) 
              << ", \"unidade\": \"booleano\", \"status\": \"" << (bomba.estaLigada() ? "LIGADA" : "DESLIGADA") << "\"}\n";

    std::cout << "{\"tag\": \"V-1\", \"valor\": " << (valvulaAuxiliar.estaAberta() ? 1 : 0) 
              << ", \"unidade\": \"booleano\", \"status\": \"" << (valvulaAuxiliar.estaAberta() ? "ABERTA" : "FECHADA") << "\"}\n";
              
    // Delimitador de fim de ciclo para o Python saber que o bloco terminou
    std::cout << "---FIM_CICLO---\n";
    std::cout.flush();
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