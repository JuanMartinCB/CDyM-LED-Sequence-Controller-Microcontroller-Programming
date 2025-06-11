/*
 * GccApplication3.c
 *
 * Created: 31/03/2025 14:32:38
 * Author : JUAN MARTIN
 */ 

#include <avr/io.h>          // Biblioteca principal para AVR (acceso a registros)
#define F_CPU 16000000UL     // Define la frecuencia del CPU como 16MHz
#include <util/delay.h>      // Funciones de retardo
#include <stdint.h>          // Tipos de datos enteros de tamaño fijo

// Globales
uint8_t secuencia = 1;	// Controla qué secuencia se está ejecutando (1, 2 o 3)
uint8_t izquierda = 7;	// Posición del LED izquierdo para la secuencia 3 (comienza en bit 7)
uint8_t derecha = 0;	// Posición del LED derecho para la secuencia 3 (comienza en bit 0)

void mostrarSecuencia(uint8_t sec){
	if (!(PINC & (1<<PINC1))){
		PORTB = sec<<3;			//Muestro el numero de la secuencia en el puerto B (01 = 1, 10 = 2, 11 = 3) mientras esté presionado
		} else {
		PORTB = 0;				//Apago si no está presionado
	}
}

uint8_t presionaBoton(void)
{
	if(!(PINC & (1<<PINC0))) {							// Verifico si esta presionado el boton. puedo usar if ( bit_is_clear(PINC, PC0) ) { … } que comprueba si PC0 está en 0
		while (!(PINC & (1<<PINC0))) _delay_ms(20);		// Si esta presionado, espero 20 ms para el debounce
		return 1;										// Botón presionado
	}
	return 0;											// Botón no presionado
}

void configuracion(){
	
	DDRD = 0xFF;							// Configuro todo el puerto D como salida
	DDRC &= ~((1<<DDC0) | (1<<DDC1));		// Configuro los bit 0 y 1 del puerto C como entrada
	PORTC |= ((1<<PORTC0) | (1<<PORTC1));	// Habilito Pull-Up en bits 0 y 1 del puerto C
	DDRB |= ((1<<DDB3) | (1<<DDB4));		// Configuro los bits 3 y 4 del puerto B como salida
}

void secuencia1(void){
	mostrarSecuencia(secuencia);	
	PORTD = 0b10101010;				// Escribo en Port D los pares
	if(presionaBoton()) {			// Verifico si esta presionado el boton
		secuencia = 2;				//Cambio a la siguiente secuencia
		return;
	}	
	_delay_ms(100);	
	PORTD = 0b01010101;				// Escribo en Port D los impares	
	if(presionaBoton()) {			// Verifico si esta presionado el boton
		secuencia = 2;				//Cambio a la siguiente secuencia
		return;
	}	
	_delay_ms(100);
}
void secuencia2(void){
	for(uint8_t i = 0; i<8; i++){
		mostrarSecuencia(secuencia);
		PORTD = (1<<i);				//Prendo el LED en la posición i
		if(presionaBoton()) {		//Verifico si esta presionado el boton
			secuencia = 3;			//Cambio a la siguiente secuencia
			return;
		}
		_delay_ms(100);
	}
}
void secuencia3(void){
	if (secuencia == 3){
		mostrarSecuencia(secuencia);
		if (izquierda > derecha) {		//Verifico si los leds se chocan
			PORTD = (1 << izquierda);
			if(presionaBoton()) {		// Verifico si esta presionado el boton
				secuencia = 1;			//Cambio a la siguiente secuencia
				izquierda = 7;			//reinicio variables
				derecha = 0;
				return;
			}
			_delay_ms(100);				// Retardo para visualizar el encendido de los LEDs
			PORTD = (1 << derecha);
			if(presionaBoton()) {		//Verifico si esta presionado el boton
				secuencia = 1;			//Cambio a la siguiente secuencia
				izquierda = 7;			//reinicio variables
				derecha = 0;
				return;
			}
			_delay_ms(100);
			izquierda--;				//Muevo a la siguiente posición
			derecha++;
		} else {
			izquierda = 7;				//reinicio variables cuando se encuentran en el medio
			derecha = 0;
		}
	}
}

int main(void)
{
	configuracion();
    while (1) 
    {				
		switch (secuencia) {
			case 1:
				secuencia1();
				break;
			case 2:
				secuencia2();
				break;
			case 3:
				secuencia3();
				break;
		}
	}			
	return 0;
}

