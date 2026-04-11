#include <linux/uinput.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "rpi_gpio.h"

#define UINPUT_DEV_NAME "Matrix Pi/4 Keyboard"
#define KEY_NONE 0
const int cols[] = { 4, 14, 15, 27, 22, 23, 24, 10, 9, 25, 11, 8, 7, 5 };
const int ncols = 14;
const int rows[] = { 6, 19, 16, 26, 20 };
const int nrows = 5;
const int keymap[] = {
	KEY_ESC,       KEY_1,        KEY_2,       KEY_3,    KEY_4,    KEY_5,     KEY_6,    KEY_7,    KEY_8,     KEY_9,    KEY_0,         KEY_MINUS,      KEY_EQUAL,      KEY_BACKSPACE,
	KEY_TAB,       KEY_Q,        KEY_W,       KEY_E,    KEY_R,    KEY_T,     KEY_Y,    KEY_U,    KEY_I,     KEY_O,    KEY_P,         KEY_LEFTBRACE,  KEY_RIGHTBRACE, KEY_BACKSLASH,
	KEY_CAPSLOCK,  KEY_A,        KEY_S,       KEY_D,    KEY_F,    KEY_G,     KEY_H,    KEY_J,    KEY_K,     KEY_L,    KEY_SEMICOLON, KEY_APOSTROPHE, KEY_NONE,       KEY_ENTER,
	KEY_LEFTSHIFT, KEY_Z,        KEY_X,       KEY_C,    KEY_V,    KEY_B,     KEY_N,    KEY_M,    KEY_COMMA, KEY_DOT,  KEY_SLASH,     KEY_GRAVE,      KEY_UP,         KEY_DELETE,
	KEY_LEFTCTRL,  KEY_LEFTMETA, KEY_LEFTALT, KEY_NONE, KEY_NONE, KEY_SPACE, KEY_NONE, KEY_NONE, KEY_NONE,  KEY_FN,   KEY_INSERT,    KEY_LEFT,       KEY_DOWN,       KEY_RIGHT,

    KEY_NONE,      KEY_F1,       KEY_F2,      KEY_F3,   KEY_F4,   KEY_F5,    KEY_F6,   KEY_F7,   KEY_F8,    KEY_F9,   KEY_F10,       KEY_F11,        KEY_F12,        KEY_NONE,
    KEY_NONE,      KEY_NONE,     KEY_NONE,    KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE,      KEY_NONE,       KEY_NONE,       KEY_NONE,
    KEY_NONE,      KEY_NONE,     KEY_NONE,    KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE,      KEY_NONE,       KEY_NONE,       KEY_NONE,
    KEY_NONE,      KEY_NONE,     KEY_NONE,    KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE,      KEY_NONE,       KEY_PAGEUP,     KEY_NONE,
    KEY_NONE,      KEY_NONE,     KEY_NONE,    KEY_NONE, KEY_NONE, KEY_NONE,  KEY_NONE, KEY_NONE, KEY_NONE,  KEY_FN,   KEY_NONE,      KEY_HOME,       KEY_PAGEDOWN,   KEY_END,	
};
const int fnrow = 5;

int uinput_init() {
    struct uinput_setup usetup;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) return -1;
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_REP);
    for (int i = 0; i < ncols * (nrows * 2); i++) {
	    if (keymap[i] != KEY_NONE) {
            ioctl(fd, UI_SET_KEYBIT, keymap[i]);
		}
    }
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1209; // Generic
    usetup.id.product = 0x7501;
    strcpy(usetup.name, UINPUT_DEV_NAME);
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
    return fd;
}
void uinput_emit(int fd, int type, int code, int val) {
   struct input_event ie;
   ie.type = type;
   ie.code = code;
   ie.value = val;
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;
   write(fd, &ie, sizeof(ie));
}

int main() {
    int uinput_fd = uinput_init();
    if (uinput_fd < 0) {
        perror("Failed to initialize UInput");
        return -1;
    }
    if (rpi_gpio_init() < 0) {
        perror("Failed to initialize RPi GPIO");
        return -1;
    }
    for (int i = 0; i < nrows; i++) {
        rpi_gpio_setup(rows[i], INPUT, PUD_OFF);
    }
    for (int i = 0; i < ncols; i++) {
        rpi_gpio_setup(cols[i], INPUT, PUD_UP);
    }
    int pressed[(nrows * 2) * ncols];
    int fnkey = 0;
    memset(pressed, 0, sizeof(pressed));
    for (;;) {
        usleep(1000000 / 60);
        int syn = 0;
        for (int i = 0; i < nrows; i++) {
            rpi_gpio_output(rows[i], 0);
            rpi_gpio_set_fn(rows[i], OUTPUT);
            usleep(3);
            uint32_t all = rpi_gpio_input_all();
            rpi_gpio_set_fn(rows[i], INPUT);
            for (int j = 0; j < ncols; j++) {
                int keycode = i * ncols + j;
                if (fnkey > 0) {
                    keycode = (i + fnrow) * ncols + j;
                }
                int val = !(all & (1 << cols[j]));
                if (val != pressed[keycode]) {
                    pressed[keycode] = val;
                    if (keymap[keycode] == KEY_FN) {
                        fnkey = val;
                    } else {
                        uinput_emit(uinput_fd, EV_KEY, keymap[keycode], val);
                        syn++;
                    }
                }
            }
        }
        if (syn) uinput_emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
    }
    return 0;
}
