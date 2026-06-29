# 🤖 Registro de Uso de Inteligência Artificial (AI-Log)

Este documento registra de forma transparente e detalhada como ferramentas de Inteligência Artificial Generativa foram utilizadas durante o desenvolvimento do projeto da **Estação de Bombeamento EB-59**. O uso da tecnologia focou em suporte arquitetural, resolução de bugs de comunicação interprocessos e formatação de documentação técnica, mantendo a autoria integral do código com os alunos.

## 1. Escopo de Utilização

A IA foi consultada em momentos específicos do desenvolvimento em que a integração entre as duas linguagens do projeto (C++ e Python) gerou conflitos de concorrência ou sintaxe:
* **Depuração de Erros de Compilação:** Resolução de erros de redefinição de escopo e caracteres perdidos (`stray '#'`) gerados por falhas de mesclagem de código no arquivo `src/main.cpp`.
* **Comunicação Assíncrona via Pipes:** Tratamento do erro crítico de conexão do Windows (`WinError 10054`) no loop assíncrono do Streamlit.
* **Comportamento Termodinâmico:** Modelagem lógica para estabilização das leituras de temperatura e nível no arquivo `src/EstacaoBombeamento.cpp`.
* **Estruturação de Testes de Validação:** Auxílio na sintaxe e asserções do framework de testes automatizados do Python.

---

## 2. Histórico de Prompts e Engenharia de Contexto

### 🐛 Interação 01: Tratamento de Erro de Conexão (WinError 10054)
* **Contexto do Problema:** O painel do Streamlit quebrava assim que o subprocesso C++ encerrava um ciclo, exibindo a exceção `ConnectionResetError: [WinError 10054] Foi forçado o cancelamento de uma conexão existente pelo host remoto`.
* **Prompt de Engenharia:** *"Tenho um script em Python com Streamlit lendo o stdout de um executável C++ via subprocess. Quando o C++ fecha ou tenta ler o stdin de forma síncrona, o Python joga ConnectionResetError no proactor_events.py. Como tratar a leitura de comandos no C++ para que ela seja não-bloqueante e não derrube o Pipe?"*
* **Solução Adotada:** Implementação da biblioteca `<conio.h>` e uso da função `_kbhit()` dentro do `while(true)` do `main.cpp`. Isso garantiu que o C++ processasse comandos do Streamlit de forma assíncrona, checando o buffer apenas se houvesse dados pendentes, eliminando a queda da conexão.

### 📐 Interação 02: Resolução de Conflitos no Arquivo Main
* **Contexto do Problema:** Ao tentar integrar as funções de teclado no loop principal, o compilador GCC retornou erros de declaração e duplicação: `error: stray '#' in program` e `error: redefinition of 'int main()'`.
* **Prompt de Engenharia:** *"O g++ está dando erro de redefinição do main e stray '#' na linha de include. Analise o erro e me mostre a estrutura correta de limpeza do main.cpp contendo o loop com _kbhit() e os cabeçalhos corretos."*
* **Solução Adotada:** Identificação de linhas duplicadas causadas por erro de colagem no terminal e reestruturação limpa do ponto de entrada do programa.

### 📊 Interação 03: Mapeamento de Novas Variáveis (Temperatura)
* **Contexto do Problema:** O edital exigia que a temperatura não crescesse indefinidamente na tela. Era necessário amarrá-la ao comportamento físico do motor da bomba.
* **Prompt de Engenharia:** *"No meu loop C++, eu tenho as variáveis sTemperatura e bomba. Como criar uma lógica simples onde a temperatura sobe gradativamente até estabilizar em 45°C se a bomba estiver ligada, e resfria até 24°C se a bomba for desligada, aplicando uma variação aleatória simulada?"*
* **Solução Adotada:** Criação de condicionais baseadas no método `bomba.estaLigada()` que incrementam ou decrementam o valor do sensor usando `rand()` para simular ruído térmico realista antes do disparo do JSON Lines.

### 📋 Interação 04: Persistência Concorrente de Telemetria em CSV
* **Contexto do Problema:** O edital exige a persistência física dos dados para auditoria. Ao salvar os dados vindos do C++ no arquivo `historico_telemetria.csv`, o script Python criava uma nova linha de cabeçalho (`Timestamp, NIVEL, PRESSAO...`) a cada ciclo de 1 segundo, poluindo o arquivo.
* **Prompt de Engenharia Utilizado:** *"Estou usando pandas em um loop while True para salvar dicionários de dados que chegam a cada segundo em um arquivo CSV usando to_csv(mode='a'). Como fazer para que o cabeçalho seja gravado apenas na primeira vez que o arquivo for criado, sem duplicar as colunas a cada linha adicionada?"*
* **Solução Adotada:** Injeção do parâmetro condicional `header=not os.path.exists(ARQUIVO_CSV)` dentro do método `to_csv`. Isso garantiu que o arquivo seja criado de forma limpa na primeira execução e receba apenas os dados brutos nos segundos subsequentes.

### 🧱 Interação 05: Prevenção de Travamento por JSON Incompleto (Parsing Corrompido)
* **Contexto do Problema:** Quando o processo C++ iniciava rápido demais ou despejava texto bruto de inicialização (ex: *"Iniciando a Estacao Bombeamento EB-59..."*), o decodificador `json.loads()` do Python tentava interpretar a string, quebrava e travava todo o painel do Streamlit com erros de `JSONDecodeError`.
* **Prompt de Engenharia Utilizado:** *"No meu script de supervisão em Python, o readline() captura tudo o que o C++ joga no terminal. Se o C++ printar uma mensagem de texto normal antes de começar a enviar o JSON, o json.loads quebra. Como criar um filtro robusto para tentar converter em JSON apenas o que realmente for uma linha válida de dados?"*
* **Solução Adotada:** Implementação de uma checagem de contorno (`if linha.startswith("{") and linha.endswith("}")`) combinada com um bloco de exceção segura `try...except json.JSONDecodeError: pass`. Isso isolou as strings de telemetria das mensagens de texto informativas do firmware, tornando o supervisor imune a crashes de processamento de strings.

### ⏱️ Interação 06: Gerenciamento de Memória de Sessão (Session State) no Gráfico
* **Contexto do Problema:** Como o Streamlit recarrega o script do zero a cada atualização da página, a tabela e os gráficos de tendência perdiam todo o histórico de dados e reiniciavam do zero a cada ciclo de 1 segundo.
* **Prompt de Engenharia Utilizado:** *"Estou plotando um gráfico de linha com Plotly Express dentro de um loop infinito no Streamlit. Como reter os dados coletados nos segundos anteriores para que o gráfico mostre a curva subindo ao longo do tempo em vez de apagar a tela a cada atualização?"*
* **Solução Adotada:** Utilização do dicionário persistente do Streamlit `st.session_state`. Inicializamos o DataFrame vazio `if "historico" not in st.session_state` e passamos a concatenar os novos dados nele a cada ciclo, aplicando uma trava de memória (`iloc[1:]`) para reter apenas os últimos 50 pontos e evitar estouro de memória no navegador.

### 🧪 Interação 07: Cobertura e Estruturação de Testes Automatizados
* **Contexto do Problema:** Era necessário validar que as regras de intertravamento da planta estavam se comportando perfeitamente. O arquivo `test_regras.py` possuía 4 testes que precisavam ser validados quanto à sintaxe correta das asserções do framework.
* **Prompt de Engenharia Utilizado:** *"Como criar testes no framework `unittest` do Python para interceptar e validar dados mockados de um dicionário, garantindo que variáveis lógicas (como acionamento e alívio de válvulas) respondam corretamente a limites predefinidos de sobrepressão e nível alto?"*
* **Solução Adotada:** Ajuste da sintaxe dos métodos `assertEqual()` e `assertTrue()` nas rotinas de teste automatizado, fechando com sucesso a cobertura dos 4 cenários críticos exigidos pelo sistema de supervisão.

### 📋 Interação 08: Organização AI_LOG
* **Contexto do Problema:** Precisava de ajudar para montagem e organização do arquivo AI_LOG.
* **Prompt de Engenharia Utilizado:** *"Me ajude a montar o AI_LOG baseado em nossa conversa"*

---
