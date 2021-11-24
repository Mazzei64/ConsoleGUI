/*  Project Name    :           ConsoleGUI Engine
*   Version         :           1.0.5.v
*   Created date    :           05/11/2021
*   Last update     :           23/11/2021
*   Author          :           Bruno Mazzei
*   Description     :   Useful and easy-to-use API for making ASCII-based console applications for
*                     the terminal console.
*   Dependencies    :           stdio.h, stdlib.h, unistd.h and string.h
*/

#ifndef __C_GUI_ENG_H__
#define __C_GUI_ENG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define byte unsigned char
#define clrscr() printf("\e[1;1H\e[2J")
#define CURSOR_SWITCH printf("\e[?25l");
#define MAX_OBJLIST_SIZE 256
#define WIDTH 238
#define HIGHT 60
#define SCREEN_SIZE 238 * 60
#define REFRESH_RATE defaut_refresh
#define SET_REFRESH_RATE(a)     if(a<0) {   \
                            fprintf(stderr,"\nERROR: Invalid refresh rate value. Can't be equal or less than 0.\n");   \
                            exit(1);                              \
                            }                           \
                            defaut_refresh = a;

#define DISPLAY_ON   int main(int argc, char** argv) {     \
                            CURSOR_SWITCH
#define __START                displayBuffer	        \
			                            = (char*)malloc(sizeof(char) * WIDTH * HIGHT);      \
                                    while(1) {                  \
                                        Refresh();                  \
                                 
#define __END                   UpdateDisplay();            \
                                                  }
#define DISPLAY_OFF    DestroyAll();           \
                        return 0;  }

extern char* displayBuffer;
static int defaut_refresh = 15;
typedef struct BaseObject {
    char* canva;
    short width;
    short hight;
} object_t;
typedef struct SqrObject {
    short id;
    object_t skeleton;
    short posi_x;
    short posi_y;
} Object;

extern object_t AppendList(Object** objectlist, byte listlen);
extern object_t AppendRight(object_t obj1, object_t obj2, int pad);
extern void Center(Object* object);
extern void DestroyAll();
extern int DestroyObject(struct SqrObject** object);
extern struct SqrObject* NewObject(int width, int hight);
extern void Refresh();
extern void SetObject(struct SqrObject* object);
extern void UpdateDisplay();

#endif