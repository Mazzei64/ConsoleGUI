#include <stdio.h>
#include "cguieng.h"

int DestroyObject(Object** object) {
	if(*object == NULL)
		return -1;
	if((*object)->canva != NULL) {
		free((*object)->canva);
		(*object)->canva = NULL;
	}
	
	free(*object);
	*object = NULL;
	return 0;
}
Object* NewObject(int width, int hight) {
	if((width < 1 || hight < 1)
	 	|| (width > WIDTH || hight > HIGHT))
		 	return NULL;

	Object* new_object = (Object*)malloc(sizeof(Object));
	new_object->canva = (char*)malloc(sizeof(char) * width * hight);
	new_object->width = width;
	new_object->hight = hight;
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