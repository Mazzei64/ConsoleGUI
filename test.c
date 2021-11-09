#include <stdio.h>
#include "cguieng.c"

int main() {
	CREATE_DISPLAY_BUFFER

	int max_X = WIDTH - 31, max_Y = HIGHT - 15, sizeofsquare = 49 * 19,
									j = 0x31, x = 1, y = 1, toast = -1;
	Object* object1 = NewObject(49, 19);
	Object* object2 = NewObject(49, 19);
	Object* object3 = NewObject(49, 19);
	Object* object4 = NewObject(49, 19);
	Object* object5 = NewObject(49, 19);
	Object* object6 = NewObject(49, 19);

	//SET_REFRESH_RATE(1);

	int count = 0;
	while (1) {
		toast++;
		Refresh();
		for (size_t i = 0; i < sizeofsquare; i++) {
			if(i%3 == 0)
			object1->canva[i] = j;
			else
			object1->canva[i] = ' ';
		}
		object1->posi_x = x;
		object1->posi_y = y;
		SetObject(object1);
		UpdateDisplay();
		if(j < 0x39)
			j = j + 0x01;
		else
			j = 0x31;

		if(toast == 2){
			x = x + 4;
			y++;
			toast = -1;
		}
		if(x == max_X || y == max_Y) {
			x = 1;
			y = 1;
		}
		usleep(250000);
		count++;
	}

	DestroyObject(&object4);
	if(object4 == NULL)
		printf("\nOBJECT DESTROYED!\n");
	else
		printf("\nOBJECT REMAINS!!\n");

	return 0;
}