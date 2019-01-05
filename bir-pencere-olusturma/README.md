## Bölüm 0 - Bir Pencere Oluşturma

Adım adım kodlarda ilerleyelim. Kodları tam haliyle görmek için GitHub'daki dosyalara bakabilirsiniz.

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

int main(int argc, char** args)
{

}
```

Bu muhtemelen Windows'tan gelen insanlar için ilk küçük farktır. Pencereyi oluşturmak isteyen programlar için özel bir main fonksiyonu yoktur, sadece standart C `main()` fonksiyonu vardır. Sonraki birkaç kod parçası `main()` fonksiyonuna aittir.

```c
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
```

İlk olarak daha sonra kullanmak için genişlik ve yüksekliği tanımlarız. Daha sonra (0 geçerek) standart ekranı açıyoruz. X, bir ağ protokolüne sahip bir istemci-sunucu modeli olarak kurulmuştur. Bu, sunucunun, hangi istemcilerin var olduğu ve hangi pencerelerin başka pencerelerle etkileşimde bulunabileceği hakkında bir fikre sahip olması gerektiği anlamına gelir. X sunucusunun kiminle konuştuğunu da bilmesi gerekiyor, bu yüzden display (bazı yerlerde ekran olarak da kullanılabilir, kastedilen her zaman display'dir) işaretçisini tutmalıyız. Bunu diğer çoğu Xlib fonksiyonuna ileteceğiz. Ayrıca, çağrının geçerli bir ekran döndürdüğünü kontrol ederiz.

Daha sonra bir kök pencere alırız. Pencereler bir ağaç yapısında düzenlenmiştir, bu nedenle oluşturduğumuz her pencerenin bir ebeveyninin olması gerekir. Burada mormal bir en üst düzey pencere oluşturuyoruz, dolayısıyla ebeveynimizin masaüstü olması gerekiyor, bu kök penceredir. Ayar penceresi olan bir uygulama yapıyor olsaydık ana penceremiz için ayarlar penceresini ebeveyn olarak kullanabilirdik.

Son olarak öntanımlı ekranı alırız. X'teki bir ekran genellikle sadece bir alan veya oluşturulacak bir arabellektir, bu bir grafik aygıtından gerçek bir monitöre veya tamamen yazılım içinde bir bellek içi hedefe olabilir.

```c
    int screenBitDepth = 24;
    XVisualInfo visinfo = {};

    if(!XMatchVisualInfo(display, defaultScreen, screenBitDepth, TrueColor, &visinfo))
    {
        printf("Hiçbir eşleşen görsel bilgi yok\n");
        exit(1);
    }
```

Burada tarama hedefimiz için ne tür bir gereksinim olduğunu tanımlarız. Bu, tek renkli ekranlar ve farklı renk derinliklerinde görüntüler olduğunda, yani X'in çıktığı 1984'te çok daha ilginçti. Bugünlerde bu neredeyse her zaman aynıdır. RGB-8-8-8 ve TrueColor için 24 bit.

`XMatchVisual` gereksinimlerimizi alır, bunları bu donanımla desteklenen bir dahili `visualInfos` listesiyle eşleştirir ve en uygun olanı döndürür. Ya da bizim gereksinimlerimizi karşılamak mümkün değilse uygun olmadığını söyler ama bu günlerde bu pek olası değildir.

```c
    XSetWindowAttributes windowAttr;
    windowAttr.background_pixel = 0;
    windowAttr.colormap = XCreateColormap(display, root,
                                          visinfo.visual, AllocNone);
    unsigned long attributeMask = CWBackPixel | CWColormap;

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
```

Pencereyi oluşturduğumuz yer burası. `XSetWindowAttributes` yapısında, pencere için bazı öznitelikleri ayarladık. Arkaplan dolgu pikselini siyah olarak belirledik ve daha önce oluşturduğumuz visualinfo'dan bir renk haritası oluşturduk. Sonra `XCreateWindow`'a hangi özelliklerin kullanılmasını istediğimizi söyleyen maskeyi, `attributeMask`'ı tanımlarız. Çünkü bütün bu ayarladıklarımızı kullanmak istiyoruz.

Belgelerine bakarsanız `XCreateWindow` bu noktada oldukça açıklayıcıdır. Şimdiye kadar hazırladığımız tüm bilgileri ona aktarıyoruz. `InputOutput`, bu pencerenin gelen olayları alıp ele alacağı ve aynı zamanda ekrana çıkış vereceği anlamına gelir. Daha sonra, pencere oluşturmanın çalışıp çalışmadığını kontrol ederiz.

```c
    XStoreName(display, window, "Merhaba Dünya!");

    XMapWindow(display, window);
    XFlush(display);

    return 0;
```

Önce penceremize uygun bir başlık verelim. Pencere başlığı, pencere yöneticisinin bir özelliğidir. `XStoreName()`, bu UPA'yı bizden bir süre gizleyen kolaylaştırıcı bir fonksiyondur. Şimdi pencereyi ekrana eşleştiriyoruz. Bu, pencerenin görünür hale geleceği anlamına gelir. Daha sonra tüm komutlarımızın sunucuya geçtiğinden emin olmak için temizleriz. Şimdi bunu çalıştırırsak hiçbir şey olmaz. Bunu bir hata ayıklayıcıda başlattığımızda ve son satırda duraklattığımızda pencerenin başarıyla oluşturulduğunu ancak hemen kapatıldığını görüyoruz. Şimdilik fena değil.

Aslında bir pencereyi hiç uğraşmadan göstersek daha iyi değil mi? Bunun için `XFlush()`'ın sonrasına aşağıdakileri ekleyin:

```c
    while(true)
    {
    }
```

Programımızı çalıştırdığımızda, pencerenin açık kaldığını görebiliyoruz. Şimdi pencerede kapat düğmesine basabiliriz ve pencere kapanır!

Ama bekle bir saniye. Programı sonsuz bir döngüde sıkıştırdık, bu döngüden nasıl uzaklaşabiliriz? Görev yöneticimize bakarsak, mutlu küçük penceremize ait sürecin hala devam ettiğini görebiliriz. Bunun nedeni pencerenin kendi programımızın sürecine doğrudan bağlı olmamasıdır. Pencereyi pencere yöneticisinden ve X sunucusunun kenarından kapatmak, sürecimizi durdurmaz.

[<- Giriş](../giris/README.md) [Bölüm 1 - Uygulamayı Kapatma ->](../uygulamayi-kapatma/README.md)
