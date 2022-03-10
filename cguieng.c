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
	if(COLOR_STATE == ENABLE) {
		new_object->skeleton.colorPathList = (colorPath_t*)calloc(width * hight, sizeof(colorPath_t));
		new_object->skeleton.colorPathCount = 0;
	}
	new_object->skeleton.canva = (char*)malloc(sizeof(char) * width * hight);
	new_object->skeleton.width = width;
	new_object->skeleton.hight = hight;
	new_object->id = objectlistCount + 1;
	new_object->state.flags.modified_state = DISABLE;
	objectlist[objectlistCount] = new_object;
	objectlistCount++;

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
    object->skeleton.colorPathCount++;
	return 1;
}
void SetObject(Object* object) {
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
	gotoxy(0,0);
	usleep(1000000 / REFRESH_RATE);
	if(COLOR_STATE == ENABLE) {
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
	if(COLOR_STATE == ENABLE) {
		const unsigned int BLANK_TAG = 0x20202020;
		int objectIndex = 0;
		int bufferlen_debug = DISPLAY_BUFFER_LEN;
		int	start = object->state.posi_x + object->state.posi_y * WIDTH + sizeof(unsigned int), 
			old_start = objectCache[object->state.cachedAt].state.posi_x + objectCache[object->state.cachedAt].state.posi_y * WIDTH + sizeof(unsigned int);

		for (size_t i = 0; i < object->skeleton.hight; i++) {
			for (size_t j = 0; j < object->skeleton.width + 2; j++) {
				if(i == 0 && j == 0)
					*(unsigned int*)(displayBuffer + old_start + j - sizeof(unsigned int)) = BLANK_TAG;

				displayBuffer[old_start + j] = 0x20;
			}
			old_start += WIDTH;
		}
		for (size_t i = 0; i < object->skeleton.hight; i++) {
			for (size_t j = 0; j < object->skeleton.width; j++) {
				if(i == 0 && j == 0) {
					unsigned int debug = (unsigned int)((object->id << 16) + COLOR_TAG_MASK);
					*(unsigned int*)(displayBuffer + start + j - sizeof(unsigned int)) = (unsigned int)((object->id << 16) + COLOR_TAG_MASK);
					unsigned int debug_2 = *(unsigned int*)(displayBuffer + start + j - sizeof(unsigned int));
				}
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
	int objectIndex = 0;
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
		/*
			This checks if the sequence of 4 bytes up the memory, from where printed_count is currently indexing,
			is maskable by the COLOR_TAG_MASK. If it is, means that we've found a section that is related to an object
			that needs to be printed.

			Next step: now we need to parse the object's ID number, from the higher-order word from this 4 byte section
			of memory and check if there in fact is an corresponding object in the objectList with the same ID word value.
		*/
		if(*(unsigned int*)(displayBuffer + bufferIndex) & COLOR_TAG_MASK == COLOR_TAG_MASK) {
			unsigned int debug = *(unsigned int*)(displayBuffer + bufferIndex);
			// Fetches the decoded word value ID from memory.
			obj_id = *(unsigned short*)(displayBuffer + bufferIndex + sizeof(unsigned short));
			bufferIndex += sizeof(unsigned int);
			/*
				Checks if there is an object that has this ID value inside the list.

				Next step: Now we need to get the objects represented by that ID value and make the required checks to it:
				first we see if it is colored enabled. If it is not, we don't need to go through the painting routine, we might
				as well skip it and raw print it to the screen. In the other case, we need to run it through the painting routine and
				make sure it is printed and painted according to its color path structure that needs to be decoded.
			*/
			if(objectlistCount >= obj_id) {
				obj_id--;							//decrements obj_id in order to facilitated indexing the list.
				if(objectlist[obj_id]->state.flags.colored == ENABLE && objectlist[obj_id]->skeleton.colorPathCount != 0) {			// Checks if it the object is supposed to be colored or not.
					/*
						Next step: Now after going through these series of basic checks, we need to build the routine that will decode the object's
						color path and thus set the printing order for coloring each character in proper order.
					*/
				#define currentColorPath	objectlist[obj_id]->skeleton.colorPathList[color_t_count]
					/*
						Logic for printing colorful chars
					*/
					unsigned short color_t_count = 0;
					unsigned int interation = 0;
					// while interations don't reach the length of the object, run it.
					while(interation < (objectlist[obj_id]->skeleton.width * objectlist[obj_id]->skeleton.hight)) {
						/*
							Check if the start of this colorPathST is the same as where we are at now running through the object's body.
							If it is true and the size of the color path list hasn't reached its end, look at the color and paint it.
						*/
						if(currentColorPath.canva_start == (interation + 1) && objectlist[obj_id]->skeleton.colorPathCount <= color_t_count + 1) {
							PAINT_ANSI256(currentColorPath.RGB);
							for (int j = 0; j < currentColorPath.canva_end - currentColorPath.canva_start; j++) {
								PrintToScreen(printed_count);
								printed_count++;
								bufferIndex++;
								interation++;
							}
							color_t_count++;
							continue;
						}
						/*
							While we don't reach the start of the nest colorpath, continue printing the object with its standard color.
						*/
						PAINT_ANSI256(STANDARD_WHITE);
						PrintToScreen(printed_count);
						printed_count++;
						interation++;
					}
				}
			}
			PAINT_ANSI256(STANDARD_WHITE);
			continue;
		}
		PrintToScreen(printed_count);
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