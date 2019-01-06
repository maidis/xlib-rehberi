## Bölüm 4 - Daha İyi Kapatma

İlk iş olarak, `toggleMaximize()` fonksiyonunu kaldıralım. Bu, pencere yöneticisi UPA'sını göstermek için iyi bir yoldu ama şu anda ona ihtiyacımız yok.

```c
    XMapWindow(display, window);

    //toggleMaximize(display, window);
    XFlush(display);
```

Artık `atom`'ları ve `ClientMessage` olaylarını öğrendik, programımızın kapanmasını ve pencereyi kapatmasını iyileştirebileceğiz. Programınızı bir terminalden çalıştırıyorsanız, pencereyi kapatırken bir hata mesajı aldığınızı fark etmiş olabilirsiniz. Bunun nedeni, pencere yöneticisinin pencereyi sizin için kapatması ve bunun en zarif şekilde olmamasıdır. Bu nedenle pencere yöneticisinin, size pencerenin kendisini kapatmadan önce pencere kapatma düğmesine basıldığını söylemesinin özel bir yolu vardır. Ama bunu kullanmak için bu özelliği tercih etmeliyiz ve yine de işe yaramazsa diye eski yöntemle yapma yolunu da tutmaya devam etmeliyiz.

```c
    // XFlush()'tan sonra
    Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
    if(!XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1))
    {
        printf("WM_DELETE_WINDOW özelliği kaydedilemedi\n");
    }
```

```c
                // DestroyNotify case'inden sonra
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
```

Burada yaptıklarımız çoğunlukla tanıdık olmalı. Bir `atom` alıyoruz, bununla ilgili ayrıntılar [belirtimde](https://www.x.org/releases/X11R7.6/doc/xorg-docs/specs/ICCCM/icccm.html#clientmessage_events) yer almakta.

Daha sonra pencere yöneticimize pencere silme hakkında bir olay almak istediğimizi bildirmek için penceremizde bir özelliği ayarlayacak `XSetWMProtocols` fonksiyonunu kullanırız, böylece silmeyi kendimiz yapabiliriz.

Bu, bizim olay döngüsümüzde yaptığımız şeydir. Bir `ClientMessage` alırız, pencerenin silinmesiyle ilgili bir mesaj olup olmadığını kontrol ederiz ve sonra penceremizi kendimiz yok ederiz ve artık penceremiz gittiğinden döngümüzden çıkılmasını söyleriz. Bu, herhangi bir hata mesajının oluşmasını önler.

[<- Bölüm 3 - Pencereyi Büyütme](../pencereyi-buyutme/README.md) [Bölüm 5 - Arabellek ->](../arabellek/README.md)
