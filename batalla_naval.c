#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "oponente.h"

const int EXITO = 0;
const int ERROR_LECTURA = 1;
const int ERROR_ESCRITURA = 2;
const int ERROR_ABRIR_ARCHIVO = 3;
const int ERROR_ARGUMENTOS = 4;
#define MAX_FILAS 10
#define MAX_COLUMNAS 10
const char AGUA = 'A';
const char TOCADO = 'T';
const char HUNDIDO = 'H';
const char BARCO = 'B';
const char NORTE = 'N';
const char SUR = 'S';
const char ESTE = 'E';
const char OESTE = 'O';
const char FORMATO_LECTURA[] = "<%d>;<%d>;<%c>;<%d>\n";

void imprimir_instrucciones() {
    printf("Bienvenido a Batalla Naval!\n");
    printf("Instrucciones:\n");
    printf("1. El juego se juega en un tablero de 10x10.\n");
    printf("2. Cada jugador tiene 5 barcos de diferentes tamaños.\n");
    printf("3. El objetivo es hundir todos los barcos del oponente.\n");
    printf("4. Para disparar, ingresa las coordenadas (fila y columna).\n");
    printf("5. El juego termina cuando un jugador hunde todos los barcos del oponente.\n");
}

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
    int balas_aliadas_falladas;
    int balas_enemigas_acertadas;
    int balas_enemigas_falladas;
    int barcos_enemigos_hundidos;
    int barcos_aliados_sobrevivientes;
} reporte_t;

void inicializar_tablero(char tablero[MAX_FILAS][MAX_COLUMNAS])
{
    for (int i = 0; i < MAX_FILAS; i++)
    {
        for (int j = 0; j < MAX_COLUMNAS; j++)
        {
            tablero[i][j] = AGUA;
        }
    }
}

void inicializar_juego(reporte_t *reporte, juego_t *juego)
{
    reporte->balas_aliadas_acertadas = 0;
    reporte->balas_aliadas_falladas = 0;
    reporte->balas_enemigas_acertadas = 0;
    reporte->balas_enemigas_falladas = 0;
    reporte->barcos_enemigos_hundidos = 0;
    reporte->barcos_aliados_sobrevivientes = CANT_BARCOS;
    inicializar_tablero(juego->tablero_aliado);
    inicializar_tablero(juego->tablero_enemigo);
}

bool es_cantidad_argumentos_validos(int argc)
{
    return (argc == 3);
}

bool son_posiciones_iguales(coordenada_t pos1, coordenada_t pos2)
{
    return (pos1.fila == pos2.fila) && (pos1.columna == pos2.columna);
}



void mostrar_tableros(juego_t *juego)
{
    int i, j;

    // Column headers for both boards (1..10)
    printf("    ");
    for (j = 1; j <= MAX_COLUMNAS; j++)
        printf("%2d ", j);
    printf("    "); // separación entre tableros
    for (j = 1; j <= MAX_COLUMNAS; j++)
        printf("%2d ", j);
    printf("\n");

    // Rows: índice a la izquierda de cada tablero, tablero aliado a la izquierda y enemigo a la derecha
    for (i = 0; i < MAX_FILAS; i++)
    {
        // Índice y fila del tablero aliado
        printf("%2d  ", i + 1);
        for (j = 0; j < MAX_COLUMNAS; j++)
            printf("%2c ", juego->tablero_aliado[i][j]);

        // separación entre tableros
        printf("   ");

        // Índice y fila del tablero enemigo
        printf("%2d  ", i + 1);
        for (j = 0; j < MAX_COLUMNAS; j++)
            printf("%2c ", juego->tablero_enemigo[i][j]);

        printf("\n");
    }
}

bool validar_datos_barco(barco_t barco, char orientacion)
{
    if (barco.largo < 2 || barco.largo > 5)
    {
        return false;
    }
    if (orientacion != NORTE && orientacion != SUR && orientacion != ESTE && orientacion != OESTE)
    {
        return false;
    }
    //Pos minima del tablero es (1,1)??
    if (barco.posiciones[0].fila < 0 || barco.posiciones[0].fila >= MAX_FILAS || barco.posiciones[0].columna < 0 || barco.posiciones[0].columna >= MAX_COLUMNAS)
    {
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
    //NO VA CANT_BARCOS, VA Barcos_jugador[i] o i...
    if (cant_barcos_leidos != 5)
    {
        printf("Cantidad de barcos inválida. Se esperaban 5 barcos.\n");
        return false;
    }
    for (int i = 0; i < CANT_BARCOS; i++)
    {
        if (juego->barcos[i].largo == 2)
        {
            cant_largo_2++;
        }
        else if (juego->barcos[i].largo == 3)
        {
            cant_largo_3++;
        }
        else if (juego->barcos[i].largo == 4)
        {
            cant_largo_4++;
        }
        else if (juego->barcos[i].largo == 5)
        {
            cant_largo_5++;
        }
    }
    return (cant_largo_2 == 1 && cant_largo_3 == 2 && cant_largo_4 == 1 && cant_largo_5 == 1);
}

int guardar_barcos(char *ruta_barcos, juego_t *juego)
{
    int i = 0;
    bool datos_validos = true;
    FILE *archivo_barcos = fopen(ruta_barcos, "r");
    if (archivo_barcos == NULL)
    {
        printf("Error al abrir el archivo.\n");
        return ERROR_ABRIR_ARCHIVO;
    }
    int leidos = fscanf(archivo_barcos, FORMATO_LECTURA, &juego->barcos[i].posiciones[0].fila, &juego->barcos[i].posiciones[0].columna, &juego->orientaciones[i], &juego->barcos[i].largo);
    while (leidos != EOF && datos_validos)
    {
        if (!validar_datos_barco(juego->barcos[i], juego->orientaciones[i]))
        {
            datos_validos = false;
        }
        i++;
        leidos = fscanf(archivo_barcos, FORMATO_LECTURA, &juego->barcos[i].posiciones[0].fila, &juego->barcos[i].posiciones[0].columna, &juego->orientaciones[i], &juego->barcos[i].largo);
    }
    fclose(archivo_barcos);
    if (!validar_cantidad_barcos(juego, i) || !datos_validos)
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}



bool validar_posiciones_barcos(juego_t *juego)
{
    return true;
}

void liberar_memoria_barcos(juego_t *juego)
{
    for (int i = 0; i < CANT_BARCOS; i++)
    {
        if(juego->barcos[i].posiciones != NULL)
        {
            free(juego->barcos[i].posiciones);
        }
    }
}

// MODULARIZAR ESTA FUNCION
// sacar el return de error de lectura dentro del while
int cargar_barcos_en_tablero(juego_t *juego)
{
    bool no_hay_superposicion = true;
    int i = 0;

    while (i < CANT_BARCOS && no_hay_superposicion)
    {
        juego->tablero_aliado[juego->barcos[i].posiciones[0].fila][juego->barcos[i].posiciones[0].columna] = BARCO;
        coordenada_t *posiciones = malloc(sizeof(coordenada_t) * (size_t)juego->barcos[i].largo);
        if (!posiciones)
        {
            return ERROR_LECTURA;
        }
        if (juego->orientaciones[i] == NORTE)
        {
            for (int j = 1; j < juego->barcos[i].largo; j++)
            {
                if (juego->tablero_aliado[juego->barcos[i].posiciones[0].fila - j][juego->barcos[i].posiciones[0].columna] != AGUA)
                {
                    no_hay_superposicion = false;
                }
                else
                {
                    juego->tablero_aliado[juego->barcos[i].posiciones[0].fila - j][juego->barcos[i].posiciones[0].columna] = BARCO;
                }
            }
        }
        else if (juego->orientaciones[i] == SUR)
        {
            for (int j = 1; j < juego->barcos[i].largo; j++)
            {
                if (juego->tablero_aliado[juego->barcos[i].posiciones[0].fila + j][juego->barcos[i].posiciones[0].columna] != AGUA)
                {
                    no_hay_superposicion = false;
                }
                else
                {
                    juego->tablero_aliado[juego->barcos[i].posiciones[0].fila + j][juego->barcos[i].posiciones[0].columna] = BARCO;
                }
            }
        }
        else if (juego->orientaciones[i] == ESTE)
        {
            for (int j = 1; j < juego->barcos[i].largo; j++)
            {
                if (juego->tablero_aliado[juego->barcos[i].posiciones[0].fila][juego->barcos[i].posiciones[0].columna + j] != AGUA)
                {
                    no_hay_superposicion = false;
                }
                else
                {
                    juego->tablero_aliado[juego->barcos[i].posiciones[0].fila][juego->barcos[i].posiciones[0].columna + j] = BARCO;
                }
            }
        }
        else if (juego->orientaciones[i] == OESTE)
        {
            for (int j = 1; j < juego->barcos[i].largo; j++)
            {
                if (juego->tablero_aliado[juego->barcos[i].posiciones[0].fila][juego->barcos[i].posiciones[0].columna - j] != AGUA)
                {
                    no_hay_superposicion = false;
                }
                else
                {
                    juego->tablero_aliado[juego->barcos[i].posiciones[0].fila][juego->barcos[i].posiciones[0].columna - j] = BARCO;
                }
            }
        }
        i++;
    }
    return EXITO;
}

void pedir_disparo(coordenada_t *disparo)
{
    printf("Ingrese la fila de su disparo (fila y columna): ");
    scanf("%d", &disparo->fila);
    printf("Ingrese la columna de su disparo (fila y columna): ");
    scanf("%d", &disparo->columna);
}

bool es_disparo_valido(coordenada_t disparo)
{
    //Validar si la coordenada ya fue disparada antes...
    while (disparo.fila < 1 || disparo.fila > MAX_FILAS || disparo.columna < 1 || disparo.columna > MAX_COLUMNAS)
    {
        printf("Coordenadas inválidas. Ingrese nuevamente.\n");
        return false;
    }
    return true;
}

bool barco_hundido()
{
    return false;
}

bool disparo_impacta_barco()
{
    return false;
}

char evaluar_disparo()
{
    if (disparo_impacta_barco())
    {
        if (barco_hundido())
        {
            return HUNDIDO;
        }
        return TOCADO;
    }
    return AGUA;
}

void actualizar_tablero(juego_t *juego, coordenada_t disparo, char resultado)
{
    juego->tablero_enemigo[disparo.fila][disparo.columna] = resultado;
}

void realizar_disparo(juego_t *juego, oponente_t *oponente, coordenada_t disparo)
{
    char resultado = oponente_recibe_disparo(oponente, disparo);
    if (resultado == AGUA)
    {
        printf("Disparo al agua.\n");
        actualizar_tablero(juego, disparo, AGUA);
    }
    else if (resultado == TOCADO)
    {
        printf("¡Tocado!\n");
        actualizar_tablero(juego, disparo, TOCADO);
    }
    else if (resultado == HUNDIDO)
    {
        printf("¡Hundido!\n");
        actualizar_tablero(juego, disparo, HUNDIDO);
    }
}

void recibir_disparo(juego_t *juego, oponente_t *oponente)
{
    //coordenada_t disparo_oponente = oponente_realiza_disparo(oponente);
    
}

int estado_juego()
{
    return 0;
}

void jugar_juego(juego_t *juego, oponente_t *oponente)
{
    coordenada_t disparo;
    disparo.fila = -1;
    disparo.columna = -1;
    mostrar_tableros(juego);
    while (estado_juego() == 0)
    {
        pedir_disparo(&disparo);
        if (es_disparo_valido(disparo))
        {
            realizar_disparo(juego, oponente, disparo);
            printf("Turno del oponente...\n");
            recibir_disparo(juego, oponente);
        }
    }
}


//POST: Devuelve 0 si el juego se sigue jugando, 1 si el jugador ganó, -1 si el oponente ganó.


int main(int argc, char *argv[])
{
    reporte_t reporte;
    juego_t juego;
    oponente_t *oponente;
    imprimir_instrucciones();
    if (es_cantidad_argumentos_validos(argc))
    {
        
        inicializar_juego(&reporte, &juego);
        if (guardar_barcos(argv[1], &juego) != EXITO)
        {
            return ERROR_LECTURA;
        }
        if (cargar_barcos_en_tablero(&juego) != EXITO)
        {
            liberar_memoria_barcos(&juego);
            return ERROR_LECTURA;
        }
        oponente = oponente_crear(juego.barcos);
        jugar_juego(&juego, oponente);
        liberar_memoria_barcos(&juego);
        return EXITO;
    }
    else
    {
        printf("Cantidad de argumentos inválida. Se esperaban 3 argumentos.\n");
        return ERROR_ARGUMENTOS;
    }
}
