#ifndef COLANDER_H
#define COLANDER_H
#include <libinput.h>
#include <libudev.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	int keyState[0xFF];
	int oldKeyState[0xFF];

	int mbState[3];
	int oldMbState[3];
	int mouseDX;
	int mouseDY;

	struct libinput* li;
	struct udev* udev;
} ColanderListener;

static int __colander_open_restricted(const char *path, int flags, void* user_data){
	int fd = open(path, flags);
	return fd < 0 ? -errno : fd;
}

static void __colander_close_restricted(int fd, void* user_data){
	close(fd);
}

const static struct libinput_interface __colander_interface = {
	.open_restricted = __colander_open_restricted,
	.close_restricted = __colander_close_restricted,
};

void colander_startListener(ColanderListener* listener);
void colander_nextEvent(ColanderListener* listener);
void colander_endListener(ColanderListener* listener);

int colander_getKey(ColanderListener listener, int key);
int colander_getKeyPressed(ColanderListener listener, int key);
int colander_getKeyReleased(ColanderListener listener, int key);

int colander_getMouseDX(ColanderListener listener);
int colander_getMouseDY(ColanderListener listener);
int colander_getMouseButton(ColanderListener listener, int button);
int colander_getMouseButtonPressed(ColanderListener listener, int button);
int colander_getMouseButtonReleased(ColanderListener listener, int button);

#endif // COLANDER_H
