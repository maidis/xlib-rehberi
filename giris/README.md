## Giriş

Programcılar sadece Microsoft dünyasında kalmak istemediklerinde Linux'ta pencerelerle doğrudan çalışmamak konusunda uyarılmışlardır. Bunu yapmak yerine GTK, Qt veya SDL kullanmaları söylenmiştir.

Bu, akıl sağlığınız için değerli bir öneri olsa da kütüphanelerin yardımı olmadan bunun nasıl yapılacağına dair eğitim içeriklerinin eksikliğine de yol açmıştır. Bu, sadece bu tür programlama yapmak isteyenler veya yeni kütüphaneler yazmayı düşünenler için işleri daha da zorlaştıran bir durumdur.

Tüm bu nedenlerden dolayı bu konuda yazılmış derli toplu bir kaynak olan [A tour through Xlib and related technologies](https://handmade.network/wiki/2834-tutorial_a_tour_through_xlib_and_related_technologies)'i çevirmeye karar verdim. Bu yazı büyük oranda aslına sadık bir çeviridir.

Ben buradaki bilgilerden SFML'i Haiku'ya aktarırken Xlib ile ilgili fonksiyonları Haiku'nun yerel fonksiyonlarına çevirebilmek için yararlı olacak genel bilgileri edinmek için yararlanacağım.


### Başlamadan Önce

Başlamadan önce tartışmamız gereken birkaç şey var. Bu eğitici X ile kullanım için Xlib kütüphanesiyle ilgilidir, yeni xcb kütüphanesi hakkında değildir. Xlib'in kullanımdan kaldırılması ve yerine xcb'nin geçmesi gerektiği düşünülse de xcb, opengl ile bağlantı kurmak için kullanılamaz ve ek olarak Xlib'e de çok benzerdir. Anlayabildiğim kadarıyla bugün Xlib olmadan bir şey yapmanın gerçekten bir yolu yoktur. Bu durumda, bir projede iki çok benzer kütüphaneyi kullanmanın bir mantığını görmüyorum ve bu bence Xlib'i herhangi bir durum hakkında üstün bir seçim haline getiriyor. Ayrıca, Xlib'in kullanımdan kaldırılmış olarak tanımlandığı gerçeğinden endişelenmenize gerek olmadığını düşünüyorum. Xlib hala en çok kullanılan iki kütüphaneden biridir. Büyük olasılıkla Xlib desteğinin sonunu görmeden önce X sisteminin ölümünü göreceğiz.

Bunları aklımızda tutarak, xlib belgelerini elimizin altında tutmalıyız. Resmi belgeler [www.x.org/wiki/ProgrammingDocumentation](https://www.x.org/wiki/ProgrammingDocumentation/) adresinde bulunabilir. Bununla birlikte belgeler için çoğunlukla [tronche.com/gui/x/xlib](https://tronche.com/gui/x/xlib/) adresindeki belgeler kullanılmıştır.

Bu eğitseli bitirdiğinizde her zaman belgelere erişin ve yeni fonksiyonlara denk geldikçe bunları okuyun. Bu belge ne yapacağınız ve nereden başlayacağınız konusunda size bir fikir verebilir, ancak resmi ve daha detaylı belgelerdeki özellikleri okumak her zaman daha iyidir.

İleride gerekli olabilecek ilgili standart belgeleri [specifications.freedesktop.org/wm-spec/1.3](https://specifications.freedesktop.org/wm-spec/1.3/) ve [www.x.org/releases/X11R7.6/doc/xorg-docs/specs/ICCCM/icccm.html](https://www.x.org/releases/X11R7.6/doc/xorg-docs/specs/ICCCM/icccm.html) adreslerinden bulunabilir.

Bunlar pencere yöneticisini kontrol etmek için gereklidir. Pencere yöneticisinin ne olduğu hakkında daha sonra kısaca konuşulacak.

Bu örnekleri inşa etmek için çok fazla kurulum gerekli değildir, sadece kodu derleyicinize aktarın ve -lX11 seçeneğiyle bağlayın. Ayrıca libx11-devel veya benzeri bir pakette bulunması muhtemel xlib geliştirme dosyalarına da ihtiyacınız vardır.


### Genel Bakış

Linux'taki pencere sisteminin birkaç parçası vardır. Aşağıdakilerin, ayrıntıların çoğunu özetleyen çok kaba bir genel bakış olduğunu unutmayın. Daha çoğunu bilmek istiyorsanız, tüm bu şeyler hakkında epey uzun Vikipedi makaleleri vardır.

- **X:** Bazen X.org, X-Server veya sadece X olarak adlandırılır. Bu şeylerin hepsi biraz farklı anlamlara sahiptir, ama bunun için endişe etmeyelim. Bu belgede buna sadece X denecektir. X, tüm pencerelerin durumunu depolayan yazılımdır. X, cihaz girişini alır ve bunu bir olay sistemi aracılığıyla uygulamalara sunar. Bu sayede pencereleri oluşturmayı kolaylaştırır. X aynı zamanda GPU sürücüsünün sıklıkla bağlandığı Linux sisteminin bir parçasıdır. Genel olarak Linux'ta OpenGL kullanmak için bir X uygulaması yapmanız gerekir. X, sunucu yazılımıdır, istemciler aynı veya farklı bilgisayarlarda olabilir.
- **Ekran yöneticisi:** Bu bir X istemci oturumunu tanımlayan programdır. Aynı veya farklı bir makinede bir X sunucusuna bağlanır. X'in hemen hemen tüm çağrıları için ekranı, bir istemci kimliği olarak sunmanız gerekir.
- **Pencere yöneticisi:** X çoğunlukla sadece pencerelerin durumunu depolar ve onlara olayları aktarır, pencere yöneticisi pencerelerinizin ne yaptığını, nasıl göründüklerini, ilk olarak ekranda nerede ve ne boyutta göründüğünü önemseyen programdır. Pencere yöneticisi ayrıca Linux diyarına pencere dekorasyonu adı verilen, başlık çubuğu ve düğmelerini de katar. Bu bizim için önemlidir çünkü bazı durumlarda pencere yöneticisiyle etkileşimde bulunmak için kullanılan UPA, X'in kendisiyle etkileşim kurmak için olandan farklıdır.
- **X uygulaması**. Bu biziz, daha doğrusu yapacağımız şeylerdir! Bu ayrıca Xlib'in de devreye girdiği yerdir. X, orijinal olarak ağ üzerinden kullanılmak üzere tasarlandı ve bugün hala bir dereceye kadar bu şekilde çalışır. Ama çoğunlukla bunun bizim için anlamı, Xlib'i kullandığımız zaman, X'in fonksiyonlarını doğrudan X'ten çağırmadığımızı bilmemiz gerektiğidir. Xlib, X tarafından kullanılan ağ protokolü etrafında bir C sarmalıyıcısıdır. Bu, yaptığımız her şeyin başka bir iş parçacığında, başka bir işlemde eşzamansız olduğu anlamına gelir. Değişikliklerin gerçekten etkili olduğundan emin olmak istediğimizde komut tamponunu temizlememiz gerekir.


### Sonunda Kodlar

Bu belgede kullanılan tüm kodlar GitHub'da mevcuttur. Parça numarası dosya adında belirtilmiştir. Bu dosyaların referans olması için muhtemelen iyi bir fikirdir.

[<- İçindekiler](../README.md) [Bölüm 0 - Bir Pencere Oluşturma ->](../bir-pencere-olusturma/README.md)
