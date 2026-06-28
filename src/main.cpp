#include "../include/EstacaoBombeamento.hpp"
#include <iostream>
#include <windows.h>
#include <chrono>
#include <conio.h> 

int main() {
    try {
        EstacaoBombeamento estacao;

        estacao.inicializarEstacao();

        estacao.processarComando("LIGAR_BOMBA");

        std::cout << "Iniciando a Estacao Bombeamento EB-59...\n";
        Sleep(1000);

        while (true) {
            while (_kbhit()) {
                std::string comando;
                std::getline(std::cin, comando);
                if (!comando.empty()) {
                    estacao.processarComando(comando);
                }
            }

            estacao.atualizarSimulacao();
            estacao.renderizarPainel();

            Sleep(1000);
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "[ERRO CRITICO] Ocorreu uma falha no dispositivo: " << e.what() << "\n";
        return 1;
    }

    return 0;
}