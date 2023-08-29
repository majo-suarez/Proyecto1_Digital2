#ifndef __DISPLAY7_H__
#define __DISPLAY7_H__

#include<Arduino.h>

extern uint8_t pinA, pinB, pinC, pinD, pinE, pinF, pinG, pindP;


//Funcion para cpnfigurar display 7 segmentos
void configurarDisplay7(uint8_t pA, uint8_t pB, uint8_t pC, uint8_t pD, uint8_t pE, uint8_t pF, uint8_t pG, uint8_t pdP);

// Funcion para desplegar valor
void desplegarValor(uint8_t valor);

// Funcion para desplegar el decimal
void desplegarPunto(boolean punto);
#endif // __DISPLAY7_H__
