#include "cguieng.h"

static void AddToList(Object* object);
static void DefragmentList(int currentId);
//use int atexit(void(*func)(void)) to run DestroyAll when process is closed unexpectedly.
void DestroyAll();

static Object** objectlist;
static int objectlistCount = 0;

char* displayBuffer;

object_t AppendList(Object** objlist, const byte listlen) {
	char* oldcanva = NULL;
	object_t objbuff = (objlist[0])->skeleton;
	
	for (size_t i = 1; i < listlen; i++) {
		if(i > 1)
			oldcanva = objbuff.canva;

		objbuff = AppendRight(objbuff, (objlist[i])->skeleton, 0);

		if(oldcanva != NULL)
			free(oldcanva);
	}
	return objbuff;
}
//update padding
object_t AppendRight(object_t obj1, object_t obj2, int pad) {
	object_t skeleton;
    short obj1count = 0, obj2count = 0, count = 0,
	len = (obj1.width * obj1.hight) + (obj2.width * obj2.hight);
	char* result = (char*)malloc(sizeof(char) * len + 1);

	skeleton.width = obj1.width + obj2.width + pad;
	skeleton.hight = obj1.hight >= obj2.hight ? obj1.hight : obj2.hight;

    while (count < len) {
        result[count] = obj1.canva[obj1count];
        count++;
        if((obj1count + 1) % obj1.width == 0 && count >= 4) {
            for (size_t j = 0; j < obj2.width; j++) {
                if(count == len) {
                    count++;
                    break;
                }
                result[count] = obj2.canva[obj2count];
                obj2count++;
                count++;
            }
            obj1count++;
            continue;
        }
        obj1count++;
    }
	skeleton.canva = result;
    return skeleton;
}
void Center(Object* object) {
	short half_width = 0, half_hight = 0;
	half_width = WIDTH % 2 != 0 ? (WIDTH / 2) + 1 : WIDTH / 2;
	half_hight = HIGHT % 2 != 0 ? (HIGHT / 2) + 1 : HIGHT / 2;
	object->state.posi_x = object->skeleton.width % 2 != 0 ?
							half_width - ((object->skeleton.width / 2) + 1) :
							half_width - object->skeleton.width / 2;
							
	object->state.posi_y = object->skeleton.hight % 2 != 0 ?
							half_hight - ((object->skeleton.hight / 2) + 1) :
							half_hight - object->skeleton.hight / 2;
}
void DestroyAll() {
	if(displayBuffer == NULL && objectlistCount == 0)
		return;
	
	for (size_t i = 0; i < objectlistCount; i++) {
		free(objectlist[i]->skeleton.canva);
		objectlist[i]->skeleton.canva = NULL;
		free(objectlist[i]);
		objectlist[i] = NULL;
	}
	
}
int DestroyObject(Object** object) {
	int currentId = (*object)->id;
	if(*object == NULL)
		return -1;

	if((*object)->skeleton.canva != NULL) {
		free((*object)->skeleton.canva);
		(*object)->skeleton.canva = NULL;
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
	new_object->skeleton.canva = (char*)malloc(sizeof(char) * width * hight);
	new_object->skeleton.width = width;
	new_object->skeleton.hight = hight;
	new_object->id = objectlistCount + 1;
	objectlist[objectlistCount] = new_object;
	objectlistCount++;

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
		fprintf(stderr, "Try creating it by using the cguieng's macro \"TURN_ON_DISPLAY\"\n");
		exit(1);
	}

    int realY = object->state.posi_y * WIDTH,
	        start = object->state.posi_x + realY;

	int objectIndex = 0;
	for (size_t i = 0; i < object->skeleton.hight; i++) {
		for (size_t j = 0; j < object->skeleton.width; j++) {
			displayBuffer[start + j] = object->skeleton.canva[objectIndex];
			objectIndex++;
		}
		start += WIDTH;
	}
}
void UpdateDisplay() {
	int i = 0;
	gotoxy(0,0);
	usleep(1000000 / REFRESH_RATE);
	while (i < WIDTH * HIGHT) {
		printf("%c", displayBuffer[i]);
		i++;
	}
	fflush(stdout);
}
/*
	Description: Auxiliary Functions Section
*/
static void AddToList(Object* object) {
	objectlist[objectlistCount - 1] = object;
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