#!/usr/bin/env bash
#
# Este script se encarga de invocar los tres programas que permiten la 
# conversion de un PNG a secuencia de pixeles, se procesan esos pixeles y
# posteriormente se convierte esa secuencia de pixeles a un archivo en formato
# PNG
#
# Autor: John Sanabria - john.sanabria@correounivalle.edu.co
# Fecha: 2024-08-22
#

for INPUT_PNG in imagenes/*.png; do
    TEMP_FILE="${INPUT_PNG%.png}.bin"  # Cambia el nombre para cada imagen
    python3 fromPNG2Bin.py ${INPUT_PNG}  # Convierte PNG a BIN
    ./main ${TEMP_FILE}  # Ejecuta el programa principal con el archivo binario
    python3 fromBin2PNG.py ${TEMP_FILE}.new  # Convierte el nuevo BIN a PNG
done
