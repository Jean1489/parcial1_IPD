#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Funciones placeholder para la carga y guardado de imágenes
void cargarImagen(int *imagen, int width, int height);
void guardarImagen(int *imagen, int width, int height);

// Función para aplicar un filtro simple
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height);

// Función para calcular la suma de los píxeles (como una estadística)
int calcularSumaPixeles(int *imagen, int width, int height);

char *filename;

int main(int argc, char* argv[]) {
    int width = 1024, height = 1024;
    int *imagen = (int *)malloc(width * height * sizeof(int));
    int *imagenProcesada = (int *)malloc(width * height * sizeof(int));

    if (argc != 2) {
      fprintf(stderr, "Dar un nombre de archivo de entrada");
      exit(1);
    }

    filename = argv[1];
    // Cargar la imagen (no paralelizable)
    cargarImagen(imagen, width, height);

    // Aplicar filtro (paralelizable)
    aplicarFiltro(imagen, imagenProcesada, width, height);

    // Calcular suma de píxeles (parte paralelizable)
    int sumaPixeles = calcularSumaPixeles(imagenProcesada, width, height);

    printf("Suma de píxeles: %d\n", sumaPixeles);

    // Guardar la imagen (no paralelizable)
    guardarImagen(imagenProcesada, width, height);

    free(imagen);
    free(imagenProcesada);
    return 0;
}

// Carga una imagen desde un archivo binario
void cargarImagen(int *imagen, int width, int height) {
    FILE *archivo = fopen(filename, "rb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para cargar la imagen");
        return;
    }

    size_t elementosLeidos = fread(imagen, sizeof(int), width * height, archivo);
    if (elementosLeidos != width * height) {
        perror("Error al leer la imagen desde el archivo");
    }

    fclose(archivo);
}

// Guarda una imagen en un archivo binario
void guardarImagen(int *imagen, int width, int height) {
    char *output_filename;

    output_filename = (char*)malloc(sizeof(char)*(strlen(filename) + 4));
    sprintf(output_filename,"%s.new",filename);
    FILE *archivo = fopen(output_filename, "wb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para guardar la imagen");
        return;
    }

    size_t elementosEscritos = fwrite(imagen, sizeof(int), width * height, archivo);
    if (elementosEscritos != width * height) {
        perror("Error al escribir la imagen en el archivo");
    }

    fclose(archivo);
}


//Secuencial
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


/*Paralelo1
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    // Definir el número de hilos como el número de CPUs de tu máquina
    int num_hilos = omp_get_num_procs(); 

    // Iniciar una región paralela para que cada hilo ejecute una parte del bucle
    #pragma omp parallel for num_threads(num_hilos)
    for (int i = 0; i < num_hilos; i++) {
        // Cálculo de la fila inicial y final que cada hilo procesará
        // start_row: fila inicial asignada al hilo i
        // end_row: fila final asignada al hilo i
        int start_row = (height - 2) * i / num_hilos + 1;  // Ajusta el rango para evitar bordes
        int end_row = (height - 2) * (i + 1) / num_hilos + 1; // Ajusta el rango para evitar bordes

        // Iterar sobre las filas asignadas a este hilo
        for (int y = start_row; y < end_row; y++) {
            // Iterar sobre cada columna, evitando bordes
            for (int x = 1; x < width - 1; x++) {
                int sumX = 0;
                int sumY = 0;

                // Aplicar máscaras de Sobel (Gx y Gy)
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        // Cálculo de la suma ponderada usando la máscara Gx
                        sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
                        // Cálculo de la suma ponderada usando la máscara Gy
                        sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
                    }
                }

                // Calcular la magnitud del gradiente
                int help = (sumX * sumX + sumY * sumY);
                // Usar pow para calcular la raíz cuadrada (elevando a la 1/2)
                int magnitude = (int)(pow(help, 1.0 / 2.0)); 
                // Normalizar el valor calculado a 8 bits (0-255)
                imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude;  
            }
        }
    }
}

*/

/*
Paralelo2

void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    // Calcular el número de núcleos (procesadores) disponibles en el sistema
    int num_nucleos = omp_get_num_procs(); 
    // Definir el número de hilos como el doble de los núcleos
    int num_hilos = num_nucleos * 2;

    // Iniciar una región paralela en la que cada hilo ejecutará una parte del bucle
    #pragma omp parallel for num_threads(num_hilos)
    for (int i = 0; i < num_hilos; i++) {
        // Cálculo de la fila inicial y final que cada hilo procesará
        // start_row: fila inicial asignada al hilo i
        // end_row: fila final asignada al hilo i
        int start_row = (height - 2) * i / num_hilos + 1;  // Ajusta el rango para evitar bordes
        int end_row = (height - 2) * (i + 1) / num_hilos + 1; // Ajusta el rango para evitar bordes

        // Iterar sobre las filas asignadas a este hilo
        for (int y = start_row; y < end_row; y++) {
            // Iterar sobre cada columna, evitando bordes
            for (int x = 1; x < width - 1; x++) {
                int sumX = 0;
                int sumY = 0;

                // Aplicar máscaras de Sobel (Gx y Gy)
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        // Cálculo de la suma ponderada usando la máscara Gx
                        sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
                        // Cálculo de la suma ponderada usando la máscara Gy
                        sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
                    }
                }

                // Calcular magnitud del gradiente
                int magnitude = (int)(pow(sumX * sumX + sumY * sumY, 0.5)); // Usar pow para raíz cuadrada
                // Normalizar el valor calculado a 8 bits (0-255)
                imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude;  
            }
        }
    }
}

*/


int calcularSumaPixeles(int *imagen, int width, int height) {
    int suma = 0;
    for (int i = 0; i < width * height; i++) {
        suma += imagen[i];
    }
    return suma;
}

