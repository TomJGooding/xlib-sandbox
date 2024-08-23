#include <X11/Xlib.h>
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
    // Connect to the X server
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Cannot open display\n");
        exit(1);
    }

    int screen = DefaultScreen(display);
    GC gc = DefaultGC(display, screen);
    Window root = RootWindow(display, screen);

    // Create and display a simple window
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
    // Subscribe to events
    XSelectInput(
        display, window, ExposureMask | ButtonPressMask | KeyPressMask
    );

    XMapRaised(display, window);

    // Run the event loop
    bool exit = false;
    XEvent event;
    char *message = "Hello from X11! Click the mouse or press any key to exit.";
    while (!exit) {
        XNextEvent(display, &event);
        switch (event.type) {
            case Expose:
                XDrawString(
                    display, window, gc, 50, 50, message, strlen(message)
                );
                break;
            case ButtonPress:
            case KeyPress:
                exit = true;
                break;
        }
    }

    // Destroy the window and disconnect from the X server
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
