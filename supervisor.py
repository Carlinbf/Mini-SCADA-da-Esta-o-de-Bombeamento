import streamlit as st
import subprocess
import json
import os
import pandas as pd
import plotly.express as px
import time
from datetime import datetime

st.set_page_config(page_title="Supervisor EB-59", layout="wide")
st.title("📡 Painel de Supervisão Digital - Estação EB-59")

if "historico" not in st.session_state:
    st.session_state.historico = pd.DataFrame(columns=["Timestamp", "NIVEL", "PRESSAO", "VAZAO", "TEMPERATURA", "BOMBA", "VALVULA"])

ARQUIVO_CSV = "historico_telemetria.csv"

alertas_placeholder = st.empty()  

st.subheader("📊 Indicadores em Tempo Real")
metricas_placeholder = st.empty()

with metricas_placeholder.container():
    col1, col2, col3, col4, col5 = st.columns(5)
    col1.metric("Nível (NI-59)", "0.00 %")
    col2.metric("Pressão (PI-59)", "0.00 bar")
    col3.metric("Vazão (FI-59)", "0.00 L/min")
    col4.metric("Temperatura (TI-59)", "0.00 °C")  
    col5.metric("Status da Bomba", "AGUARDANDO")

st.markdown("---")
st.subheader("📈 Gráficos de Tendência")
grafico_placeholder = st.empty()

st.markdown("---")
tabela_placeholder = st.empty()

def iniciar_supervisao():
    diretorio_atual = os.path.dirname(os.path.abspath(__file__))
    caminho_executavel = os.path.join(diretorio_atual, "dispositivo.exe")

    processo = subprocess.Popen(
        [caminho_executavel], 
        stdout=subprocess.PIPE, 
        stdin=subprocess.PIPE,  
        stderr=subprocess.STDOUT, 
        text=True,
        bufsize=1,
        cwd=diretorio_atual  
    )

    st.session_state["processo_sub"] = processo
    dados_acumulados = {}
    status_sensores = {}  

    while True:
        linha = processo.stdout.readline()
        if not linha and processo.poll() is not None:
            break
        
        linha = linha.strip()
        
        if linha == "---FIM_CICLO---":
            if dados_acumulados:
                dados_acumulados["Timestamp"] = datetime.now().strftime("%H:%M:%S")
                
                novo_df = pd.DataFrame([dados_acumulados])
                novo_df.to_csv(ARQUIVO_CSV, mode='a', header=not os.path.exists(ARQUIVO_CSV), index=False)
                
                st.session_state.historico = pd.concat([st.session_state.historico, novo_df], ignore_index=True)
                if len(st.session_state.historico) > 50:
                    st.session_state.historico = st.session_state.historico.iloc[1:]

                with alertas_placeholder.container():
                    status_nivel = status_sensores.get("NI-59", "OK")
                    nivel_val = dados_acumulados.get("NIVEL", 0.0)
                    if status_nivel == "FALHA_TRAVADO":
                        st.error(f"🚨 **CRÍTICO:** O Sensor de Nível (NI-59) está TRAVADO em {nivel_val:.1f}%! Bomba desligada por segurança.")
                    elif status_nivel == "ALTO":
                        st.warning(f"⚠️ **ATENÇÃO:** Nível máximo atingido ({nivel_val:.1f}%)! Intertravamento acionado.")
                    elif status_nivel == "BAIXO":
                        st.warning(f"⚠️ **ATENÇÃO:** Nível crítico baixo ({nivel_val:.1f}%)! Bomba acionada.")

                    status_pressao = status_sensores.get("PI-59", "OK")
                    pressao_val = dados_acumulados.get("PRESSAO", 0.0)
                    if status_pressao == "SOBREPRESSAO":
                        st.error(f"💥 **ALERTA:** Sobrepressão severa ({pressao_val:.1f} bar)! Válvula de alívio aberta.")
                    elif status_pressao == "ALIVIO":
                        st.info(f"🔄 **SISTEMA EM ALÍVIO:** Válvula auxiliar aberta. Pressão: {pressao_val:.1f} bar.")

                with metricas_placeholder.container():
                    col1, col2, col3, col4, col5 = st.columns(5)
                    col1.metric("Nível (NI-59)", f"{dados_acumulados.get('NIVEL', 0):.2f} %")
                    col2.metric("Pressão (PI-59)", f"{dados_acumulados.get('PRESSAO', 0):.2f} bar")
                    col3.metric("Vazão (FI-59)", f"{dados_acumulados.get('VAZAO', 0):.2f} L/min")
                    col4.metric("Temperatura (TI-59)", f"{dados_acumulados.get('TEMPERATURA', 0):.2f} °C")  # <-- Atualização dinâmica
                    status_bomba = "LIGADA" if dados_acumulados.get("BOMBA", 0) == 1 else "DESLIGADA"
                    col5.metric("Status da Bomba", status_bomba)

                with grafico_placeholder.container():
                    fig_nivel = px.line(st.session_state.historico, x="Timestamp", y="NIVEL", title="Tendência do Nível (%)")
                    fig_pressao = px.line(st.session_state.historico, x="Timestamp", y="PRESSAO", title="Tendência da Pressão (bar)")
                    
                    c1, c2 = st.columns(2)
                    c1.plotly_chart(fig_nivel, use_container_width=True)
                    c2.plotly_chart(fig_pressao, use_container_width=True)

                with tabela_placeholder.container():
                    st.subheader("📋 Histórico Recente (Persistido em CSV)")
                    st.dataframe(st.session_state.historico.tail(10), use_container_width=True)
                
                dados_acumulados = {}
        
        elif linha.startswith("{") and linha.endswith("}"):
            try:
                objeto = json.loads(linha)
                tag = objeto.get("tag")
                valor = objeto.get("valor")
                status = objeto.get("status", "OK")  
                
                status_sensores[tag] = status

                if tag == "NI-59": dados_acumulados["NIVEL"] = float(valor)
                elif tag == "PI-59": dados_acumulados["PRESSAO"] = float(valor)
                elif tag == "FI-59": dados_acumulados["VAZAO"] = float(valor)
                elif tag == "TI-59": dados_acumulados["TEMPERATURA"] = float(valor)  
                elif tag == "B-1": dados_acumulados["BOMBA"] = int(valor)
                elif tag == "V-1": dados_acumulados["VALVULA"] = int(valor)
            except json.JSONDecodeError:
                pass 

        time.sleep(0.01)

st.sidebar.header("🔧 Controle Operacional")

if st.sidebar.button("Iniciar Monitoramento em Tempo Real"):
    st.sidebar.success("Conectado à Estação EB-59!")
    iniciar_supervisao()

st.sidebar.markdown("---")
st.sidebar.subheader("🛠️ Manutenção de Campo")

if st.sidebar.button("Reparar Sensor de Nível (Reset)"):
    if "processo_sub" in st.session_state and st.session_state["processo_sub"].poll() is None:
        try:
            st.session_state["processo_sub"].stdin.write("CORRIGIR_FALHA_NIVEL\n")
            st.session_state["processo_sub"].stdin.flush()
            st.sidebar.success("🔧 Comando enviado: Falha eliminada no firmware.")
        except Exception as e:
            st.sidebar.error(f"Falha ao comunicar com o hardware: {e}")
    else:
        st.sidebar.warning("O monitoramento ativo não foi detectado.")