## Bölüm 2 - Minimum Boyut

Artık işin zor tarafını hallettik, UPA'nın geri kalan işleyişine daha çok alışmak için bazı incelikler ekleyelim. Basit bir şeyle başlayalım.

Pencerenin kenarını tutup daha küçük olacak şekilde yeniden boyutlandırdığımızda pencereyi ne kadar küçük yapabileceğimize dair bir sınır olmadığını fark ederiz. Bunu değiştirelim ve en küçük boy için bir sınır verelim.

```c
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
```

```c
// ...
// main içinde, XMapWindow()'dan önce

setSizeHint(display, window, 400, 300, 0, 0);
```

Bir `XSizeHints` yapısını doldurup ayarlıyoruz. Bayrakları sadece sıfır olmayan bir boyut geçildiğinde ayarlayarak, 0'ın ayarlanmamış anlamına gelmesini sağlarız. Verilen parametrelerle fonksiyonumuzu çağırdığımızda istediği kadar büyük, ancak 400x300'den küçük olmayan bir pencere alırız.

Boyut ipuçları, pencere başlığı gibi bir başka pencere yöneticisi özelliğidir ve kolaylık fonksiyonlarından bir diğeridir.

[<- Bölüm 1 - Uygulamayı Kapatma](../uygulamayi-kapatma/README.md) [Bölüm 3 - Pencereyi Büyütme ->](../pencereyi-buyutme/README.md)
