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
static int objectlistCount = 0;
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
	if(COLOR_STATE == ENABLE)
		new_object->skeleton.colorPath = (colorPath_t*)calloc(width * hight, sizeof(colorPath_t));	
	new_object->skeleton.canva = (char*)malloc(sizeof(char) * width * hight);
	new_object->skeleton.width = width;
	new_object->skeleton.hight = hight;
	new_object->id = objectlistCount + 1;
	new_object->state.flags.modified_state = DISABLE;
	objectlist[objectlistCount] = new_object;
	objectlistCount++;

	return new_object;
}
void SetObject(Object* object) {
	static byte begin = DISABLE;
	if(displayBuffer == NULL) {
		fprintf(stderr, "\nERROR: Display hasn't been created.\n");
		fprintf(stderr, "Try creating it by using the cguieng's macro \"DISPLAY_ON\"\n");
		exit(EXIT_FAILURE);
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
//UNTESTED
static void BuffObj(Object* object) {
	if(COLOR_STATE == ENABLE) {
		const unsigned int BLANK_TAG = 0x20202020;
		int objectIndex = 0;
		int	start = object->state.posi_x + object->state.posi_y * WIDTH + sizeof(unsigned int), 
			old_start = objectCache[object->state.cachedAt].state.posi_x + objectCache[object->state.cachedAt].state.posi_y * WIDTH + sizeof(unsigned int);

		for (size_t i = 0; i < object->skeleton.hight; i++) {
			for (size_t j = 0; j < object->skeleton.width + 2; j++) {
				if(i == 0 && j == 0)
					*(unsigned int*)(displayBuffer + old_start + j - sizeof(unsigned int)) = *(unsigned int*)BLANK_TAG;

				displayBuffer[old_start + j] = 0x20;
			}
			old_start += WIDTH;
		}
		for (size_t i = 0; i < object->skeleton.hight; i++) {
			for (size_t j = 0; j < object->skeleton.width; j++) {
				if(i == 0 && j == 0)
					*(unsigned int*)(displayBuffer + start + j - sizeof(unsigned int)) = *(unsigned int*)((object->id << 16) + COLOR_TAG_MASK);

				displayBuffer[start + j] = object->skeleton.canva[objectIndex];
				objectIndex++;
			}
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
static void printColoredDisplay() {
	int i = 0;
	unsigned short obj_id;
	while (i < DISPLAY_BUFFER_LEN) {
		if(*(unsigned int*)(displayBuffer + i) & COLOR_TAG_MASK == COLOR_TAG_MASK) {
			obj_id = *(unsigned short*)(displayBuffer + i + sizeof(unsigned short)) - 1;
			/*
					At this point you've managed to decode the buffer for the objects id,
				now it is time for you to read the orientations the object provides in order to color it.

				The colorPath of an object is organized in the following order: [<foreground or background>, <color byte(RGB)>, Start-Canva-Index, End-Canva-Index, ...].
				It is a contiguous 4 byte divisable array where each end every 4 bytes index has to contain this information structure if not zero.
			*/
		
			objectlist[obj_id]->skeleton.colorPath;
		}
		i++;
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