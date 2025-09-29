#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Label.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    XtAppContext app_ctx;
    Widget toplevel, hello;

    toplevel = XtOpenApplication(
        &app_ctx,
        "Test",
        NULL,
        0,
        &argc,
        argv,
        NULL,
        sessionShellWidgetClass,
        NULL,
        0
    );

    hello = XtVaCreateManagedWidget(
        "hello", labelWidgetClass, toplevel, XtNlabel, "Hello, World!", NULL
    );

    XtRealizeWidget(toplevel);

    XtAppMainLoop(app_ctx);
}
