#include "colander.h"
#include <linux/input-event-codes.h>
#include <local/logger.h>

void colander_startListener(ColanderListener* listener){
	listener->udev = udev_new();
	listener->li = libinput_udev_create_context(&__colander_interface, NULL, listener->udev);
	for(int i = 0; i < 0xff; i++){
		listener->keyState[i] = 0;
		listener->oldKeyState[i] = 0;
	}
	listener->mbState[0] = 0;
	listener->mbState[1] = 0;
	listener->mbState[2] = 0;
	listener->oldMbState[0] = 0;
	listener->oldMbState[1] = 0;
	listener->oldMbState[2] = 0;
	listener->mouseDX = 0;
	listener->mouseDY = 0;
	libinput_udev_assign_seat(listener->li, "seat0");
	libinput_dispatch(listener->li);
}
void colander_nextEvent(ColanderListener* listener){
	struct libinput_event* event;
	for(int i = 0; i < 0xff; i++){
		listener->oldKeyState[i] = listener->keyState[i];
	}
	listener->oldMbState[0] = listener->mbState[0];
	listener->oldMbState[1] = listener->mbState[1];
	listener->oldMbState[2] = listener->mbState[2];
	event = libinput_get_event(listener->li);
	if(event != NULL){
		if(libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY){
			struct libinput_event_keyboard* kb;
			kb = libinput_event_get_keyboard_event(event);
			int kbKey = libinput_event_keyboard_get_key(kb);
			int kbState = libinput_event_keyboard_get_key_state(kb);
			listener->keyState[kbKey] = kbState;
		}
		else if(libinput_event_get_type(event) == LIBINPUT_EVENT_POINTER_MOTION){
			struct libinput_event_pointer* pointer;
			pointer = libinput_event_get_pointer_event(event);
			listener->mouseDX = libinput_event_pointer_get_dx(pointer);
			listener->mouseDY = libinput_event_pointer_get_dy(pointer);
		}
		else if(libinput_event_get_type(event) == LIBINPUT_EVENT_POINTER_BUTTON){
			struct libinput_event_pointer* pointer;
			pointer = libinput_event_get_pointer_event(event);
			int buttonState = libinput_event_pointer_get_button_state(pointer);
			switch(libinput_event_pointer_get_button(pointer)){
				case BTN_LEFT: listener->mbState[0] = buttonState; break;
				case BTN_MIDDLE: listener->mbState[1] = buttonState; break;
				case BTN_RIGHT: listener->mbState[2] = buttonState; break;
				default: break;
			};
		}
	}

	libinput_event_destroy(event);
	libinput_dispatch(listener->li);
}
void colander_endListener(ColanderListener* listener){
	libinput_unref(listener->li);
}

int colander_getKey(ColanderListener listener, int key){
	return listener.keyState[key];
}
int colander_getKeyPressed(ColanderListener listener, int key){
	return listener.keyState[key] != listener.oldKeyState[key] && listener.keyState[key] == 1;
}
int colander_getKeyReleased(ColanderListener listener, int key){
	return listener.keyState[key] != listener.oldKeyState[key] && listener.keyState[key] == 0;
}

int colander_getMouseDX(ColanderListener listener){ return listener.mouseDX; }
int colander_getMouseDY(ColanderListener listener){ return listener.mouseDY; }
int colander_getMouseButton(ColanderListener listener, int button){
	switch(button){
		case BTN_LEFT: return listener.mbState[0];
		case BTN_MIDDLE: return listener.mbState[1];
		case BTN_RIGHT: return listener.mbState[2];
		default: break;
	}
	return 0;
}
int colander_getMouseButtonPressed(ColanderListener listener, int button){
	switch(button){
		case BTN_LEFT: return listener.mbState[0] != listener.oldMbState[0] && listener.mbState[0] == 1;
		case BTN_MIDDLE: return listener.mbState[1] != listener.oldMbState[1] && listener.mbState[1] == 1;
		case BTN_RIGHT: return listener.mbState[2] != listener.oldMbState[2] && listener.mbState[2] == 1;
		default: break;
	}
	return 0;
}
int colander_getMouseButtonReleased(ColanderListener listener, int button){
	switch(button){
		case BTN_LEFT: return listener.mbState[0] != listener.oldMbState[0] && listener.mbState[0] == 0;
		case BTN_MIDDLE: return listener.mbState[1] != listener.oldMbState[1] && listener.mbState[1] == 0;
		case BTN_RIGHT: return listener.mbState[2] != listener.oldMbState[2] && listener.mbState[2] == 0;
		default: break;
	}
	return 0;
}

