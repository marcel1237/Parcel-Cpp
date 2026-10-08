#!/bin/bash
# Script para executar o Parcel C++

cd "$(dirname "$0")"

if [ -f "Parcel C++" ]; then
    echo "🚀 Executando Parcel C++..."
    "./Parcel C++"
else
    echo "❌ Executável 'Parcel C++' não encontrado. Por favor, compile o projeto primeiro."
    exit 1
fi
