#ifndef COMUN_H
#define COMUN_H

 //protocolo de comunicacion
typedef struct {
    int tipo_mensaje; // 1: Turno, 2: Resultado, 3: Fin de juego
    int id_jugador;
    int dado;
} Mensaje;

#endif