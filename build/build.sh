#!/bin/bash
# Script para compilar o Parcel C++

# Navega para o diretório do script (pasta build)
cd "$(dirname "$0")"

echo "🛠️ Iniciando compilação..."
cmake ..
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Compilação concluída com sucesso!"
else
    echo "❌ Erro durante a compilação."
    exit 1
fi
