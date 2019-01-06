## Bölüm 6 - Değişen pencere boyutuna uyum

Pencereyle oynadığınızda farkedeceğiniz şeylerden biri, pencerenin boyutunun değiştirilmesinin çok fazla bir şey yapmamasıdır. Daha büyük bir pencereden elde ettiğimiz yeni alan tamamen siyahtır ki hiç hoş değil gerçekten! Siyahlığın nedeni pencereyi oluştururken arka plan pikselini siyah olarak ayarlamamızdır.

Pencere boyutu değiştiğinde `xWindowBuffer` resmimizin boyutunu değiştirmek için pencere boyutu değişikliğinde bize gönderilen başka bir olayı yakalamamız gerekir. Sonra görüntüyü yok edip yeni genişlik ve yükseklikle yeniden oluşturmalıyız. Tüm bu kod parçalarının birbirine nasıl uyduğunu takip etmekte sorun yaşıyorsanız depoda yer alan kaynak kodların tam haline bakabileceğinizi unutmayın.

```c
    // windowOpen = 1;'den ve ana döngüden önce

    int sizeChange = 0;
```

```c
                // switch(ev.type) içinde

                case ConfigureNotify:
                {
                    XConfigureEvent* e = (XConfigureEvent*) &ev;
                    width = e->width;
                    height = e->height;
                    sizeChange = 1;
                }
                break;
```

```c
        // event döngüsünden sonra, ızgara çiziminden önce

        if(sizeChange)
        {
            sizeChange = 0;
            XDestroyImage(xWindowBuffer); // Ayırdığımızı belleği serbest bırakır;
            windowBufferSize = width * height * pixelBytes;
            mem  = (char*)malloc(windowBufferSize);

            xWindowBuffer = XCreateImage(display, visinfo.visual, visinfo.depth,
                                         ZPixmap, 0, mem, width, height,
                                         pixelBits, 0);
        }
```

Öncelikle `sizeChange`'i deklare eder ve `0` olarak ayarlarız. Sonra `ConfigureNotify` olaylarını ele almak için başka bir `case` ekleriz. Bunlar, pencerenin geçerli genişliğini ve yüksekliğini içerir. Burada genişliğin ve yüksekliğin değiştiğini belirtmek için `sizeChange`'i `1` olarak ayarladık. Tek bir karede çeşitli yapılandırma olayları alabileceğimizden (ana program döngümüzün bir dönüşü) olay döngüsünden sonra biraz geçene kadar halihazırdaki resmin imhasını ve yeniden oluşturulmasını erteleriz.

`if(sizeChange)`'de görüntüyü imha ederiz, yeni genişlik ve yüksekliği kullanarak belleği tekrar ayırırız, sonra `XCreateImage`'i ilk olarak yaptığımız gibi çağırırız. `XDestroyImage()`, kullandığımız belleği serbest bırakır, eğer `malloc` kullanmaktan kaçınmak ve daha fazla pratik bellek yönetimi yapmak istiyorsak `XImage` yapısını kendimiz doldurmak zorunda kalacağız.

Tüm bunlarla artık penceremizi yeniden boyutlandırabilir ve ızgarayı pencereyi dolduracak şekilde genişletebiliriz! Ama belki şimdiden yaptığımız şeyle ilgili bir sorun tespit etmiş olabilirsiniz.

[<- Bölüm 5 - Arabellek](../arabellek/README.md) [Bölüm 7 - Bu titreme hakkında... ->](../bu-titreme-hakkinda/README.md)
