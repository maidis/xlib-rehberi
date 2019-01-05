## Bölüm 1 - Uygulamayı Kapatma

Bu rahatsızlığı çözmek için, pencere kapalıyken `while` döngüsünü `false` olarak ayarlamanın bir yolunu bulmaya ihtiyacımız var. Bunu yapmanın yolu olaylardan geçiyor.

```c
    int windowOpen = 1;
    while(windowOpen)
    {
        XEvent ev = {};
        while(XPending(display) > 0)
        {
            XNextEvent(display, &ev);
            switch(ev.type)
            {
            case DestroyNotify: {
                XDestroyWindowEvent* e = (XDestroyWindowEvent*) &ev;
                if(e->window == window)
                {
                    windowOpen = 0;
                }
            }
            break;
            }
        }
    };
```

Önceki bölümdeki `while` döngüsünü bununla değiştiriyoruz. Bu temel olay döngüsüdür. Bekleyen olaylar varken çalışır. Bir olay olduğunda, olay yığınından bir olay alır ve olay türüne göre anahtarlama yapar. Burada önemsediğimiz `DestroyNotify` olayıdır, genel olayı bu belirli olaya dönüştürmemiz gerekiyor. Şimdi olayın kendi penceremizde hedeflenip hedeflenmediğini kontrol ediyoruz ve eğer durum buysa `windowOpen`'ı `false` olarak ayarlıyoruz.

Burada dikkat edilmesi gereken, her bir olayın hangi pencere için kullanıldığını söylemesidir. Zaten sadece kendi ana penceremize ait olan olayları almalıyız ancak bu, birkaç pencere açtıysanız doğru olmayabilir, emin olmak için kontrol ediyoruz. Bu kontrolün neden mevcut olduğunu göz önünde bulundurmayı unutmayacaksanız yalnızca bir pencereniz açık olduğu sürece onu kullanmayabilirsiniz.

Olay işlemenin çalışmasını sağlamak için yazdığımız kodda ayarlanması gereken bir şey var. `windowAttrs`'ta bir ek değer daha ve `attributeMask`'ta bazı yeni değerler eklemek zorundayız (Bu kod `XCreateWindow` ile pencere oluşturmadan önce gelmeli).

```c
    windowAttr.event_mask = StructureNotifyMask;
    unsigned long attributeMask = CWBackPixel | CWColormap | CWEventMask;
```

Pencere niteliklerimizi ayarladığımızda, bir başka nitelik olan `event_mask`'ı ekliyoruz. Belgelere bulabileceğiniz gibi, `DestroyNotify` olayı, `StructureNotify` grubunun bir parçasıdır, bu yüzden pencerede bu tür olayları almak istediğimizi X'e bildirmek için `StructureNotifyMask` değerini ayarlamak zorundayız. Bu tür bir olaya dahil olma durumu, aldığımız olayların miktarını ve dolayısıyla aslında önemsemediğimiz olaylar aracılığıyla çalışan olay döngüsünde geçirdiğimiz zamanı sınırlar. Hangi özniteliklerin ayarlandığını `XCreateWindow()`'a bildiren öznitelik maskemizde `CWEventMask`'ı ekliyoruz.

Şimdi açık kalan bir pencere ve pencere kapatıldığında sona eren bir uygulamamız var. Oldukça muntazam ha?

[<- Bölüm 0 - Bir Pencere Oluşturma](../bir-pencere-olusturma/README.md) [Bölüm 2 - Minimum Boyut ->](../minimum-boyut/README.md)
