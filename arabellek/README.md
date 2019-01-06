## Bölüm 5 - Arabellek

Pencereye nasıl bir şey ekleyeceğimizi göstermek için doğrudan bir bellek arabelleğine yazacağız, daha sonra da bunu bir Xlib fonksiyonu kullanarak pencereye çizeceğiz.

`XFlush()` fonksiyonunun hemen ardından aşağıdaki büyük kod parçasını ekliyoruz.

```c
    int pixelBits = 32;
    int pixelBytes = pixelBits / 8;
    int windowBufferSize = width * height * pixelBytes;
    char* mem  = (char*)malloc(windowBufferSize);

    XImage* xWindowBuffer = XCreateImage(display, visinfo.visual, visinfo.depth,
                                         ZPixmap, 0, mem, width, height,
                                         pixelBits, 0);
    GC defaultGC = DefaultGC(display, defaultScreen);
```

İlk iki satır kendini açıklıyordur zaten. Sadece resmimizin 32 bit boyutunda piksellere sahip olacağını, sonra gerekeceği için bunu bayt haline çevrilmesini, daha sonra da ihtiyacımız olan bellek miktarını hesaplamak için genişliğimiz ve yüksekliğimizle çarpımını tanımlarız. Sonra bu hafıza miktarını `malloc` ile bellekte tahsis ederiz.

İş sonraki iki fonksiyondadır. Bu görüntü arabelleğini sadece kendimiz için istemediğimizden ve pencerede görüntülemek için X'e vermemiz gerektiğinden onu X uyumlu bir görüntü yapısına sarmamız gerekiyor.

`XCreateImage`'e ekranı ve penceremiz için yarattığımız `visinfo`'daki görseli geçeriz. Daha sonra, yine `visinfo`'dan piksel derinliğini veririz. Bir sonraki argüman ilginçtir.

Burada `ZPixmap`'i biçim için geçiyoruz. Ama neden `ZPixmap`? Belgelere göre, bu argüman `XYBitmap`, `XYPixmap` veya `ZPixmap`'ten biri olablir. Burada yaptığımız şeyin bir `XYBitmap` olduğunu düşünürseniz sizi suçlamam. Her şeyden önce bir genişliğimiz ve yüksekliğimiz var ve böyle bir şey görsem ben de biteşlem derim.

Bir `ZPixmap` seçmemizin temeline ulaşmak için, bir bitişlem ve bir `pixmap` (piksel haritası) arasındaki farkı elde etmemiz gerekiyor. X-konuşmalarında bir bitişlem, programınızda yalnızca yerel olarak kullandığınız bir görüntüdür. Bir `pixmap` ise X sunucusuna gönderilebilen bir bitişlemdir ve bu nedenle bir monitörde görüntülenebilir. Bu, bir `pixmap` istediğimiz anlamına geliyor.

Yani geriye kalan, `ZPixmap` ve `XYPixmap` arasındaki seçimdir. Bunun hakkında bilgi bulmak zor, ancak [X sunucu belirtiminde](https://www.x.org/releases/X11R7.6/doc/xorg-server/Xserver-spec.html) konusu geçer.

> Pixmap görüntüler sunucuya iki yoldan biriyle aktarılır: `XYPixmap` veya `ZPixmap` ile. XYPixmap'ler, bağlantı yapılandırmasındaki bitmap dolgu kurallarını kullanan, görüntünün her bit düzlemi için bir tane olan bitmapler dizisidir. `ZPixmap`'ler, uygun derinlik için biçim kurallarını (dolgu vb.) kullanan, her piksel için bir tane olan bir dizi `bit`, `nibble`, `byte` veya `word`'dür.

Bu yüzden `XYPixmap`'ler birkaç bit düzlemini geçmek içindir. Bitplane'ler, X'in ilk çıktığı 1984 yılında oldukça havalı olan, ancak temel olarak artık kullanılmayan şeylerden bir diğeridir. Bu makalenin kapsamını tam olarak aşmamak için hadi gelin onları göz ardı etmeyi kabul edelim. Daha çok okuduğumuzda, `ZPixmap`'in dolambaçlı açıklamasının kullanım durumumuza daha iyi uyduğunu görüyoruz. Bizim durumumuzda bu, her piksel için 4 baytlık bir word dizisidir.

Bu büyük olanı hallettikten sonraki üç argüman bize biraz soluk aldırtıyor. Basitçe tahsis ettiğimiz hafızayı, penceremizin ve resmimizin genişliğini ve yüksekliğini iletiriz.

Bir sonraki değişken belgelerde `bitmap_pad` olarak geçiyor. Nasıl tanımlandığına bakmanızı istiyorum.

> Bir tarama çizgisinin nicemini belirtir (8, 16 veya 32). Başka bir deyişle, istemci belleğinde bir tarama çizgisinin başlangıcı, bir sonraki tarama çizgisinin başlangıcından bu bitler kadar ayrılır.

Bu, piksel boyutunu bit cinsinden tarif etmenin çok dolambaçlı bir yoludur. X belgelerini okurken bunun gibi birçok can sıkıcı açıklama göreceksiniz.

Açıklamada ayrıca tarama çizgilerinden de bahsedilmektedir. Amaçlarımız için bir `scanline`, basitçe bitmap'te bir satır veya yatay bir çizgidir. `width * pixelBytes` boyutuna sahiptir.

Son argüman `bytes_per_line`'dır. Bu argüman aslında her satırdaki bayt sayısıyla hiçbir ilgisi yoktur. Bir satırın başlangıcından bir sonraki satırın başlangıcına kadar olan uzaklıktır. Böylece, görüntünüz 3 piksel genişliğinde, 4 bayt pikselse ve her satır arasında 2 bayt boşluk varsa, bu argüman `3 * 4 + 2` olarak ayarlanır. Fakat eğer bizim durumumuzda olduğu gibi resmimizin çizgileri bitişikse burada bir `0` geçebiliriz ve `XCreateImage()` değeri kendisi hesaplayacaktır.

Oluşturduğumuz son şey grafik bağlamıdır. Bu sadece X için, çizdiğimiz asıl donanıma özgü bir bilgidir. Gerçek ekranda Xlib ile bir şeyler çizmek istediğimizde buna her zaman ihtiyacımız var.

Ardından, olay döngüsünden sonra görüntü hafızamıza bir miktar desen çizmek için bir şeyler ekleriz. Sonuçta işe yarayıp yaramadığını görmek isteriz. Bu kod, resmin tamamı üzerine siyah beyaz bir ızgara çizer.

```c
        // burada Event döngüsü ve ondan sonra da aşağıdakiler

        int pitch = width * pixelBytes;
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
```

Bu bölümdeki son adım olarak, kendi hafızamızdaki görüntüyü pencereye çekmemiz gerekir. Aşağıdakileri ızgara çizim kodumuzdan hemen sonrasına ekleyin.

```c
        XPutImage(display, window,
                  defaultGC, xWindowBuffer, 0, 0, 0, 0,
                  width, height);
```

`XPutImage()`, `display`'i, pencereyi ve daha önce oluşturduğumuz `defaultGC`'yi alır. Pencereye çizdiğimizden `GC`'ye ihtiyacımız var. Daha sonra oluşturduğumuz `XImage`'i veriyoruz.

Sonraki dört değer, kaynak görüntünün yanı sıra hedef görüntüdeki ofsetlerdir. Bunu, pencerenin bir bölümünü görüntümüzün bir bölümünden güncellemek isteseydik kullanırdık. Ama işleri basit tuttuğumuzdan burada sadece dört sıfır var. Ardından gelenler genişlik ve yükseklik.

Bunu derleyip çalıştırırsanız beyaz bir arka planda siyah bir ızgara görürsünüz.

[<- Bölüm 4 - Daha İyi Kapatma](../daha-iyi-kapatma/README.md) [Bölüm 6 - Değişen pencere boyutuna uyum ->](../degisken-pencere-boyutuna-uyum/README.md)
