#include "ripes_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SW0 (0x01)

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
        cmx = rand() % (LED_MATRIX_0_WIDTH - 2);
        cmy = rand() % (LED_MATRIX_0_HEIGHT -2);
    }while(!manzana_libre(cmx, cmy));
    
    cmnx = cmx;
    cmny = cmy;
    dibujar_manzana(cmx, cmy);
}

int main() {
    unsigned int color_cian = 0x0000FFFF; //Color para encender el led
    unsigned int apagar = 0x0000000; //Color para apagar el led
    
    //int para aprovechar signos negativos
    int x = 0;
    int y = 0;
    
    int mov_x = 0;
    int mov_y = 0;
    
    //variable para indicarnos que el usuario perdio, dependiendo el caso
    bool game_lost = false;
    
    //1 = true, para que siga funcionando el ciclo
    while (1) { 
            while ((*switch_base & SW0) && !game_lost) { //Cambiar condicion para utilizar
            spawnear_manzana();
            pintar_bloque_2x2(x, y, color_cian);
            delay();
        
        //apagamos los leds para hacer el efecto de la serpiente o el de desplazarnos
            pintar_bloque_2x2(x, y, apagar);
        
            if (*d_pad_up == 1) { mov_x = 0;  mov_y = -1; }
            if (*d_pad_do == 1) { mov_x = 0;  mov_y = 1;  }
            if (*d_pad_le == 1) { mov_x = -1; mov_y = 0;  }
            if (*d_pad_ri == 1) { mov_x = 1;  mov_y = 0;  }
    
        //Al llegar a los bordes hacemos que se regrese o rebote
            if (x + mov_x < 0 || x + mov_x > (int)LED_MATRIX_0_WIDTH - 2) {
                //Lo utilizamos como un paso hacia atras
                mov_x = -mov_x;
            }
            if (y + mov_y < 0 || y + mov_y > (int)LED_MATRIX_0_HEIGHT - 2) {
                mov_y = -mov_y;
            }    
        //ya no se depende de los botones
        //al dar otra vuelta, aumentamos/actualizamos los contadores
            x += mov_x;
            y += mov_y;
        }
        //En cuanto salimos del ciclo del juego, entramos a este if
        //reiniciamos el juego para que game_lost sea false otra vez y entramos al 1er while
        if(game_lost == true){
            game_lost = false;
            x = 0;
            y = 0;
            mov_x = 1;
            mov_y = 0;
        }
    }
    
    return 0;
} 