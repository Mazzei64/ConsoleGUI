/*  Project Name    :           ConsoleGUI Engine
*   Version         :           1.0.5.v
*   Created date    :           05/11/2021
*   Last update     :           28/11/2021
*   Author          :           Bruno Mazzei
*   Description     :   Useful and easy-to-use API for making ASCII-based console applications for
*                     the terminal console.
*   Dependencies    :           stdio.h, stdlib.h, unistd.h and string.h
*/

#ifndef __C_GUI_ENG_H__
#define __C_GUI_ENG_H__

#include <sys/select.h>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "colors.h"

#define byte unsigned char
#define word unsigned short
#define NB_ENABLE 1
#define NB_DISABLE 0
#define EXIT exit_status = 0;
#define clrscr() printf("\e[1;1H\e[2J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define SETOFF_COLORSTATE colored_state = 0;
#define SETON_COLORSTATE colored_state = 1;
#define CURSOR_SWITCH printf("\e[?25l");
#define MAX_OBJLIST_SIZE 256
#define MAX_OBJSIZE 14280
#define WIDTH 238
#define HIGHT 60
#define SCREEN_SIZE 14280
#define REFRESH_RATE defaut_refresh
#define SET_REFRESH_RATE(a)     if(a<0) {   \
                            fprintf(stderr,"\nERROR: Invalid refresh rate value. Can't be equal or less than 0.\n");   \
                            exit(1);                              \
                            }                           \
                            defaut_refresh = a;

#define DISPLAY_ON   int main(int argc, char** argv) {     \
                            CURSOR_SWITCH                       \
                            SetTerminalSTDINBlkSt(NB_ENABLE);

#define __START                displayBuffer	        \
			                            = (char*)malloc(sizeof(char) * WIDTH * HIGHT);      \
                                    while(exit_status) {                  \
                                        Refresh();                  \
                                 
#define __END                   UpdateDisplay();            \
                                                  }
#define DISPLAY_OFF    DestroyAll();           \
                       SetTerminalSTDINBlkSt(NB_DISABLE);        \
                        return 0;  }

extern char* displayBuffer;
static byte defaut_refresh = 15;
static byte exit_status = 1;
byte colored_state = 1;

typedef struct FlagsField {
    byte modified_state : 1;
    byte enabled_state : 1;
} flags_t;

typedef struct BaseObject {
    char* canva;
    word width;
    word hight;

} object_t;
typedef struct StObject {
    byte posi_x;
    byte posi_y;
    flags_t flags;
} stobject_t;
typedef struct SqrObject {
    short id;
    object_t skeleton;
    stobject_t state;
} Object;
extern object_t AppendList(Object** objectlist, byte listlen);
extern object_t AppendRight(object_t obj1, object_t obj2, int pad);
extern void Center(Object* object);
extern void DestroyAll();
extern int DestroyObject(struct SqrObject** object);
extern char Key();
extern struct SqrObject* NewObject(int width, int hight);
extern void Refresh();
extern void SetTerminalSTDINBlkSt(byte state);
extern void SetObject(struct SqrObject* object);
extern void UpdateDisplay();

#endif