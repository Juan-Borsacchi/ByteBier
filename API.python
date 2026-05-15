from flask import Flask, request, jsonify
from datetime import datetime
import psycopg2
from flask_cors import CORS

app = Flask(__name__)
CORS(app) # Permite que o Dashboard e a API se comuniquem sem bloqueios

# ==========================================
# CONFIGURAÇÃO DO BANCO DE DADOS (ByteBier)
# ==========================================
DB_CONFIG = {
    "dbname": "ByteBier",
    "user": "postgres", # Usuário padrão do sistema
    "password": "root", # Sua senha definida: root
    "host": "localhost", # O banco está na sua máquina
    "port": "5432" # Porta padrão do PostgreSQL
}

# ==========================================
# FAIXAS DE TEMPERATURA POR ETAPA
# ==========================================
FAIXAS = {
    "mostura": {"min": 62, "max": 72, "aviso": 2},
    "fervura": {"min": 95, "max": 100, "aviso": 1},
    "fermentacao": {"min": 18, "max": 24, "aviso": 3},
    "maturacao": {"min": 0, "max": 5, "aviso": 3}
}

def verificar_alerta(cur, reading_id, grupo_id, etapa, temp_liq, faixa):
    """Lógica de alertas e severidade"""
    lim_min = faixa["min"]
    lim_max = faixa["max"]
    tolerancia = faixa["aviso"]

    if temp_liq < lim_min or temp_liq > lim_max:
        # Calcula a distância do limite para definir a gravidade
        diff = min(abs(temp_liq - lim_min), abs(temp_liq - lim_max))
        
        # Se estiver fora do limite mas dentro da tolerância = WARNING
        # Se ultrapassar a tolerância = CRITICAL
        sev = "WARNING" if diff <= tolerancia else "CRITICAL"
        
        msg = f"Alerta na etapa {etapa}: {temp_liq}°C está fora da faixa ({lim_min}-{lim_max}°C)"
        threshold_str = f"{lim_min}-{lim_max}"

        cur.execute(
            """INSERT INTO alerts (reading_id, grupo_id, tipo, severidade, mensagem, valor, threshold)
               VALUES (%s, %s, %s, %s, %s, %s, %s)""",
            (reading_id, grupo_id, "TEMP_FORA_FAIXA", sev, msg, temp_liq, threshold_str)
        )
        return True
    return False

@app.route("/api/leitura", methods=["POST"])
def receber_leitura():
    # request.get_json(silent=True) evita o erro 400 automático
    dados = request.get_json(silent=True)
    if not dados:
        print(f"[{datetime.now()}] Erro: Recebido pacote vazio ou JSON inválido.")
        return jsonify({"status": "erro", "mensagem": "JSON inválido"}), 400

    # Extração dos dados enviados pelo ESP32
    grupo_id = dados.get("grupo_id", 1)
    temp_amb = dados.get("temp_amb", 0)
    temp_liq = dados.get("temp_liq", 0)
    umidade = dados.get("umidade", 0)
    etapa = dados.get("etapa", "fermentacao").lower()

    conn = None
    try:
        conn = psycopg2.connect(**DB_CONFIG)
        cur = conn.cursor()

        # 1. Salva a leitura na tabela principal
        cur.execute(
            """INSERT INTO sensor_readings (grupo_id, temp_amb, temp_liq, umidade, etapa)
               VALUES (%s, %s, %s, %s, %s) RETURNING id""",
            (grupo_id, temp_amb, temp_liq, umidade, etapa)
        )
        reading_id = cur.fetchone()[0]

        # 2. Verifica se essa leitura deve gerar um alerta
        faixa = FAIXAS.get(etapa)
        alerta_gerado = False
        
        if faixa:
            alerta_gerado = verificar_alerta(cur, reading_id, grupo_id, etapa, temp_liq, faixa)
        
        conn.commit()
        print(f"[{datetime.now()}] Dados recebidos: Liq: {temp_liq}°C | Etapa: {etapa} | Alerta: {alerta_gerado}")

        return jsonify({
            "status": "sucesso",
            "id": reading_id,
            "alerta": alerta_gerado
        }), 201

    except Exception as e:
        if conn: conn.rollback()
        print(f"Erro ao processar no Banco: {e}")
        return jsonify({"status": "erro", "mensagem": str(e)}), 500
    finally:
        if conn: conn.close()

if __name__ == "__main__":
    # O host 0.0.0.0 permite que o ESP32 encontre o PC na rede Wi-Fi
    app.run(host="0.0.0.0", port=5000, debug=True)
