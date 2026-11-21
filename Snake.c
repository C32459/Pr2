#include "ripes_system.h"

#define SW0 (0x01)
#define SW1 (0x02)
#define SW2 (0x04)
#define SW3 (0x08)

volatile unsigned int * led_base = (int*) LED_MATRIX_0_BASE;
volatile unsigned int * d_pad_up = (int*) D_PAD_0_UP;
volatile unsigned int * d_pad_do = (int*) D_PAD_0_DOWN;
volatile unsigned int * d_pad_le = (int*) D_PAD_0_LEFT;
volatile unsigned int * d_pad_ri = (int*) D_PAD_0_RIGHT;
volatile unsigned int * switch_base = (int*) SWITCHES_0_BASE;   
// Función encargada de pintar un LED en (x, y)
void pintar_bloque_2x2(unsigned int x, unsigned int y, unsigned int color) {
    volatile unsigned int *matriz = (volatile unsigned int *)LED_MATRIX_0_BASE;
    unsigned int w = LED_MATRIX_0_WIDTH;

    matriz[y * w + x]           = color; // Arriba-izquierda
    matriz[y * w + (x + 1)]     = color; // Arriba-derecha
    matriz[(y + 1) * w + x]     = color; // Abajo-izquierda
    matriz[(y + 1) * w + (x + 1)] = color; // Abajo-derecha
}

void apagar_led(unsigned int x, unsigned int y, unsigned int apagar){
    volatile unsigned int *boton = (volatile unsigned int *)LED_MATRIX_0_BASE;
    unsigned int f = LED_MATRIX_0_WIDTH;
    
    boton[y * f + x]           = 0; // Arriba-izquierda //Desaparecen los ultimos dos
    boton[y * f + (x - 1)]     = 0; // Arriba-derecha    //leds, pero solo avanza horizontal
    boton[(y + 1) * f + x]     = 0; // Abajo-izquierda
    boton[(y + 1) * f + (x - 1)] = 0; // Abajo-derecha
}

int main() {
    unsigned int color_cian = 0x0000FFFF;
    unsigned int apagar = 0x0000000;
    
    unsigned int x = 0;
    unsigned int y = 0;
    pintar_bloque_2x2(x, y, color_cian);

    while(1){
        unsigned int mov = 0;
        if(*d_pad_up == 1 && y > 0){
            pintar_bloque_2x2(x,y,apagar);
            y--;
            mov = 1;
        }else if(*d_pad_do == 1&& y < LED_MATRIX_0_HEIGHT){
            pintar_bloque_2x2(x,y,apagar);
            y++;
            mov = 1;
        }else if(*d_pad_le == 1 && x > 0){
            pintar_bloque_2x2(x,y,apagar);
            x--;
            mov = 1;
        }else if(*d_pad_ri == 1 && x < LED_MATRIX_0_WIDTH){
            pintar_bloque_2x2(x,y,apagar);
            x++;
            mov = 1;
        }
        if(mov){
            pintar_bloque_2x2(x,y,color_cian);
        } 
    }    
    return 0;
} 
