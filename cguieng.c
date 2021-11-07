#include<stdio.h>
#include "cguieng.h"

static void AddToList(Object* object);
static void DefragmentList(int currentId);
void DestroyAll();

static Object** objectlist;
static int objectlistCount = 0;

char* displayBuffer;

int DestroyObject(Object** object) {
	int currentId = (*object)->id;
	if(*object == NULL)
		return -1;

	if((*object)->canva != NULL) {
		free((*object)->canva);
		(*object)->canva = NULL;
	}
	free(*object);
	*object = NULL;

	DefragmentList(currentId);

	return 0;
}
Object* NewObject(int width, int hight) {
	if(objectlist == NULL)
		objectlist = (Object**)malloc(sizeof(Object*) * MAX_OBJLIST_SIZE);

	if((width < 1 || hight < 1)
	 	|| (width > WIDTH || hight > HIGHT))
		 	return NULL;

	Object* new_object = (Object*)malloc(sizeof(Object));
	new_object->canva = (char*)malloc(sizeof(char) * width * hight);
	new_object->width = width;
	new_object->hight = hight;
	new_object->id = objectlistCount + 1;
	objectlist[objectlistCount] = new_object;
	objectlistCount++;
	AddToList(new_object);
	return new_object;
}
void Refresh() {
	for (size_t i = 0; i < WIDTH * HIGHT; i++) {
		displayBuffer[i] = ' ';
	}
}
void SetObject(Object *object) {
	if(displayBuffer == NULL) {
		fprintf(stderr, "\nERROR: Display hasn't been created.\n");
		fprintf(stderr, "Try creating it by using the cguieng's macro \"CREATE_DISPLAY_BUFFER\"\n");
		return;
	}

    int realY = object->posi_y * WIDTH,
	        start = object->posi_x + realY;

	for (size_t i = 0; i < object->hight; i++) {
		for (size_t j = 0; j < object->width; j++) {
			displayBuffer[start + j] = object->canva[j * i];
		}
		start += WIDTH;
	}
}
void UpdateDisplay() {
	int i = 0;
	clrscr();
	usleep(1000000 / REFRESH_RATE);
	while (i < WIDTH * HIGHT) {
		printf("%c", displayBuffer[i]);
		i++;
	}
	fflush(stdout);
}
void DestroyAll() {
	if(displayBuffer == NULL)
		return;
	
	for (size_t i = 0; i < objectlistCount; i++) {
		free(objectlist[i]->canva);
		objectlist[i]->canva = NULL;
		free(objectlist[i]);
		objectlist[i] = NULL;
	}
	
}
/*
	Description: Auxiliary Functions Section
*/
static void AddToList(Object* object) {
	objectlist[objectlistCount] = object;
}
static void DefragmentList(int currentId) {
	for (size_t i = (currentId - 1); i < objectlistCount + 1; i++) {
		objectlist[i] = objectlist[i+1];
	}
	objectlist[objectlistCount - 1] = 0x00;
	objectlistCount--;
	for (size_t i = (currentId - 1); i < objectlistCount; i++) {
		objectlist[i]->id -= 1;
	}	
}