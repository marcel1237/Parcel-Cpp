#!/bin/bash
# Script para compilar o Parcel C++

echo "🛠️ Iniciando compilação..."
cmake ..
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Compilação concluída com sucesso!"
else
    echo "❌ Erro durante a compilação."
    exit 1
fi
