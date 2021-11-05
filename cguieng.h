#ifndef __C_GUI_ENG_H__
#define __C_GUI_ENG_H__

#include <stdlib.h>
#include <unistd.h>

#define clrscr() printf("\e[1;1H\e[2J")
#define WIDTH 238
#define HIGHT 60
#define SCREEN_SIZE 238 * 60
static int defaut_refresh = 15;
#define REFRESH_RATE defaut_refresh
#define SET_REFRESH_RATE(a)     if(a<0) {   \
                            fprintf(stderr,"\nWARNING: Invalid refresh rate value. Can't be less than 0.\n");   \
                            }                           \
                            defaut_refresh = a;

static char* displayBuffer;
#define CREATE_DISPLAY_BUFFER displayBuffer	        \
			 = (char*)malloc(sizeof(char) * WIDTH * HIGHT);

typedef struct SqrObject {
    char* canva;
    int posi_x;
    int posi_y;
    int width;
    int hight;
} Object;

extern Object* NewObject(int width, int hight);
extern void Refresh();
extern void SetObject(Object* );
extern void UpdateDisplay();

#endif