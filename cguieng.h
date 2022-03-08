/*  Project Name    :           ConsoleGUI Engine
*   Version         :           1.1.0.v
*   Created date    :           05/11/2021
*   Last update     :           28/11/2021
*   Author          :           Bruno Mazzei
*   Description     :   Useful and easy-to-use API for making ASCII-based console applications for
*                     the terminal console.
*   Dependencies    :           sys/select.h, stdio.h, stdlib.h, unistd.h, string.h, termio.h, colors.h, errno.h
*/

#ifndef __C_GUI_ENG_H__
#define __C_GUI_ENG_H__

#include <sys/select.h>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "colors.h"

#define byte unsigned char
#define word unsigned short
#define ENABLE 1
#define DISABLE 0
#define clrscr() printf("\e[1;1H\e[2J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define CURSOR_SWITCH printf("\e[?25l");
#define COLOR_TAG_MASK 0x00002323 //"##\0\0" --> little endian.
#define MAX_OBJLIST_SIZE 65535
#define MAX_OBJSIZE 14280
#define WIDTH width
#define HIGHT hight
#define SCREEN_SIZE screen_size
#define DISPLAY_BUFFER_LEN displayBufferLen
#define RUNNING running
#define EXIT running = 0;
#define SET_COLORSTATE_ON colored_state = 0;
#define SET_COLORSTATE_ON colored_state = 1;
#define COLOR_STATE colored_state
#define REFRESH_RATE defaut_refresh_rate
#define SET_REFRESH_RATE(a)     if(a<=0) {   \
                            fprintf(stderr,"\nERROR: Invalid refresh rate value. Can't be equal or less than 0.\n");   \
                            exit(EXIT_FAILURE);                              \
                            }                           \
                            defaut_refresh_rate = a;            

extern char* displayBuffer;
static short width = 238;
static short hight = 60;
static short screen_size = 14280;
static short displayBufferLen = 14280;
static byte defaut_refresh_rate = 15;
static byte running = 1;
static byte colored_state = 1;

#define DISPLAY_ON   int main(int argc, char** argv) {     \
                            CURSOR_SWITCH                      \
                            SetTerminalSTDINBlkSt(ENABLE);

#define __START             if(COLOR_STATE == ENABLE)          \
                                DISPLAY_BUFFER_LEN *= (sizeof(unsigned int) + 1)        \
                            displayBuffer	        \
                                        = (char*)calloc(DISPLAY_BUFFER_LEN, sizeof(char));      \
                                while(RUNNING) {                  \
                                 
#define __END           UpdateDisplay();            \
                                              }           
                                                  
#define DISPLAY_OFF        DestroyAll();                \
                       SetTerminalSTDINBlkSt(DISABLE);        \
                        return 0;  }

typedef struct FlagsField {
    byte cached : 1;
    byte modified_state : 1;
    byte colored: 1;
} flags_t;

typedef struct BaseObject {
    char* canva;
    word width;
    word hight;
    byte* colorPath;
} object_t;
typedef struct StObject {
    byte posi_x;
    byte posi_y;
    short cachedAt;
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
extern void LoadCanvaFromFile(Object* object, const char* __PATH);
extern struct SqrObject* NewObject(int width, int hight);
extern void SetTerminalSTDINBlkSt(byte state);
extern void SetObject(struct SqrObject* object);
extern void UpdateDisplay();

#endif