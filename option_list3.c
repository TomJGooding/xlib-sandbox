#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdbool.h>
#include <stddef.h>
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

    char *colors[2] = {"blue", "light gray"};
    unsigned long color_bits[2];
    XColor closest, exact;
    for (size_t i = 0; i < 2; i++) {
        XAllocNamedColor(
            dpy, XDefaultColormap(dpy, scr), colors[i], &exact, &closest
        );
        color_bits[i] = exact.pixel;
    }
    XGCValues gc_values;
    gc_values.background = WhitePixel(dpy, scr);
    gc_values.foreground = BlackPixel(dpy, scr);
    valuemask = GCBackground | GCForeground;
    GC base_gc = XCreateGC(dpy, win, valuemask, &gc_values);
    gc_values.background = color_bits[0];  // blue
    gc_values.foreground = WhitePixel(dpy, scr);
    GC highlighted_gc = XCreateGC(dpy, win, valuemask, &gc_values);
    gc_values.background = color_bits[1];  // light gray
    gc_values.foreground = BlackPixel(dpy, scr);
    GC hovered_gc = XCreateGC(dpy, win, valuemask, &gc_values);

    valuemask = CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes opt_list_attrs;
    opt_list_attrs.background_pixel = WhitePixel(dpy, scr);
    opt_list_attrs.border_pixel = BlackPixel(dpy, scr);
    opt_list_attrs.event_mask = KeyPressMask | LeaveWindowMask;
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
    opt_item_attrs.event_mask =
        ExposureMask | EnterWindowMask | ButtonPressMask;
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
    int opt_idx = 0;
    int hover_idx = -1;
    while (!exit) {
        XNextEvent(dpy, &event);
        switch (event.type) {
            case Expose:
                for (size_t i = 0; i < 12; i++) {
                    GC gc = base_gc;
                    if (i == opt_idx) {
                        gc = highlighted_gc;
                        XSetWindowBackground(dpy, opt_items[i], color_bits[0]);
                        XClearWindow(dpy, opt_items[i]);
                    }
                    XDrawString(
                        dpy, opt_items[i], gc, 8, 16, opts[i], opt_lens[i]
                    );
                }
                break;
            case LeaveNotify:
                if (hover_idx != -1 && hover_idx != opt_idx) {
                    XSetWindowBackground(
                        dpy, opt_items[hover_idx], WhitePixel(dpy, scr)
                    );
                    XClearWindow(dpy, opt_items[hover_idx]);
                    XDrawString(
                        dpy,
                        opt_items[hover_idx],
                        base_gc,
                        8,
                        16,
                        opts[hover_idx],
                        opt_lens[hover_idx]
                    );
                }
                hover_idx = -1;
                break;
            case EnterNotify:
                if (hover_idx != -1 && hover_idx != opt_idx) {
                    XSetWindowBackground(
                        dpy, opt_items[hover_idx], WhitePixel(dpy, scr)
                    );
                    XClearWindow(dpy, opt_items[hover_idx]);
                    XDrawString(
                        dpy,
                        opt_items[hover_idx],
                        base_gc,
                        8,
                        16,
                        opts[hover_idx],
                        opt_lens[hover_idx]
                    );
                }
                for (size_t i = 0; i < 12; i++) {
                    if (opt_items[i] == event.xcrossing.window) {
                        hover_idx = i;
                        break;
                    }
                }
                if (hover_idx != opt_idx) {
                    XSetWindowBackground(
                        dpy, opt_items[hover_idx], color_bits[1]
                    );
                    XClearWindow(dpy, opt_items[hover_idx]);
                    XDrawString(
                        dpy,
                        opt_items[hover_idx],
                        hovered_gc,
                        8,
                        16,
                        opts[hover_idx],
                        opt_lens[hover_idx]
                    );
                }
                break;
            case ButtonPress:
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
                for (size_t i = 0; i < 12; i++) {
                    if (opt_items[i] == event.xbutton.window) {
                        opt_idx = i;
                        break;
                    }
                }
                XSetWindowBackground(dpy, opt_items[opt_idx], color_bits[0]);
                XClearWindow(dpy, opt_items[opt_idx]);
                XDrawString(
                    dpy,
                    opt_items[opt_idx],
                    highlighted_gc,
                    8,
                    16,
                    opts[opt_idx],
                    opt_lens[opt_idx]
                );
                break;
            case KeyPress:
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                switch (keysym) {
                    case XK_Return:
                        printf("%s\n", opts[opt_idx]);
                        exit = true;
                    case XK_Down:
                        if (opt_idx < 11) {
                            unsigned long bg_pixel = WhitePixel(dpy, scr);
                            GC gc = base_gc;
                            if (hover_idx == opt_idx) {
                                bg_pixel = color_bits[1];
                                gc = hovered_gc;
                            }
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], bg_pixel
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                            opt_idx++;
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], color_bits[0]
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                highlighted_gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                        }
                        break;
                    case XK_Up:
                        if (opt_idx > 0) {
                            unsigned long bg_pixel = WhitePixel(dpy, scr);
                            GC gc = base_gc;
                            if (hover_idx == opt_idx) {
                                bg_pixel = color_bits[1];
                                gc = hovered_gc;
                            }
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], bg_pixel
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                gc,
                                8,
                                16,
                                opts[opt_idx],
                                opt_lens[opt_idx]
                            );
                            opt_idx--;
                            XSetWindowBackground(
                                dpy, opt_items[opt_idx], color_bits[0]
                            );
                            XClearWindow(dpy, opt_items[opt_idx]);
                            XDrawString(
                                dpy,
                                opt_items[opt_idx],
                                highlighted_gc,
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
