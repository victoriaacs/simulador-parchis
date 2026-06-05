#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "../include/comun.h"
#include "fichas.c"

int main() {
    int sockets[4][2]; 
    int pipes[4][2];   

    printf("Iniciando el Árbitro (Proceso Padre)...\n");

    for (int i = 0; i < 4; i++) {
        pipe(pipes[i]); 
        socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]); 

        if (fork() == 0) { // Creación de procesos jugadores 
            close(sockets[i][0]);
            close(pipes[i][0]);

            run_hijo(sockets[i][1], pipes[i][1], i);

            exit(0);
        }
        close(sockets[i][1]);
    }

    // Planificador Round Robin 
    for (int ronda = 0; ronda < 1; ronda++) {
        for (int i = 0; i < 4; i++) {
            Mensaje msg = {1, i, 0};
            write(sockets[i][0], &msg, sizeof(Mensaje));
            printf("Árbitro: Turno otorgado al jugador %d.\n", i);
        }
    }

    // Recolección final de procesos [cite: 12]
    for (int i = 0; i < 4; i++) {
        wait(NULL);
        printf("Árbitro: Proceso %d recolectado.\n", i);
    }

    return 0;
}