import streamlit as st
import subprocess
import json
import os
import pandas as pd
import plotly.express as px
import time
from datetime import datetime

# Configuração da página do Streamlit
st.set_page_config(page_title="Supervisor EB-59", layout="wide")
st.title("📡 Painel de Supervisão Digital - Estação EB-59")

# Inicializa o histórico de dados na sessão do Streamlit para os gráficos
if "historico" not in st.session_state:
    st.session_state.historico = pd.DataFrame(columns=["Timestamp", "NIVEL", "PRESSAO", "VAZAO", "BOMBA", "VALVULA"])

# Arquivo físico para persistência exigido pelo edital
ARQUIVO_CSV = "historico_telemetria.csv"

# Espaços reservados para atualização dinâmica da tela
metricas_placeholder = st.empty()
grafico_placeholder = st.empty()
tabela_placeholder = st.empty()

def iniciar_supervisao():
    # Dispara o binário C++ capturando a saída padrão
    processo = subprocess.Popen(
        ["./dispositivo"], 
        stdout=subprocess.PIPE, 
        stderr=subprocess.STDOUT, 
        text=True,
        bufsize=1
    )

    dados_acumulados = {}

    while True:
        linha = processo.stdout.readline()
        if not linha and processo.poll() is not None:
            break
        
        linha = linha.strip()
        
        # Se atingiu o fim do ciclo de leitura, processa e plota
        if linha == "---FIM_CICLO---":
            if dados_acumulados:
                dados_acumulados["Timestamp"] = datetime.now().strftime("%H:%M:%S")
                
                # Transforma em DataFrame
                novo_df = pd.DataFrame([dados_acumulados])
                
                # 1. PERSISTÊNCIA COMPLETA: Grava no arquivo CSV físico (modo Append)
                novo_df.to_csv(ARQUIVO_CSV, mode='a', header=not os.path.exists(ARQUIVO_CSV), index=False)
                
                # 2. Atualiza a memória de sessão do Streamlit
                st.session_state.historico = pd.concat([st.session_state.historico, novo_df], ignore_index=True)
                if len(st.session_state.historico) > 50:
                    st.session_state.historico = st.session_state.historico.iloc[1:]

                # 3. Renderiza os cartões de métricas atuais
                with metricas_placeholder.container():
                    col1, col2, col3, col4 = st.columns(4)
                    col1.metric("Nível (NI-59)", f"{dados_acumulados.get('NIVEL', 0):.2f} %")
                    col2.metric("Pressão (PI-59)", f"{dados_acumulados.get('PRESSAO', 0):.2f} bar")
                    col3.metric("Vazão (FI-59)", f"{dados_acumulados.get('VAZAO', 0):.2f} L/min")
                    status_bomba = "LIGADA" if dados_acumulados.get("BOMBA", 0) == 1 else "DESLIGADA"
                    col4.metric("Status da Bomba", status_bomba)

                # 4. Renderiza os Gráficos Temporais (Plotly)
                with grafico_placeholder.container():
                    fig_nivel = px.line(st.session_state.historico, x="Timestamp", y="NIVEL", title="Tendência do Nível (%)")
                    fig_pressao = px.line(st.session_state.historico, x="Timestamp", y="PRESSAO", title="Tendência da Pressão (bar)")
                    
                    c1, c2 = st.columns(2)
                    c1.plotly_chart(fig_nivel, use_container_width=True)
                    c2.plotly_chart(fig_pressao, use_container_width=True)

                # 5. Exibe a tabela histórica na tela
                with tabela_placeholder.container():
                    st.subheader("📋 Histórico Recente de Telemetria (Gravado em CSV)")
                    st.dataframe(st.session_state.historico.tail(10), use_container_width=True)
                
                # Reseta para o próximo ciclo
                dados_acumulados = {}
        
        # Se for uma linha de dados, realiza o parsing do JSON
        elif linha.startswith("{") and linha.endswith("}"):
            try:
                objeto = json.loads(linha)
                tag = objeto.get("tag")
                valor = objeto.get("valor")
                
                # Mapeia as tags do contrato de dados para as colunas do Dataframe
                if tag == "NI-59": dados_acumulados["NIVEL"] = float(valor)
                elif tag == "PI-59": dados_acumulados["PRESSAO"] = float(valor)
                elif tag == "FI-59": dados_acumulados["VAZAO"] = float(valor)
                elif tag == "B-1": dados_acumulados["BOMBA"] = int(valor)
                elif tag == "V-1": dados_acumulados["VALVULA"] = int(valor)
            except json.JSONDecodeError:
                pass # Ignora linhas malformadas decorrentes de ruídos

        time.sleep(0.05)

if st.button("Iniciar Monitoramento em Tempo Real"):
    st.success("Conectado à Estação EB-59 via JSON Lines.")
    iniciar_supervisao()