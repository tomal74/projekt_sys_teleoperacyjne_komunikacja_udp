# Komunikacja z wykorzystaniem protokołu UDP - Projekt 

Komunikacja klient serwer pomiędzy dwoma PCtami. Program działa pod Windowsem, jest pisany pod VSC 2022. Przedstawia realizacje zadań z zakresu komunikacji UDP oraz sterowania robotem.

## Autorzy
- [@tomal74](https://www.github.com/tomal74) - wszelkie pytania tomal74@o2.pl
- [@Paweu00](https://www.github.com/Paweu00) 


## Server
#### Dostępne są dwa pliki serwera:  
[Serwer 1](/SYS_TELE_UDP_SERV) - realizuje odbiór danych z UDPa, wyświetlanie odebranych danych oraz odesłanie tych samych danych z powrotem do klienta. Serwer wyświetla także adres i port źródłowy odebranych danych. Plik `Packet.h` zawiera strukturę `Packet`, która reprezentuje format przesyłanych danych. UWAGA! Należy pamiętać aby wielkość struktury `Packet` (sizeof), nie był wiekszy od zdefiniowanego rozmiaru bufora odbiorczego/nadawczego w pliku `main.cpp` - (BUFLEN).

[Serwer 2](/SYS_TELE_UDP_SERV_STEROWANIE_ROBOTEM) - realizuje odbiór prędkości zadanych przez klienta UDP i symuluję robota mobilnego klasy (2,0). Zapewnia także linearyzację przez sprzeżenie zwrotne (sterowanie z dyszelkiem) oraz implementuje unikanie wcześniej dodanych przeszkód z wykorzystaniem funkcji potencjałowej. Całość wyświetlana jest przy użyciu narzędzia typu command-line - `Gnuplot`.  
Dlatego też należy najpierw zainstalować Gnuplota - [Installing Gnuplot on Windows](http://spiff.rit.edu/classes/ast601/gnuplot/install_windows.html). 

#### Pliki zawierają autorskie klasy:  
- `softDiffRobot`, która symuluje działanie robota mobilnego o napędzie różnicowy  
- `SoftTimer`, która realizuję pracę timerów softwarowych, dzięki którym można realizować funkcję nieblokujące.   
- `Packet` - strukturę, w której umieszczony jest format przesyłanych przez UDP danych.
Plik `main.cpp` zawiera główną pętle programu, w której cyklicznie odbierane są dane z UDP a następnie komendy, które zostały odebrane z klienta steują robotem.
 


## Client
Klient realizuje dwa zadania. Sprawdzanie opóźnienia dostarczania pakietów wraz z % utraty pakietów i sterowanie robotem poprzez wysyłanie prędkości w osiach x i y.  
Wybór zadania realizuje się poprzez ustawienie zmiennej `new_task` na `KOMUNIKACJA` lub `STEROWANIE`.

### Sterowanie

Prędkości zadawane są w osiach x i y poprzez naciskanie strzałek z klawiatury. Funkcja `GetKeyState` odpowiada za wykrywanie odpowiednich przycisków.

### Komunikacja

Komunikacja pozwala na wysyłanie wiadomości oraz licznika. Po wysłaniu program czeka 0.5s na potwierdzenie odbioru od serwera. Brak potwierdzenia w określinym czasie powoduje zwiększenie % utraconych pakietów i wysłanie kolejnego pakietu.