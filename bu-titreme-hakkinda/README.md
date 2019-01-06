## Bölüm 7 - Bu titreme hakkında...

İşleri yapma şeklimiz şimdi pencere boyutunu değiştirirken titremeye neden oluyor. Ana döngümüzü tam hızda çalıştırdığımız için bu titreme MİB'iniz yavaşsa daha belirgindir.

Olan şey şu ki, penceremizin boyutu değişir değişmez tüm pencerenin rengi siyah (pencere arka planı piksel rengi) olur. Bir hata ayıklayıcıda çalıştırılsa ve `ConfigureNotify` olayına ulaşır ulaşmaz durdursak bile, pencere halihazırda siyahtır. Burada, X sisteminin tutarsız doğası biraz sıkıntı vericidir. X iç pencere mantığının yürütülmesi ve kodumuz eşleşmemiştir.

Bu sorun için tam bir çözüm olmasa da bu eğitimin kapsamı için yeterli olacak çözüme bakalım şimdi.

Suçlu, penceredeki `gravity` özniteliğidir. Öntanımlı olarak bu, X'e yeniden boyutlandırmada pencere içeriğini atmasını söyleyen `ForgetGravity` değerine ayarlanır. Bunun olmasını istemiyoruz, bu yüzden `StaticGravity`'e ayarlamalıyız. Bu titremeyi durdurur.

Bu, pencereyi oluşturmadan önce belirlediğimiz pencere özniteliklerimizin artık şunun gibi görüneceği anlamına gelir:

```c
    XSetWindowAttributes windowAttr;
    windowAttr.bit_gravity = StaticGravity;
    windowAttr.background_pixel = 0;
    windowAttr.colormap = XCreateColormap(display, root,
                                          visinfo.visual, AllocNone);
    windowAttr.event_mask = StructureNotifyMask;
    unsigned long attributeMask = CWBitGravity | CWBackPixel | CWColormap | CWEventMask;
```

Sadece `windowAttr.bit_gravity`'i `StaticGravity` olarak ayarlamamız ve `attributeMask`'a `CWBitGravity`'i eklememiz gerekir.

[<- Bölüm 6 - Değişen pencere boyutuna uyum](../degisen-pencere-boyutuna-uyum/README.md) [Bölüm 8 - KeyPress ve KeyRelease->](../keypress-ve-keyrelease/README.md)
