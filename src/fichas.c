#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/socket.h>
#include "../include/comun.h"

typedef struct{
    int id_ficha;
    int id_jugador;
} DatosFicha;

static pthread_mutex_t mutex_fichas = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_turno = PTHREAD_COND_INITIALIZER;
static int dado_global = 0;
static int turno_activo = 0;

typedef struct{
    int movimientos_totales;
} Estadisticas;

void* logica_ficha(void* arg){
    DatosFicha* ficha = (DatosFicha*) arg;

    pthread_mutex_lock(&mutex_fichas);
    while(!turno_activo){
        pthread_cond_wait(&cond_turno, &mutex_fichas);
    }

    printf(" [Jugador %d | Ficha %d]: dado = %d, quiero moverme.\n",
            ficha->id_jugador, ficha->id_ficha, dado_global);
    
    pthread_mutex_unlock(&mutex_fichas);

    return NULL;
}

void run_hijo(int socket_hijo, int pipe_estructura, int id_jugador){
    srand(time(NULL) ^ getpid());

    pthread_t hilos[4];
    DatosFicha fichas[4];

    for(int i=0; i<4; i++){
        fichas[i].id_ficha = i;
        fichas[i].id_jugador = id_jugador;
        pthread_create(&hilos[i], NULL, logica_ficha, &fichas[i]);
    }

    Mensaje msg;
    if(read(socket_hijo, &msg, sizeof(Mensaje)) > 0){
        printf("Jugador %d: turno recibido.\n", id_jugador);

        int dado = rand() % 6 + 1;
        printf("Jugador %d: saque un %d.\n", id_jugador, dado);

        pthread_mutex_lock(&mutex_fichas);
        dado_global = dado;
        turno_activo = 1;
        pthread_cond_broadcast(&cond_turno);
        pthread_mutex_unlock(&mutex_fichas);

        for(int i=0; i<4; i++){
            pthread_join(hilos[i], NULL);
        }

        Mensaje respuesta ={2, id_jugador, dado};
        write(socket_hijo, &respuesta, sizeo(Mensaje));
    }

    Estadisticas stats = {1};
    write(pipe_estructura, &stats, sizeof(Estadisticas));

    pthread_mutex_destroy(&mutex_fichas);
    pthread_cond_destroy(&cond_turno);
    close(socket_hijo);
    close(pipe_estructura);
}
