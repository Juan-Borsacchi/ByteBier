# 🍺 ByteBier - Sistema de Monitoramento e Controle de Cerveja Artesanal

**Versão do Firmware:** `v2.0.0`

Este repositório contém a solução completa para o monitoramento automatizado e controle térmico dos processos para a produção de cerveja artesanal. O ecossistema é composto por um firmware para ESP32, uma API de telemetria em Flask, um banco de dados PostgreSQL e um dashboard de dados em Streamlit.

---

# 📂 Estrutura de Arquivos do Repositório

Conforme estruturado no repositório, o projeto é composto por:

- `Firmware_Esp32.cpp` → Código-fonte completo do firmware para o microcontrolador ESP32.
- `API.py` → Backend em Flask que gerencia o motor de regras, limites operacionais e alertas.
- `ByteBier.sql` → Script de modelagem e criação do banco de dados relacional PostgreSQL.
- `Dashboard.py` → Interface gráfica em Streamlit para visualização em tempo real e análise de KPIs.
- `requirements.txt` → Arquivo com todas as dependências Python do projeto.
- `README.md` → Guia completo de instalação, configuração, compilação e execução do sistema.

---

# 🛠️ Especificações Técnicas e Pinagem (ESP32)

| Componente Periférico | Pino GPIO | Descrição Funcional |
|---|---|---|
| **Sensor DS18B20** | GPIO 5 | Monitoramento térmico do fluido/líquido |
| **Sensor DHT22** | GPIO 4 | Monitoramento térmico e higrométrico ambiente |
| **Módulo Relé** | GPIO 23 | Atuador de potência para controle de temperatura |
| **Botão Avançar (+)** | GPIO 18 | Incrementa setpoint / Avança etapa |
| **Botão Recuar (-)** | GPIO 19 | Decrementa setpoint / Recua etapa |
| **LED Wi-Fi** | GPIO 2 | Indicador visual de estações conectadas ao AP |
| **Display LCD 16x2** | I2C | Exibição local de leituras e setpoint |

---

# ⚙️ Arquitetura do Sistema

O sistema ByteBier opera em uma arquitetura distribuída composta por quatro módulos principais:

1. **Firmware ESP32**
   - Responsável pela leitura dos sensores, controle do relé, atualização do display LCD e envio de dados via HTTP.

2. **API Flask**
   - Recebe os dados enviados pelo ESP32.
   - Valida leituras.
   - Processa alertas.
   - Realiza gravações no banco PostgreSQL.

3. **Banco PostgreSQL**
   - Armazena histórico de leituras térmicas, umidade, alertas e estados operacionais.

4. **Dashboard Streamlit**
   - Exibe métricas em tempo real.
   - Atualiza gráficos automaticamente.
   - Permite acompanhamento visual do processo cervejeiro.

---

# 📦 Dependências e Pré-requisitos

## 1. Dependências do Backend e Dashboard (Python)

Para automatizar e facilitar a instalação de todas as bibliotecas Python necessárias, utilize o arquivo `requirements.txt` incluído no repositório.

Abra o terminal na pasta do projeto e execute:

```bash
pip install -r requirements.txt
```

---

## 2. Dependências do Firmware (Arduino IDE)

Para compilar o arquivo `Firmware_Esp32.cpp`, instale as seguintes bibliotecas diretamente pelo gerenciador da Arduino IDE:

**Ferramentas → Gerenciar Bibliotecas...**

### Bibliotecas Necessárias

- **OneWire** *(Jim Studt, Tom Pollard, etc.)*
- **DallasTemperature** *(Miles Burton)*
- **DHT sensor library** *(Adafruit)*
- **LiquidCrystal_I2C** *(Frank de Brabander)*

---

# 🛠️ Instruções de Compilação e Upload do Firmware

## 1. Configuração da Arduino IDE

Abra a Arduino IDE e certifique-se de que o suporte às placas ESP32 está instalado:

```text
Ferramentas → Placa → Gerenciador de Placas
```

Pesquise por:

```text
esp32 by Espressif Systems
```

Instale a versão mais recente.

---

## 2. Abertura do Código

Abra o arquivo:

```text
Firmware_Esp32.cpp
```

---

## 3. Seleção da Placa

Na Arduino IDE:

```text
Ferramentas → Placa → ESP32 Arduino → ESP32 Dev Module
```

*(ou o modelo exato utilizado no projeto)*

---

## 4. Seleção da Porta COM

Conecte o ESP32 ao computador via USB e selecione a porta correta:

```text
Ferramentas → Porta
```

---

## 5. Compilação e Upload

1. Clique em **Verificar** ✔️ para validar a compilação.
2. Clique em **Carregar** ➡️ para enviar o firmware ao ESP32.

Após o upload, abra o **Monitor Serial** para acompanhar logs de inicialização e conectividade.

---

# 🚀 Instruções de Configuração e Uso do Sistema

## 📌 Passo 1 — Inicialização do Banco de Dados

1. Abra o PostgreSQL através do:
   - pgAdmin
   - terminal `psql`
   - ou outro cliente SQL.

2. Conecte-se utilizando o usuário padrão:

```text
Usuário: postgres
Senha: root
```

3. Execute o script:

```text
ByteBier.sql
```

Esse script irá:

- Criar o banco de dados `ByteBier`
- Criar tabelas:
  - `sensor_readings`
  - `alerts`
- Configurar timezone e estrutura relacional.

---

## 📌 Passo 2 — Execução da API Flask (Backend)

Abra o terminal na pasta do projeto e execute:

```bash
python API.python
```

A API iniciará na porta padrão:

```text
http://localhost:5000
```

---

## 📌 Passo 3 — Execução do Dashboard Streamlit

Abra um segundo terminal na mesma pasta do projeto e execute:

```bash
streamlit run Dashboard.python
```

O painel abrirá automaticamente no navegador em:

```text
http://localhost:8501
```

---

## 📌 Passo 4 — Conectividade Local com o ESP32

Quando ligado, o ESP32 criará automaticamente um ponto de acesso Wi-Fi próprio:

| Configuração | Valor |
|---|---|
| **SSID** | `ByteBier_PROJET` |
| **Senha** | `12345678` |

### Conexão Obrigatória

O computador onde a API Flask e o Dashboard estão rodando deve estar conectado à rede:

```text
ByteBier_PROJET
```

Após conectado:

- O ESP32 enviará requisições HTTP locais.
- Os dados serão gravados automaticamente no PostgreSQL.
- O dashboard será atualizado em tempo real a cada 5 segundos.

---

# 📊 Funcionalidades do Sistema

## ✅ Monitoramento em Tempo Real

- Temperatura do líquido
- Temperatura ambiente
- Umidade ambiente
- Estado do relé

---

## ✅ Controle Térmico Automatizado

- Controle automático via setpoint
- Ativação/desativação do relé
- Ajuste manual com botões físicos

---

## ✅ Dashboard Inteligente

- Gráficos em tempo real
- Histórico operacional
- KPIs térmicos
- Indicadores de alerta

---

## ✅ Sistema de Alertas

- Temperatura fora da faixa
- Falha de sensores
- Instabilidade operacional
- Registro persistente no banco

---

# 🧠 Tecnologias Utilizadas

| Tecnologia | Finalidade |
|---|---|
| **ESP32** | Controle embarcado |
| **C++ / Arduino Framework** | Firmware |
| **Python** | Backend e Dashboard |
| **Flask** | API REST |
| **Streamlit** | Dashboard interativo |
| **PostgreSQL** | Banco de dados relacional |
| **HTTP REST** | Comunicação entre módulos |

---

# 🔒 Observações Importantes

- Certifique-se de que a API Flask esteja rodando antes do ESP32 iniciar o envio de dados.
- O computador deve permanecer conectado à rede do ESP32.
- O PostgreSQL deve estar ativo durante toda a operação do sistema.
- O intervalo padrão de atualização é de **5 segundos**.
