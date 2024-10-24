import numpy as np
from PIL import Image
import sys
import os


if len(sys.argv) == 1:
  print("Dar nombre de archivo")
  sys.exit(1)

INPUT_FILE = sys.argv[1]
FILENAME = os.path.splitext(INPUT_FILE)[0]
OUTPUT_FILE = f"{FILENAME}.PNG"

# Leer el archivo binario y convertirlo a un array de NumPy
width, height = 1024, 1024  # Asegúrate de que estas dimensiones coincidan con las usadas en el programa C
array_imagen = np.fromfile(INPUT_FILE, dtype='int32').reshape((height, width))

# Convertir el array a una imagen en escala de grises
imagen = Image.fromarray(array_imagen.astype('uint8'))

# Guardar la imagen en formato PNG
imagen.save(OUTPUT_FILE)

