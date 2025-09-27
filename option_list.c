#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *opts[] = {
    "Aerilon",
    "Aquaria",
    "Canceron",
    "Caprica",
    "Gemenon",
    "Leonis",
    "Libran",
    "Picon",
    "Sagittaron",
    "Scorpia",
    "Tauron",
    "Virgon",
};

int main() {
    int opts_lens[12];
    for (int i = 0; i < 12; i++) {
        opts_lens[i] = strlen(opts[i]);
    }

    Display *dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "cannot open X11 display\n");
        exit(1);
    }

    int scr = DefaultScreen(dpy);
    XSetWindowAttributes win_attrs;
    win_attrs.background_pixel = WhitePixel(dpy, scr);
    win_attrs.border_pixel = BlackPixel(dpy, scr);
    unsigned long valuemask = CWBackPixel | CWBorderPixel;
    Window win = XCreateWindow(
        dpy,
        RootWindow(dpy, scr),
        0,    // x
        0,    // y
        800,  // width
        600,  // height
        2,    // border_width
        DefaultDepth(dpy, scr),
        InputOutput,
        DefaultVisual(dpy, scr),
        valuemask,
        &win_attrs
    );

    XGCValues gc_values;
    gc_values.background = WhitePixel(dpy, scr);
    gc_values.foreground = BlackPixel(dpy, scr);
    valuemask = GCForeground | GCBackground;
    GC base_gc = XCreateGC(dpy, win, valuemask, &gc_values);
    gc_values.background = BlackPixel(dpy, scr);
    gc_values.foreground = WhitePixel(dpy, scr);
    GC hover_gc = XCreateGC(dpy, win, valuemask, &gc_values);

    valuemask = CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes opt_list_attrs;
    opt_list_attrs.background_pixel = WhitePixel(dpy, scr);
    opt_list_attrs.border_pixel = BlackPixel(dpy, scr);
    opt_list_attrs.event_mask = LeaveWindowMask;
    Window optlist = XCreateWindow(
        dpy,
        win,
        20,             // x
        20,             // y
        100 + 4,        // width
        (25 * 12) + 4,  // height
        2,              // border_width
        DefaultDepth(dpy, scr),
        InputOutput,
        DefaultVisual(dpy, scr),
        valuemask,
        &opt_list_attrs
    );
    XSetWindowAttributes opt_item_attrs;
    opt_item_attrs.background_pixel = WhitePixel(dpy, scr);
    opt_item_attrs.border_pixel = BlackPixel(dpy, scr);
    opt_item_attrs.event_mask = ExposureMask | EnterWindowMask | ButtonPress;
    Window opt_items[12];
    for (int i = 0; i < 12; i++) {
        opt_items[i] = XCreateWindow(
            dpy,
            optlist,
            0,       // x
            i * 25,  // y
            100,     // width
            25,      // height
            2,       // border_width
            DefaultDepth(dpy, scr),
            InputOutput,
            DefaultVisual(dpy, scr),
            valuemask,
            &opt_item_attrs
        );
    }

    XMapWindow(dpy, win);
    XMapWindow(dpy, optlist);
    for (int i = 0; i < 12; i++) {
        XMapWindow(dpy, opt_items[i]);
    }

    int exit = 0;
    XEvent event;
    int opt_idx = 0;
    while (!exit) {
        XNextEvent(dpy, &event);
        switch (event.type) {
            case Expose:
                for (int i = 0; i < 12; i++) {
                    XDrawString(
                        dpy,
                        opt_items[i],
                        base_gc,
                        8,   // x
                        16,  // y
                        opts[i],
                        opts_lens[i]
                    );
                }
                break;
            case LeaveNotify:
                XSetWindowBackground(
                    dpy, opt_items[opt_idx], WhitePixel(dpy, scr)
                );
                XClearWindow(dpy, opt_items[opt_idx]);
                XDrawString(
                    dpy,
                    opt_items[opt_idx],
                    base_gc,
                    8,   // x
                    16,  // y
                    opts[opt_idx],
                    opts_lens[opt_idx]
                );
                break;
            case EnterNotify:
                XSetWindowBackground(
                    dpy, opt_items[opt_idx], WhitePixel(dpy, scr)
                );
                XClearWindow(dpy, opt_items[opt_idx]);
                XDrawString(
                    dpy,
                    opt_items[opt_idx],
                    base_gc,
                    8,   // x
                    16,  // y
                    opts[opt_idx],
                    opts_lens[opt_idx]
                );
                for (int i = 0; i < 12; i++) {
                    if (opt_items[i] == event.xcrossing.window) {
                        opt_idx = i;
                        break;
                    }
                }
                XSetWindowBackground(
                    dpy, opt_items[opt_idx], BlackPixel(dpy, scr)
                );
                XClearWindow(dpy, opt_items[opt_idx]);
                XDrawString(
                    dpy,
                    opt_items[opt_idx],
                    hover_gc,
                    8,   // x
                    16,  // y
                    opts[opt_idx],
                    opts_lens[opt_idx]
                );
                break;
            case ButtonPress:
                for (int i = 0; i < 12; i++) {
                    if (opt_items[i] == event.xbutton.window) {
                        opt_idx = i;
                        break;
                    }
                }
                printf("%s\n", opts[opt_idx]);
                exit = 1;
                break;
        }
    }

    XUnmapWindow(dpy, win);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}
