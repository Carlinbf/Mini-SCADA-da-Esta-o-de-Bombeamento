#include "../include/EstacaoBombeamento.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        EstacaoBombeamento estacao;

        estacao.inicializarEstacao();

        estacao.processarComando("LIGAR_BOMBA");

        std::cout << "Iniciando a Estacao Bombeamento EB-59...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (true) {
            estacao.atualizarSimulacao();

            estacao.renderizarPainel();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "[ERRO CRITICO] Ocorreu uma falha no dispositivo: " << e.what() << "\n";
        return 1;
    }

    return 0;
}