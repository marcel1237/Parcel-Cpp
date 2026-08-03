#!/bin/bash
# Script para executar o Parcel C++

# Navega para o diretório do script (pasta build)
cd "$(dirname "$0")"

if [ -f "./Parcel C++" ]; then
    echo "🚀 Iniciando Parcel C++..."
    "./Parcel C++"
else
    echo "❌ Executável './Parcel C++' não encontrado. Rode o script de build primeiro."
    exit 1
fi
