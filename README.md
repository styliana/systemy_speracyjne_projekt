# Salon fryzjerski | Projekt na Systemy Operacyjne

Link do github: https://github.com/styliana/systemy_operacyjne_projekt

Zadanie stanowi rozszerzenie problemu śpiącego fryzjera/golibrody na wielu fryzjerów:

https://pl.wikipedia.org/wiki/Problem_śpiącego_golibrody

W salonie pracuje F fryzjerów (F>1) i znajduje się w nim N foteli (N<F). Salon jest czynny w godzinach od Tp do Tk. Klienci przychodzą do salonu w losowych momentach czasu. W salonie znajduje się poczekalnia, która może pomieścić K klientów jednocześnie. Każdy klient rozlicza usługę z fryzjerem przekazując mu kwotę za usługę przed rozpoczęciem strzyżenia. Fryzjer wydaje resztę po zakończeniu obsługi klienta a w przypadku braku możliwości wydania reszty klient musi zaczekać, aż fryzjer znajdzie resztę w kasie. Kasa jest wspólna dla wszystkich fryzjerów. Płatność może być dokonana banknotami o nominałach 10zł, 20zł i 50zł.

**Zasada działania fryzjera (cyklicznie):**

- wybiera klienta z poczekalni (ewentualnie czeka, jeśli go jeszcze nie ma);

- znajduje wolny fotel;

- pobiera opłatę za usługę i umieszcza we wspólnej kasie (opłata może być również przekazana do wspólnej kasy bezpośrednio przez klienta, ale fryzjer musi znać kwotę, żeby wyliczyć resztę do wydania);
- realizuje usługę;

- zwalnia fotel;

- wylicza resztę i pobiera ze wspólnej kasy, a jeśli nie jest to możliwe, czeka, aż pojawią się odpowiednie nominały w wyniku pracy innego fryzjera;

- przekazuje resztę klientowi.

**Zasada działania klienta (cyklicznie):**

- zarabia pieniądze;

- przychodzi do salonu fryzjerskiego;

- jeśli jest wolne miejsce w poczekalni, siada i czeka na obsługę (ewentualnie budzi fryzjera), a w przypadku braku miejsc opuszcza salon i wraca do zarabiania pieniędzy;

- po znalezieniu fryzjera płaci za usługę;

- czeka na zakończenie usługi;

- czeka na resztę;

- opuszcza salon i wraca do zarabiania pieniędzy.

Na polecenie kierownika (sygnał 1) dany fryzjer kończy pracę przed zamknięciem salonu. Na polecenie kierownika (sygnał 2) wszyscy klienci (ci którzy siedzą na fotelach i z poczekalni) natychmiast opuszczają salon. Napisz procedury Kasjer (kierownik), Fryzjer i Klient symulujące działanie salonu fryzjerskiego.

**Przykładowe testy:**
- Test wolnego fotela: Sprawdzono, czy fryzjer zajmuje pierwszy wolny fotel, a klient poprawnie opuszcza poczekalnię po rozpoczęciu usługi.

- Test brak miejsca w poczekalni: Zweryfikowano, że klient opuszcza salon, gdy poczekalnia jest pełna.

- Test zakończenia usługi: Sprawdzono, czy fryzjer po zakończeniu obsługi zwalnia fotel i przekazuje resztę klientowi.

- Test opłaty i wydania reszty: Zweryfikowano, że fryzjer poprawnie oblicza resztę i czeka na odpowiednie nominały w kasie, jeśli są niedostępne.

- Test poczekalni: Sprawdzono, czy klienci są obsługiwani w kolejności przybycia.
