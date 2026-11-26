#include "ripes_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SW0 (0x01)
#define SW1 (0x02)
#define LONGITUD 200 //Constante

volatile unsigned int * led_base = (int*) LED_MATRIX_0_BASE;
volatile unsigned int * d_pad_up = (int*) D_PAD_0_UP;
volatile unsigned int * d_pad_do = (int*) D_PAD_0_DOWN;
volatile unsigned int * d_pad_le = (int*) D_PAD_0_LEFT;
volatile unsigned int * d_pad_ri = (int*) D_PAD_0_RIGHT;
volatile unsigned int * switch_base = (int*) SWITCHES_0_BASE;   
volatile unsigned int * matriz = (volatile unsigned int*)LED_MATRIX_0_BASE;
int mactiva = 0;
int cmnx = 0;
int cmny = 0;

void iniciar_led(int x, int y, unsigned int color){
    int indice = y * LED_MATRIX_0_WIDTH + x;
    matriz[indice] = color;
}

void iniciar_snake(unsigned int color){
    iniciar_led(LED_MATRIX_0_WIDTH / 2, LED_MATRIX_0_HEIGHT / 2, color);
}

// Función para pintar un LED en (x, y)
void pintar_bloque_2x2(int x, int y, unsigned int color) {
    volatile unsigned int *matriz = (volatile unsigned int *)LED_MATRIX_0_BASE;
     int w = LED_MATRIX_0_WIDTH;

    matriz[y * w + x]           = color; // Arriba-izquierda
    matriz[y * w + (x + 1)]     = color; // Arriba-derecha
    matriz[(y + 1) * w + x]     = color; // Abajo-izquierda
    matriz[(y + 1) * w + (x + 1)] = color; // Abajo-derecha
}

bool colorear(int x, int y, unsigned int color){
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

void delay() { //Darle tiempo al usuario de moverse y que no ejecute demasiado rapido
    for (volatile int i = 0; i < 40000; i++);
}

void dibujar_manzana(int x, int y){
    iniciar_led(x,y,0x00FF0000); //arriba izquierda
    iniciar_led(x+1, y, 0x00FF0000); //arriba derecha
    iniciar_led(x, y+1, 0x00FF0000); //abajo izquierda
    iniciar_led(x+1, y+1,0x00FF0000); //abajo derrecha
}

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

void spawnear_manzana(void){

    if(mactiva){
        return;
    }
    
    int cmx = 0;
    int cmy = 0;
    
    do{
        cmx = rand() % ((LED_MATRIX_0_WIDTH - 2)/ 2) * 2; //Formula para corrimiento de bits y alinear los leds
        cmy = rand() % ((LED_MATRIX_0_HEIGHT -2)/ 2) * 2;
    }while(!manzana_libre(cmx, cmy));
    
    cmnx = cmx;
    cmny = cmy;
    dibujar_manzana(cmx, cmy);
    mactiva = 1;
}

void limpiar_matriz(void) {
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
			matriz[i] = 0;
    }
} 

int main() {
    unsigned int color_cian = 0x0000FFFF; //Color para encender el led
    unsigned int apagar = 0x0000000; //Color para apagar el led
    unsigned int manzana = 0x00FF0000;
    
    //int para aprovechar signos negativos
     int x[LONGITUD];
     int y[LONGITUD];
     int cola = 1;
     
     int mov_x = 0;
     int mov_y = 0;
    
    //variable para indicarnos que el usuario perdio, dependiendo el caso
    bool game_lost = false;
    bool habia_mzn = false;
    
    //1 = true, para que siga funcionando el ciclo
    while (1) { 
            limpiar_matriz(); //Con esto acá reiniciamos todo el juego
            while (!(*switch_base & SW0)) {
            }        
            mactiva = 0;
            cmnx = 0;
            cmny = 0;
            
            cola = 1;
            x[0] = ((LED_MATRIX_0_WIDTH / 2) / 2) * 2;
            y[0] = ((LED_MATRIX_0_HEIGHT / 2) / 2) * 2;
            mov_x = 2;
            mov_y = 0;
            game_lost = false;
            habia_mzn = false;
        
            while(!game_lost && (*switch_base & SW0)){
                spawnear_manzana();
                
                for(int i = 0; i < cola; i++){
                    pintar_bloque_2x2(x[i], y[i], color_cian);
                }
                
                delay();
                
                for(int i = 0; i < cola; i++){
                    pintar_bloque_2x2(x[i], y[i], apagar);
                }
                
                //para leer el dpad y evitarnos hacer el giro 180
                if (*d_pad_up == 1) { mov_x = 0;  mov_y = -2; }
                if (*d_pad_do == 1) { mov_x = 0;  mov_y = 2;  }
                if (*d_pad_le == 1) { mov_x = -2; mov_y = 0;  }
                if (*d_pad_ri == 1) { mov_x = 2;  mov_y = 0;  } //2 para alinear leds y evitar que al dar vuelta se conviertan en 3 leds
                
                int next_x = x[0] + mov_x;
                int next_y = y[0] + mov_y;
                
        //Al llegar a los bordes tenemos que perder, entonces al entrar al ciclo game_lost será true y terminamos el ciclo
                if (next_x < 0 || next_x > (int)LED_MATRIX_0_WIDTH - 2) {
                    game_lost = true; 
                }
                if (next_y < 0 || next_y > (int)LED_MATRIX_0_HEIGHT - 2) {
                    game_lost = true;
                }
                else {
                    for (int i = 1; i < cola; i++) { //sigue el movimiento de la cabeza y revisa que la misma no choque con su propio cuerpo
                        if (next_x == x[i] && next_y == y[i]) {
                            game_lost = true;
                            break;
                        }
                    }
                }
                if (!game_lost){
                    int ant_cola_x = x[cola -1]; //guardamos la posición de la cola
                    int ant_cola_y = y[cola -1]; //si incrementamos se vuelve a guardar el nuevo valor
                    
                    habia_mzn = false; //revisamos si se come o no la manzana en ese momento
                
                    if (colorear(next_x, next_y, manzana)) {
                        pintar_bloque_2x2(cmnx, cmny, apagar); //si se comio a la manzana la quitamos
                        mactiva = 0;//generamos otra manzana
                        habia_mzn = true; //nuestra bandera para saber si se comio la manzana o no
                    }    
                    for (int i = cola - 1; i > 0; i--) { 
                        x[i] = x[i - 1];
                        y[i] = y[i - 1];
                    }
        //ya no se depende de los botones
        //al dar otra vuelta, aumentamos/actualizamos los contadores
                    x[0] += mov_x; //nos movemos con la posicion de la cabeza, ese dato guardado anteriormente
                    y[0] += mov_y;
                
                    if (habia_mzn && cola < LONGITUD) {
                        x[cola] = ant_cola_x; //colocamos la nueva zona de la serpiente en su posicion anterior
                        y[cola] = ant_cola_y;
                        cola++; //aumentamos la serpiente
                    }
                }
        //En cuanto salimos del ciclo del juego, entramos a este if
        //reiniciamos el juego para que game_lost sea false otra vez y entramos al 1er while
            if(game_lost){
                while (*switch_base & SW0) {
                }
                game_lost = false;
            }
        }
    }
    return 0;
}