import streamlit as st
import pandas as pd
import psycopg2
import plotly.express as px
from datetime import datetime, timedelta
import pytz

# 1. Configurações da Página
st.set_page_config(page_title="ByteBier Monitor", layout="wide", page_icon="🍺")

# Configuração de Fuso Horário Brasil (São Paulo)
fuso_sp = pytz.timezone('America/Sao_Paulo')

# 2. Conexão com o Banco ByteBier
def conectar_bd():
    return psycopg2.connect(
        dbname="ByteBier",
        user="postgres",
        password="root",
        host="localhost",
        port="5432"
    )

# 3. Estilização
st.markdown("""
<style>
.main { background-color: #f5f5f5; }
.stMetric {
    background-color: #000000;
    color: white;
    padding: 15px;
    border-radius: 10px;
    box-shadow: 2px 2px 5px rgba(0,0,0,0.1);
}
/* Forçar cores do texto dentro do card preto */
[data-testid="stMetricValue"] { color: white !important; }
[data-testid="stMetricLabel"] { color: #aaaaaa !important; }
[data-testid="stMetricDelta"] { color: #00ff00 !important; }
</style>
""", unsafe_allow_html=True)

# 4. Sidebar
with st.sidebar:
    st.image("https://cdn-icons-png.flaticon.com/512/931/931949.png", width=100)
    st.header("ByteBier Control")
    btn_atualizar = st.button("🔄 Forçar Atualização")
    st.divider()
    st.info("Atualização automática: 5s")
    # Exibe a hora atual do seu PC (São Paulo)
    hora_agora = datetime.now(fuso_sp).strftime('%H:%M:%S')
    st.write(f"🕒 Hora Local: {hora_agora}")

# 5. Conteúdo Principal com Atualização Automática
@st.fragment(run_every=5)
def renderizar_conteudo():
    try:
        conn = conectar_bd()
        # Busca as últimas 50 leituras
        df = pd.read_sql("SELECT * FROM sensor_readings ORDER BY timestamp DESC LIMIT 50", conn)
        # Busca os últimos 5 alertas
        df_alertas = pd.read_sql("SELECT * FROM alerts ORDER BY created_at DESC LIMIT 5", conn)
        conn.close()

        if not df.empty:
            ultima = df.iloc[0]
            agora = datetime.now(fuso_sp).replace(tzinfo=None)
            ts_banco = pd.to_datetime(ultima['timestamp'])

            if ts_banco.tzinfo is not None:
                timestamp_ultima = ts_banco.astimezone(fuso_sp).replace(tzinfo=None)
            else:
                timestamp_ultima = ts_banco - timedelta(hours=3)

            # Cálculo de segundos desde a última mensagem do ESP32
            segundos_atraso = (agora - timestamp_ultima).total_seconds()
            sistema_online = segundos_atraso < 40 # Margem de 40s para oscilações

            # --- KPIs (Métricas Superiores)
            c1, c2, c3, c4 = st.columns(4)
            c1.metric("🌡️ Líquido", f"{ultima['temp_liq']}°C")
            c2.metric("🌡️ Ambiente", f"{ultima['temp_amb']}°C")
            c3.metric("💧 Umidade", f"{ultima['umidade']}%")

            # Status Dinâmico de Conexão e Alerta
            if not sistema_online:
                c4.error(f"❌ OFFLINE ({int(segundos_atraso)}s)")
                st.toast("Conexão com ESP32 perdida!", icon="⚠️")
            elif not df_alertas.empty:
                # Ajuste de fuso para o tempo do alerta também
                ts_alerta = pd.to_datetime(df_alertas.iloc[0]['created_at'])
                if ts_alerta.tzinfo is not None:
                    ultimo_alerta_time = ts_alerta.astimezone(fuso_sp).replace(tzinfo=None)
                else:
                    ultimo_alerta_time = ts_alerta - timedelta(hours=3)

                # Se o alerta aconteceu nos últimos 20 segundos, mostra em destaque
                if (agora - ultimo_alerta_time).total_seconds() < 20:
                    sev = df_alertas.iloc[0]['severidade']
                    if sev == 'CRITICAL': 
                        c4.error(f"🚨 {sev}")
                    else: 
                        c4.warning(f"⚠️ {sev}")
                else:
                    c4.success("✅ STATUS: ESTÁVEL")
            else:
                c4.success("✅ STATUS: TUDO OK")

            # --- GRÁFICO E TABELA ---
            st.divider()
            col_graf, col_tab = st.columns([2, 1])
            
            with col_graf:
                st.subheader("📈 Histórico de Temperatura")
                df_plot = df.copy()
                # Ajusta o horário do eixo X para São Paulo (subtraindo 3h do UTC do banco)
                df_plot['timestamp'] = pd.to_datetime(df_plot['timestamp']) - timedelta(hours=3)
                df_plot = df_plot.sort_values('timestamp')
                
                fig = px.line(df_plot, x='timestamp', y=['temp_liq', 'temp_amb'],
                              labels={'value': '°C', 'timestamp': 'Horário'},
                              color_discrete_map={'temp_liq': '#1f77b4', 'temp_amb': '#ff7f0e'},
                              template="plotly_white")
                fig.update_layout(legend=dict(orientation="h", yanchor="bottom", y=1.02, xanchor="right", x=1))
                st.plotly_chart(fig, use_container_width=True)
                
            with col_tab:
                st.subheader("⚠️ Últimos Alertas")
                if not df_alertas.empty:
                    # Ajusta hora dos alertas na tabela (UTC -> SP)
                    df_alertas['Hora'] = (pd.to_datetime(df_alertas['created_at']) - timedelta(hours=3)).dt.strftime('%H:%M:%S')
                    st.dataframe(df_alertas[['Hora', 'severidade', 'valor']], use_container_width=True, hide_index=True)
                else:
                    st.write("Nenhum alerta crítico registrado.")

            # --- DADOS BRUTOS ---
            with st.expander("🔍 Ver registros no Banco de Dados"):
                st.write(df.head(10))
        else:
            st.warning("Aguardando dados do ESP32... Verifique se a API está rodando.")
    except Exception as e:
        st.error(f"Erro no Dashboard: {e}")

# --- EXECUÇÃO ---
st.title("🍺 ByteBier - Dashboard de Monitoramento")
renderizar_conteudo()

if btn_atualizar:
    st.rerun()
