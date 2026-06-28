import streamlit as st
import subprocess
import json
import os
import pandas as pd
import plotly.express as px
import time
from datetime import datetime

# Configuração da página web do Streamlit
st.set_page_config(page_title="Supervisor EB-59", layout="wide")
st.title("📡 Painel de Supervisão Digital - Estação EB-59")

# Inicializa o histórico na memória de sessão do Streamlit para alimentar os gráficos
if "historico" not in st.session_state:
    st.session_state.historico = pd.DataFrame(columns=["Timestamp", "NIVEL", "PRESSAO", "VAZAO", "BOMBA", "VALVULA"])

# Nome do arquivo físico para persistência de dados exigido pelo edital
ARQUIVO_CSV = "historico_telemetria.csv"

# Elementos dinâmicos da interface que atualizam sem dar refresh na página
metricas_placeholder = st.empty()
grafico_placeholder = st.empty()
tabela_placeholder = st.empty()

def iniciar_supervisao():
    # Executa o binário C++ capturando a saída padrão (stdout)
    # Descobre o caminho absoluto da pasta onde o supervisor.py está rodando
    diretorio_atual = os.path.dirname(os.path.abspath(__file__))
    caminho_executavel = os.path.join(diretorio_atual, "dispositivo.exe")

    # Inicia o executável passando o caminho correto e definindo o diretório de trabalho
    processo = subprocess.Popen(
        [caminho_executavel], 
        stdout=subprocess.PIPE, 
        stderr=subprocess.STDOUT, 
        text=True,
        bufsize=1,
        cwd=diretorio_atual  # Define a pasta atual como o diretório de execução
    )

    dados_acumulados = {}

    while True:
        linha = processo.stdout.readline()
        if not linha and processo.poll() is not None:
            break
        
        linha = linha.strip()
        
        # Quando o ciclo fecha, consolida e atualiza a interface
        if linha == "---FIM_CICLO---":
            if dados_acumulados:
                # O Python injeta o carimbo de tempo real no momento da aquisição
                dados_acumulados["Timestamp"] = datetime.now().strftime("%H:%M:%S")
                
                novo_df = pd.DataFrame([dados_acumulados])
                
                # REQUISITO: Salva fisicamente no arquivo CSV (modo append)
                novo_df.to_csv(ARQUIVO_CSV, mode='a', header=not os.path.exists(ARQUIVO_CSV), index=False)
                
                # Atualiza o histórico da sessão do navegador (limita aos últimos 50 para performance)
                st.session_state.historico = pd.concat([st.session_state.historico, novo_df], ignore_index=True)
                if len(st.session_state.historico) > 50:
                    st.session_state.historico = st.session_state.historico.iloc[1:]

                # Renderiza os blocos de métricas superiores (Dashboard)
                with metricas_placeholder.container():
                    col1, col2, col3, col4 = st.columns(4)
                    col1.metric("Nível (NI-59)", f"{dados_acumulados.get('NIVEL', 0):.2f} %")
                    col2.metric("Pressão (PI-59)", f"{dados_acumulados.get('PRESSAO', 0):.2f} bar")
                    col3.metric("Vazão (FI-59)", f"{dados_acumulados.get('VAZAO', 0):.2f} L/min")
                    status_bomba = "LIGADA" if dados_acumulados.get("BOMBA", 0) == 1 else "DESLIGADA"
                    col4.metric("Status da Bomba", status_bomba)

                # Renderiza os Gráficos de Tendência Temporal usando Plotly
                with grafico_placeholder.container():
                    fig_nivel = px.line(st.session_state.historico, x="Timestamp", y="NIVEL", title="Tendência do Nível (%)")
                    fig_pressao = px.line(st.session_state.historico, x="Timestamp", y="PRESSAO", title="Tendência da Pressão (bar)")
                    
                    c1, c2 = st.columns(2)
                    c1.plotly_chart(fig_nivel, use_container_width=True)
                    c2.plotly_chart(fig_pressao, use_container_width=True)

                # Renderiza a tabela com o histórico consultável
                with tabela_placeholder.container():
                    st.subheader("📋 Histórico Recente (Persistido em CSV)")
                    st.dataframe(st.session_state.historico.tail(10), use_container_width=True)
                
                # Reseta o dicionário temporário para o próximo segundo
                dados_acumulados = {}
        
        # Realiza o parsing seguro de cada linha do contrato JSON
        elif linha.startswith("{") and linha.endswith("}"):
            try:
                objeto = json.loads(linha)
                tag = objeto.get("tag")
                valor = objeto.get("valor")
                
                if tag == "NI-59": dados_acumulados["NIVEL"] = float(valor)
                elif tag == "PI-59": dados_acumulados["PRESSAO"] = float(valor)
                elif tag == "FI-59": dados_acumulados["VAZAO"] = float(valor)
                elif tag == "B-1": dados_acumulados["BOMBA"] = int(valor)
                elif tag == "V-1": dados_acumulados["VALVULA"] = int(valor)
            except json.JSONDecodeError:
                pass 

        time.sleep(0.01)

# Botão de controle de ativação do supervisor
if st.button("Iniciar Monitoramento em Tempo Real"):
    st.success("Conectado à Estação EB-59 via contrato JSON Lines.")
    iniciar_supervisao()