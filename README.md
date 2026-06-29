# Mini-SCADA-da-Esta-o-de-Bombeamento

Assinatura Operacional da Dupla (EB-59) 

| Item | Valor da Dupla | Observação / Faixa da Especificação |
| --- | --- | --- |
| **Identificador da estação** | EB-59 | Baseado na identidade da dupla.|
| **Sensores obrigatórios** | NÍVEL, PRESSÃO, TEMPERATURA, VAZÃO | Atende ao mínimo de 4 tipos de sensores.|
| **Nível baixo** | 28% | Limite de alerta (Faixa: 20% a 35%).|
| **Nível alto** | 88% | Limite de alerta (Faixa: 75% a 90%).|
| **Pressão alta** | 6.8 bar | Limite de alarme (Faixa: 5.0 bar a 8.0 bar).|
| **Falha obrigatória** | Sensor de nível travado | Falha simulada específica da dupla.|
| **Regra extra** | Alívio de Pressão por Válvula Auxiliar | Regra de controle específica da dupla.|

---

# 📡 Sistema de Supervisão Digital - Estação de Bombeamento EB-59

## 📝 Descrição do Projeto
O projeto consiste em um sistema distribuído de missão crítica para monitoramento e controle automatizado de uma estação de bombeamento industrial (EB-59). O sistema simula o comportamento dinâmico de variáveis físicas (nível, pressão, vazão e temperatura), aplicando regras de intertravamento operacional e segurança em nível de firmware. 

O objetivo principal é demonstrar o tratamento de falhas em sistemas ciber-físicos, simulando o congelamento de um sensor de campo e executando uma rotina de parada de emergência automática, com capacidade de diagnóstico e recuperação remota através de uma interface de supervisão moderna.

---

## 🏗️ Arquitetura em Duas Camadas

O sistema adota uma arquitetura descentralizada dividida em duas camadas fundamentais:

```text
+--------------------------------------------------------+
|               CAMADA DE SUPERVISÃO (Python)           |
|  - Interface SCADA Web via Streamlit                   |
|  - Ingestão Assíncrona e Persistência CSV              |
|  - Telediagnóstico e Envio de Comandos de Manutenção   |
+-----------------------------------+--------------------+
                                    | ▲
              Comandos via stdin    | |  Telemetria via stdout
              (Strings de Controle) | |  (JSON Lines)
                                    ▼ |
+-----------------------------------+--------------------+
|                  CAMADA DE CAMPO (C++17)               |
|  - Loop de Controle de Tempo Real Não-Bloqueante       |
|  - Fábrica de Sensores e Máquina de Estados            |
|  - Intertravamento de Segurança e Firmware de Campo    |
+--------------------------------------------------------+

1. **Camada de Campo (Edge/Firmware):** Desenvolvida em **C++17**, é responsável por executar o loop físico de controle da planta, atualizar os estados dos atuadores com base nas leituras dos sensores e cuspir a telemetria no canal de saída padrão (`stdout`).
2. **Camada de Supervisão (SCADA/Host):** Desenvolvida em **Python 3**, utiliza o **Streamlit** como servidor web de interface humana-máquina (IHM). Ela gerencia o subprocesso do C++, faz o parsing assíncrono dos dados e permite a atuação remota via canal de entrada padrão (`stdin`).

---

## 📊 Diagrama de Classes

         +---------------------------------------+
         |               Sensor                  | <--- (Classe Abstrata Base)
         +---------------------------------------+
         | # valor: double                       |
         | # status: string                      |
         +---------------------------------------+
         | + virtual ler(): void = 0             |
         | + getValor(): double                  |
         | + getStatus(): string                 |
         +---------------------------------------+
                             ▲
                             |
       +---------------------+---------------------+---------------------+
       |                     |                     |                     |
+----------------------+ +----------------------+ +----------------------+ +----------------------+
|     SensorNivel      | |     SensorPressao    | |   SensorTemperatura  | |      SensorVazao     |
+----------------------+ +----------------------+ +----------------------+ +----------------------+
| - falhaAtiva: bool   | |                      | |                      | |                      |
+----------------------+ +----------------------+ +----------------------+ +----------------------+
| + ler(): void        | | + ler(): void        | | + ler(): void        | | + ler(): void        |
| + forcarFalha(): void| |                      | |                      | |                      |
| + normalizar(): void | |                      | |                      | |                      |
+----------------------+ +----------------------+ +----------------------+ +----------------------+

         +---------------------------------------+
         |          SensorFactory                | <--- (Padrão Factory Method)
         +---------------------------------------+
         | + criarSensor(tipo: string): Sensor* |
         +---------------------------------------+

         +---------------------------------------+
         |         EstacaoBombeamento            |
         +---------------------------------------+
         | - sensores: vector<Sensor*>           |
         | - bomba: Bomba                        |
         | - valvulaAuxiliar: Valvula            |
         +---------------------------------------+
         | + inicializarEstacao(): void          |
         | + atualizarSimulacao(): void          |
         | + renderizarPainel(): void            |
         | + processarComando(cmd: string): void |
         +---------------------------------------+

---

## 📜 Contrato de Comunicação JSON

A troca de mensagens segue o padrão **JSON Lines**, onde cada amostragem de sensor/atuador é enviada como uma string JSON válida em uma única linha, finalizada por um delimitador de ciclo (`---FIM_CICLO---`).

### Modelo de Mensagem dos Sensores/Atuadores:

```json
{"tag": "NI-59", "valor": 55.20, "unidade": "%", "status": "OK"}
{"tag": "PI-59", "valor": 4.20, "unidade": "bar", "status": "OK"}
{"tag": "TI-59", "valor": 32.50, "unidade": "C", "status": "OK"}
{"tag": "FI-59", "valor": 49.80, "unidade": "L/min", "status": "OK"}
{"tag": "B-1", "valor": 1, "unidade": "booleano", "status": "LIGADA"}
{"tag": "V-1", "valor": 0, "unidade": "booleano", "status": "FECHADA"}
---FIM_CICLO---

```

### Comandos de Controle Remoto (Supervisor ➡️ Campo):

* `FORCAR_FALHA_NIVEL\n` -> Força o congelamento artificial do sensor de nível.
* `CORRIGIR_FALHA_NIVEL\n` -> Restaura o firmware do sensor para o modo dinâmico e reseta o contador de ciclos.

---

## 🛠️ Instruções de Compilação e Execução

### Pré-requisitos

* Compilador GCC (C++17 habilitado).
* Python 3.8+ instalado.

### 1. Compilação da Camada de Campo (C++)

No terminal (Git Bash ou CMD), execute os comandos abaixo na pasta raiz do projeto:

```bash
g++ -std=c++17 src/*.cpp -o dispositivo.exe

```

### 2. Instalação de Dependências e Execução do Streamlit (Python)

Instale as dependências de visualização e processamento de dados e inicie o painel:

```bash
pip install streamlit pandas plotly

streamlit run supervisor.py

```

O painel abrirá automaticamente no seu navegador em `http://localhost:8501`.

---

## 🧪 Instruções de Execução dos Testes

O projeto conta com uma suíte de testes automatizados para validação das regras de intertravamento do motor e comportamento de falha do sensor:

Para rodar os testes unitários via linha de comando, execute:

```bash
python -m unittest testes/test_regras.py

```

*Os cenários de teste incluem: verificação de ligamento em nível baixo (<28%), desligamento automático em nível alto (>88%), desarme imediato por intertravamento sob status `FALHA_TRAVADO` e certifica que a válvula auxiliar é aberta automaticamente para drenar o sistema quando a pressão ultrapassa o limite crítico de 6.8 bar.*

---

## ✍️ Assinatura Operacional e Divisão de Responsabilidades

* **Integrante 1 (Carlos / GitHub: Carlinbf):**
* Responsável pelo desenvolvimento da **Camada de Campo (C++) e Supervisão (Python)**.


* **Integrante 2 (Arthur / GitHub: arthurmotta23 ):**
* Responsável pelo desenvolvimento da **Camada de Campo (C++) e Supervisão (Python)**.



---

## 📐 Decisões de Padrões de Projeto (Design Patterns)

* **Factory Method (`SensorFactory`):** Centraliza a lógica de instanciação dos sensores de nível, vazão, pressão e temperatura. Isso desacopla a classe principal `EstacaoBombeamento` das implementações concretas dos sensores, facilitando a adição de novos instrumentos de medição no futuro sem alterar o núcleo da aplicação.
* **Polimorfismo Baseado em Interfaces:** O vetor de ponteiros `std::vector<Sensor*>` permite que o ciclo de atualização (`sensor->ler()`) trate todos os instrumentos de forma genérica e uniforme, aplicando o princípio Open/Closed do SOLID.

---

## ⚠️ Limitações Conhecidas

* **Monotarefa Bloqueante por Sleep:** O loop principal do C++ baseia-se na função `Sleep(1000)` do Windows para cadenciar o tempo. Em ambientes industriais reais, utiliza-se temporizadores baseados em interrupção de hardware ou arquiteturas multithreading (RTOS) para evitar que o processamento trave durante a espera de E/S.
* **Dependência de SO (OS-Lock):** O controle de entrada assíncrona de comandos utiliza a biblioteca `<conio.h>` e a função `_kbhit()`, que são nativas do ecossistema Windows, limitando a portabilidade direta do binário para sistemas POSIX (Linux/MacOS) sem adaptação no arquivo `main.cpp`.

```

```