# 🍺 ByteBier - Sistema de Monitoramento e Controle de Cerveja Artesanal

**Versão do Firmware:** `v2.0.0`

Este repositório contém a solução completa para o monitoramento automatizado e controle térmico de processos de produção de cerveja artesanal. O ecossistema é composto por um firmware para ESP32, uma API de telemetria em Flask, um banco de dados PostgreSQL e um dashboard de dados em Streamlit.

---

## 📂 Estrutura de Arquivos do Repositório

Conforme estruturado no repositório, o projeto é composto por:
* `Firmware_Esp32.cpp`: Código-fonte completo do firmware para o microcontrolador ESP32.
* `API.python`: Backend em Flask que gerencia o motor de regras, limites operacionais e alertas.
* `ByteBier.sql`: Script de modelagem e criação do banco de dados relacional PostgreSQL.
* `Dashboard.python`: Interface gráfica em Streamlit para visualização em tempo real e análise de KPIs.
* `requirements.txt`: Arquivo de automação com todas as dependências Python do projeto.
* `README.md`: Este guia com as instruções obrigatórias de compilação, configuração e uso.

---

## 📦 Pré-requisitos e Instalação de Bibliotecas

### 1. Dependências do Backend e Dashboard (Python)
Para automatizar e facilitar a instalação de todas as bibliotecas Python necessárias, utilize o arquivo `requirements.txt` incluído no repositório. Abra o terminal na pasta do projeto e execute o comando abaixo:

```bash
pip install -r requirements.txt
