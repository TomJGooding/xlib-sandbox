#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdbool.h>
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
    size_t opt_lens[12];
    for (size_t i = 0; i < 12; i++) {
        opt_lens[i] = strlen(opts[i]);
    }

    Display *dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stdout, "cannot open X11 display\n");
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
        0,
        0,
        800,
        600,
        2,
        DefaultDepth(dpy, scr),
        InputOutput,
        DefaultVisual(dpy, scr),
        valuemask,
        &win_attrs
    );

    XGCValues gc_values;
    gc_values.background = WhitePixel(dpy, scr);
    gc_values.foreground = BlackPixel(dpy, scr);
    valuemask = GCBackground | GCForeground;
    GC base_gc = XCreateGC(dpy, win, valuemask, &gc_values);
    gc_values.background = BlackPixel(dpy, scr);
    gc_values.foreground = WhitePixel(dpy, scr);
    GC select_gc = XCreateGC(dpy, win, valuemask, &gc_values);

    valuemask = CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes opt_list_attrs;
    opt_list_attrs.background_pixel = WhitePixel(dpy, scr);
    opt_list_attrs.border_pixel = BlackPixel(dpy, scr);
    opt_list_attrs.event_mask = KeyPressMask;
    Window opt_list = XCreateWindow(
        dpy,
        win,
        20,
        20,
        100 + 4,
        (25 * 12) + 4,
        2,
        DefaultDepth(dpy, scr),
        InputOutput,
        DefaultVisual(dpy, scr),
        valuemask,
        &opt_list_attrs
    );
    XSetWindowAttributes opt_item_attrs;
    opt_item_attrs.background_pixel = WhitePixel(dpy, scr);
    opt_item_attrs.border_pixel = BlackPixel(dpy, scr);
    opt_item_attrs.event_mask = ExposureMask;
    Window opt_items[12];
    for (size_t i = 0; i < 12; i++) {
        opt_items[i] = XCreateWindow(
            dpy,
            opt_list,
            0,
            i * 25,
            100,
            25,
            2,
            DefaultDepth(dpy, scr),
            InputOutput,
            DefaultVisual(dpy, scr),
            valuemask,
            &opt_item_attrs
        );
    }

    XMapWindow(dpy, win);
    XMapWindow(dpy, opt_list);
    for (size_t i = 0; i < 12; i++) {
        XMapWindow(dpy, opt_items[i]);
    }

    bool exit = false;
    XEvent event;
    size_t opt_idx = 0;
    while (!exit) {
        XNextEvent(dpy, &event);
        switch (event.type) {
            case Expose:
                for (size_t i = 0; i < 12; i++) {
                    GC gc = base_gc;
                    if (i == opt_idx) {
                        gc = select_gc;
                        XSetWindowBackground(
                            dpy, opt_items[i], BlackPixel(dpy, scr)
                        );
                        XClearWindow(dpy, opt_items[i]);
                    }
                    XDrawString(
                        dpy, opt_items[i], gc, 8, 16, opts[i], opt_lens[i]
                    );
                }
                break;
            case KeyPress:
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                switch (keysym) {
                    case XK_Return:
                        printf("%s\n", opts[opt_idx]);
                        exit = true;
                    case XK_Down:
                        if (opt_idx < 11) {
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], WhitePixel(dpy, scr)
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                base_gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                            opt_idx++;
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], BlackPixel(dpy, scr)
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                select_gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                        }
                        break;
                    case XK_Up:
                        if (opt_idx > 0) {
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], WhitePixel(dpy, scr)
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                base_gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                            opt_idx--;
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], BlackPixel(dpy, scr)
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                select_gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                        }
                        break;
                }
                break;
        }
    }

    XUnmapWindow(dpy, win);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}
