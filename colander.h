#ifndef COLANDER_H
#define COLANDER_H
#include <libinput.h>
#include <libudev.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input-event-codes.h>
#include <local/logger.h>

#define COLANDERDEF static inline

typedef struct {
	int keyState[0xFF];
	int oldKeyState[0xFF];

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

COLANDERDEF void colander_startListener(ColanderListener* listener);
COLANDERDEF void colander_nextEvent(ColanderListener* listener);
COLANDERDEF void colander_endListener(ColanderListener* listener);

COLANDERDEF int colander_getKey(ColanderListener listener, int key);
COLANDERDEF int colander_getKeyPressed(ColanderListener listener, int key);
COLANDERDEF int colander_getKeyReleased(ColanderListener listener, int key);

#endif // COLANDER_H
#ifdef COLANDER_IMPLEMENTATION

COLANDERDEF void colander_startListener(ColanderListener* listener){
	listener->udev = udev_new();
	listener->li = libinput_udev_create_context(&__colander_interface, NULL, listener->udev);
	for(int i = 0; i < 0xff; i++){
		listener->keyState[i] = 0;
		listener->oldKeyState[i] = 0;
	}
	libinput_udev_assign_seat(listener->li, "seat0");
	libinput_dispatch(listener->li);
}
COLANDERDEF void colander_nextEvent(ColanderListener* listener){
	struct libinput_event* event;
	for(int i = 0; i < 0xff; i++){
		listener->oldKeyState[i] = listener->keyState[i];
	}
	event = libinput_get_event(listener->li);
	if(event != NULL && libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY){
		struct libinput_event_keyboard* kb;
		kb = libinput_event_get_keyboard_event(event);
		int kbKey = libinput_event_keyboard_get_key(kb);
		int kbState = libinput_event_keyboard_get_key_state(kb);
		listener->keyState[kbKey] = kbState;
	}

	libinput_event_destroy(event);
	libinput_dispatch(listener->li);
}
COLANDERDEF void colander_endListener(ColanderListener* listener){
	libinput_unref(listener->li);
}

COLANDERDEF int colander_getKey(ColanderListener listener, int key){
	return listener.keyState[key];
}
COLANDERDEF int colander_getKeyPressed(ColanderListener listener, int key){
	return listener.keyState[key] != listener.oldKeyState[key] && listener.keyState[key] == 1;
}
COLANDERDEF int colander_getKeyReleased(ColanderListener listener, int key){
	return listener.keyState[key] != listener.oldKeyState[key] && listener.keyState[key] == 0;
}

#endif // COLANDER_IMPLEMENTATION
