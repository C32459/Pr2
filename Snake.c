#include "ripes_system.h"

#define SW0 (0x01)

volatile unsigned int * led_base = (int*) LED_MATRIX_0_BASE;
volatile unsigned int * d_pad_up = (int*) D_PAD_0_UP;
volatile unsigned int * d_pad_do = (int*) D_PAD_0_DOWN;
volatile unsigned int * d_pad_le = (int*) D_PAD_0_LEFT;
volatile unsigned int * d_pad_ri = (int*) D_PAD_0_RIGHT;
volatile unsigned int * switch_base = (int*) SWITCHES_0_BASE;   

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

int main() {
    unsigned int color_cian = 0x0000FFFF; //Color para encender el led
    unsigned int apagar = 0x0000000; //Color para apagar el led
    
    //int para aprovechar signos negativos
    int x = 0;
    int y = 0;
    
    int mov_x = 0;
    int mov_y = 0;

    while (1) { //Cambiar condicion para utilizar

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
   
    return 0;
} #include "ripes_system.h"

#define SW0 (0x01)

volatile unsigned int * led_base = (int*) LED_MATRIX_0_BASE;
volatile unsigned int * d_pad_up = (int*) D_PAD_0_UP;
volatile unsigned int * d_pad_do = (int*) D_PAD_0_DOWN;
volatile unsigned int * d_pad_le = (int*) D_PAD_0_LEFT;
volatile unsigned int * d_pad_ri = (int*) D_PAD_0_RIGHT;
volatile unsigned int * switch_base = (int*) SWITCHES_0_BASE;   

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

int main() {
    unsigned int color_cian = 0x0000FFFF; //Color para encender el led
    unsigned int apagar = 0x0000000; //Color para apagar el led
    
    //int para aprovechar signos negativos
    int x = 0;
    int y = 0;
    
    int mov_x = 0;
    int mov_y = 0;

    while (1) { //Cambiar condicion para utilizar

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
   
    return 0;
} 
