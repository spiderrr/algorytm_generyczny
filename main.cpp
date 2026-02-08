#include <fstream.h>
#include <conio.h>
#include <stdlib.h>
#include <graphics.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>

// Typy wyliczeniowe i stałe
enum BOOL { FALSZ, PRAWDA };
enum sposob { losowo, kolejno, na_1 };

const char nazwa_stala[] = "test.gen";
double pmutation = 0.0333;
double pcross = 0.6;
int popsize = 20;
int bit; 
int word;
int il_chrom = 0;
const int cyfra = 4;
const int il_plik = 2;
const int il_linia = 8;
const int margines = 32;

// --- KLASY PODSTAWOWE ---

class dane {
public:
    int a;
    int b;
};

class rozmieszczenie {
public:
    int arkusz;
    int poz_x;
    int poz_y;
    int obrot;
};

// --- SZABLON WEKTORA ---

template<class T> 
class wektor {
    T *wsk;
    int rozmiar;
public:
    wektor(int r) {
        if (r <= 0 || r > 1000) cout << "Za duzy rozmiar tablicy";
        wsk = new T[rozmiar = r];
    }
    T& operator[](int i) {
        if (i < 0 || i >= rozmiar) {
            cout << "Niewlasciwy indeks tablicy";
        }
        return wsk[i];
    }
    int jaki_rozmiar() { return rozmiar; }
    ~wektor() { delete[] wsk; }
};

// --- OBSŁUGA BŁĘDÓW ---

void blad(char *napis) {
    cout << napis << endl;
    exit(1);
}

// --- KLASA LINIA_PLIKU ---

class linia_pliku {
public:
    linia_pliku() {}
    ~linia_pliku() {}
    
    int linia(const char *l) {
        int ilosc = 0;
        int i = 0;
        char *temp = new char[cyfra];
        if (!temp) blad("Za malo pamieci");
        memset(temp, 0, cyfra);
        
        while (l[i] != '\0') {
            if (isdigit(l[i])) {
                char s[2] = {l[i], '\0'};
                strcat(temp, s);
            }
            if (isspace(l[i]) || l[i] == '\n' || l[i+1] == '\0') {
                ilosc = atoi(temp);
                break; 
            }
            i++;
        }
        delete[] temp;
        return ilosc;
    }

    dane linia2(const char *l) {
        dane dd;
        int ill = 0;
        int i = 0;
        char *temp = new char[cyfra];
        if (!temp) blad("Za malo pamieci");
        
        while (l[i] != '\0') {
            memset(temp, 0, cyfra);
            while (isdigit(l[i])) {
                char s[2] = {l[i], '\0'};
                strcat(temp, s);
                i++;
            }
            if (strlen(temp) > 0) {
                if (ill == 0) dd.a = atoi(temp);
                if (ill == 1) dd.b = atoi(temp);
                ill++;
            }
            if (l[i] == '\0') break;
            i++;
        }
        delete[] temp;
        return dd;
    }
};

// --- KLASY LOGIKI ---

class ilosc_danych {
public:
    int ilosc;
    ilosc_danych() {}
    int przekarz_ilosc(const char *nazwa_we) {
        linia_pliku lp;
        char *wierszyk = new char[il_linia];
        if (!wierszyk) blad("Za malo pamieci");
        
        ifstream pliczek(nazwa_we);
        if (!pliczek) {
            blad("Blad otwarcia pliku: ");
            cout << nazwa_we;
            exit(1);
        }
        pliczek.getline(wierszyk, il_linia);
        ilosc = lp.linia(wierszyk);
        pliczek.close();
        delete[] wierszyk;
        return ilosc;
    }
};

class wczytaj : public linia_pliku {
public:
    int ilosc;
    int rozmiar_a, rozmiar_b;
    dane *d;
    
    wczytaj() {}
    void przekarz_dane(const char *nazwa_we) {
        char *wiersz = new char[il_linia];
        if (!wiersz) blad("Za malo pamieci");

        ifstream plik(nazwa_we);
        if (!plik) {
            blad("Blad otwarcia pliku: ");
            cout << nazwa_we;
            exit(1);
        }

        plik.getline(wiersz, il_linia);
        ilosc = linia(wiersz);
        d = new dane[ilosc];

        plik.getline(wiersz, il_linia);
        dane ddd = linia2(wiersz);
        rozmiar_a = ddd.a;
        rozmiar_b = ddd.b;

        int i = 0;
        while (!plik.eof() && i < ilosc) {
            plik.getline(wiersz, il_linia);
            if (wiersz[0] != '\0') {
                d[i] = linia2(wiersz);
                i++;
            }
        }
        plik.close();
        delete[] wiersz;
    }
};

class rozmieszczenie_klasa : public wczytaj {
public:
    int *ilosc_arkuszy;
    void rozloz(rozmieszczenie *r, sposob s, int pop) {
        randomize();
        if (s == losowo) {
            for (int i = 0; i < ilosc; i++) {
                r[i].poz_x = random(rozmiar_a - d[i].a + 1);
                r[i].poz_y = random(rozmiar_b - d[i].b + 1);
                r[i].arkusz = random(ilosc);
                r[i].obrot = random(2);
            }
        }
        // ... (reszta logiki sposobów kolejno/na_1)
    }
};

// --- KODOWANIE GENETYCZNE ---

class kodowanie {
    int size;
    int *tablica;
public:
    kodowanie(int s) : size(s) {
        tablica = new int[size];
    }
    ~kodowanie() { delete[] tablica; }

    void b10to2(int liczba) {
        for (int i = size - 1; i >= 0; i--) {
            tablica[i] = liczba % 2;
            liczba /= 2;
        }
    }

    void koduj(const rozmieszczenie *r, int *chrom, int ilosc_el) {
        int word_idx = 0;
        for (int k = 0; k < ilosc_el; k++) {
            b10to2(r[k].arkusz);
            for (int j = 0; j < size; j++) chrom[word_idx++] = tablica[j];
            
            b10to2(r[k].poz_x);
            for (int j = 0; j < size; j++) chrom[word_idx++] = tablica[j];

            b10to2(r[k].poz_y);
            for (int j = 0; j < size; j++) chrom[word_idx++] = tablica[j];

            chrom[word_idx++] = r[k].obrot;
        }
    }
};

// --- GŁÓWNA KLASA APLIKACJI ---

class glowny {
public:
    char nazwa[80];
    int popsize_val;

    glowny() {
        strcpy(nazwa, nazwa_stala);
        popsize_val = popsize;
    }

    void Run() {
        clrscr();
        cout << "ALGORYTM GENETYCZNY\n\n";
        cout << "Podaj nazwe pliku wejsciowego [" << nazwa_stala << "]: ";
        
        char ch = cin.peek();
        if (ch != '\n') cin >> nazwa;
        else cin.ignore();

        ilosc_danych id;
        int n = id.przekarz_ilosc(nazwa);
        
        wczytaj dane_wej;
        dane_wej.przekarz_dane(nazwa);

        cout << "Prawdopodobienstwo mutacji [" << pmutation << "]: ";
        if (cin.peek() != '\n') cin >> pmutation; else cin.ignore();

        cout << "Wielkosc populacji [" << popsize_val << "]: ";
        if (cin.peek() != '\n') cin >> popsize_val; else cin.ignore();

        // Alokacja populacji
        rozmieszczenie *populacja = new rozmieszczenie[n * popsize_val];
        if (!populacja) blad("Za malo pamieci dla populacji");

        cout << "\nObliczenia... ENTER aby kontynuowac";
        getch();

        // Zwolnienie pamięci
        delete[] populacja;
    }
};

void main(int argc, char *argv[]) {
    if (argc > 1) bit = atoi(argv[1]);
    else bit = 5;

    word = bit * 3 + 1; // arkusz + x + y + obrot
    
    glowny APLICATION;
    APLICATION.Run();
}


#include <iostream.h>
#include <stdlib.h>
#include <string.h>

// Definicje pomocnicze
enum BOOL { FALSZE = 0, PRAWDA = 1 };
typedef int* BIN;

typedef struct {
    int a;
    int b;
} ARKUSZ;

typedef struct {
    char arkusz;
    int poz_x;
    int poz_y;
    char obrot;
} ELEMENT;

typedef ELEMENT* ELEMENTY;

void Blad(char* str) { 
    cout << str << endl; 
    exit(0); 
}

// --- KLASA OSOBNIK ---

class Osobnik {
private:
    BIN kod;            // Chromosom
    int dl_kodu;        // Długość kodu binarnego
    int il_arkuszy;     // Liczba wykorzystanych arkuszy
    int il_elementow;   // Liczba elementów do rozmieszczenia
    float doskonalosc;  // Funkcja celu (fitness)

    void Wypelnij_pola(int il_ark, int a, int b);

public:
    Osobnik(BIN kodd, int dl_k, int il_ark, int il_el);
    ~Osobnik();
    BIN Kod() { return kod; }
    float Doskonalosc() { return doskonalosc; }
    int I1_arkuszy() { return il_arkuszy; }
};

Osobnik::Osobnik(BIN kodd, int dl_k, int il_ark, int il_el) {
    dl_kodu = dl_k;
    il_elementow = il_el;
    
    // Poprawiona alokacja chromosomu
    kod = new int[dl_kodu];
    if (!kod) Blad("Nie styka pamieci dla chromosomu");
    
    // Kopiowanie kodu
    for(int i = 0; i < dl_kodu; i++) {
        kod[i] = kodd[i];
    }
    
    // Wstępna ocena osobnika (funkcja uproszczona w OCR)
    il_arkuszy = il_ark;
    doskonalosc = 0.0; 
}

Osobnik::~Osobnik() {
    if (kod) delete[] kod;
}

// --- KLASA POPULACJA ---

class Populacja {
private:
    int wlk_populacji;
    Osobnik** populacja; // Tablica wskaźników do osobników

public:
    Populacja(int wlk);
    ~Populacja();
    
    char* Selekcja(bool b);
    int Selekcja(Osobnik& osbn);
    void Przygarniecie(Osobnik* osbn, int pos);
    
    Osobnik* CrossingOver(Osobnik& ona, Osobnik& on);
    Osobnik* Mutacja(Osobnik& osbn);
};

Populacja::Populacja(int wlk) {
    wlk_populacji = wlk;
    populacja = new Osobnik*[wlk_populacji];
    if (!populacja) Blad("Za malo pamieci by utworzyc Populacje");
}

Populacja::~Populacja() {
    for (int i = 0; i < wlk_populacji; i++) {
        delete populacja[i];
    }
    delete[] populacja;
}

// Przykład implementacji selekcji (oczyszczony z błędów OCR)
char* Populacja::Selekcja(bool b) {
    int numer = random(wlk_populacji);
    static char tab[20];
    itoa(numer, tab, 10);
    
    if (b) {
        int numer2 = random(wlk_populacji);
        strcat(tab, ",");
        char tab2[10];
        itoa(numer2, tab2, 10);
        strcat(tab, tab2);
    }
    return tab; 
}

void Populacja::Przygarniecie(Osobnik* osbn, int pos) {
    if (pos >= 0 && pos < wlk_populacji) {
        if (populacja[pos]) delete populacja[pos];
        populacja[pos] = osbn;
    }
}

// --- KLASA APLIKACJA ---

class Aplikacja {
public:
    long GetPole_ark();
    long GetPole_elem();
    void Generacja_populacji(int rozmiar);
};

void Aplikacja::Generacja_populacji(int rozmiar) {
    Populacja pop(rozmiar);
    for (int i = 0; i < rozmiar; i++) {
        // Logika generowania losowego kodu binarnego
        int dl_k = 100; // przykładowa długość
        BIN kod_tmp = new int[dl_k];
        for (int j = 0; j < dl_k; j++) kod_tmp[j] = random(2);
        
        Osobnik* nowy = new Osobnik(kod_tmp, dl_k, 0, 10);
        pop.Przygarniecie(nowy, i);
        
        delete[] kod_tmp;
    }
}
