/*
Import setninger for aa kunne bruke bibliotektet
til OLED skjermen
*/
#include <SD.h>         // For sd kort
#include <SPI.h>        // For sd kort
#include <AudioZero.h>  // For lyd
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>  // For tid

// Skjermen vaar er 128x32
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Definerer et skjerm objekt som heter display
#define OLED_RESET -1  //reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Knappene til tangene:
const int vTan = 2;  //venstre
const int mTan = 3;  //midten
const int hTan = 4;  //hoyre

// Avslutt knapp
const int avslutt = 5;

// Variabler for om knappene er "aktive"
bool vA = false;
bool mA = false;
bool hA = false;

// Volum meteret og volum:
const int vMeter = 21;
int volum = 0;


// Navn paa sangene i sd-kortet lagret paa hver sin sjanger liste:
char *jazzTekst[] = { "Harry James & His Orchestra - It's Been A Long, Long Time",
                      "David Grant & Jaki Graham - Could It Be I'm Falling In Love",
                      "Louis Armstrong - What A Wonderful World" };

char *klassiskTekst[] = { "Beethoven - FÃ¼r Elise",
                          "Giacomo Puccini & Giovacchino Forzano - O Mia Babbino Caro",
                          "Georges Bizet - Carmen Instrumental" };

char *discoverTekst[] = { "Drake - Passionfruit",
                          "PinkPantheress & Ice Spice - Boy's a liar Pt. 2",
                          "Taylor Swift - Karma" };

// Navn paa filene
char *jazzFil[] = { "J1.wav", "J2.wav", "J3.wav" };
char *klassiskFil[] = { "K1.wav", "K2.wav", "K3.wav" };
char *discoverFil[] = { "D1.wav", "D2.wav", "D3.wav" };

// Antall jazz sanger
const int antJazz = 3;
const int antKlassisk = 3;
const int antDiscover = 3;

// Rekkefolgen foer den blir gjort tilfeldig
int rekkefolgeJazz[] = { 0, 1, 2 };
int rekkefolgeKlassisk[] = { 0, 1, 2 };
int rekkefolgeDiscover[] = { 0, 1, 2 };

// Teller for aa holde styr paa hvor langt inn i listen den er
int tellerJazz = 0;
int tellerKlassisk = 0;
int tellerDiscover = 0;

// Variabel som holder styr paa hvilken dag det er
int dag = weekday();

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }  // Venter til serial har startet

  // Knappene har input pullup for aa ungaa unoeyaktige inputs
  pinMode(vTan, INPUT_PULLUP);
  pinMode(mTan, INPUT_PULLUP);
  pinMode(hTan, INPUT_PULLUP);

  /*
  DISPLAY FOR OLED SETUP
  */
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 FEILET"));
    while (true) { ; }
  }

  display.display();  // Display maa kalles for aa kunne vise det som blir skrevet
  display.clearDisplay();

  display.setTextSize(3);
  display.setRotation(2);                  // 180, grader rotasjon, slik at det passer med der hvor vi loddet
  display.drawPixel(1, 1, SSD1306_WHITE);  // Tegner en piksel, (standard for aa se naar skjermen skrus paa)
  //display.println(F("Skrus paa.."));
  display.display();

  displayString("Designerne.ino"); // !!
  display.display();


  delay(2000);

  display.clearDisplay();

  /*
  TID PAA DAGEN SETUP
  */
  setTime(11, 32, 0, 24, 5, 2023);
  // Maa sette det opp manuelt pga mangel paa arduino komponenter ||||||||||<<<<<<<<<<< VIKTIG <<<<<<<<<<<||||||||||


  /*
  HOYTTALER SETUP
  */
  Serial.println("Initialiserer SD kortet...");

  if (!SD.begin(SDCARD_SS_PIN)) {  // Finner det innebygde sd kortet (SDCARD_SS_PIN)
    Serial.println("Noe galt skjedde!");
    while (true)
      ;
  }

  Serial.println("Suksessfull.");

  // Setter sample raten til 44100hz*2
  // AudioZero.begin(2 * 44100);

  // Shuffler listene en gang for aa faa en tilfeldig rekkefolge paa sangene
  shuffle(rekkefolgeJazz, antJazz);
  shuffle(rekkefolgeKlassisk, antKlassisk);
  shuffle(rekkefolgeDiscover, antDiscover);
}

void loop() {
  // Naar det er en ny dag skal alt sammen forsyves med et hakk, hvis det skulle vaere slik at de haar naadd slutten av rekkefolgen saa lages det en ny en
  if (dag != weekday()) {
    dag = weekday();
    if (++tellerJazz == antJazz) {
      shuffleRiktig(rekkefolgeJazz, antJazz);
      tellerJazz = 0;
    }
    if (++tellerKlassisk == antKlassisk) {
      shuffleRiktig(rekkefolgeKlassisk, antKlassisk);
      tellerKlassisk = 0;
    }
    if (++tellerDiscover == antDiscover) {
      shuffleRiktig(rekkefolgeDiscover, antDiscover);
      tellerDiscover = 0;
    }
  }

  // If sjekker for om knapper blir trykket ned
  if (digitalRead(vTan) == LOW) {
    spill("C4.wav");
    delay(500);
    for (int i = 0; i < 2; i++) {
    	displayString("Spiller av: " + String(jazzTekst[rekkefolgeJazz[tellerJazz]]));
    }
    spill(jazzFil[rekkefolgeJazz[tellerJazz]]);  // Spiller av musikk fra den tilsvarende listen valgt av rekkefolgen
    while (digitalRead(vTan) == LOW) { ; }  // Venter helt til bruker har sluppet knappen
    delay(500);                             // Liten delay som passer paa at den ikke registrerer et andre trykk paa samme trykk
  }
  if (digitalRead(mTan) == LOW) {
    spill("D4.wav");  // Spiller av note D fjerde oktav for midterste piano tangent
    delay(500);
    for (int i = 0; i < 2; i++) {
    	displayString("Spiller av: " + String(klassiskTekst[rekkefolgeKlassisk[tellerKlassisk]]));
    }
    spill(klassiskFil[rekkefolgeKlassisk[tellerKlassisk]]);
    while (digitalRead(mTan) == LOW) { ; }
    delay(500);
  }
  if (digitalRead(hTan) == LOW) {
    spill("E4.wav");
    delay(500);
    for (int i = 0; i < 2; i++) {
    	displayString("Spiller av: " + String(discoverTekst[rekkefolgeDiscover[tellerDiscover]]));
    }
    spill(discoverFil[rekkefolgeDiscover[tellerDiscover]]);
    while (digitalRead(hTan) == LOW) { ; }
    delay(500);
  }

  volum = map(analogRead(21), 0, 1023, 0, 10);  // Setter volumet til et tall mellom 0 til 10 ut ifra potentiometerets verdi
  // AudioZero.setVolum(volum); //er en funksjon i AudioZero, men er ikke implementert av utviklerene. Mer info i rapport
}

// Shuffler en rekkefolge til den starter paa riktig maate, ikke starter med hva den forrige rekkefolgen sluttet med
void shuffleRiktig(int liste[], int lengde) {
  // Lager en helt ny liste som ikke har samme minne adresse som den andre listen
  int nyListe[lengde];
  for (int i = 0; i < lengde; i++) {
    nyListe[i] = liste[i];
  }
  while (true) {  // Shuffler helt til kriteriene er moett
    shuffle(nyListe, lengde);
    if (nyListe[0] != liste[lengde - 1]) {  // Sjekker om den nye listen ikke starter med hva den forrige sluttet med
      for (int i = 0; i < lengde; i++) {    // Bytter ut verdiene
        liste[i] = nyListe[i];
      }
      break;
    }
  }
}

// Endrer posisjonen paa elementene i listen tilfeldig
void shuffle(int liste[], int lengde) {
  for (int i = 0; i < lengde; i++) {
    int pos = random(lengde);
    int tmp = liste[i];
    liste[i] = liste[pos];
    liste[pos] = tmp;
  }
}

// Sjekker om et element finnes i en liste med heltall
bool harElement(int liste[], int element) {
  for (int i = 0; i < sizeof(liste); i++) {
    if (liste[i] == element) return true;
  }
  return false;
}

// Spill av fil funk:
void spill(String filnavn) {
  AudioZero.begin(2 * 44100);  //starter aa lese
  File musikkFil = SD.open(filnavn);
  if (!musikkFil) {
    Serial.println("Fant ikke fil");
    return;
  }

  if (filnavn.equals("C4.wav") || filnavn.equals("D4.wav") || filnavn.equals("E4.wav")) {
    // Skal ikke vise feedback lydene fra tangentene, kun sanger
    AudioZero.play(musikkFil);
  } else {
    AudioZero.play(musikkFil);
  }

  AudioZero.end();
}

/*
Tar string som parameter og displayer den
bruk denne paa hvilken sang som spilles
*/
void displayString(String str) {
  display.clearDisplay();
  display.setRotation(2);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  /*
    Beregner lengden paa teksten som skal vises i piksler
  */
  int strLength = str.length() * 6 * 3;  // Hver karakter er  6 pixels bred. * 3  fordi tekst stoerrelse er sat til 3
  int pos = SCREEN_WIDTH;                // Start posisjon er utenfor skjermen paa hoyere side

  // Skrolle loop
  /*
    Kjoerer saa lenge pos er stoerre enn 
    negativ lengde av tekst
  */
  while (pos > -strLength) {
    display.clearDisplay();
    display.setCursor(pos, 0);
    display.println(str);
    display.display();
    // delay(1); // farten for skrolling
    pos--;
  }

  delay(1);
}