#include "cguieng.h"

static void AddToList(Object* object);
static void BuffObj(Object* object);
static void DefragmentList(int currentId);
static void DefragmentCache(int cachedAt);
static char* itoa(int __val, int base);
static void printColoredDisplay();
static void ZeroCachedObject(Object* object);
void DestroyAll();

static Object** objectlist;
static word objectlistCount = 0;
static Object objectCache [MAX_OBJLIST_SIZE];
static short objectCacheCount = 0;

char* displayBuffer;

object_t AppendList(Object** objlist, const byte listlen) { //AppendGroup
	PAINT_ANSI256(MAGENTA);
    printf("Address inside AppendList: %p\n", objectlist[11]->skeleton.colorPathList);
	object_t oldstate;
	oldstate.canva = NULL;
	char* oldcanva = NULL;
	object_t objbuff = (objlist[0])->skeleton;
	
	PAINT_ANSI256(MAGENTA);
    printf("Address before calling AppendRight in AppendList: %p\n", objectlist[11]->skeleton.colorPathList);
	for (unsigned int i = 1; i < listlen; i++) {
		if(i > 1) {
			oldstate.canva = objbuff.canva;
		}

		objbuff = AppendRight(objbuff, objlist[i]->skeleton, 0);
		
		if(oldstate.canva != NULL) {
			free(oldstate.canva);
		}
	}
	PAINT_ANSI256(MAGENTA);
    printf("Address after having called AppendRight in AppendList: %p\n", objectlist[11]->skeleton.colorPathList);
	// The return of this new object_t is what is causing the problem
	return objbuff;
}
static int JoinColorPaths(object_t* obj1, object_t* obj2) {
	if(COLOR_STATE == DISABLE)
		return 0;
	if(obj1->colorPathList == NULL || obj2->colorPathList == NULL)
		return -1;
	if(obj1->colorPathCount == 0 && obj2->colorPathCount == 0)
		return -2;
	if(obj2->colorPathCount == 0)
		return -3;
	if(obj1->colorPathCount == 0) {
		for (unsigned short i = 0; i < obj2->colorPathCount; i++) {
			obj1->colorPathList[i] = obj2->colorPathList[i];
		}
		obj1->colorPathCount = obj2->colorPathCount;
		return 2;
	}
	unsigned short obj1ColorPathCount = obj1->colorPathCount;
	for (unsigned short i = 0; i < obj2->colorPathCount; i++) {
		obj1->colorPathList[obj1ColorPathCount] = obj2->colorPathList[i];
		obj1ColorPathCount++;
	}
	obj1->colorPathCount += obj2->colorPathCount;
	return 1;
}
// TO REVISE!!!!
object_t AppendRight(object_t obj1, object_t obj2, int pad) {
	object_t skeleton;
    unsigned short obj1count = 0, obj2count = 0, count = 0,
	len = (obj1.width * obj1.hight) + (obj2.width * obj2.hight);
	char* canvaResult = (char*)malloc(sizeof(char) * len + 1);
	
	skeleton.width = obj1.width + obj2.width + pad;
	skeleton.hight = obj1.hight >= obj2.hight ? obj1.hight : obj2.hight;
	skeleton.colorPathList = (colorPath_t*)malloc(sizeof(colorPath_t) * skeleton.hight * skeleton.width);
	skeleton.colorPathCount = 0;
    while (count < len) {
        canvaResult[count] = obj1.canva[obj1count];
        count++;
        if((obj1count + 1) % obj1.width == 0 && count >= 4) {
            for (size_t j = 0; j < obj2.width; j++) {
                if(count == len) {
                    count++;
                    break;
                }
                canvaResult[count] = obj2.canva[obj2count];
                obj2count++;
                count++;
            }
            obj1count++;
            continue;
        }
        obj1count++;
    }
	if(JoinColorPaths(&skeleton, &obj1) == -1) {
		fprintf(stderr,"ERROR: NULL reference exception -- Reference to the object\'s colorPathList is non-existent in function: %s\n", __func__);
		EXIT;
	}
	if(JoinColorPaths(&skeleton, &obj2) == -1) {
		fprintf(stderr,"ERROR: NULL reference exception -- Reference to the object\'s colorPathList is non-existent in function: %s\n", __func__);
		EXIT;
	}
	skeleton.canva = canvaResult;
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
	object->state.flags.modified_state = ENABLE;
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
// Has to update object cache.
int DestroyObject(Object** object) {
	int currentId = (*object)->id,
		currentCached = (*object)->state.cachedAt;
	if(*object == NULL)
		return -1;

	if((*object)->skeleton.canva != NULL) {
		free((*object)->skeleton.canva);
		(*object)->skeleton.canva = NULL;
	}
	free(*object);
	*object = NULL;

	DefragmentList(currentId);
	DefragmentCache(currentCached);
	return 0;
}
char Key() {

	char input;

	struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);                                                                 
    FD_SET(STDIN_FILENO, &fds); 
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);

	usleep(1);
	if(FD_ISSET(STDIN_FILENO, &fds) != 0) {
		input = fgetc(stdin);
		fprintf(stderr, "\b ");
		return input;
	}
	return 0;
}
void LoadCanvaFromFile(Object* object, const char* __PATH) {
	FILE* fp;
	short MAX_SIZE = object->skeleton.width + 2;
	char buffer[MAX_SIZE];
	if((fp = fopen(__PATH, "r")) == NULL) {
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	while (fgets(buffer, MAX_SIZE, fp) != NULL) {
		if(buffer[MAX_SIZE - 2] == '\n')
			buffer[MAX_SIZE - 2] = '\0';
		
		strcat(object->skeleton.canva, buffer);
	}
	fclose(fp);
}
Object* NewObject(int width, int hight) {
	if(objectlist == NULL)
		objectlist = (Object**)malloc(sizeof(Object*) * MAX_OBJLIST_SIZE);

	if((width < 1 || hight < 1)
	 	|| (width > WIDTH || hight > HIGHT))
		 	return NULL;

	Object* new_object = (Object*)malloc(sizeof(Object));
	new_object->skeleton.canva = (char*)malloc(sizeof(char) * width * hight);
	if(COLOR_STATE == ENABLE) {
		new_object->skeleton.colorPathList = (colorPath_t*)calloc(width * hight, sizeof(colorPath_t));
		new_object->skeleton.colorPathCount = 0;
	}
	new_object->skeleton.width = width;
	new_object->skeleton.hight = hight;
	new_object->state.flags.modified_state = DISABLE;
	objectlist[objectlistCount] = new_object;
	objectlistCount++;
	new_object->id = objectlistCount;
	return new_object;
}
int PaintObject(Object* object, byte RGB) {
	byte some = COLOR_STATE;
	if(object == NULL || RGB > 255 || RGB < 0 || COLOR_STATE == DISABLE) {
		return 0;
	}

	colorPath_t clockColorPath;
    clockColorPath.canva_start = 1;
    clockColorPath.canva_end = object->skeleton.hight * object->skeleton.width;
    clockColorPath.RGB = RED;
    object->skeleton.colorPathList[0] = clockColorPath;
	object->state.flags.colored = ENABLE;
    object->skeleton.colorPathCount++;
	printf("Color Path Address at PaintObject: %p\n", object->skeleton.colorPathList);
	fflush(stdout);
	return 1;
}
void SetObject(Object* object) {
	printf("Adress in SetObject: %p\n", objectlist[11]->skeleton.colorPathList);
	if(object->skeleton.width == objectlist[9]->skeleton.hight) {
		printf("Same Object in SetObject.\n");
	}
	static byte begin = DISABLE;
	if(displayBuffer == NULL) {
		fprintf(stderr, "\nERROR: Display hasn't been created.\n");
		fprintf(stderr, "Try creating it by using the cguieng's macro \"DISPLAY_ON\"\n");
		EXIT
		return;
	}
	if(begin == DISABLE) {
		for (size_t i = 0; i < DISPLAY_BUFFER_LEN; i++) {
			displayBuffer[i] = ' ';
		}
		begin = ENABLE;
	}
	if(object->state.flags.cached == DISABLE) {
		objectCache[objectCacheCount] = *object;
		object->state.cachedAt = objectCacheCount;
		objectCacheCount++;
		object->state.flags.cached = ENABLE;
	}
	if (object->state.flags.cached == ENABLE) {
		if(object->state.flags.modified_state == ENABLE) {
			BuffObj(object);
			object->state.flags.modified_state = DISABLE;
		}	
	}
}
void SetTerminalSTDINBlkSt(byte state) {
	struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);

    if (state==ENABLE) {
        ttystate.c_lflag &= ~ICANON & ~ECHO;
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==DISABLE) {
        ttystate.c_lflag |= ICANON | ECHO;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
void UpdateDisplay() {
	int i = 0;
	// gotoxy(0,0);
	usleep(1000000 / REFRESH_RATE);
	if(COLOR_STATE == ENABLE) {
		printf("Adress before calling printColoredDisplay: %p\n", objectlist[11]->skeleton.colorPathList);
		printColoredDisplay();
	}
	while (i < SCREEN_SIZE) {
		PrintToScreen(i);
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
//UNTESTED
static void BuffObj(Object* object) {
	printf("Adress in BuffObj: %p\n", objectlist[11]->skeleton.colorPathList);
	int objectIndex = 0;
	if(COLOR_STATE == ENABLE) {
		const unsigned int BLANK_TAG = (unsigned int)0x20202020;
		int bufferlen_debug = DISPLAY_BUFFER_LEN;
		int	start = object->state.posi_x + object->state.posi_y * WIDTH + sizeof(unsigned int), 
			old_start = objectCache[object->state.cachedAt].state.posi_x + objectCache[object->state.cachedAt].state.posi_y * WIDTH + sizeof(unsigned int);

		*(unsigned int*)(displayBuffer + old_start - sizeof(unsigned int)) = BLANK_TAG;

		for (size_t i = 0; i < object->skeleton.hight; i++) {
			for (size_t j = 0; j < object->skeleton.width + sizeof(unsigned short); j++) {
				displayBuffer[old_start + j] = 0x20;
			}
			old_start += WIDTH;
		}

		*(unsigned int*)(displayBuffer + start - sizeof(unsigned int)) = (unsigned int)((object->id << 16) + COLOR_TAG_MASK);

		for (size_t i = 0; i < object->skeleton.hight; i++) {
			for (size_t j = 0; j < object->skeleton.width; j++) {
				displayBuffer[start + j] = object->skeleton.canva[objectIndex];
				objectIndex++;
			}
			// Wrong... new buffer is 5 times the area of the last one...
			start += WIDTH;
		}
		objectCache[object->state.cachedAt].state.posi_x = object->state.posi_x;
		objectCache[object->state.cachedAt].state.posi_y = object->state.posi_y;
		objectCache[object->state.cachedAt].skeleton.hight = object->skeleton.hight;
		objectCache[object->state.cachedAt].skeleton.width = object->skeleton.width;

		return;
	}
	int	start = object->state.posi_x + object->state.posi_y * WIDTH,
		old_start = objectCache[object->state.cachedAt].state.posi_x + objectCache[object->state.cachedAt].state.posi_y * WIDTH;
	for (size_t i = 0; i < object->skeleton.hight; i++) {
		for (size_t j = 0; j < object->skeleton.width + 2; j++) {
			displayBuffer[old_start + j] = ' ';
		}
		old_start += WIDTH;
	}
	for (size_t i = 0; i < object->skeleton.hight; i++) {
		for (size_t j = 0; j < object->skeleton.width; j++) {
			displayBuffer[start + j] = object->skeleton.canva[objectIndex];
			objectIndex++;
		}
		start += WIDTH;
	}
	objectCache[object->state.cachedAt].state.posi_x = object->state.posi_x;
	objectCache[object->state.cachedAt].state.posi_y = object->state.posi_y;
	objectCache[object->state.cachedAt].skeleton.hight = object->skeleton.hight;
	objectCache[object->state.cachedAt].skeleton.width = object->skeleton.width;
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
static void DefragmentCache(int cachedAt) {
	for (size_t i = (cachedAt - 1); i < objectCacheCount + 1; i++) {
		objectCache[i] = objectCache[i+1];
	}
	ZeroCachedObject(&objectCache[objectCacheCount - 1]);
	objectCacheCount--;
	for (size_t i = (cachedAt - 1); i < objectCacheCount; i++) {
		objectCache[i].state.cachedAt -= 1;
	}	
}
/*
	I think it would be best and more efficient if you created an encoded display buffer for reference the actual buffer... this isn't going well...
*/
static void printColoredDisplay() {
	int bufferIndex = sizeof(unsigned int),printed_count = 0;
	unsigned short obj_id;
	while(printed_count < SCREEN_SIZE) {
	
		if(*(unsigned int*)(displayBuffer + bufferIndex) & COLOR_TAG_MASK == COLOR_TAG_MASK) {
			unsigned int debug = *(unsigned int*)(displayBuffer + bufferIndex);

			obj_id = *(unsigned short*)(displayBuffer + bufferIndex + sizeof(unsigned short));
			bufferIndex += sizeof(unsigned int);
			
			if(objectlistCount >= obj_id) {
				obj_id -= 1;							
				if(objectlist[obj_id]->state.flags.colored == ENABLE && objectlist[obj_id]->skeleton.colorPathCount != 0) {
					
				#define currentColorPath	objectlist[obj_id]->skeleton.colorPathList[color_t_count]
					
					
					
					unsigned short color_t_count = 0;
					unsigned int interation = 0;
					while(interation < (objectlist[obj_id]->skeleton.width * objectlist[obj_id]->skeleton.hight)) {
						if(objectlist[obj_id]->skeleton.colorPathList != NULL) {
							printf("path is not null\n");
							printf("Decoded ID(obj_id): %d\n", obj_id);
							printf("Objects ID by decoded ID: %d\n", objectlist[obj_id]->id);
							printf("Objects Width by decoded ID: %d\n", objectlist[obj_id]->skeleton.width);
							colorPath_t* colorPathAddrs = objectlist[obj_id]->skeleton.colorPathList;
							printf("Color Path Address: %p\n", colorPathAddrs);
							colorPath_t currentPath = *(objectlist[obj_id]->skeleton.colorPathList);
						}
						int debug = currentColorPath.canva_start;	// Segmentation fault
						int debug_2 = objectlist[obj_id]->skeleton.colorPathCount;
						if(currentColorPath.canva_start == (interation + 1) && objectlist[obj_id]->skeleton.colorPathCount <= color_t_count + 1) {
							// PAINT_ANSI256(currentColorPath.RGB);
							for (int j = 0; j < currentColorPath.canva_end - currentColorPath.canva_start; j++) {
								// PrintToScreen(printed_count);
								printed_count++;
								bufferIndex++;
								interation++;
							}
							color_t_count++;
							continue;
						}
						
						// PAINT_ANSI256(STANDARD_WHITE);
						// PrintToScreen(printed_count);
						printed_count++;
						interation++;
					}
				}
			}
			// PAINT_ANSI256(STANDARD_WHITE);
			continue;
		}
		// PrintToScreen(printed_count);
		printed_count++;
		bufferIndex++;
	}
}

static void ZeroCachedObject(Object* object) {
	object->id = 0;
	object->skeleton.canva = NULL;
	object->skeleton.hight = 0;
	object->skeleton.width = 0;
	object->state.posi_x = 0;
	object->state.posi_y = 0;
	object->state.flags.cached = 0;
	object->state.flags.modified_state = 0;
	object->state.cachedAt = 0;
}