#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN_X 0
#define WIN_Y 0
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define WIN_BORDER 1

int main() {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Cannot open display\n");
        exit(1);
    }

    int screen = DefaultScreen(display);
    GC gc = DefaultGC(display, screen);
    Window root = RootWindow(display, screen);

    Window window = XCreateSimpleWindow(
        display,
        root,
        WIN_X,
        WIN_Y,
        WIN_WIDTH,
        WIN_HEIGHT,
        WIN_BORDER,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );
    XSelectInput(
        display,
        window,
        KeyPressMask | ButtonPressMask | Button1MotionMask | ButtonReleaseMask
    );

    XMapRaised(display, window);

    bool exit = false;
    XEvent event;

    bool drawing = false;
    int prev_draw_x = 0;
    int prev_draw_y = 0;

    while (!exit) {
        XNextEvent(display, &event);
        switch (event.type) {
            case KeyPress:
                if (XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0) ==
                    XK_q) {
                    exit = true;
                }
                break;
            case ButtonPress:
                if (event.xbutton.button == Button1) {
                    XDrawPoint(
                        display, window, gc, event.xbutton.x, event.xbutton.y
                    );
                    drawing = true;
                    prev_draw_x = event.xbutton.x;
                    prev_draw_y = event.xbutton.y;
                }
                break;
            case MotionNotify:
                if (drawing) {
                    XDrawLine(
                        display,
                        window,
                        gc,
                        prev_draw_x,
                        prev_draw_y,
                        event.xmotion.x,
                        event.xmotion.y
                    );
                    prev_draw_x = event.xmotion.x;
                    prev_draw_y = event.xmotion.y;
                }
                break;
            case ButtonRelease:
                drawing = false;
                break;
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
