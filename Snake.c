#include "ripes_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SW0 (0x01)
#define LONGITUD 200 //Constante

volatile unsigned int * led_base = (int*) LED_MATRIX_0_BASE;
volatile unsigned int * d_pad_up = (int*) D_PAD_0_UP;
volatile unsigned int * d_pad_do = (int*) D_PAD_0_DOWN;
volatile unsigned int * d_pad_le = (int*) D_PAD_0_LEFT;
volatile unsigned int * d_pad_ri = (int*) D_PAD_0_RIGHT;
volatile unsigned int * switch_base = (int*) SWITCHES_0_BASE;   
volatile unsigned int * matriz = (volatile unsigned int*)LED_MATRIX_0_BASE;
int manzana_activa = 0;
int coordenada_manzana_nueva_x = 0;
int coordenada_manzana_nueva_y = 0;

void iniciar_led(int x, int y, unsigned int color){
    int indice = y * LED_MATRIX_0_WIDTH + x;
    matriz[indice] = color;
}

//Función para pintar LED
void pintar_led(int x, int y, unsigned int color) {
    volatile unsigned int *matriz = (volatile unsigned int *)LED_MATRIX_0_BASE;
     int w = LED_MATRIX_0_WIDTH;

    matriz[y * w + x] = color; //Arriba izquierda
    matriz[y * w + (x + 1)] = color; //Arriba derecha
    matriz[(y + 1) * w + x] = color; //Abajo izquierda
    matriz[(y + 1) * w + (x + 1)] = color; // Abajo derecha
}

//Evalua si algún led de un bloque 2x2 ya tiene algún color 
bool revisar_color(int x, int y, unsigned int color){
    int c = LED_MATRIX_0_WIDTH;
    for(int cy = 0; cy < 2; cy++){
        for(int cx = 0; cx < 2; cx++){
            if (matriz[(y + cy) * c + (x + cx)] == color) {
                return true;
            }
        }
    }
    return false;
}

//Controla / ralentiza la ejecución (velocidad de la serpiente) para dar tiempo al usuario de moverse
void delay() { 
    for (volatile int i = 0; i < 40000; i++);
}

//Dibuja la manzana en las coordenadas correspondientes
void dibujar_manzana(int x, int y){
    iniciar_led(x,y,0x00FF0000); //Arriba izquierda
    iniciar_led(x+1, y, 0x00FF0000); //Arriba derecha
    iniciar_led(x, y+1, 0x00FF0000); //Abajo izquierda
    iniciar_led(x+1, y+1,0x00FF0000); //Abajo derecha
}

//Busca un lugar válido (libre) para la manzana
int manzana_libre(int x, int y){
    for(int i = 0; i<4; i++){
        int dx = i%2;
        int dy = i / 2;
        int indice = (y + dy) * LED_MATRIX_0_WIDTH + (x + dx);
        
        if(matriz[indice] != 0){
            return 0;
        }
    }
    return 1;
}

//Encargada de dibujar manzanas, tras evaluar si no hay otra en la matriz
void spawnear_manzana(void){
    if(manzana_activa){
        return;
    }
    
    int coordenada_manzana_x = 0;
    int coordenada_manzana_y = 0;
    
    do{
        //Se consiguen coordenadas pares (corrección que mencionó en clase)
        coordenada_manzana_x = rand() % ((LED_MATRIX_0_WIDTH - 2)/ 2) * 2; 
        coordenada_manzana_y = rand() % ((LED_MATRIX_0_HEIGHT -2)/ 2) * 2;
    }while(!manzana_libre(coordenada_manzana_x, coordenada_manzana_y));
    
    coordenada_manzana_nueva_x = coordenada_manzana_x;
    coordenada_manzana_nueva_y = coordenada_manzana_y;
    dibujar_manzana(coordenada_manzana_x, coordenada_manzana_y);
    manzana_activa = 1;
}

//Apaga los leds de toda la matriz
void limpiar_matriz(void) {
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
			matriz[i] = 0;
    }
} 

int main() {
    unsigned int color_snake = 0x20D618; 
    unsigned int apagar = 0x0000000; //Color "default" / leds apagados
    unsigned int manzana = 0x00FF0000;
    
     int x[LONGITUD];
     int y[LONGITUD];
     int cola = 1;
     
     int mov_x = 0;
     int mov_y = 0;
    
    bool juego_perdido = false;
    bool hay_manzana = false;
    
    //Condicional para que el juego siga activo
    while (1) { 

        //Reestablecimiento de todos los valores para reiniciar el juego
        limpiar_matriz();

            while (!(*switch_base & SW0)) {
            }     

            manzana_activa = 0;
            coordenada_manzana_nueva_x = 0;
            coordenada_manzana_nueva_y = 0;
            
            cola = 1;
            x[0] = ((LED_MATRIX_0_WIDTH / 2) / 2) * 2;
            y[0] = ((LED_MATRIX_0_HEIGHT / 2) / 2) * 2;
            mov_x = 2;
            mov_y = 0;
            juego_perdido = false;
            hay_manzana = false;
            
            //Evalua si el jugador no ha perdido y el switch cero no ha sido apagado
            while(!juego_perdido && (*switch_base & SW0)){
                spawnear_manzana();
                
                for(int i = 0; i < cola; i++){
                    pintar_led(x[i], y[i], color_snake);
                }
                
                delay();
                
                for(int i = 0; i < cola; i++){
                    pintar_led(x[i], y[i], apagar);
                }
                
                //Lee el dpad y hace que el movimiento / giro sea de dos leds (para mantener lo que comentó en clase) 
                if (*d_pad_up == 1) { mov_x = 0;  mov_y = -2; }
                if (*d_pad_do == 1) { mov_x = 0;  mov_y = 2;  }
                if (*d_pad_le == 1) { mov_x = -2; mov_y = 0;  }
                if (*d_pad_ri == 1) { mov_x = 2;  mov_y = 0;  } //2 para alinear leds y evitar que al dar vuelta se conviertan en 3 leds
                
                int next_x = x[0] + mov_x;
                int next_y = y[0] + mov_y;
                
                //Al tocar cualquiera de los bordes el jugador debe perder, evaluamos si esa condición se cumple
                if (next_x < 0 || next_x > (int)LED_MATRIX_0_WIDTH - 2) {
                    juego_perdido = true; 
                }
                if (next_y < 0 || next_y > (int)LED_MATRIX_0_HEIGHT - 2) {
                    juego_perdido = true;
                }
                //Si eso no sucede la cabeza sigue en movimiento y revisa que la misma no choque con su propio cuerpo
                else {
                    for (int i = 1; i < cola; i++) {
                        if (next_x == x[i] && next_y == y[i]) {
                            juego_perdido = true;
                            break;
                        }
                    }
                }

                //Evalua que el jugador no haya perdido, en ese caso:
                if (!juego_perdido){
                    //Guarda la posición de la cola
                    int ant_cola_x = x[cola -1]; 
                    int ant_cola_y = y[cola -1];
                    
                    //Se iguala a cero para decir que NO ha comido una manzana en esta iteración
                    hay_manzana = false; 
                    
                    //Revisa si los leds de las coordenadas siguientes son del color de la manzana
                    if (revisar_color(next_x, next_y, manzana)) {
                        //De ser así la come, por lo que debemos apagar los leds de la pocisión donde estaba la manzana
                        pintar_led(coordenada_manzana_nueva_x, coordenada_manzana_nueva_y, apagar);
                        //Al igualar a cero se triggerea la función de spawnear_manzana()
                        manzana_activa = 0;
                        //Y se declara verdadero porque comió una manzana
                        hay_manzana = true;
                    }    
                    

                    //Desplaza la parte del cuerpo / cola hacia la pocisión de la parte anterior
                    for (int i = cola - 1; i > 0; i--) { 
                        x[i] = x[i - 1];
                        y[i] = y[i - 1];
                    }
       
                    //Al dar otra vuelta, aumentamos/actualizamos los contadores
                    x[0] += mov_x; //nos movemos con la posicion de la cabeza, ese dato guardado anteriormente
                    y[0] += mov_y;
                
                    if (hay_manzana && cola < LONGITUD) {
                        x[cola] = ant_cola_x; //colocamos la nueva zona de la serpiente en su posicion anterior
                        y[cola] = ant_cola_y;
                        cola++; //aumentamos la serpiente
                    }
                }
            //Se evalua si el jugador perdió
            if(juego_perdido){
                //Si tras haber perdido el valor del switch cambia a apagado se reestablece el valor a false
                while (*switch_base & SW0) {
                }
                juego_perdido = false;
            }
        }
    }
    return 0;
}