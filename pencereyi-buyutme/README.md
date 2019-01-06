## Bölüm 3 - Pencereyi Büyütme

Şimdi de pencereyi büyütmeye çalışalım. Bunun için herhangi bir kolaylık yok. Bu bölümde pencere yöneticisi, atomları ve özellikleriyle iletişim kurmak için kullanılan UPA hakkında bilgi edineceğiz. Bu kesinlikle başlangıçta belirtilen pencere yöneticisi belgelerini açmanızın gerektiği zamandır.

`Atom`'ları anlamak zor değildir, onların karmaşık görünmesine neden olan sadece saçma isimleridir. Penceremize ait bilgi ve özellikler, X'in içinde saklanır ve bunlar özellik olarak adlandırılır. Özellikler benzersiz etiketlerle tanımlanır, bu etiketlere de `atom` denir. Özünde, X bazı nedenlerden dolayı anahtar-değer çiftleri olan atom-özellik çiftlerini çağırır. Muhtemelen nedenini sormamak en iyisi.

```c
Status toggleMaximize(Display* display, Window window)
{
    XClientMessageEvent ev = {};
    Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
    Atom maxH = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom maxV = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

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
```

```c
    // ...
    // main içinde, XMapWindow() sonrasında

    toggleMaximize(display, window);
```

Atom-özellik çiftleri olay sistemi aracılığıyla X'e gönderilir. İstemciden sunucuya akan, `ClientMessage` isminde özel bir olay türü vardır.

Atom'ların ismine göre sorgulanması gerekir. Fonksiyon ayrıca eğer mevcut değilse bir `Atom`'un yaratılıp yaratılmayacağını bilmek ister, bunu istemeyiz. Var olması gereken standartlaştırılmış `Atom`'ları kullanıyoruz, eğer yoklarsa muhtemelen bir yazım hatası yaptık ve bunu bilmek isteriz. `Atom`'lar sadece tamsayı indeksleridir, ama özel bir `None` değeri de vardır, bu yüzden buna karşı kontrol ediyoruz. Ayrıca, sadece `_NET_WM_STATE` Atom'unu kontrol ederiz çünkü [belirtime](https://specifications.freedesktop.org/wm-spec/1.3/ar01s05.html#idm140238712324208) göre bu atom'un varlığı diğer ikisinin varlığını da gerektirir.

Şimdi istemci mesajını dolduruyoruz. Tür, önceden tanımlanmış bir int olan `ClientMessage`'dır.

Biçim 32'dir. Bu, 32 bit anlamına gelir ve bu mesajın alıcısına `ev.data.*` dizisine ne tür veriler eklediğimizi anlatır. Bu veri dizisi parçası aşağıdakileri içeren bir birleşik yapıdır:

```c
union {
    char b[20];
    short s[10];
    long l[5];
} data;
```

Biçim için olası değerler, `char`, `short` ve `long` türlerine karşılık gelen bit boyutları olan 8, 16 veya 32'dir.

Sonra penceremizi ve `atom message_type`'ımızı geçiyoruz. Veri dizisi, belirtime göre anahtar belirtmek için 2 ile başlar (`NETWMSTATETOGGLE` öğesini 2 olarak tanımlar, ancak bendeki başlık dosyaları bu tanımlamayı içermiyor). Bunu takiben, dikey ve yatay maksimizasyon için iki `atom` vardır. Son numara bir [kaynak göstergesidir](https://specifications.freedesktop.org/wm-spec/1.3/ar01s07.html#sourceindication), bu programınızın normal bir kullanıcı uygulaması, bir görev çubuğu gibi bir şey veya protokolün önceki ve sonraki bir sürümü olup olmadığını gösteren bir sayıdır. Normal bir uygulama için kaynak göstergesi 1'dir.

Olayımız bittiğinde onu yoluna göndeririz. Yine, `_NET_WM_STATE` belirtimine uygun olarak, bu tür bir mesajın kök pencereye (masaüstü gibi bir şey) gönderilmesi gerektiğini biliyoruz. Kullanılacak `event_mask` benim için tamamen anlaşılır değil, fakat benim testlerimde maksimizasyon sadece verilen bir maske varsa etkili oluyor ve istenen pencere yapısındaki herhangi bir değişiklik olarak ifade edilebilecek `SubstructureNotifyMask`, bu durumda bir anlam ifade eder.

Tüm bu bilgilerin nereden geldiğini ve bu bilgileri nasıl bulabileceğinizi anlayabilmeniz için bu noktada belgelendirmeye ve belirtimlere kendinizi alıştırdığınızdan emin olun. Farklı sorunları kendiniz çözmeniz gerektiğinde öğrendiğiniz en önemli beceri bu olacaktır.

[<- Bölüm 2 - Minimum Boyut](../minimum-boyut/README.md) [Bölüm 4 - Daha İyi Kapatma ->](../daha-iyi-kapatma/README.md)
