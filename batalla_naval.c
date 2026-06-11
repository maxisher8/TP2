#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "oponente.h"

#define MAX_FILAS 10
#define MAX_COLUMNAS 10

const int MIN_FILAS = 1;
const int MIN_COLUMNAS = 1;
const int EXITO = 0;
const int ERROR_LECTURA = 1;
const int ERROR_ESCRITURA = 2;
const int ERROR_ABRIR_ARCHIVO = 3;
const int ERROR_ARGUMENTOS = 4;
const int INICIALIZACION_VALIDA = 0;
const int INICIALIZACION_INVALIDA = -1;
const int CANT_ARGS = 3;
const int LARGO_MINIMO = 2;
const int LARGO_MAXIMO = 5;
const int LARGO_3 = 3;
const int LARGO_4 = 4;
const int CANT_LARGO_2 = 1;
const int CANT_LARGO_3 = 2;
const int CANT_LARGO_4 = 1;
const int CANT_LARGO_5 = 1;
const int JUGADOR_GANO = 1;
const int OPONENTE_GANO = -1;
const int SIGUE_JUGANDO = 0;
const char AGUA = 'A';
const char TOCADO = 'T';
const char HUNDIDO = 'H';
const char BARCO = 'B';
const char DISPARO = 'X';
const char VACIO = '-';
const char NORTE = 'N';
const char SUR = 'S';
const char ESTE = 'E';
const char OESTE = 'O';
const char FORMATO_LECTURA[] = "%d;%d;%c;%d";

typedef struct juego
{
    char tablero_aliado[MAX_FILAS][MAX_COLUMNAS];
    char tablero_enemigo[MAX_FILAS][MAX_COLUMNAS];
    barco_t barcos[CANT_BARCOS];
    char orientaciones[CANT_BARCOS];
} juego_t;

typedef struct reporte
{
    int balas_aliadas_acertadas;
    int balas_aliadas_erradas;
    int balas_enemigas_acertadas;
    int balas_enemigas_erradas;
    int barcos_enemigos_hundidos;
    int barcos_aliados_sobrevivientes;
} reporte_t;

void inicializar_tablero(char tablero[MAX_FILAS][MAX_COLUMNAS])
{
    for (int i = 0; i < MAX_FILAS; i++)
    {
        for (int j = 0; j < MAX_COLUMNAS; j++)
        {
            tablero[i][j] = VACIO;
        }
    }
}

void inicializar_juego(reporte_t *reporte, juego_t *juego)
{
    reporte->balas_aliadas_acertadas = INICIALIZACION_VALIDA;
    reporte->balas_aliadas_erradas = INICIALIZACION_VALIDA;
    reporte->balas_enemigas_acertadas = INICIALIZACION_VALIDA;
    reporte->balas_enemigas_erradas = INICIALIZACION_VALIDA;
    reporte->barcos_enemigos_hundidos = INICIALIZACION_VALIDA;
    reporte->barcos_aliados_sobrevivientes = CANT_BARCOS;
    inicializar_tablero(juego->tablero_aliado);
    inicializar_tablero(juego->tablero_enemigo);
}

bool es_cantidad_argumentos_validos(int argc)
{
    return (argc == CANT_ARGS);
}

bool son_posiciones_iguales(coordenada_t pos1, coordenada_t pos2)
{
    return ((pos1.fila == pos2.fila) && (pos1.columna == pos2.columna));
}

void mostrar_tableros(juego_t *juego)
{
    printf("\n==== TU TABLERO (DEFENSA) ====       ==== TABLERO ENEMIGO (ATAQUE) ====\n");
    printf("   1 2 3 4 5 6 7 8 9 10                1 2 3 4 5 6 7 8 9 10\n");
    for (int i = 0; i < MAX_FILAS; i++)
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < MAX_COLUMNAS; j++)
        {
            printf("%c ", juego->tablero_aliado[i][j]);
        }
        printf("             %2d ", i + 1);
        for (int j = 0; j < MAX_COLUMNAS; j++)
        {
            printf("%c ", juego->tablero_enemigo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

bool validar_datos_barco(barco_t barco, char orientacion)
{
    if (barco.largo < LARGO_MINIMO || barco.largo > LARGO_MAXIMO)
    {
        printf("Largo del barco inválido: %d. Se esperaba un largo entre %d y %d.\n", barco.largo, LARGO_MINIMO, LARGO_MAXIMO);
        return false;
    }
    if (orientacion != NORTE && orientacion != SUR && orientacion != ESTE && orientacion != OESTE)
    {
        printf("Orientación del barco inválida: %c. Se esperaba 'N', 'S', 'E' o 'O'.\n", orientacion);
        return false;
    }
    bool es_valida = barco.posiciones[0].fila < MIN_FILAS || barco.posiciones[0].fila > MAX_FILAS || barco.posiciones[0].columna < MIN_COLUMNAS || barco.posiciones[0].columna > MAX_COLUMNAS;
    if (es_valida)
    {
        printf("Posición del barco inválida: (%d,%d). Se esperaba una posición dentro del tablero.\n", barco.posiciones[0].fila, barco.posiciones[0].columna);
        return false;
    }
    return true;
}

bool validar_cantidad_barcos(juego_t *juego, int cant_barcos_leidos)
{
    int cant_largo_2 = 0;
    int cant_largo_3 = 0;
    int cant_largo_4 = 0;
    int cant_largo_5 = 0;
    if (cant_barcos_leidos != CANT_BARCOS)
    {
        printf("cant_barcos_leidos: %d\n", cant_barcos_leidos);
        printf("Cantidad de barcos inválida. Se esperaban %d barcos.\n", CANT_BARCOS);
        return false;
    }
    for (int i = 0; i < CANT_BARCOS; i++)
    {
        if (juego->barcos[i].largo == LARGO_MINIMO)
        {
            cant_largo_2++;
        }
        else if (juego->barcos[i].largo == LARGO_3)
        {
            cant_largo_3++;
        }
        else if (juego->barcos[i].largo == LARGO_4)
        {
            cant_largo_4++;
        }
        else if (juego->barcos[i].largo == LARGO_MAXIMO)
        {
            cant_largo_5++;
        }
    }
    return (cant_largo_2 == CANT_LARGO_2 && cant_largo_3 == CANT_LARGO_3 && cant_largo_4 == CANT_LARGO_4 && cant_largo_5 == CANT_LARGO_5);
}

int guardar_barcos(char *ruta_barcos, juego_t *juego)
{
    int i = 0;
    bool datos_validos = true;
    int fila_temp = INICIALIZACION_INVALIDA;
    int columna_temp = INICIALIZACION_INVALIDA;
    FILE *archivo_barcos = fopen(ruta_barcos, "r");
    if (archivo_barcos == NULL)
    {
        printf("Error al abrir el archivo.\n");
        return ERROR_ABRIR_ARCHIVO;
    }
    int leidos = fscanf(archivo_barcos, FORMATO_LECTURA, &fila_temp, &columna_temp, &juego->orientaciones[i], &juego->barcos[i].largo);
    juego->barcos[i].posiciones = malloc(sizeof(coordenada_t) * (size_t)juego->barcos[i].largo);
    if(!juego->barcos[i].posiciones)
    {
        printf("Error al asignar memoria para las posiciones del barco.\n");
        fclose(archivo_barcos);
        return ERROR_LECTURA;
    }
    juego->barcos[i].posiciones[0].fila = fila_temp;
    juego->barcos[i].posiciones[0].columna = columna_temp;
    while (leidos != EOF && datos_validos && i < CANT_BARCOS - 1)
    {
        if (!validar_datos_barco(juego->barcos[i], juego->orientaciones[i]))
        {
            datos_validos = false;
        }
        i++;
        leidos = fscanf(archivo_barcos, FORMATO_LECTURA, &fila_temp, &columna_temp, &juego->orientaciones[i], &juego->barcos[i].largo);
        juego->barcos[i].posiciones = malloc(sizeof(coordenada_t) * (size_t)juego->barcos[i].largo);
        if (!juego->barcos[i].posiciones)
        {
            printf("Error al asignar memoria para las posiciones del barco.\n");
            datos_validos = false;
        }
        else{
            juego->barcos[i].posiciones[0].fila = fila_temp;
            juego->barcos[i].posiciones[0].columna = columna_temp;
        }
    }
    fclose(archivo_barcos);
    if (!validar_cantidad_barcos(juego, i + 1) || !datos_validos)
    {
        return ERROR_LECTURA;
    }

    return EXITO;
}

void liberar_memoria_barcos(juego_t *juego)
{
    for (int i = 0; i < CANT_BARCOS; i++)
    {
        if (juego->barcos[i].posiciones != NULL)
        {
            free(juego->barcos[i].posiciones);
        }
    }
}

// MODULARIZAR ESTA FUNCION
int cargar_barcos_en_tablero(juego_t *juego)
{
    bool lectura_valida = true;
    int i = 0;
    int j = 0;
    while (i < CANT_BARCOS && lectura_valida)
    {
        if (!juego->barcos[i].posiciones)
        {
            lectura_valida = false;
        }
        else
        {
            int base_fila = juego->barcos[i].posiciones[0].fila - 1;
            int base_col = juego->barcos[i].posiciones[0].columna - 1;
            int largo = juego->barcos[i].largo;

            if (juego->orientaciones[i] == NORTE)
            {
                j = 0;
                while (j < largo && lectura_valida)
                {
                    int fila = base_fila - j;
                    int col = base_col;
                    juego->barcos[i].posiciones[j].fila = fila;
                    juego->barcos[i].posiciones[j].columna = col;
                    if (fila < 0 || fila >= MAX_FILAS || col < 0 || col >= MAX_COLUMNAS || juego->tablero_aliado[fila][col] != VACIO)
                    {
                        lectura_valida = false;
                    }
                    juego->tablero_aliado[fila][col] = BARCO;
                    j++;
                }
            }
            else if (juego->orientaciones[i] == SUR)
            {
                j = 0;
                while (j < largo && lectura_valida)
                {
                    int fila = base_fila + j;
                    int col = base_col;
                    juego->barcos[i].posiciones[j].fila = fila;
                    juego->barcos[i].posiciones[j].columna = col;
                    if (fila < 0 || fila >= MAX_FILAS || col < 0 || col >= MAX_COLUMNAS || juego->tablero_aliado[fila][col] != VACIO)
                    {
                        lectura_valida = false;
                    }
                    juego->tablero_aliado[fila][col] = BARCO;
                    j++;
                }
            }
            else if (juego->orientaciones[i] == ESTE)
            {
                j = 0;
                while (j < largo && lectura_valida)
                {
                    int fila = base_fila;
                    int col = base_col + j;
                    juego->barcos[i].posiciones[j].fila = fila;
                    juego->barcos[i].posiciones[j].columna = col;
                    if (fila < 0 || fila >= MAX_FILAS || col < 0 || col >= MAX_COLUMNAS || juego->tablero_aliado[fila][col] != VACIO)
                    {
                        lectura_valida = false;
                    }
                    juego->tablero_aliado[fila][col] = BARCO;
                    j++;
                }
            }
            else if (juego->orientaciones[i] == OESTE)
            {
                j = 0;
                while (j < largo && lectura_valida)
                {
                    int fila = base_fila;
                    int col = base_col - j;
                    juego->barcos[i].posiciones[j].fila = fila;
                    juego->barcos[i].posiciones[j].columna = col;
                    if (fila < 0 || fila >= MAX_FILAS || col < 0 || col >= MAX_COLUMNAS || juego->tablero_aliado[fila][col] != VACIO)
                    {
                        lectura_valida = false;
                    }
                    juego->tablero_aliado[fila][col] = BARCO;
                    j++;
                }
            }
            i++;
        }
    }
    if (!lectura_valida)
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}

void pedir_disparo(coordenada_t *disparo)
{
    printf("Ingrese la fila y columna de su disparo en formato fila;columna: ");
    scanf("%d;%d", &disparo->fila, &disparo->columna);
}

bool coordenada_ya_disparada(juego_t juego, coordenada_t disparo)
{
    char estado_casilla = juego.tablero_enemigo[disparo.fila - 1][disparo.columna - 1];
    return ((estado_casilla == TOCADO) || (estado_casilla == HUNDIDO) || (estado_casilla == AGUA));
}

bool es_disparo_valido(juego_t juego, coordenada_t disparo)
{
    if ((disparo.fila >= MIN_FILAS) && (disparo.fila <= MAX_FILAS) && (disparo.columna >= MIN_COLUMNAS) && (disparo.columna <= MAX_COLUMNAS))
    {
        if (coordenada_ya_disparada(juego, disparo))
        {
            printf("Coordenada ya disparada. Ingrese otra.\n");
            return false;
        }
        return true;
    }
    printf("Coordenadas fuera del tablero. Ingrese nuevamente.\n");
    return false;
}

void actualizar_tablero(char tablero[MAX_FILAS][MAX_COLUMNAS], coordenada_t disparo, char resultado)
{
    tablero[disparo.fila - 1][disparo.columna - 1] = resultado;
}

void realizar_disparo(juego_t *juego, oponente_t *oponente, coordenada_t disparo, reporte_t *reporte)
{
    char resultado = oponente_recibe_disparo(oponente, disparo);
    if (resultado == AGUA)
    {
        printf("Disparo al agua.\n");
        actualizar_tablero(juego->tablero_enemigo, disparo, AGUA);
        reporte->balas_aliadas_erradas++;
    }
    else if (resultado == TOCADO)
    {
        printf("¡Tocado!\n");
        actualizar_tablero(juego->tablero_enemigo, disparo, TOCADO);
        reporte->balas_aliadas_acertadas++;
    }
    else if (resultado == HUNDIDO)
    {
        printf("¡Hundido!\n");
        actualizar_tablero(juego->tablero_enemigo, disparo, HUNDIDO);
        reporte->balas_aliadas_acertadas++;
        reporte->barcos_enemigos_hundidos++;
    }
}

bool es_hundido(juego_t *juego)
{
    bool barco_hundido = false;
    for (int i = 0; i < CANT_BARCOS; i++)
    {
        bool barco_estaba_hundido = true;
        bool pos_tocadas = true;
        for (int j = 0; j < juego->barcos[i].largo; j++)
        {
            int fila = juego->barcos[i].posiciones[j].fila;
            int col = juego->barcos[i].posiciones[j].columna;
            if (juego->tablero_aliado[fila][col] != HUNDIDO)
            {
                barco_estaba_hundido = false;
            }
            if (juego->tablero_aliado[fila][col] != TOCADO)
            {
                pos_tocadas = false;
            }
        }
        if (!barco_estaba_hundido && pos_tocadas)
        {
            for (int j = 0; j < juego->barcos[i].largo; j++)
            {
                int fila = juego->barcos[i].posiciones[j].fila;
                int col = juego->barcos[i].posiciones[j].columna;
                juego->tablero_aliado[fila][col] = HUNDIDO;
            }
            barco_hundido = true;
        }
    }
    return barco_hundido;
}

void recibir_disparo(juego_t *juego, oponente_t *oponente, reporte_t *reporte)
{
    coordenada_t disparo_oponente = oponente_realiza_disparo(oponente);
    int fila = disparo_oponente.fila;
    int col = disparo_oponente.columna;
    if (juego->tablero_aliado[fila][col] == BARCO)
    {
        printf("El oponente disparó a (%d, %d) y te tocó un barco.\n", fila + 1, col + 1);
        juego->tablero_aliado[fila][col] = TOCADO;
        reporte->balas_enemigas_acertadas++;
        if (es_hundido(juego))
        {
            printf("¡Un barco fue hundido!\n");
            reporte->barcos_aliados_sobrevivientes--;
        }
    }
    else if (juego->tablero_aliado[fila][col] == VACIO)
    {
        printf("El oponente disparó a (%d, %d) y disparó al agua.\n", fila + 1, col + 1);
        juego->tablero_aliado[fila][col] = AGUA;
        reporte->balas_enemigas_erradas++;
    }
}

bool jugador_gano(reporte_t reporte)
{
    if (reporte.barcos_enemigos_hundidos == CANT_BARCOS)
    {
        return true;
    }
    return false;
}

bool oponente_gano(reporte_t reporte)
{
    if (reporte.barcos_aliados_sobrevivientes == 0)
    {
        return true;
    }
    return false;
}

// POST: Devuelve 0 si el juego se sigue jugando, 1 si el jugador ganó, -1 si el oponente ganó.
int estado_juego(reporte_t reporte)
{
    if (jugador_gano(reporte))
    {
        printf("¡Felicidades! ¡Has ganado la partida!\n");
        return JUGADOR_GANO;
    }
    else if (oponente_gano(reporte))
    {
        printf("¡Oh no! El oponente ha ganado la partida. ¡Mejor suerte la próxima vez!\n");
        return OPONENTE_GANO;
    }
    return SIGUE_JUGANDO;
}

void jugar_juego(juego_t *juego, oponente_t *oponente, reporte_t *reporte)
{
    coordenada_t disparo;
    disparo.fila = -1;
    disparo.columna = -1;
    pedir_disparo(&disparo);
    printf("Realizando disparo a (%d, %d)...\n", disparo.fila, disparo.columna);
    if (es_disparo_valido(*juego, disparo))
    {
        realizar_disparo(juego, oponente, disparo, reporte);
        mostrar_tableros(juego);
        if (estado_juego(*reporte) == SIGUE_JUGANDO)
        {
            printf("Turno del oponente...\n");
            system("clear");
            recibir_disparo(juego, oponente, reporte);
            mostrar_tableros(juego);
        }
    }
}

void escribir_reporte(char *ruta_reporte, reporte_t reporte)
{
    FILE *archivo_reporte = fopen(ruta_reporte, "w");
    if (archivo_reporte == NULL)
    {
        printf("Error al abrir el archivo de reporte para escritura.\n");
        return;
    }
    fprintf(archivo_reporte, "Balas aliadas acertadas: %d\n", reporte.balas_aliadas_acertadas);
    fprintf(archivo_reporte, "Balas aliadas erradas: %d\n", reporte.balas_aliadas_erradas);
    fprintf(archivo_reporte, "Balas enemigas acertadas: %d\n", reporte.balas_enemigas_acertadas);
    fprintf(archivo_reporte, "Balas enemigas erradas: %d\n", reporte.balas_enemigas_erradas);
    fprintf(archivo_reporte, "Barcos enemigos hundidos: %d\n", reporte.barcos_enemigos_hundidos);
    fprintf(archivo_reporte, "Barcos aliados sobrevivientes: %d\n", reporte.barcos_aliados_sobrevivientes);
    fclose(archivo_reporte);
}

int main(int argc, char *argv[])
{
    reporte_t reporte;
    juego_t juego;
    oponente_t *oponente;
    if (es_cantidad_argumentos_validos(argc))
    {

        inicializar_juego(&reporte, &juego);
        if (guardar_barcos(argv[1], &juego) != EXITO)
        {
            printf("Error al guardar los barcos desde el archivo.\n");
            return ERROR_LECTURA;
        }
        if (cargar_barcos_en_tablero(&juego) != EXITO)
        {
            printf("Error al cargar los barcos en el tablero.\n");
            liberar_memoria_barcos(&juego);
            return ERROR_LECTURA;
        }
        oponente = oponente_crear(juego.barcos);
        mostrar_tableros(&juego);
        while (estado_juego(reporte) == SIGUE_JUGANDO)
        {
            jugar_juego(&juego, oponente, &reporte);
        }
        escribir_reporte(argv[2], reporte);
        liberar_memoria_barcos(&juego);
        oponente_destruir(oponente);
        return EXITO;
    }
    else
    {
        printf("Cantidad de argumentos inválida. Se esperaban 3 argumentos.\n");
        return ERROR_ARGUMENTOS;
    }
}
