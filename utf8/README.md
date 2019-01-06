## Bölüm 9 - XInput'tan UTF-8 Karakterler (Hayır, oyun çubuğu olanından değil)

Burası, işlerin başa çıkması biraz daha sinir bozucu olduğu yer oluyor. UTF-8 bu günlerde öntanımlı karakter kodlaması, iyi ki de öyle. Ancak X eskidir ve temel klavye UPA'sı da öyledir. O zamanlar UTF-8 bile mevcut değildi, bu yüzden bir noktada UTF-8 desteği için X'in temel klavye özelliklerine yönelik bir uzantı tanımlandı, ancak bu da daha çok ayarlama zahmeti getirdi. Bu eklenti `XInput` olarak adlandırılır ve bir uzantı olarak adlandırılmasına rağmen, bugünlerde herhangi bir Linux sisteminde mevcut olduğu varsayılabilir. Bu fonksiyonların çoğuna, Xlib belgelerinde yer verilmiştir, `Xutf8LookupString`, farklı standartlar kümesindendir ve bir man sayfasına sahiptir.

```c
    // setSizeHint(display, window, 400, 300, 0, 0); satırından sonra

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
```

Çok az kazanç için çok fazla kod. Bu, bize tamamen özelliksiz bir `XInput` ayarlaması verir ve temel klavye UPA'sının herhangi bir ayarlama yapmadan yaptığını yapar.

İlk önce `XOpenIM()` ile bir `XInput` Yöntemi açtık. Bu `display`'i alır ve sonrasında bir kaynak veritabanıyla ilgisi olan üç değer vardır. Öntanımlı olarak bunları görmezden gelebiliriz. Bu giriş yöntemiyle `XGetIMValues()`'u çağırabiliriz, `XInput` UPA'sındaki birçok fonksiyon gibi bu da bir değişken argüman sayılı fonksiyondur. `XNQueryInputStyle` makrosunu kullanarak ne tür bir değer elde etmek istediğimizi belirtiriz, daha sonra bunu doldurmak için bir işaretçi veriririz ve değişken sayılı argümanı `NULL` ile sonlandırırız. Şimdi girdi biçimlerinin bir listesine sahibiz. Bir giriş biçiminin ne olduğundan tam olarak emin değilim, fakat daha sonra ihtiyacımız olacağından bir tane almak zorundayız.

Şimdi tüm girdi biçimlerinin bir listesine sahip olduğumuzdan birini seçip onu kullanalım. Geri aldığımız tüm biçimlerin üzerinden geçiyoruz, eğer biçim `XIMPreeditNothing` ve `XIMStatusNothing` bayraklarını içeriyorsa bunu alıp çıkarıyoruz. Belgelendirme de bu konuda epey belirsiz, tek bildiğim, bu iki tanımlama ve girdi biçiminin hiçbir şekilde özel olmadığıdır.

Girdi yöntemi ve sorgulanan girdi biçimiyle bir girdi bağlamı oluşturabiliriz, bu asıl ihtiyaç duyduğumuz kısımdır. `XCreateIC()` başka bir değişken argüman sayılı fonksiyondur. Girdi yöntemini alır, `bestMatchStyle`'ımızı sağlarız ve penceremizin ne olduğunu ve buna odaklanması gerektiğini söyleriz (odaktayken olayları ona gönderir). `Varargs`, `NULL` ile sonlandırılır.

Şimdi neden girdi bağlamına ihtiyacımız olduğuna bakalım.

```c
                    // KeyPress: case'i içinde
                    // XKeyPressedEvent* e = (XKeyPressedEvent*) &ev; satırından sonra

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
```

`Xutf8LookupString()` fonksiyonu bir girdi bağlamı, bir `XKeyPressedEvent`, döndürülen UTF-8 karakteri için bir arabellek ve arabellek boyutu alır, bir işaretçi içinde karşılık gelen `KeySym`'i ve son bir argüman olarak aldığı `Status` işaretçisini döndürebilir.

Bundan sonra olanlar yine çok basit. `status`'u kontrol ediyoruz ve eğer bir karakterse yazdırıyoruz.

[<- Bölüm 8 - KeyPress ve KeyRelease](../keypress-ve-keyrelease/README.md) [Sonuç ->](../sonuc/README.md)
