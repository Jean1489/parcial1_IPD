# Procesamiento de imágenes

Este directorio contiene un conjunto de códigos que permiten aplicar un conjunto de filtros básicos a una imagen usando el lenguaje C.
Los filtros que se pueden aplicar son:

- Filtro de desenfoque.
- Filtro de detección de bordes.
- Filtro de Realce. 

El programa `main.c` contiene el código para aplicar estos filtros.
Este programa lee la información de los bits que representan una imagen y hace las transformaciones necesarias.
Se han desarrollado dos scripts en Python que permiten extraer los bits de información de una imagen en formato PNG (`fromPNG2Bin.py`) y toma un conjunto de bits y los almacena de regreso en una imagen en formato PNG (`fromBin2PNG.py`).

Se ha desarrollado un script en Bash llamado `all.sh` y el cual integra los códigos descritos anteriormente para aplicar un filtro a una imagen. 

El `Makefile` permite la compilación y ejecución de los códigos/archivos descritos anteriormente.

- `make all` permite la compilación y la ejecución del programa.
- `make compile` permite la compilación del programa `main.c`.
- `make clean` borra archivos creados durante la compilación y la ejecución del programa.

## Descripción 

El script `all.sh` usa tres programas le aplican un filtro a una imagen PNG de 1024x1024.

El script `all.sh` contiene lo siguiente:

```
#!/usr/bin/env bash
INPUT_PNG="image.png"
TEMP_FILE="image.bin"
python3 fromPNG2Bin.py ${INPUT_PNG}
./main ${TEMP_FILE}
python3 fromBin2PNG.py ${TEMP_FILE}.new
```

Los tres programas que se usan son `fromPNG2Bin.py`, `fromBin2PNG.py` y `main`. 
En este caso se asume que `image.png` es una imagen PNG de 1024x1024.
Lo que hace el script `fromPNG2Bin.py` es convertir la imagen de formanto PNG a una secuencia de pixeles.
Esa secuencia de pixeles queda almacenada en `image.bin` (`${TEMP_FILE}`).
Sobre los datos en `image.bin` se aplica un filtro que está en el archivo `./main`. 
Al terminar la ejecución del programa `main` se genera un archivo en este caso llamado `image.bin.new`.
El archivo `image.bin.new` es pasado al script `fromBin2PNG.py` y genera un nuevo archivo llamado `image.bin.PNG`. 
Este archivo es el archivo que contiene el PNG alterado.

## Otros posibles filtros

Para usar los filtros que se presentan a continuación se debe cambiar la función `aplicarFiltro` que se encuentra en el programa `main.c`.

### Filtro de desenfoque (Blur Filter)

```
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    // Recorre cada píxel de la imagen (excepto los bordes)
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            // Promedia los valores de los píxeles vecinos
            int sum = 0;
            sum += imagen[(y - 1) * width + (x - 1)];  // Superior Izquierda
            sum += imagen[(y - 1) * width + x];        // Superior
            sum += imagen[(y - 1) * width + (x + 1)];  // Superior Derecha
            sum += imagen[y * width + (x - 1)];        // Izquierda
            sum += imagen[y * width + x];              // Centro
            sum += imagen[y * width + (x + 1)];        // Derecha
            sum += imagen[(y + 1) * width + (x - 1)];  // Inferior Izquierda
            sum += imagen[(y + 1) * width + x];        // Inferior
            sum += imagen[(y + 1) * width + (x + 1)];  // Inferior Derecha

            imagenProcesada[y * width + x] = sum / 9;  // Asigna el promedio al píxel procesado
        }
    }
}

```

### Filtro de detección de bordes (Edge Detection) - Filtro Sobel

```
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0;
            int sumY = 0;

            // Aplicar máscaras de Sobel (Gx y Gy)
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
                    sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
                }
            }

            // Calcular magnitud del gradiente
            int magnitude = abs(sumX) + abs(sumY);
            imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude;  // Normalizar a 8 bits
        }
    }
}

```

### Filtro de Realce (Sharpen Filter)

```
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum = 0;

            // Aplicar kernel de realce
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += imagen[(y + ky) * width + (x + kx)] * kernel[ky + 1][kx + 1];
                }
            }

            // Clampeo del valor para que esté entre 0 y 255
            imagenProcesada[y * width + x] = (sum > 255) ? 255 : (sum < 0) ? 0 : sum;
        }
    }
}

```

