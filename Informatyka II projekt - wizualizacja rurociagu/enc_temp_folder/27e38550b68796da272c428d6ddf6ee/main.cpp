//Informatyka II projekt - Wizualizacja ruroci¹gu z systemem rêcznej i automatycznej symulacji pewnego procesu technologicznego

//Opis programu:
//Symulacja przewiduje zarówno sterowanie rêczne jak i automatyczne uk³adu 5 zbiorników i systemu rur.

//Przewidziane s¹ takie funkcjonalnoœci jak : rêczny przesy³ dowolnej iloœci cieczy pomiêdzy zbiornikami, mo¿liwoœæ mieszania cieczy w przewidzianym do tego zbiorniku, mo¿liwoœæ podgrzewania i ch³odzenia cieczy do okreœlonej temperatury w przewidzianym do tego zbiorniku, mo¿liwoœæ sterowania 2 zaworami wpuszczaj¹cymi i spuszczaj¹cymi ciecz z systemu.

//U¿ytkownik ma mo¿liwoœæ wybrania tak¿e jednego z 4 przygotowanych scenariuszy automatycznych : Och³adzanie lub podgrzewanie cieczy w zbiorniku nr. 4 (o ile znajduje siê w nim ciecz) do zadanej temperatury, wymieszanie cieczy w zbiorniku 3 (o ile znajduje siê w nim ciecz) oraz przeprowadzenie pe³nego, automatycznego procesu technologicznego.

//U¿ytkownik ma mo¿liwoœæ przyciskiem F1 przejœæ do ekranu opisuj¹cego wszystkie przyciski, F2, by sprawdziæ szczegó³owe dane dotycz¹ce zbiorników i cieczy, F3 by wybraæ scenariusz sterowania automatycznego.W przypadku, w którym u¿ytkownik bêdzie wydawaæ polecenie niemo¿liwe do zrealizowania przez program, na ekranie pojawi siê specjalny komunikat - alert, z odpowiedni¹ wiadomoœci¹.Szczegó³y dotycz¹ce przeprowadzanych czynnoœci bêd¹ tak¿e wysy³ane do konsoli.

//U¿ytkownik ma w dowolnym momencie mo¿liwoœæ zapisania stanu symulacji do pliku "backup.txt", wczytania stanu animacji z tego pliku, oraz mo¿liwoœæ zamkniêcia programu klawiszem escape(wymagane bêdzie dodatkowe potwierdzenie).

#include <SFML/Graphics.hpp> //sfml
#include <vector> //wektory
#include <iostream> 
#include <string> 
#include <algorithm> //fcje min, max
#include <fstream> //do tworzenia backupu
#include <sstream> //Konwersje miêdzy tekstem a liczbami i odwrotnie
#include <windows.h>

// Klasa efektu wizualnego
class babelki {
    //B¹belki s¹ dodawane do wektora w funkcji dodaj_babelki, przesuwane w aktual_babelki, a gdy wyjd¹ poza zbiornik (babelki_poza_zb), s¹ usuwane.
public:
    sf::CircleShape wyglad_babelkow;
    sf::Vector2f predkosc_babelkow; //wektor przechowuje predkosc w x i y

    babelki(float x, float y, bool flaga_babelki) { //flaga_babelki 
        wyglad_babelkow.setRadius(flaga_babelki ? 5.0f : 3.0f); //Jeœli flaga_babelki jest true, wybiera wartoœæ po ? (czyli 5.0f)., jest false, wybiera wartoœæ po : (czyli 3.0f).
        wyglad_babelkow.setFillColor(flaga_babelki ? sf::Color::Red : sf::Color(173, 216, 230));
        wyglad_babelkow.setPosition(x, y);
        predkosc_babelkow = flaga_babelki ? sf::Vector2f(0, -2.0f) : sf::Vector2f(0, 1.0f); //b¹belki lec¹ w góre czy lub dó³
    }

    void update() {
        wyglad_babelkow.move(predkosc_babelkow);
    }

    bool babelki_poza_zb(float babelki_poza_zbiornikiem) {
        return wyglad_babelkow.getPosition().y < babelki_poza_zbiornikiem || wyglad_babelkow.getPosition().y > babelki_poza_zbiornikiem + 200; //bêdzie return true gdy b¹belki bêd¹ poni¿ej zbiornika || lub wyjd¹ ponad zbiornik
    }
};

// Klasa zbiornika
class Zbiornik {
public:
    sf::RectangleShape kszatlt;
    float poziom_napelnienia;
    std::vector<babelki> babelki;

    Zbiornik(float x, float y) : poziom_napelnienia(0) {  // Inicjalizacja poziomu nape³nienia na 0
        kszatlt.setPosition(x, y);
        kszatlt.setSize(sf::Vector2f(100, 200));
        kszatlt.setFillColor(sf::Color::White);
        kszatlt.setOutlineThickness(2);
        kszatlt.setOutlineColor(sf::Color::Black);
    }

    static std::vector<Zbiornik> utworz_zbiorniki() {
        std::vector<Zbiornik> wzbiorniki =
        {
            Zbiornik(50, 200),
            Zbiornik(250, 200),
            Zbiornik(450, 200),
            Zbiornik(650, 200),
        };

        Zbiornik zbiornik_5(450, 500);
        zbiornik_5.kszatlt.setSize(sf::Vector2f(300, 100));
        wzbiorniki.push_back(zbiornik_5);

        return wzbiorniki;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(kszatlt);
        sf::RectangleShape fill(sf::Vector2f(kszatlt.getSize().x, kszatlt.getSize().y * (poziom_napelnienia / 100)));
        fill.setPosition(kszatlt.getPosition().x, kszatlt.getPosition().y + kszatlt.getSize().y * (1 - poziom_napelnienia / 100));
        fill.setFillColor(sf::Color::Cyan);
        window.draw(fill);

        for (auto& i : babelki) {
            window.draw(i.wyglad_babelkow);
        }
    }

    void aktual_babelki() { //Funkcja przesuwa wszystkie b¹belki w zbiorniku i usuwa te, które wysz³y poza jego obszar. Jeœli b¹belek jest poza zbiornikiem, jest usuwany z listy.

        for (auto i = babelki.begin(); i != babelki.end();) {
            i->update();
            if (i->babelki_poza_zb(kszatlt.getPosition().y)) {
                i = babelki.erase(i);
            }
            else {
                ++i;
            }
        }
    }

    void dodaj_babelki(bool flaga_babelki) {
        float x = kszatlt.getPosition().x + (rand() % 80) + 10; //liczba losowa od 0 do 79+10
        float y = flaga_babelki ? kszatlt.getPosition().y + kszatlt.getSize().y : kszatlt.getPosition().y;
        babelki.emplace_back(x, y, flaga_babelki); //emplace back = lepsze push back (nie trzeba tworzyæ tymczasowego obiektu)
    }
};

// Klasa rury
class rura {
public:
    sf::RectangleShape kszatlt;
    std::vector<sf::RectangleShape> wskazniki_przeplywu;
    float predkosc_wiz_w_rurach;
    bool aktywny_przeplyw_w_rurze;
    bool rura_jest_pionowa;

    rura(float x, float y, float szerokosc, float wyokosc)
        : predkosc_wiz_w_rurach(2.0f), aktywny_przeplyw_w_rurze(false), rura_jest_pionowa(wyokosc > szerokosc) {
        kszatlt.setPosition(x, y);
        kszatlt.setSize(sf::Vector2f(szerokosc, wyokosc));
        kszatlt.setFillColor(sf::Color::Black);

        float rozmiar_wskaznikow_przeplywu = 10.0f;
        float odleglosc_pomiedzy_wskaznikami_przeplywu = 20.0f;

        if (rura_jest_pionowa) {
            for (float i = 0; i < wyokosc; i += odleglosc_pomiedzy_wskaznikami_przeplywu) {// Tworzy wskaŸniki przep³ywu w pionowej rurze, je co 20
                sf::RectangleShape wskaznik_przeplywu(sf::Vector2f(szerokosc, rozmiar_wskaznikow_przeplywu));
                wskaznik_przeplywu.setFillColor(sf::Color::Cyan);
                wskaznik_przeplywu.setPosition(x, y + i);
                wskazniki_przeplywu.push_back(wskaznik_przeplywu);
            }
        }
        else {
            for (float i = 0; i < szerokosc; i += odleglosc_pomiedzy_wskaznikami_przeplywu) {// Tworzy wskaŸniki przep³ywu w poziomej rurze, je co 20
                sf::RectangleShape wskaznik_przeplywu(sf::Vector2f(rozmiar_wskaznikow_przeplywu, wyokosc));
                wskaznik_przeplywu.setFillColor(sf::Color::Cyan);
                wskaznik_przeplywu.setPosition(x + i, y);
                wskazniki_przeplywu.push_back(wskaznik_przeplywu);
            }
        }
    }

    static std::vector<rura> utworz_rury() {
        return {
            rura(150, 382, 100, 20), //rura 0
            rura(290, 400, 20, 50),  //rura 1
            rura(290, 450, 100, 20), //rura 2
            rura(390, 170, 20, 300), //rura 3
            rura(390, 150, 300, 20), //rura 4
            rura(490, 170, 20, 30),  //rura 5
            rura(690, 150, 20, 50),  //rura 6
            rura(90, 105, 20, 100),  //rura 7
            rura(690, 400, 20, 100), //rura 8 
            rura(490, 400, 20, 100), //rura 9
            rura(590, 600, 20, 90),  //rura 10
        };
    }

    void update() {
        if (aktywny_przeplyw_w_rurze) {
            for (auto& wskaznik : wskazniki_przeplywu) {
                if (rura_jest_pionowa) {
                    wskaznik.move(0, predkosc_wiz_w_rurach);

                    // Jeœli wskaŸnik wychodzi poza doln¹ granicê, wraca na górê
                    if (predkosc_wiz_w_rurach > 0 && wskaznik.getPosition().y >= kszatlt.getPosition().y + kszatlt.getSize().y) {
                        wskaznik.setPosition(wskaznik.getPosition().x, kszatlt.getPosition().y);
                    }

                    // Jeœli wskaŸnik wychodzi poza górn¹ granicê, wraca na dó³
                    if (predkosc_wiz_w_rurach < 0 && wskaznik.getPosition().y + wskaznik.getSize().y <= kszatlt.getPosition().y) {
                        wskaznik.setPosition(wskaznik.getPosition().x, kszatlt.getPosition().y + kszatlt.getSize().y - wskaznik.getSize().y);
                    }
                }
                else {
                    wskaznik.move(predkosc_wiz_w_rurach, 0);

                    // Jeœli wskaŸnik wychodzi poza praw¹ granicê, wraca na lew¹
                    if (predkosc_wiz_w_rurach > 0 && wskaznik.getPosition().x >= kszatlt.getPosition().x + kszatlt.getSize().x) {
                        wskaznik.setPosition(kszatlt.getPosition().x, wskaznik.getPosition().y);
                    }

                    // Jeœli wskaŸnik wychodzi poza lew¹ granicê, wraca na praw¹
                    if (predkosc_wiz_w_rurach < 0 && wskaznik.getPosition().x + wskaznik.getSize().x <= kszatlt.getPosition().x) {
                        wskaznik.setPosition(kszatlt.getPosition().x + kszatlt.getSize().x - wskaznik.getSize().x, wskaznik.getPosition().y);
                    }
                }
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(kszatlt); // Rysowanie cia³a rury

        // Rysowanie wskaŸników przep³ywu tylko, gdy aktywny_przeplyw_w_rurze == true
        if (aktywny_przeplyw_w_rurze) {
            for (auto& wskaznik : wskazniki_przeplywu) {
                window.draw(wskaznik);
            }
        }
    }
};

// Klasa zaworu
class zawor {
public:
    sf::CircleShape zawor_wyglad;
    bool jest_otwarty;

    zawor(float x, float y) :jest_otwarty(false) {
        zawor_wyglad.setRadius(20);
        zawor_wyglad.setOrigin(20, 20);
        zawor_wyglad.setPosition(x, y);
        zawor_wyglad.setFillColor(sf::Color::Red);
    }

    void przelacz_zawor() {
        jest_otwarty = !jest_otwarty;
        zawor_wyglad.setFillColor(jest_otwarty ? sf::Color::Green : sf::Color::Red);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(zawor_wyglad);
    }
};

// Klasa p³omienia
class plomien {
public:
    sf::ConvexShape ksztalt_plomienia;

    plomien(float x, float y) {
        ksztalt_plomienia.setPointCount(12); // Nieregularny kszta³t z 12 punktów
        ksztalt_plomienia.setPoint(0, sf::Vector2f(30, 60));
        ksztalt_plomienia.setPoint(1, sf::Vector2f(20, 55));
        ksztalt_plomienia.setPoint(2, sf::Vector2f(10, 40));
        ksztalt_plomienia.setPoint(3, sf::Vector2f(10, 30));
        ksztalt_plomienia.setPoint(4, sf::Vector2f(20, 40));
        ksztalt_plomienia.setPoint(5, sf::Vector2f(25, 20));
        ksztalt_plomienia.setPoint(6, sf::Vector2f(30, 10));
        ksztalt_plomienia.setPoint(7, sf::Vector2f(35, 20));
        ksztalt_plomienia.setPoint(8, sf::Vector2f(40, 40));
        ksztalt_plomienia.setPoint(9, sf::Vector2f(50, 30));
        ksztalt_plomienia.setPoint(10, sf::Vector2f(50, 40));
        ksztalt_plomienia.setPoint(11, sf::Vector2f(40, 55));

        ksztalt_plomienia.setFillColor(sf::Color(255, 165, 0)); // Pomarañczowy œrodek
        ksztalt_plomienia.setOutlineThickness(2);
        ksztalt_plomienia.setOutlineColor(sf::Color(255, 69, 0)); // Czerwony kontur
        ksztalt_plomienia.setScale(0.5f, 0.5f); // Skalowanie 50%
        ksztalt_plomienia.setPosition(x, y);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(ksztalt_plomienia);
    }

    static std::vector<plomien> utworzplomienie() {
        std::vector<plomien> wplomien;
        wplomien.emplace_back(740, 400);
        wplomien.emplace_back(710, 400);
        wplomien.emplace_back(630, 400);
        wplomien.emplace_back(660, 400);
        return wplomien;
    }
};

//Klasa panelu kontrolnego
class Panel_kontrolny {
public:
    sf::RectangleShape panel_kontrolny_ksztalt;
    sf::Text tytul;
    std::vector<sf::Text> info_o_zb;
    sf::Font czcionka;

    Panel_kontrolny(float szerokosc, float wyokosc, const std::string& fontPath) {
        // Czcionka
        if (!czcionka.loadFromFile(fontPath)) {
            throw std::runtime_error("B³¹d ³adowania czcionki");
        }

        // Wyglad panelu kontrolnego
        panel_kontrolny_ksztalt.setSize(sf::Vector2f(szerokosc, wyokosc));
        panel_kontrolny_ksztalt.setFillColor(sf::Color(150, 150, 150));
        panel_kontrolny_ksztalt.setOutlineThickness(2);
        panel_kontrolny_ksztalt.setOutlineColor(sf::Color::Black);

        // Inicjalizacja tytulu
        tytul.setFont(czcionka);
        tytul.setString(L"Panel Kontrolny Ruroci¹gu");
        tytul.setCharacterSize(20);
        tytul.setFillColor(sf::Color::Black);
        tytul.setPosition(szerokosc / 2 - 150, 10); //po œrodku 

        // Inicjalizuj informacje o zbiornikach
        for (int i = 0; i < 5; ++i) {
            sf::Text info;
            info.setFont(czcionka);
            info.setCharacterSize(16);
            info.setFillColor(sf::Color::Black);
            info.setPosition(20 + i * 160, 40); // Równe roz³o¿enie informacji - 20 od brzegu lewego, potem ka¿da info co 160
            info_o_zb.push_back(info);
        }
    }

    void aktual_informacji(const std::vector<Zbiornik>& wzbiorniki, float temperatura_zb_4, bool mikser_on, float poziom_wymieszania) {
        info_o_zb[0].setString("Zbiornik 1: " + std::to_string((int)wzbiorniki[0].poziom_napelnienia) + "%"); //to_string Zwraca ci¹g znaków reprez. wart. liczb.
        info_o_zb[1].setString("Zbiornik 2: " + std::to_string((int)wzbiorniki[1].poziom_napelnienia) + "%");
        info_o_zb[2].setString("Zbiornik 3: " + std::to_string((int)wzbiorniki[2].poziom_napelnienia) + "%\nMikser: " + (mikser_on ? "ON" : "OFF") + "\nPoz. wymiesz.: " + std::to_string((int)poziom_wymieszania)); // ? -> je¿eli jest true to 1sza opcja, je¿eli false to to po :
        info_o_zb[3].setString("Zbiornik 4: " + std::to_string((int)wzbiorniki[3].poziom_napelnienia) + "%\nTemperatura: " + std::to_string((int)temperatura_zb_4) + "C");
        info_o_zb[4].setString("Zbiornik 5: " + std::to_string((int)wzbiorniki[4].poziom_napelnienia) + "%");
    }

    void draw(sf::RenderWindow& window) {
        window.draw(panel_kontrolny_ksztalt);
        window.draw(tytul);
        for (const auto& info : info_o_zb) {
            window.draw(info);
        }
    }
};

// Funkcja scenariuszy, ekran F3
enum rodzaje_scenariuszy {
    Scenariusz_0 = 0, //scenariusz 0
    Scenariusz_grzej_do_50, //scenariusz 1
    Scenariusz_ochladzaj_do_10, //scenariusz 2
    Scenariusz_pelny_cykl_auto, //scenariusz 3
    Scenariusz_mieszanie = 5 //scenariusz 4
};
class Menedzer_scenariuszy {
private:
    sf::RectangleShape tlo_ekran_scenariuszy;
    sf::Text info_aktyw_scenariusz;
    sf::Text tekst_scenariusze;

public:
    Menedzer_scenariuszy(const sf::Font& czcionka) {
        // T³o dla aktywnego scenariusza
        tlo_ekran_scenariuszy.setSize(sf::Vector2f(400, 50));
        tlo_ekran_scenariuszy.setFillColor(sf::Color(200, 200, 200));
        tlo_ekran_scenariuszy.setPosition(20, 620);

        // Tekst dla aktywnego scenariusza
        info_aktyw_scenariusz.setFont(czcionka);
        info_aktyw_scenariusz.setCharacterSize(20);
        info_aktyw_scenariusz.setFillColor(sf::Color::Black);
        info_aktyw_scenariusz.setPosition(30, 625);

        // Tekst listy scenariuszy
        tekst_scenariusze.setFont(czcionka);
        tekst_scenariusze.setString(
            "Scenariusze:\n"
            L"Wybierz scenariusz odpowiednim przyciskiem. \n\n"
            L"1 - Podgrzewaj ciecz w zbiorniku 4 a¿ do osi¹gniêcia temperatury 50°C. \n"
            L"2 - Och³adzaj ciecz w zbiorniku 4 a¿ do osi¹gniêcia temperatury 10°C.\n"
            L"3 - Mieszaj ciecz w zbiorniku 3 a¿ do pe³nego wymieszania.\n"
            L"4 - Tryb automatyczny - przeprowadŸ pe³ny proces technologiczny automatycznie.\n"
            L"\nS - Zatrzymaj dzia³anie scenariusza w dowolnym momencie.\n"
            L"\nF3 - Powrót"
        );
        tekst_scenariusze.setCharacterSize(20);
        tekst_scenariusze.setFillColor(sf::Color::Black);
        tekst_scenariusze.setPosition(50, 50);
    }

    void rysuj_info_aktyw_scenariusz(sf::RenderWindow& window) const {
        window.draw(tlo_ekran_scenariuszy);
        window.draw(info_aktyw_scenariusz);
    }

    void rysuj_ekran_scenariuszy(sf::RenderWindow& window) const {
        // T³o dla okna scenariuszy
        sf::RectangleShape tlo_ekran_scenariuszy(sf::Vector2f(window.getSize().x, window.getSize().y));
        tlo_ekran_scenariuszy.setFillColor(sf::Color(200, 200, 200, 230));

        // Rysowanie t³a
        window.draw(tlo_ekran_scenariuszy);

        // Rysowanie tekstu scenariuszy
        window.draw(tekst_scenariusze);
    }

    void synchro_nazwe_aktw_scenariusza(rodzaje_scenariuszy type) {
        info_aktyw_scenariusz.setString(tekst_info_aktyw_scenariusz(type));
    }

    static std::wstring tekst_info_aktyw_scenariusz(rodzaje_scenariuszy type) {
        switch (type) {
        case Scenariusz_grzej_do_50:
            return L"Scenariusz: Podgrzewanie do 50°C";
        case Scenariusz_ochladzaj_do_10:
            return L"Scenariusz: Ch³odzenie do 10°C";
        case Scenariusz_pelny_cykl_auto:
            return L"Scenariusz: Pe³ny proces automatyczny";
        case Scenariusz_mieszanie:
            return L"Scenariusz: Mieszanie w zbiorniku 3";
        default:
            return L"Brak aktywnego scenariusza";
        }
    }
};

// Ekran statusu F2
void ekran_statusu(sf::RenderWindow& window, const std::vector<Zbiornik>& wzbiorniki, float temperatura_zb_4, bool mikser_on, float poziom_wymieszania) {
    sf::Font czcionka;
    czcionka.loadFromFile("arial.ttf");
    std::wstring info = L"Status:\n"; // Szeroki ci¹g znaków (Unicode)
    for (size_t i = 0; i < wzbiorniki.size(); i++) {
        info += L"Zbiornik " + std::to_wstring((int)i + 1) + L": " + std::to_wstring((int)wzbiorniki[i].poziom_napelnienia) + L"%";
        if (i == 2) info += L" | Mikser:" + std::wstring(mikser_on ? L"ON" : L"OFF") + L" | Poziom wymieszania cieczy: " + std::to_wstring((int)poziom_wymieszania);
        if (i == 3) info += L" | Temperatura w zb. 4: " + std::to_wstring((int)temperatura_zb_4) + L"°C";
        info += L"\n";
    }
    info += L"\nF2 - Powrót";

    sf::Text t;
    t.setFont(czcionka);
    t.setString(info); // Ustawienie szerokiego ci¹gu znaków
    t.setCharacterSize(20);
    t.setFillColor(sf::Color::Black);
    t.setPosition(50, 50);

    sf::RectangleShape tlo_ekran_statusu(sf::Vector2f(window.getSize().x, window.getSize().y));
    tlo_ekran_statusu.setFillColor(sf::Color(200, 200, 200, 230));

    while (true) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::F2) {
                return;
            }
        }

        window.clear();
        window.draw(tlo_ekran_statusu);
        window.draw(t);
        window.display();
    }
}

// Ekran pomocy F1
void pokaz_ekran_pomocy(sf::RenderWindow& window) {
    sf::Font czcionka; czcionka.loadFromFile("arial.ttf");
    sf::Text ekran_pomocy;
    ekran_pomocy.setFont(czcionka);
    ekran_pomocy.setString(
        "Instrukcja:\n"
        L"Z - Otwórz/zamknij zawór nape³niaj¹cy przy zbiorniku nr. 1\n"
        "X - Otwórz/zamknij zawór spustowy przy zbiorniku nr. 5\n"
        "H - Za³¹cz/roz³¹cz ogrzewanie w zbiorniku nr. 4\n"
        L"C - Za³¹cz/roz³¹cz och³adzanie w zbiorniku nr. 4\n"
        L"M - Za³¹cz/roz³¹cz mieszalnik w zbiorniku nr. 3\n"
        L"Strza³ki lewo/prawo - przesy³aj ciecz do zbiornika po lewej/prawej\n"
        L"Strza³ki góra/dó³ - zmieñ kontrolowany zbiornik\n"
        "F1 - Ekran pomocy\n"
        "F2 - Ekran statusu wszystkich zbiorników\n"
        "F3 - Ekran ze scenariuszami\n"
        "F4 - Zapisz stan symulacji do pliku\n"
        "F5 - Wczytaj stan symulacji z pliku\n"
        L"Esc - Wyjœcie z potwierdzeniem\n"
        L"\nF1 - Powrót"
    );

    ekran_pomocy.setCharacterSize(20);
    ekran_pomocy.setFillColor(sf::Color::Black);
    ekran_pomocy.setPosition(50, 50);

    sf::RectangleShape tlo_ekran_pomocy(sf::Vector2f(window.getSize().x, window.getSize().y));
    tlo_ekran_pomocy.setFillColor(sf::Color(200, 200, 200, 230));

    while (true) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::F1) {
                return;
            }
        }
        window.clear();
        window.draw(tlo_ekran_pomocy);
        window.draw(ekran_pomocy);
        window.display();
    }
}

//Alerty
void alert(sf::RenderWindow& window, const std::wstring& tytul, const std::wstring& powod_alertu) {
    sf::Font czcionka;
    if (!czcionka.loadFromFile("arial.ttf")) {
        std::wcerr << L"Nie mo¿na za³adowaæ czcionki!\n";
        return;
    }

    // T³o alertu
    sf::RectangleShape tlo_alertu(sf::Vector2f(400, 200));
    tlo_alertu.setFillColor(sf::Color(255, 255, 200));
    tlo_alertu.setOutlineThickness(2);
    tlo_alertu.setOutlineColor(sf::Color::Black);
    tlo_alertu.setPosition(200, 250);

    // Tekst g³ówny (tytu³)
    sf::Text tytulText;
    tytulText.setFont(czcionka);
    tytulText.setCharacterSize(20);
    tytulText.setFillColor(sf::Color::Black);
    tytulText.setString(tytul);

    // Tekst powodu alertu
    sf::Text tekst_powod_alertu;
    tekst_powod_alertu.setFont(czcionka);
    tekst_powod_alertu.setCharacterSize(18);
    tekst_powod_alertu.setFillColor(sf::Color::Black);
    tekst_powod_alertu.setString(powod_alertu);

    // Tekst - naciœnij enter, aby zamkn¹æ
    sf::Text tekst_enter_zeby_zamknac;
    tekst_enter_zeby_zamknac.setFont(czcionka);
    tekst_enter_zeby_zamknac.setCharacterSize(14);
    tekst_enter_zeby_zamknac.setFillColor(sf::Color::Black);
    tekst_enter_zeby_zamknac.setString(L"Naciœnij Enter, aby zamkn¹æ.");

    // Wyœrodkowanie tekstów w tle alertu
    tytulText.setPosition(
        tlo_alertu.getPosition().x + (tlo_alertu.getSize().x - tytulText.getLocalBounds().width) / 2,
        tlo_alertu.getPosition().y + 30
    );

    tekst_powod_alertu.setPosition(
        tlo_alertu.getPosition().x + (tlo_alertu.getSize().x - tekst_powod_alertu.getLocalBounds().width) / 2,
        tytulText.getPosition().y + 40
    );

    tekst_enter_zeby_zamknac.setPosition(
        tlo_alertu.getPosition().x + (tlo_alertu.getSize().x - tekst_enter_zeby_zamknac.getLocalBounds().width) / 2,
        tekst_powod_alertu.getPosition().y + 50
    );

    while (true) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                return; // Zamknij alert po wciœniêciu Enter
            }
        }

        window.clear(sf::Color::White);
        window.draw(tlo_alertu);
        window.draw(tytulText);
        window.draw(tekst_powod_alertu);
        window.draw(tekst_enter_zeby_zamknac);
        window.display();
    }
}

// Zapis stanu
void Zapis_stanu(const std::vector<Zbiornik>& wzbiorniki, float temperatura_zb_4, bool mikser_on, float poziom_wymieszania, bool zawor_spustowy_otwarty, bool zawor_napelniajacy_otwarty, rodzaje_scenariuszy aktywny_scenariusz) {

    std::ofstream file("backup.txt"); //Otwiera plik backup.txt do zapisu

    if (!file.is_open()) {
        std::cerr << "Nie mo¿na zapisaæ pliku!\n";
        return;
    }

    //<< zapisanie do pliku 
    file << "poziom_napelnielnienia_zbiornikow=";
    for (size_t i = 0; i < wzbiorniki.size(); i++) {
        file << wzbiorniki[i].poziom_napelnienia;
        if (i < wzbiorniki.size() - 1) file << ",";
    }
    file << "\temperatura_zb_4=" << temperatura_zb_4 << "\n";
    file << "mikser_on=" << (mikser_on ? 1 : 0) << "\n";
    file << "poziom_wymieszania=" << poziom_wymieszania << "\n";
    file << "zawor_spustowy_otwarty=" << (zawor_spustowy_otwarty ? 1 : 0) << "\n";
    file << "zawor_napelniajacy_otwarty=" << (zawor_napelniajacy_otwarty ? 1 : 0) << "\n";
    file << "aktywny_scenariusz=" << (int)aktywny_scenariusz << "\n";
    file.close();

    std::cout << "Stan zapisany do backup.txt\n";
}

//poziom wymieszania
void aktualizujPoziomWymieszania(float& poziom_wymieszania, float poziom_napelnienia, bool mikser_on) {
    if (mikser_on && poziom_napelnienia > 0.0f) {
        float wspolczynnik = poziom_napelnienia / 100.0f; // Skala na podstawie poziomu wody
        poziom_wymieszania = std::min(100.0f, poziom_wymieszania + 0.05f * wspolczynnik);
    }
    else if (poziom_napelnienia <= 0.0f) {
        poziom_wymieszania = 0.0f; // Reset, jeœli brak wody
    }
}

// Funkcja potwierdzenia wyjscia
bool komunikat_wyjscia(sf::RenderWindow& window) {
    sf::Font czcionka; czcionka.loadFromFile("arial.ttf");
    sf::Text tekst_czy_wychodzisz;
    tekst_czy_wychodzisz.setFont(czcionka);
    tekst_czy_wychodzisz.setString(L"Czy na pewno chcesz wyjœæ z programu?\nY - Tak, N - Nie");
    tekst_czy_wychodzisz.setCharacterSize(20);
    tekst_czy_wychodzisz.setFillColor(sf::Color::Black);
    tekst_czy_wychodzisz.setPosition(50, 50);
    sf::RectangleShape tlo_ekran_potw_wyjscia(sf::Vector2f(window.getSize().x, window.getSize().y));
    tlo_ekran_potw_wyjscia.setFillColor(sf::Color(200, 200, 200, 230));

    while (true) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                return true;
            }
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Y) return true;
                if (ev.key.code == sf::Keyboard::N) return false;
            }
        }

        window.clear();
        window.draw(tlo_ekran_potw_wyjscia);
        window.draw(tekst_czy_wychodzisz);
        window.display();
    }
}

// Transfer cieczy
bool przesylanie_cieczy(std::vector<Zbiornik>& wzbiorniki, std::vector<rura>& wrura, int ze_zb, int do_zb, float predkosc_przesylu, float kierunek) {
    // Blokada przesy³u wody miêdzy zbiornikami 3 i 4
    if ((ze_zb == 3 && do_zb == 2) || (ze_zb == 2 && do_zb == 3)) {
        std::wcout << L"Przep³yw miêdzy zbiornikami 3 i 4 jest zablokowany.\n";
        return false; // Przerwij funkcjê - przesy³ zablokowany
    }

    if ((ze_zb == 2 && do_zb == 1) || (ze_zb == 3 && do_zb == 1)) {
        std::wcout << L"Przep³yw miêdzy zbiornikami 3 a 2 i 4 a 2 jest zablokowany.\n";
        return false; // Przerwij funkcjê - przesy³ zablokowany
    }

    // Blokada przesy³u wody ze zbiornika 5 do zbiornika 4
    if (ze_zb == 4 && do_zb == 3) {
        std::wcout << L"Przep³yw ze zbiornika 5 do zbiornika 4 jest zablokowany.\n";
        return false; // Przerwij funkcjê - przesy³ zablokowany
    }

    // Sprawdzenie, czy zbiornik docelowy nie jest pe³ny, a Ÿród³owy nie jest pusty
    if (wzbiorniki[do_zb].poziom_napelnienia >= 100.0f || wzbiorniki[ze_zb].poziom_napelnienia <= 0.0f) {
        return false; // Nie mo¿na przesy³aæ wody
    }

    // Obliczenie iloœci cieczy do przes³ania
    float ilosc_cieczy_do_przesl = std::min(predkosc_przesylu, std::min(wzbiorniki[ze_zb].poziom_napelnienia, 100.0f - wzbiorniki[do_zb].poziom_napelnienia));

    if (ilosc_cieczy_do_przesl > 0) {
        // Aktualizacja poziomów cieczy w zbiornikach
        wzbiorniki[ze_zb].poziom_napelnienia -= ilosc_cieczy_do_przesl;
        wzbiorniki[do_zb].poziom_napelnienia += ilosc_cieczy_do_przesl;

        // Ustawienie przep³ywu dla odpowiednich rur
        if (ze_zb == 0 && do_zb == 1) { // Zbiornik 1 -> Zbiornik 2
            wrura[0].aktywny_przeplyw_w_rurze = true;
            wrura[0].predkosc_wiz_w_rurach = kierunek;
        }
        else if (ze_zb == 1 && do_zb == 2) { // Zbiornik 2 -> Zbiornik 3
            wrura[1].aktywny_przeplyw_w_rurze = true;
            wrura[1].predkosc_wiz_w_rurach = kierunek;
        }
        else if (ze_zb == 2 && do_zb == 4) { // Zbiornik 3 -> Zbiornik 5
            wrura[9].aktywny_przeplyw_w_rurze = true;
            wrura[9].predkosc_wiz_w_rurach = kierunek;
        }

        else if (ze_zb == 3 && do_zb == 4) { // Zbiornik 4 -> Zbiornik 5
            wrura[8].aktywny_przeplyw_w_rurze = true; // Aktywacja przep³ywu w rurze 10
            wrura[8].predkosc_wiz_w_rurach = kierunek; // Prêdkoœæ wskaŸników
        }

        return true; // Przesy³ zakoñczony pomyœlnie
    }

    return false; // Brak przesy³u
}

// Wczytanie stanu
bool wczytaj_stan(std::vector<Zbiornik>& wzbiorniki, float& temperatura_zb_4, bool& mikser_on, float& poziom_wymieszania, bool& zawor_spustowy_otwarty, bool& zawor_napelniajacy_otwarty, rodzaje_scenariuszy& aktywny_scenariusz) {

    std::ifstream file("backup.txt"); // Otwieranie pliku backup.txt do odczytu

    if (!file.is_open()) {
        std::cerr << "Brak pliku backup.txt!\n";
        return false;
    }

    std::string wiersz;

    // Odczyt linii z pliku w pêtli
    while (std::getline(file, wiersz)) {
        if (wiersz.empty()) continue; // Pomijanie pustych linii
        size_t pos = wiersz.find("="); // Szukanie znaku '=' w linii
        if (pos == std::string::npos) continue; // Jeœli '=' nie znaleziono, pomijamy liniê
        std::string key = wiersz.substr(0, pos); //wyci¹ga tekst przed '='
        std::string wartosc = wiersz.substr(pos + 1); //wyci¹ga tekst po '='

        // Sprawdzanie, które has³o jest obecne w linii
        if (key == "poziom_napelnielnienia_zbiornikow") {
            std::stringstream stringstream(wartosc);
            std::string step;
            size_t i = 0;
            while (std::getline(stringstream, step, ',') && i < wzbiorniki.size()) {
                wzbiorniki[i].poziom_napelnienia = std::stof(step); //string do float
                i++;
            }
        }
        else if (key == "temperatura_zb_4") {
            temperatura_zb_4 = std::stof(wartosc); //string do float
        }
        else if (key == "mikser_on") {
            mikser_on = (std::stoi(wartosc) == 1); //string do int
        }
        else if (key == "poziom_wymieszania") {
            poziom_wymieszania = std::stof(wartosc); //string do float
        }
        else if (key == "zawor_spustowy_otwarty") {
            zawor_spustowy_otwarty = (std::stoi(wartosc) == 1); //string do int
        }
        else if (key == "zawor_napelniajacy_otwarty") {
            zawor_napelniajacy_otwarty = (std::stoi(wartosc) == 1); //string do int
        }
        else if (key == "aktywny_scenariusz") {
            int scenariusz = std::stoi(wartosc); //string do int
            aktywny_scenariusz = (rodzaje_scenariuszy)scenariusz;
        }
    }
    file.close();
    std::cout << "Stan wczytany z backup.txt\n";
    return true;
}

//przep³yw zbiornik 2 do 3 i 4
bool rozgaleznik_przeplywu(std::vector<Zbiornik>& wzbiorniki, std::vector<rura>& wrura, int ze_zb, int do_zb_3, int do_zb_4, float predkosc_przesylu) {
    if (wzbiorniki[do_zb_3].poziom_napelnienia >= 100.0f && wzbiorniki[do_zb_4].poziom_napelnienia >= 100.0f) {
        return false; // Oba zbiorniki docelowe s¹ pe³ne
    }

    float ilosc_do_przeslania_2_34 = std::min(predkosc_przesylu, wzbiorniki[ze_zb].poziom_napelnienia);
    if (ilosc_do_przeslania_2_34 > 0) {
        float podziel_ciecz = ilosc_do_przeslania_2_34 / 2.0f;

        if (wzbiorniki[do_zb_3].poziom_napelnienia < 100.0f) {
            wzbiorniki[do_zb_3].poziom_napelnienia += std::min(podziel_ciecz, 100.0f - wzbiorniki[do_zb_3].poziom_napelnienia);
        }

        if (wzbiorniki[do_zb_4].poziom_napelnienia < 100.0f) {
            wzbiorniki[do_zb_4].poziom_napelnienia += std::min(podziel_ciecz, 100.0f - wzbiorniki[do_zb_4].poziom_napelnienia);
        }

        wzbiorniki[ze_zb].poziom_napelnienia -= ilosc_do_przeslania_2_34;

        // Ustawienie przep³ywu dla rur od zbiornika 2 do zbiorników 3 i 4

        wrura[1].aktywny_przeplyw_w_rurze = true; // Rura 2
        wrura[2].aktywny_przeplyw_w_rurze = true; // Rura 3
        wrura[3].aktywny_przeplyw_w_rurze = true; // Rura 4
        wrura[4].aktywny_przeplyw_w_rurze = true; // Rura 5
        wrura[5].aktywny_przeplyw_w_rurze = true; // Rura 6
        wrura[6].aktywny_przeplyw_w_rurze = true; // Rura 7

        wrura[1].predkosc_wiz_w_rurach = 1.0f; // Rura 2
        wrura[2].predkosc_wiz_w_rurach = 1.0f; // Rura 3
        wrura[3].predkosc_wiz_w_rurach = -1.0f; // Rura 4 (korekta kierunku)
        wrura[4].predkosc_wiz_w_rurach = 1.0f; // Rura 5
        wrura[5].predkosc_wiz_w_rurach = 1.0f; // Rura 6
        wrura[6].predkosc_wiz_w_rurach = 1.0f; // Rura 7

        return true;
    }
    return false;
}

//Nazwy scenariuszy
std::wstring tekst_info_aktyw_scenariusz(rodzaje_scenariuszy scenariusz) {
    switch (scenariusz) {
    case Scenariusz_grzej_do_50:
        return L"Scenariusz: Podgrzewanie do 50°C";
    case Scenariusz_ochladzaj_do_10:
        return L"Scenariusz: Ch³odzenie do 10°C";
    case Scenariusz_pelny_cykl_auto:
        return L"Scenariusz: Pe³ny proces automatyczny";
    case Scenariusz_mieszanie:
        return L"Scenariusz: Mieszanie zbiornika 3";
    default:
        return L"Brak aktywnego scenariusza";
    }
}

int main() {

    //Jêzyk
    setlocale(LC_ALL, "pl_PL.UTF-8");
    std::locale::global(std::locale("pl_PL.UTF-8"));
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    //czcionka
    sf::Font czcionka;
    if (!czcionka.loadFromFile("arial.ttf")) {
        std::cerr << L"Nie mo¿na za³adowaæ czcionki!\n";
        return -1;
    }

    //renderowanie
    sf::RenderWindow window(sf::VideoMode(800, 700), "Wizualizacja Rurociagu");
    window.setFramerateLimit(60);
    std::vector<plomien> wplomien;
    std::vector<Zbiornik> wzbiorniki = Zbiornik::utworz_zbiorniki();
    std::vector<rura> wrura = rura::utworz_rury();
    zawor zawor_napelniajacy(100, 150);
    zawor zawor_spustowy(600, 630);

    //Ustawianie wartoœci
    int wybrany_zbiornik = 0;
    bool zawor_spustowy_otwarty = false;
    float szybkosc_napelniania_zb = 1.0;
    float predkosc_oprozniania_zb = 0.5f;
    float predkosc_przesylu = 0.5;
    float temperatura_zb_4 = 20.0f;
    float poprzedni_poziom_napelnienia_zb3 = wzbiorniki[2].poziom_napelnienia;

    // Mieszalnik w zbiorniku 3
    sf::RectangleShape mikser(sf::Vector2f(60, 5));
    mikser.setFillColor(sf::Color::Black);
    mikser.setOrigin(30, 2.5f); // przenosi punkt pocz¹tkowy na œrodek miksera
    mikser.setPosition(wzbiorniki[2].kszatlt.getPosition().x + 50, wzbiorniki[2].kszatlt.getPosition().y + 100);
    bool mikser_on = false;
    bool zb_4_pusty = false; // Flaga œledz¹ca, czy zbiornik 4 zosta³ opró¿niony
    bool ekran_ze_scenariuszami = false;
    float kat_miksera = 0.0f;
    float poziom_wymieszania = 0.0f;

    //Scenariusz
    Menedzer_scenariuszy Menedzer_scenariuszy(czcionka);
    rodzaje_scenariuszy aktywny_scenariusz = Scenariusz_0;
    Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);

    while (window.isOpen()) {
        sf::Event event;

        // Tworzenie obiektu Panel_kontrolny
        Panel_kontrolny Panel_kontrolny(800, 100, "arial.ttf");

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                Zapis_stanu(wzbiorniki, temperatura_zb_4, mikser_on, poziom_wymieszania, zawor_spustowy_otwarty, zawor_napelniajacy.jest_otwarty, aktywny_scenariusz);
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (ekran_ze_scenariuszami) {

                    // Rysowanie listy scenariuszy
                    Menedzer_scenariuszy.rysuj_ekran_scenariuszy(window);

                    // Przyciski do sterowania scenariuszami
                    if (event.key.code == sf::Keyboard::Num1) {
                        if (wzbiorniki[3].poziom_napelnienia > 0.0f) {
                            aktywny_scenariusz = Scenariusz_grzej_do_50;
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz); // Aktualizacja nazwy
                        }
                        else {
                            alert(window, L"Nie mo¿na uruchomiæ scenariusza", L"Zbiornik 4 jest pusty.");
                        }
                        ekran_ze_scenariuszami = false; // Zamknij ekran F3 po wyborze
                    }
                    if (event.key.code == sf::Keyboard::Num2) {
                        if (wzbiorniki[3].poziom_napelnienia > 0.0f) {
                            aktywny_scenariusz = Scenariusz_ochladzaj_do_10;
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz); // Aktualizacja nazwy
                        }
                        else {
                            alert(window, L"Nie mo¿na uruchomiæ scenariusza", L"Zbiornik 4 jest pusty.");
                        }
                        ekran_ze_scenariuszami = false; // Zamknij ekran F3 po wyborze
                    }
                    if (event.key.code == sf::Keyboard::Num3) {
                        if (wzbiorniki[2].poziom_napelnienia > 0.0f) {
                            aktywny_scenariusz = Scenariusz_mieszanie;
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz); // Aktualizacja nazwy
                        }
                        else {
                            alert(window, L"Nie mo¿na uruchomiæ scenariusza", L"Zbiornik 3 jest pusty.");
                        }
                        ekran_ze_scenariuszami = false; // Zamknij ekran F3 po wyborze
                    }
                    if (event.key.code == sf::Keyboard::Num4) {
                        aktywny_scenariusz = Scenariusz_pelny_cykl_auto;
                        Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz); // Synchronizacja nazwy
                        ekran_ze_scenariuszami = false; // Zamknij ekran F3 po wyborze
                    }
                    if (event.key.code == sf::Keyboard::F3) {
                        ekran_ze_scenariuszami = !ekran_ze_scenariuszami; // Prze³¹cz widocznoœæ ekranu scenariuszy
                    }
                }
                //Pozosta³e przyciski do sterowania 
                //fcja isKeyPressed sprawdza, czy klawisz jest wciœniêty. event keycode nie wykrywa trzymania.
                else {
                    if (event.key.code == sf::Keyboard::F1) {
                        pokaz_ekran_pomocy(window);
                    }
                    if (event.key.code == sf::Keyboard::F2) {
                        ekran_statusu(window, wzbiorniki, temperatura_zb_4, mikser_on, poziom_wymieszania);
                    }
                    if (event.key.code == sf::Keyboard::F3) {
                        ekran_ze_scenariuszami = !ekran_ze_scenariuszami; // Prze³¹cz widocznoœæ ekranu scenariuszy
                    }
                    if (event.key.code == sf::Keyboard::F4) {
                        Zapis_stanu(wzbiorniki, temperatura_zb_4, mikser_on, poziom_wymieszania, zawor_spustowy_otwarty, zawor_napelniajacy.jest_otwarty, aktywny_scenariusz);
                    }
                    if (event.key.code == sf::Keyboard::F5) {
                        wczytaj_stan(wzbiorniki, temperatura_zb_4, mikser_on, poziom_wymieszania, zawor_spustowy_otwarty, zawor_napelniajacy.jest_otwarty, aktywny_scenariusz);
                        zawor_spustowy.zawor_wyglad.setFillColor(zawor_spustowy_otwarty ? sf::Color::Green : sf::Color::Red);
                        zawor_napelniajacy.zawor_wyglad.setFillColor(zawor_napelniajacy.jest_otwarty ? sf::Color::Green : sf::Color::Red);
                    }
                    //fcja isKeyPressed sprawdza, czy klawisz jest wciœniêty. event keycode nie wykrywa trzymania.
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                        if (wzbiorniki[3].poziom_napelnienia > 0.0f) {
                            wzbiorniki[3].dodaj_babelki(false);
                            temperatura_zb_4 -= 0.1f;
                        }
                        else {
                            alert(window, L"Nie mo¿na ch³odziæ", L"Zbiornik 4 jest pusty.");
                        }
                    }
                    //fcja isKeyPressed sprawdza, czy klawisz jest wciœniêty. event keycode nie wykrywa trzymania.
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
                        if (wzbiorniki[3].poziom_napelnienia > 0.0f) {
                            wzbiorniki[3].dodaj_babelki(true); // B¹belki lec¹ w górê
                            temperatura_zb_4 += 0.1f;

                            // Tworzenie p³omieni, jeœli jeszcze ich nie ma
                            if (wplomien.empty()) {
                                wplomien = plomien::utworzplomienie(); // U¿ycie metody klasy
                            }
                        }
                        else {
                            alert(window, L"Nie mo¿na grzaæ", L"Zbiornik 4 jest pusty.");
                        }
                    }
                    if (event.key.code == sf::Keyboard::M) {
                        if (!mikser_on) { // W³¹cz mieszanie
                            if (wzbiorniki[2].poziom_napelnienia > 0.0f) {
                                mikser_on = true;
                            }
                            else {
                                alert(window, L"Nie mo¿na mieszaæ", L"Zbiornik 3 jest pusty.");
                            }
                        }
                        else { // Wy³¹cz mieszanie
                            mikser_on = false;
                        }
                    }
                    if (event.key.code == sf::Keyboard::S) {
                        if (aktywny_scenariusz != Scenariusz_0) {
                            aktywny_scenariusz = Scenariusz_0; // Zatrzymaj aktywny scenariusz
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz); // Synchronizacja nazwy
                            wplomien.clear();                // Usuñ p³omienie (jeœli by³y aktywne)
                            mikser_on = false;               // Wy³¹cz mieszanie
                            poziom_wymieszania = 0.0f;               // Zresetuj poziom mieszania
                            std::wcout << L"Aktywny scenariusz zosta³ zatrzymany.\n";
                        }
                    }
                    if (event.key.code == sf::Keyboard::X) {
                        zawor_spustowy_otwarty = !zawor_spustowy_otwarty;
                        zawor_spustowy.zawor_wyglad.setFillColor(zawor_spustowy_otwarty ? sf::Color::Green : sf::Color::Red);
                        wrura[10].aktywny_przeplyw_w_rurze = zawor_spustowy_otwarty;  // Aktywacja przep³ywu w rurze nr. 11 (indeks 10)
                        wrura[10].predkosc_wiz_w_rurach = 1.0f; // Prêdkoœæ wskaŸników przep³ywu
                    }
                    if (event.key.code == sf::Keyboard::Z) {
                        zawor_napelniajacy.przelacz_zawor();

                        if (zawor_napelniajacy.jest_otwarty && wzbiorniki[0].poziom_napelnienia < 100.0f) {
                            wrura[7].aktywny_przeplyw_w_rurze = true;
                            wrura[7].predkosc_wiz_w_rurach = 1.0f;
                        }
                        else {
                            wrura[7].aktywny_przeplyw_w_rurze = false;
                        }
                    }
                    //Oblicza resztê z dzielenia wybrany_zbiornik + 1 przez l. wszyst. zb. (wzbiorniki.size()), gdy wart. wybrany_zbiornik przekroczy ostatni indeks(np.wzbiorniki.size() - 1), wraca na pocz¹tek(indeks 0).
                    if (event.key.code == sf::Keyboard::Up) {
                        wybrany_zbiornik = (wybrany_zbiornik + 1) % wzbiorniki.size();

                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        wybrany_zbiornik = (wybrany_zbiornik - 1 + wzbiorniki.size()) % wzbiorniki.size();
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && wybrany_zbiornik > 0) {
                        if (wzbiorniki[wybrany_zbiornik - 1].poziom_napelnienia < 100.0f) {
                            przesylanie_cieczy(wzbiorniki, wrura, wybrany_zbiornik, wybrany_zbiornik - 1, predkosc_przesylu, -1.0f);
                        }
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                        if (wybrany_zbiornik == 1) {
                            // Przep³yw ze zbiornika 2 do zbiorników 3 i 4
                            if (wzbiorniki[2].poziom_napelnienia < 100.0f || wzbiorniki[3].poziom_napelnienia < 100.0f) {
                                rozgaleznik_przeplywu(wzbiorniki, wrura, 1, 2, 3, predkosc_przesylu);
                            }
                        }
                        else if (wybrany_zbiornik == 2 && wzbiorniki[4].poziom_napelnienia < 100.0f) {
                            // Przep³yw z 3 do 5
                            przesylanie_cieczy(wzbiorniki, wrura, 2, 4, predkosc_przesylu, 1.0f);
                        }
                        else if (wybrany_zbiornik == 3 && wzbiorniki[4].poziom_napelnienia < 100.0f) {
                            // Przep³yw z 4 do 5
                            przesylanie_cieczy(wzbiorniki, wrura, 3, 4, predkosc_przesylu, 1.0f);
                        }
                        else if (wybrany_zbiornik < wzbiorniki.size() - 1 && wzbiorniki[wybrany_zbiornik + 1].poziom_napelnienia < 100.0f) {
                            // Standardowy przep³yw do nastêpnego zbiornika
                            przesylanie_cieczy(wzbiorniki, wrura, wybrany_zbiornik, wybrany_zbiornik + 1, predkosc_przesylu, 1.0f);
                        }
                    }
                    if (event.key.code == sf::Keyboard::Escape) {
                        bool flaga_potw_wyjscia = komunikat_wyjscia(window);
                        if (flaga_potw_wyjscia) {
                            window.close();
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::White);

        if (ekran_ze_scenariuszami) {
            // Rysowanie listy scenariuszy za pomoc¹ Menedzer_scenariuszy
            Menedzer_scenariuszy.rysuj_ekran_scenariuszy(window);
        }
        else {
            //Grzanie zbiornika 4
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
                if (wzbiorniki[3].poziom_napelnienia > 0.0f) {
                    wzbiorniki[3].dodaj_babelki(true); // B¹belki lec¹ w górê
                    temperatura_zb_4 += 0.1f;

                    // Tworzenie p³omieni, jeœli jeszcze ich nie ma
                    if (wplomien.empty()) {
                        wplomien = plomien::utworzplomienie();
                    }
                }
                else {
                    alert(window, L"Nie mo¿na grzaæ", L"Zbiornik 4 jest pusty.");
                }
            }
            else {
                // Usuñ p³omienie, gdy grzanie przestaje byæ aktywne
                wplomien.clear();
            }

            // Ch³odzenie zbiornika 4
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                if (wzbiorniki[3].poziom_napelnienia > 0.0f) {
                    wzbiorniki[3].dodaj_babelki(false);
                    temperatura_zb_4 -= 0.1f;
                }
                else {
                    alert(window, L"Nie mo¿na ch³odziæ", L"Zbiornik 4 jest pusty.");
                }
            }

            temperatura_zb_4 = std::max(0.0f, std::min(100.0f, temperatura_zb_4)); //std::max zwraca wiêksz¹ z dwóch //ustawia temperature zb. 4 min 0 a max 100
            wzbiorniki[3].aktual_babelki();

            //Nape³nianie zbiornika 1
            if (zawor_napelniajacy.jest_otwarty) {
                wzbiorniki[0].poziom_napelnienia = std::min(100.0f, wzbiorniki[0].poziom_napelnienia + szybkosc_napelniania_zb);//ustawia poziom napelnienia zb.  min 0 a max 100
                wrura[7].aktywny_przeplyw_w_rurze = true;
            }

            // Zatrzymanie animacji rury nr 8, jeœli zbiornik 1 jest pe³ny
            if (wzbiorniki[0].poziom_napelnienia >= 100.0f) {
                wrura[7].aktywny_przeplyw_w_rurze = false;
            }

            if (zawor_spustowy_otwarty && wzbiorniki[4].poziom_napelnienia > 0.0f) {
                // Opró¿nianie zbiornika 4
                wzbiorniki[4].poziom_napelnienia = std::max(0.0f, wzbiorniki[4].poziom_napelnienia - predkosc_oprozniania_zb);
                wrura[10].aktywny_przeplyw_w_rurze = true;  // Aktywacja przep³ywu w rurze nr. 11
                wrura[10].predkosc_wiz_w_rurach = 1.0f; // Prêdkoœæ przep³ywu
                zb_4_pusty = false; // Zbiornik nie jest jeszcze pusty
            }
            else {
                wrura[10].aktywny_przeplyw_w_rurze = false; // Dezaktywacja przep³ywu, gdy zawór zamkniêty lub brak wody

                if (wzbiorniki[4].poziom_napelnienia <= 0.0f && !zb_4_pusty) {
                    // Reset temperatury, gdy zbiornik 4 zosta³ w³aœnie opró¿niony
                    temperatura_zb_4 = 20.0f;
                    zb_4_pusty = true; // Oznaczamy, ¿e reset zosta³ wykonany
                    std::wcout << L"Zbiornik 4 jest pusty. Temperatura zosta³a zresetowana do 20°C.\n";
                }
            }

            for (size_t i = 0; i < wrura.size(); ++i) {
                if (i != 7 && i != 10) { // Zachowaj stan rury nr. 8 i nr. 11
                    wrura[i].aktywny_przeplyw_w_rurze = false;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && wybrany_zbiornik > 0) {
                if (wzbiorniki[wybrany_zbiornik - 1].poziom_napelnienia < 100.0f) {
                    przesylanie_cieczy(wzbiorniki, wrura, wybrany_zbiornik, wybrany_zbiornik - 1, predkosc_przesylu, -1.0f);
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                if (wybrany_zbiornik == 1) {
                    // Przep³yw ze zbiornika 2 do zbiorników 3 i 4
                    if (wzbiorniki[2].poziom_napelnienia < 100.0f || wzbiorniki[3].poziom_napelnienia < 100.0f) {
                        rozgaleznik_przeplywu(wzbiorniki, wrura, 1, 2, 3, predkosc_przesylu);
                    }
                }
                else if (wybrany_zbiornik == 2 && wzbiorniki[4].poziom_napelnienia < 100.0f) {
                    // Przep³yw z 3 do 5
                    przesylanie_cieczy(wzbiorniki, wrura, 2, 4, predkosc_przesylu, 1.0f);
                }
                else if (wybrany_zbiornik == 3 && wzbiorniki[4].poziom_napelnienia < 100.0f) {
                    // Przep³yw z 4 do 5
                    przesylanie_cieczy(wzbiorniki, wrura, 3, 4, predkosc_przesylu, 1.0f);
                }
                else if (wybrany_zbiornik < wzbiorniki.size() - 1 && wzbiorniki[wybrany_zbiornik + 1].poziom_napelnienia < 100.0f) {
                    // Standardowy przep³yw do nastêpnego zbiornika
                    przesylanie_cieczy(wzbiorniki, wrura, wybrany_zbiornik, wybrany_zbiornik + 1, predkosc_przesylu, 1.0f);
                }
            }

            if (aktywny_scenariusz != Scenariusz_0) {
                Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                switch (aktywny_scenariusz) {

                case Scenariusz_grzej_do_50: // Grzanie do 50°C
                    if (wzbiorniki[3].poziom_napelnienia > 0.0f) { // SprawdŸ, czy w zbiorniku 4 jest woda
                        if (temperatura_zb_4 < 50.0f) {
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                            temperatura_zb_4 += 0.1f; // Zwiêksz temperaturê
                            wzbiorniki[3].dodaj_babelki(true); // B¹belki lec¹ w górê

                            // Tworzenie p³omieni, jeœli jeszcze ich nie ma
                            if (wplomien.empty()) {
                                wplomien = plomien::utworzplomienie();
                            }
                        }
                        else {
                            aktywny_scenariusz = Scenariusz_0; // Zakoñcz scenariusz po osi¹gniêciu 50°C
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                            wplomien.clear(); // Usuñ p³omienie po zakoñczeniu
                            std::wcout << L"Grzanie zakoñczone: Zbiornik 4 osi¹gn¹³ 50°C.\n";
                        }
                    }
                    else {
                        alert(window, L"Nie mo¿na grzaæ", L"Zbiornik 4 jest pusty.");
                        aktywny_scenariusz = Scenariusz_0; // Przerwij scenariusz
                        Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                        wplomien.clear(); // Usuñ p³omienie, jeœli scenariusz jest anulowany
                    }
                    break;

                case Scenariusz_ochladzaj_do_10: // Ch³odzenie do 10°C
                    if (wzbiorniki[3].poziom_napelnienia > 0.0f) { // SprawdŸ, czy w zbiorniku 4 jest woda
                        if (temperatura_zb_4 > 10.0f) {
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);

                            temperatura_zb_4 -= 0.05f;
                            wzbiorniki[3].dodaj_babelki(false);
                        }
                        else {
                            aktywny_scenariusz = Scenariusz_0; // Zakoñcz scenariusz
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                        }
                    }
                    else {
                        alert(window, L"Nie mo¿na ch³odziæ", L"Zbiornik 4 jest pusty.");
                        aktywny_scenariusz = Scenariusz_0; // Przerwij scenariusz
                        Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                    }
                    break;

                case Scenariusz_pelny_cykl_auto: {
                    static int krok = 0; // Zmienna do œledzenia etapu scenariusza
                    Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);

                    switch (krok) {
                    case 0: // Nape³nianie zbiornika 1
                        zawor_napelniajacy.jest_otwarty = true; // Otwieranie zaworu
                        wrura[7].aktywny_przeplyw_w_rurze = true;
                        wrura[7].predkosc_wiz_w_rurach = 1.0f;

                        if (wzbiorniki[0].poziom_napelnienia >= 100.0f) { // Gdy zbiornik 1 pe³ny
                            zawor_napelniajacy.jest_otwarty = false; // Zamykanie zaworu
                            wrura[7].aktywny_przeplyw_w_rurze = false;
                            krok++;
                            std::wcout << L"Zbiornik 1 nape³niony. Przechodzê do przesy³u do zbiornika 2.\n";
                        }
                        break;

                    case 1: // Przesy³ ze zbiornika 1 do zbiornika 2
                        if (!przesylanie_cieczy(wzbiorniki, wrura, 0, 1, predkosc_przesylu, 1.0f)) {
                            krok++;
                            std::wcout << L"Zbiornik 2 nape³niony. Przechodzê do przesy³u do zbiorników 3 i 4.\n";
                        }
                        break;

                    case 2: // Przesy³ ze zbiornika 2 do zbiorników 3 i 4
                        if (!rozgaleznik_przeplywu(wzbiorniki, wrura, 1, 2, 3, predkosc_przesylu)) {
                            krok++;
                            std::wcout << L"Zbiorniki 3 i 4 nape³nione. Uruchamiam mieszanie i grzanie.\n";
                        }
                        break;

                    case 3: // Uruchom mieszanie i grzanie
                        mikser_on = true;
                        if (temperatura_zb_4 < 50.0f) {
                            temperatura_zb_4 += 0.1f; // Grzanie zbiornika 4
                            wzbiorniki[3].dodaj_babelki(true); // B¹belki lec¹ w górê

                            // Dodawanie p³omieni
                            if (wplomien.empty()) {
                                wplomien = plomien::utworzplomienie();
                            }
                        }

                        aktualizujPoziomWymieszania(poziom_wymieszania, wzbiorniki[2].poziom_napelnienia, mikser_on);

                        if (temperatura_zb_4 >= 50.0f && poziom_wymieszania >= 100.0f) {
                            krok++;
                            mikser_on = false;
                            wplomien.clear(); // Usuñ p³omienie po zakoñczeniu grzania
                            std::wcout << L"Mieszanie i grzanie zakoñczone. Przesy³am wodê do zbiornika 5.\n";
                        }
                        break;

                    case 4: // Przesy³ ze zbiorników 3 i 4 do zbiornika 5
                        if (!przesylanie_cieczy(wzbiorniki, wrura, 2, 4, predkosc_przesylu, 1.0f) &&
                            !przesylanie_cieczy(wzbiorniki, wrura, 3, 4, predkosc_przesylu, 1.0f)) {
                            krok++;
                            std::wcout << L"Woda przes³ana do zbiornika 5. Opró¿niam zbiornik 5.\n";
                        }
                        break;

                    case 5: // Opró¿nianie zbiornika 5
                        zawor_spustowy_otwarty = true; // Otwieranie zaworu spustowego
                        wrura[10].aktywny_przeplyw_w_rurze = true;
                        wrura[10].predkosc_wiz_w_rurach = 1.0f;

                        if (wzbiorniki[4].poziom_napelnienia <= 0.0f) { // Gdy zbiornik 5 jest pusty
                            zawor_spustowy_otwarty = false; // Zamkniêcie zaworu
                            wrura[10].aktywny_przeplyw_w_rurze = false;
                            krok = 0;
                            aktywny_scenariusz = Scenariusz_0; // Zakoñczenie scenariusza
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz); // Aktualizacja wyœwietlanej nazwy
                            std::wcout << L"Pe³ny proces automatyczny zakoñczony.\n";
                        }
                        break;
                    }
                    break;
                }

                case Scenariusz_mieszanie:
                    if (wzbiorniki[2].poziom_napelnienia > 0.0f) {
                        mikser_on = true;
                        aktualizujPoziomWymieszania(poziom_wymieszania, wzbiorniki[2].poziom_napelnienia, mikser_on);
                        if (poziom_wymieszania >= 100.0f) {
                            mikser_on = false;
                            aktywny_scenariusz = Scenariusz_0;
                            Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                        }
                    }
                    else {
                        alert(window, L"Nie mo¿na mieszaæ", L"Zbiornik 3 jest pusty.");
                        poziom_wymieszania = 0.0f;
                        mikser_on = false;
                        aktywny_scenariusz = Scenariusz_0;
                        Menedzer_scenariuszy.synchro_nazwe_aktw_scenariusza(aktywny_scenariusz);
                    }
                    break;

                default:
                    break;
                }


                temperatura_zb_4 = std::max(0.0f, std::min(100.0f, temperatura_zb_4));
                for (auto& t : wzbiorniki) {
                    t.poziom_napelnienia = std::max(0.0f, std::min(100.0f, t.poziom_napelnienia));
                }
            }

            if (mikser_on) {
                kat_miksera += 5.0f;
                if (kat_miksera >= 360.0f) kat_miksera -= 360.0f;
                mikser.setRotation(kat_miksera);
            }

            aktualizujPoziomWymieszania(poziom_wymieszania, wzbiorniki[2].poziom_napelnienia, mikser_on);

            // Aktualizuj poprzedni poziom wody
            poprzedni_poziom_napelnienia_zb3 = wzbiorniki[2].poziom_napelnienia;

            // Aktualizuj rury
            for (auto& rura : wrura) rura.update();

            // Aktualizacja nazwy scenariusza
            std::wstring initialScenarioName = tekst_info_aktyw_scenariusz(aktywny_scenariusz);

            // Aktualizuj p³omienie
            for (auto& plomien : wplomien) {
                plomien.draw(window);
            }

            if (!ekran_ze_scenariuszami) {
                // Rysowanie aktywnego scenariusza za pomoc¹ Menedzer_scenariuszy
                Menedzer_scenariuszy.rysuj_info_aktyw_scenariusz(window);
            }

            zawor_napelniajacy.draw(window);
            zawor_spustowy.draw(window);
            for (auto& p : wrura) p.draw(window);
            for (size_t i = 0; i < wzbiorniki.size(); i++) {
                if ((int)i == wybrany_zbiornik) wzbiorniki[i].kszatlt.setOutlineColor(sf::Color::Blue);
                else wzbiorniki[i].kszatlt.setOutlineColor(sf::Color::Black);
                wzbiorniki[i].draw(window);
            }

            // UAKTUALNIENIE STANU ZAWORÓW TUTAJ!
            zawor_napelniajacy.zawor_wyglad.setFillColor(zawor_napelniajacy.jest_otwarty ? sf::Color::Green : sf::Color::Red);
            zawor_spustowy.zawor_wyglad.setFillColor(zawor_spustowy_otwarty ? sf::Color::Green : sf::Color::Red);

            mikser.setFillColor(mikser_on ? sf::Color::Blue : sf::Color::Black);
            window.draw(mikser);
            Menedzer_scenariuszy.rysuj_info_aktyw_scenariusz(window);
        }

        if (!ekran_ze_scenariuszami) {
            // Aktualizacja informacji w panelu kontrolnym
            Panel_kontrolny.aktual_informacji(wzbiorniki, temperatura_zb_4, mikser_on, poziom_wymieszania);

            // Rysowanie ca³ego panelu kontrolnego
            Panel_kontrolny.draw(window);
        }

        window.display();
    }

    return 0;
}
