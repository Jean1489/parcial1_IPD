#include <stdio.h>
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


void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    // Código que aplica un filtro a cada píxel (paralelizable)
    for (int i = 0; i < width * height; i++) {
        imagenProcesada[i] = imagen[i] / 2;  // Ejemplo de operación de filtro
    }
}

int calcularSumaPixeles(int *imagen, int width, int height) {
    int suma = 0;
    for (int i = 0; i < width * height; i++) {
        suma += imagen[i];
    }
    return suma;
}

