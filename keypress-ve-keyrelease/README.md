## Bölüm 8 - KeyPress ve KeyRelease

Şimdi klavye girdilerinin nasıl idare edildiğini görelim. Bunu olayların kaynağından alırız.

`ConfigureNotify` olayı ile daha önce yaptığımız gibi, X'e bu tür bir olayı almak istediğimizi söylemek için pencere üzerinde bir bayrak ayarlamalıyız.

```c
    windowAttr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask;
```

Bir tuşa ne zaman basıldığını ve tuşun ne zaman serbest bırakıldığını bilmek istiyoruz.

```c
                case KeyPress:
                {
                    XKeyPressedEvent* e = (XKeyPressedEvent*) &ev;

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
                        printf("sol ok tuşu bırakıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Right))
                        printf("sağ ok tuşu bırakıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Up))
                        printf("yukarı ok tuşu bırakıldı\n");
                    if(e->keycode == XKeysymToKeycode(display, XK_Down))
                        printf("aşağı ok tuşu bırakıldı\n");
                }
                break;
```

Bu iki tür olayı ele almak için olay döngüsüne `case`'ler eklememiz gerekir. Bu tür tuş yönetimi çok basittir. Gelen olayı daha önce yaptığımız gibi topladık ve kontrol etmek istediğimiz tuşlardan birine ait olup olmadığını kontrol ettik. Xlib'de burada yer alan iki farklı kavram vardır, biri `keycode`, diğeri ise `Keysym` veya tuş sembolüdür.

Bir `keycode`, klavyenizdeki gerçek tuşlara atanan bir sayıdır. Sol ok tuşunuz, sağ ok tuşunuzdan farklı bir `keycode`'a sahiptir ve her ikisi de A tuşundan farklıdır. Ancak, sadece hangi fiziksel tuşa basıldığını bilmek istemiyorsak, kullanıcının hangi sembole de bastığını bilmek istiyorsak? Tuş sembolleri bunun için var.

Bir sembolden bir keycode sorgulamak için `XKeysymToKeycode()`'u kullanıyoruz. Hangi klavye düzeniyle çalışacağını bilmesi için `display`'imizi ve ardından tanımlanmış `keysym` makrolarından birini geçeriz. Tanımlı `keysym`'lerin tam listesini `/usr/include/X11/keysymdef.h` dosyasında bulabilirsiniz.

Bunun ve sonraki örneğin terminalde çalıştığına emin olun; böylece `printf` çıktısını görebilirsiniz.

Bu, temel X UPA'sındaki temel klavye kullanımıdır. Daha sonra yapacaklarımız için daha derine inmeliyiz.

[<- Bölüm 7 - Bu titreme hakkında...](../bu-titreme-hakkinda/README.md) [Bölüm 9 - XInput'tan UTF-8 Karakterler (Hayır, oyun çubuğu olanından değil) ->](../utf8/README.md)
