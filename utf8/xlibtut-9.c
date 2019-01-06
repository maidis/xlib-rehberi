#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

void setSizeHint(Display* display, Window window,
                 int minWidth, int minHeight,
                 int maxWidth, int maxHeight)
{
    XSizeHints hints = {};
    if(minWidth > 0 && minHeight > 0)
        hints.flags |= PMinSize;
    if(maxWidth > 0 && maxHeight > 0)
        hints.flags |= PMaxSize;

    hints.min_width = minWidth;
    hints.min_height = minHeight;
    hints.max_width = maxWidth;
    hints.max_height = maxHeight;

    XSetWMNormalHints(display, window, &hints);
}

Status toggleMaximize(Display* display, Window window)
{
    XClientMessageEvent ev = {};
    Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
    Atom maxH  =  XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom maxV  =  XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

    if(wmState == None)
        return 0;

    ev.type = ClientMessage;
    ev.format = 32;
    ev.window = window;
    ev.message_type = wmState;
    ev.data.l[0] = 2; // belirtime göre _NET_WM_STATE_TOGGLE 2; Benim başlıklarımda tanımlı değil
    ev.data.l[1] = maxH;
    ev.data.l[2] = maxV;
    ev.data.l[3] = 1;

    return XSendEvent(display, DefaultRootWindow(display), False,
                      SubstructureNotifyMask,
                      (XEvent *)&ev);
}

int main(int argc, char** args)
{
    int width = 800;
    int height = 600;

    Display* display = XOpenDisplay(0);

    if(!display)
    {
        printf("Hiçbir ekran yok\n");
        exit(1);
    }

    Window root = DefaultRootWindow(display);
    int defaultScreen = DefaultScreen(display);

    int screenBitDepth = 24;
    XVisualInfo visinfo = {};
    if(!XMatchVisualInfo(display, defaultScreen, screenBitDepth, TrueColor, &visinfo))
    {
        printf("Hiçbir eşleşen görsel bilgi yok\n");
        exit(1);
    }

    XSetWindowAttributes windowAttr;
    windowAttr.bit_gravity = StaticGravity;
    windowAttr.background_pixel = 0;
    windowAttr.colormap = XCreateColormap(display, root,
                                          visinfo.visual, AllocNone);
    windowAttr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask;
    unsigned long attributeMask = CWBitGravity | CWBackPixel | CWColormap | CWEventMask;

    Window window = XCreateWindow(display, root,
                                  0, 0,
                                  width, height, 0,
                                  visinfo.depth, InputOutput,
                                  visinfo.visual, attributeMask, &windowAttr);

    if(!window)
    {
        printf("Pencere düzgün şekilde oluşturulamadı\n");
        exit(1);
    }

    XStoreName(display, window, "Merhaba Dünya!");
    setSizeHint(display, window, 400, 300, 0, 0);

    XIM xInputMethod = XOpenIM(display, 0, 0, 0);
    if(!xInputMethod)
    {
        printf("Giriş Yöntemi açılamadı\n");
    }

    XIMStyles* styles = 0;
    if(XGetIMValues(xInputMethod, XNQueryInputStyle, &styles, NULL) || !styles)
    {
        printf("Giriş Biçimleri alınamadı\n");
    }

    XIMStyle bestMatchStyle = 0;
    for(int i=0; i<styles->count_styles; i++)
    {
        XIMStyle thisStyle = styles->supported_styles[i];
        if (thisStyle == (XIMPreeditNothing | XIMStatusNothing))
        {
            bestMatchStyle = thisStyle;
            break;
        }
    }
    XFree(styles);

    if(!bestMatchStyle)
    {
        printf("Eşleşen bir giriş biçimi belirlenemedi\n");
    }

    XIC xInputContext = XCreateIC(xInputMethod, XNInputStyle, bestMatchStyle,
                                  XNClientWindow, window,
                                  XNFocusWindow, window,
                                  NULL);
    if(!xInputContext)
    {
        printf("Giriş Bağlamı oluşturulamadı\n");
    }

    XMapWindow(display, window);

    //toggleMaximize(display, window);
    XFlush(display);

    int pixelBits = 32;
    int pixelBytes = pixelBits / 8;
    int windowBufferSize = width * height * pixelBytes;
    char* mem = (char*)malloc(windowBufferSize);

    XImage* xWindowBuffer = XCreateImage(display, visinfo.visual, visinfo.depth,
                                         ZPixmap, 0, mem, width, height,
                                         pixelBits, 0);
    GC defaultGC = DefaultGC(display, defaultScreen);

    Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
    if(!XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1))
    {
        printf("WM_DELETE_WINDOW özelliği kaydedilemedi\n");
    }

    int sizeChange = 0;
    int windowOpen = 1;
    while(windowOpen)
    {
        XEvent ev = {};
        while(XPending(display) > 0)
        {
            XNextEvent(display, &ev);
            switch(ev.type)
            {
                case DestroyNotify:
                {
                    XDestroyWindowEvent* e = (XDestroyWindowEvent*) &ev;
                    if(e->window == window)
                    {
                        windowOpen = 0;
                    }
                }
                break;
                case ClientMessage:
                {
                    XClientMessageEvent* e = (XClientMessageEvent*)&ev;
                    if((Atom)e->data.l[0] == WM_DELETE_WINDOW)
                    {
                        XDestroyWindow(display, window);
                        windowOpen = 0;
                    }
                }
                break;
                case ConfigureNotify:
                {
                    XConfigureEvent* e = (XConfigureEvent*) &ev;
                    width = e->width;
                    height = e->height;
                    sizeChange = 1;
                }
                break;
                case KeyPress:
                {
                    XKeyPressedEvent* e = (XKeyPressedEvent*) &ev;

                    int symbol = 0;
                    Status status = 0;
                    Xutf8LookupString(xInputContext, e, (char*)&symbol,
                                      4, 0, &status);

                    if(status == XBufferOverflow)
                    {
                        // 24 bitden büyük utf-8 karakterleri olmadığından olmamalı
                        // Ancak bir karakter katarı tamponuna doğrudan yazmak için kullanıldığında dikkat edilmesi gereken şeyler var
                        printf("Klavye sembolü eşlemesi oluşturmaya çalışırken arabellek taşması\n");
                    }
                    else if(status == XLookupChars)
                    {
                        printf("%s\n", (char*)&symbol);
                    }

                    if(e->keycode == XKeysymToKeycode(display, XK_Left))
                        printf("sol ok tuşuna basıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Right))
                        printf("sağ ok tuşuna basıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Up))
                        printf("yukarı ok tuşuna basıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Down))
                        printf("aşağı ok tuşuna basıldı\n");
                }
                break;
                case KeyRelease:
                {
                    XKeyPressedEvent* e = (XKeyPressedEvent*) &ev;

                    if(e->keycode == XKeysymToKeycode(display, XK_Left))
                        printf("sol tuşu bırakıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Right))
                        printf("sağ tuşu bırakıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Up))
                        printf("yukarı tuşu bırakıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Down))
                        printf("aşağı tuşu bırakıldı\n");
                }
                break;
            }
        }

        if(sizeChange)
        {
            sizeChange = 0;
            XDestroyImage(xWindowBuffer); // Ayırdığımız belleği serbest bırakır;
            windowBufferSize = width * height * pixelBytes;
            mem = (char*)malloc(windowBufferSize);

            xWindowBuffer = XCreateImage(display, visinfo.visual, visinfo.depth,
                                         ZPixmap, 0, mem, width, height,
                                         pixelBits, 0);
        }

        int pitch = width*pixelBytes;
        for(int y=0; y<height; y++)
        {
            char* row = mem+(y*pitch);
            for(int x=0; x<width; x++)
            {
                unsigned int* p = (unsigned int*) (row+(x*pixelBytes));
                if(x%16 && y%16)
                {
                    *p = 0xffffffff;
                }
                else
                {
                    *p = 0;
                }
            }
        }

        XPutImage(display, window,
                  defaultGC, xWindowBuffer, 0, 0, 0, 0,
                  width, height);
    };

    return 0;
}