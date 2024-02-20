#include <DS3231.h>
#include <Wire.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <pitches.h>
#include <EEPROM.h>  

#define CLK 2
#define DIO 3

DS3231 clock;
RTCDateTime dt;

// Funguje s Arduino Nano Old Bootloader
// Toto je vylepsena verze puvodniho LED budiku, program je upraven a je tu vice pisnicek
// zapojeni RTC  SDA-A4, SCL-A5
// zapojeni TM1637  CLK-D2, DLO-D3
// na pin9 je zapojen pres 100ohm rezistor levy reproduktor
// na pin10 je zapojen pres 100ohm rezistor pravy reproduktor
// volne piny D11  
// D12 - senzorove tlacitko k zobrazeni casu alarmu - po stisku je HIGH
 
byte jas = 7; // pres den je 7  vecer a v noci 1
int jeAlarm = 0;
int casalarmuhod = 18;
int casalarmumin = 18;
byte budetobudit = 0;  // pokud ano 1, pokud ne 0
byte melodie = 2; // po restartu bude budit melodie 2
byte pozadavekmelodie = 1;
byte test = 0;  // pokud neco testujeme je hodnota 1 
int pot1Value = 500;  // pin A0
int pot2Value = 500;  // pin A1
int pot3Value = 500;  // pin A2
byte funkce = 0; 
byte pin8 = 1;
byte pin12 = 0;
byte c1 = 1;
byte c2 = 2;
byte c3 = 5;
byte c4 = 9;
byte hodina = 23;
byte minuta = 53;
byte sekunda = 50;
int rok = 2000;
byte denvtydnu = 1;
byte den = 1;
byte mesic = 1;
byte a1hodina = 1;
byte a1minuta = 1;
byte a2hodina = 0;
byte a2minuta = 0;
int teplota = 10;
byte zap = 0;
byte vyp = 0; // pomocna promenna
byte zap1 = 0;
byte vyp1 = 0;
byte t1hodina = 0;
byte t1minuta = 0;
byte t1den = 1; 
byte t1mesic = 1;
byte zobrazujcas = 1;
byte alarmdenvtydnu = 9;
int opakovanialarmu = 1;  //pokud je 1 opakovani funguje, pokud je 0 budik se ten den uz neopakuje
byte minutaposunutehoalarmu = 10;
byte hodinaposunutehoalarmu = 12;
byte pozadavekopakovanialarmu = 1;
byte pozadavekposunutiminuty = 2;
int pozadavek = 0;
int verzeDen=2101;  //editovat pri zmene
int verzeRok=2024;  //editovat pri zmene
// nahrano do Arduina  21-01-2024 vecer 


// melodie - testovaci
int melody0[] = { NOTE_C4, NOTE_E4, NOTE_G4}; 
int pocetnot0 = 3;
int noteDurations0[] = { 4, 4, 4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
// melodie - Slys jaky to nad rekou - Cely liturgicky rok
int melody1[] = {
  NOTE_D3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, 0,
  NOTE_D3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, 0,
  NOTE_D3, NOTE_C4, 0, NOTE_C4, NOTE_B3, 0, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_D4, 0, NOTE_D3, NOTE_C4, 0, NOTE_C4, NOTE_B3, 0,
  NOTE_B3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, 0
};
int pocetnot1 = 51;
int noteDurations1[] = {
  2,2,2,2,2,2,2,2,2,4,4,2,2,2,2,2,2,2,2,2,2,2,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,4,2,2
};
// melodie - Z nebe posel vychazi - Adventni doba
int melody2[] = {
 NOTE_G3, NOTE_G3, NOTE_FS3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_G3, NOTE_FS3, NOTE_E3, NOTE_D3, NOTE_D3,
 NOTE_G3, NOTE_G3, NOTE_FS3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_G3, NOTE_FS3, NOTE_E3, NOTE_D3, NOTE_D3,
NOTE_A3, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_A3, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_G3, 0
};
int pocetnot2 = 39;
int noteDurations2[] = {2,2,2,2,2,2,1,1,2,2,2,2,1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,8}; 

// melodie - clock Big-Ben - Cely liturgicky rok 
int melody3[] = {
  NOTE_E4, NOTE_C4, NOTE_D4, NOTE_G3, 0, NOTE_G3, NOTE_D4, NOTE_E4, NOTE_C4, 0 };
int noteDurations3[] = {2,2,2,2,2,2,2,2,2,2};
int pocetnot3 = 10; 

// melodie - nesem vam noviny 222 - Vanocni doba
int melody4[] = {
 NOTE_G4, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_F4, 
 NOTE_G4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_C4, 0, 
 NOTE_G4, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_F4, 
 NOTE_G4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_C4, 0, 
 NOTE_C4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_G3, 
 NOTE_C4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_G3, 
 NOTE_G4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_C4
};
int pocetnot4 = 60;
int noteDurations4[] = {2,4,4,4,4,2,4,4,4,4,2,4,4,4,4,1,8,2,4,4,4,4,2,4,4,4,4,2,4,4,4,4,1,8,
2,4,4,4,4,2,4,4,4,4,2,4,4,4,4,2,4,4,4,4,2,4,4,4,4,1}; 

// melodie - ach muj nejsladsi Jezisi 301 - Postni doba 
int melody5[] = {
 NOTE_D3, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_B3, NOTE_A3, 0, 
 NOTE_A3, NOTE_B3, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_A3, NOTE_G3, 0,
 NOTE_D3, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_B3, NOTE_A3, 0, 
 NOTE_A3, NOTE_B3, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_A3, NOTE_G3, 0,
 NOTE_B3, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_B3, NOTE_C4, NOTE_A3, 0,
 NOTE_B3, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_B3, NOTE_C4, NOTE_A3,0,
 NOTE_D3, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_B3, NOTE_A3, 0,
 NOTE_A3, NOTE_B3, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3
};
int pocetnot5 = 72;
int noteDurations5[] = {2,2,2,2,2,2,2,2,8,4,4,2,2,2,2,1,2,8,2,2,2,2,2,2,2,2,8,4,4,2,2,2,2,1,2,8,
2,2,2,2,2,2,2,2,8,4,4,2,2,2,2,2,2,2,8,2,2,2,2,2,2,2,2,8,4,4,2,2,2,2,1,2}; 

// melodie - 403 - Hle vstal jiz z mrtvych
int melody6[] = {
 NOTE_D4, NOTE_G4, NOTE_G4, NOTE_B4, NOTE_G4, NOTE_D4, NOTE_C4, NOTE_B3, 0,
 NOTE_D4, NOTE_E4, NOTE_E4, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_D4, NOTE_E4, NOTE_FS4, NOTE_G4, 0,
 NOTE_A4, NOTE_A4, NOTE_B4, NOTE_CS5, NOTE_D5, 0,
 NOTE_A4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 0,
 NOTE_D5, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_G4, 0,
 NOTE_D5, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_G4, 0,
 NOTE_D4, NOTE_G4, NOTE_B4, NOTE_G4, NOTE_D5, 0,
 NOTE_B4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_FS4, NOTE_G4 
};
int pocetnot6 = 66;
int noteDurations6[] = {4,2,4,2,4,2,4,2,8,4,2,4,2,4,4,4,8,8,2,8,
4,4,4,4,2,8,4,4,4,4,2,8,4,4,4,4,4,4,4,2,4,2,8,4,4,4,4,4,4,4,2,4,2,8,
4,2,8,8,2,8,8,8,4,4,4,2}; 

// melodie - asi popular - zatim je rozpracovane
int melody7[] = {
 NOTE_D3, NOTE_G3, NOTE_A3 
};
int pocetnot7 = 3;
int noteDurations7[] = {2,2,2}; 


TM1637Display displej(CLK, DIO);

const uint8_t SEG_DB[] = {
  SEG_G,  // pomlcka         
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, // D  
  SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // B                           
  SEG_G    // pomlcka         
  };
const uint8_t SEG_VER[] = {
  SEG_C | SEG_F | SEG_G, // V leva cast
  SEG_B | SEG_E | SEG_G, // V prava cast  
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E                           
  SEG_E | SEG_G // R  
  };
const uint8_t SEG_OPAK[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G, // P  
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, // A                            
  SEG_G  // pomlcka  
  };

const uint8_t SEG_TEST[] = {
  SEG_D | SEG_E | SEG_F | SEG_G, // T
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E  
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G, // S                            
  SEG_D | SEG_E | SEG_F | SEG_G  // T
  };

const uint8_t SEG_NENE[] = {
  SEG_C | SEG_E | SEG_G, // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E  
  SEG_C | SEG_E | SEG_G, // n                            
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G // E  
  };
const uint8_t SEG_ALARM[] = {
	SEG_G,  // pomlcka         
	SEG_A | SEG_B | SEG_C | SEG_G | SEG_E | SEG_F, // A  
	SEG_F | SEG_E | SEG_D, // L                           
	SEG_G    // pomlcka         
	};
const uint8_t SEG_ULOZ[] = {
	SEG_G,  // pomlcka         
	SEG_D | SEG_B | SEG_C | SEG_E | SEG_F, // U 
	SEG_F | SEG_E | SEG_D, // L                           
	SEG_G    // pomlcka         
	};
const uint8_t SEG_ROK[] = {
	SEG_G,           // pomlcka
	SEG_D | SEG_E | SEG_F, // L  
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,  // P
  SEG_G, // pomlcka
  };
const uint8_t SEG_MESIC[] = {
	SEG_A | SEG_B | SEG_E | SEG_F,        
	SEG_A | SEG_B | SEG_C | SEG_F,  
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,                            
	SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
	};
const uint8_t SEG_DEN[] = {
	SEG_B | SEG_C | SEG_E | SEG_D | SEG_G,          
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,  
	SEG_C | SEG_E | SEG_G,                            
	SEG_G,
  };
const uint8_t SEG_TEPLOTA[] = {
	SEG_D | SEG_E | SEG_F | SEG_G,     
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,   
	SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,                
	SEG_D | SEG_E | SEG_F,
	};
const uint8_t SEG_MELO[] = {
	SEG_A | SEG_B | SEG_E | SEG_F,        
	SEG_A | SEG_B | SEG_C | SEG_F,  
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,                            
	SEG_D | SEG_E | SEG_F,
	};

void setup() 
{
pinMode(4, OUTPUT); // nastavuji alarm
pinMode(5, OUTPUT); // nastavuji cas
pinMode(6, OUTPUT); // nastavuji mesic a den
pinMode(7, OUTPUT); // sviti pokud to bude budit
pinMode(9, OUTPUT); // zapojeny levy 1W repro 
pinMode(10, OUTPUT); // zapojeny pravy 1W repro

pinMode(12, INPUT); // senzorovy spinac  
pinMode(8, INPUT_PULLUP); // vypinac alarm on-off  
pinMode(13, OUTPUT); // on-board LED 

displej.setBrightness(jas); // pres den 7 , vecer a v noci 1

EEPROM.get(1, opakovanialarmu);  //precte z EEPROMu
EEPROM.get(3, pozadavekposunutiminuty);  
EEPROM.get(5, melodie);  
if (opakovanialarmu>1) {opakovanialarmu=1;};
if (pozadavekposunutiminuty>15 or pozadavekposunutiminuty<2) {pozadavekposunutiminuty=2;};
if (melodie>9 or melodie<1) {melodie=1;};  

Serial.begin(9600);

  // Initialize DS3231
  // Serial.println("Initialize DS3231");;
  clock.begin();

  // Set sketch compiling time
 // clock.setDateTime(__DATE__, __TIME__);
 clock.setBattery(1,0); // Dulezite nastaveni 
 
 clock.armAlarm1(false);
 clock.armAlarm2(false);
 checkAlarms();


digitalWrite(9, LOW); digitalWrite(10, LOW); // L a P reproduktor vypnut
test=1; 
displej.clear(); displej.setSegments(SEG_DB);
delay(1800);
displej.clear(); displej.setSegments(SEG_VER);
delay(2000);
displej.clear(); displej.showNumberDec(verzeDen, true);
delay(2000);
displej.clear(); displej.showNumberDec(verzeRok, true);
delay(2000);
displej.clear(); displej.setSegments(SEG_TEST);
digitalWrite(4, HIGH); 
digitalWrite(5, HIGH); 
digitalWrite(6, HIGH);
digitalWrite(7, HIGH);

// prehraje testovaci zvukovou sekvenci -levy a potom pravy kanal
for (int thisNote = 0; thisNote < pocetnot0; thisNote++) {
   int noteDuration0 = 1000 / noteDurations0[thisNote]; tone(9, melody0[thisNote], noteDuration0);
    int pauseBetweenNotes = noteDuration0 * 1.30; delay(pauseBetweenNotes); };
    noTone(9); 
digitalWrite(4, LOW); digitalWrite(5, LOW); 
digitalWrite(6, LOW); digitalWrite(7, LOW);
for (int thisNote = 0; thisNote < pocetnot0; thisNote++) {
   int noteDuration0 = 1000 / noteDurations0[thisNote]; tone(10, melody0[thisNote], noteDuration0);
    int pauseBetweenNotes = noteDuration0 * 1.30; delay(pauseBetweenNotes); };
    noTone(10);  test=0;

 }


void pripravLED()    // nasledujici blok nachysta 4 cislice pro zobrazeni casu
{  
if (hodina > 19) { c1=2; c2=hodina-20;} ;
if (hodina > 9 and hodina<20) { c1=1; c2=hodina-10;};
if (hodina < 10) { c1=0; c2=hodina;} ;
if (minuta > 49) { c3=5; c4=minuta-50 ;} ;
if (minuta > 39 and minuta < 50) { c3=4; c4=minuta-40 ;} ;
if (minuta > 29 and minuta < 40) { c3=3; c4=minuta-30 ;} ;
if (minuta > 19 and minuta < 30) { c3=2; c4=minuta-20 ;} ;
if (minuta > 9 and minuta < 20) { c3=1; c4=minuta-10 ;} ;
if (minuta < 10) {c3=0; c4=minuta; } ;
if (minuta > 59) {c3=9; c4=9 ;} ;
if (minuta < 0) {c3=8; c4=8 ;} ;
if (hodina > 23) {c1=9; c2=9 ;} ;
if (hodina < 0) {c1=8; c2=8 ;} ;
}

void nastaveniJasu()  //Nastaveni promenne jas podle casu a dne v tydnu
{
if (hodina > 20 and denvtydnu<6) {jas = 1; }; // vecer a v noci je jas na 1 
if (hodina > 22 and denvtydnu==6) {jas = 1;}; 
if (hodina > 21 and denvtydnu==7) {jas = 1;};
if (hodina < 7 and denvtydnu<6) {jas = 1;}; 
if (hodina > 6 and hodina <21 and denvtydnu<6) {jas = 7;}; 
if (hodina > 9 and hodina <23 and denvtydnu==6) {jas = 7;}; 
if (hodina > 7 and hodina <21 and denvtydnu==7) {jas = 7;};
}

void tedHraje() //Blok zajistuje hrani melodie  
{
 displej.setBrightness(7);  
 if (melodie>7 or melodie<1) {melodie=7;};
 if (melodie==1) {
  for (int thisNote = 0; thisNote < pocetnot1; thisNote++) {
  int noteDuration1 = 1000 / noteDurations1[thisNote];
   tone(9, melody1[thisNote], noteDuration1);
   pin8=digitalRead(8); 
   if (pin8==1 and test==0) {noTone(9); jeAlarm=70; thisNote=99;};
   if (test==1 and thisNote>15) {noTone(9); thisNote=99;};
   int pauseBetweenNotes = noteDuration1 * 1.30; delay(pauseBetweenNotes); noTone(9); };};

if (melodie==2) {
  for (int thisNote = 0; thisNote < pocetnot2; thisNote++) {
  int noteDuration2 = 800 / noteDurations2[thisNote];
  tone(9, melody2[thisNote], noteDuration2);
  pin8=digitalRead(8); 
  if (pin8==1 and test==0) {noTone(9); jeAlarm=70; thisNote=99;};
  if (test==1 and thisNote>15) {noTone(9); thisNote=99;};
  int pauseBetweenNotes = noteDuration2 * 1.30; delay(pauseBetweenNotes); noTone(9); };};

if (melodie==3) {
  for (int thisNote = 0; thisNote < pocetnot3; thisNote++) {
  int noteDuration3 = 1000 / noteDurations3[thisNote];
   tone(9, melody3[thisNote], noteDuration3);
   pin8=digitalRead(8); 
   if (pin8==1 and test==0) {noTone(9); jeAlarm=70; thisNote=99;};
   if (test==1 and thisNote>15) {noTone(9); thisNote=99;};
   int pauseBetweenNotes = noteDuration3 * 1.30; delay(pauseBetweenNotes); noTone(9); };};

  if (melodie==4) {
  for (int thisNote = 0; thisNote < pocetnot4; thisNote++) {
  int noteDuration4 = 800 / noteDurations4[thisNote];
   tone(9, melody4[thisNote], noteDuration4);
   pin8=digitalRead(8); 
   if (pin8==1 and test==0) {noTone(9); jeAlarm=70; thisNote=99;};
   if (test==1 and thisNote>15) {noTone(9); thisNote=99;};
   int pauseBetweenNotes = noteDuration4 * 1.30; delay(pauseBetweenNotes); noTone(9); };};
    
  if (melodie==5) {
  for (int thisNote = 0; thisNote < pocetnot5; thisNote++) {
  int noteDuration5 = 900 / noteDurations5[thisNote];
   tone(9, melody5[thisNote], noteDuration5);
   pin8=digitalRead(8); 
   if (pin8==1 and test==0) {noTone(9); jeAlarm=70; thisNote=99;};
   if (test==1 and thisNote>15) {noTone(9); thisNote=99;};
   int pauseBetweenNotes = noteDuration5 * 1.30; delay(pauseBetweenNotes); noTone(9); };};
  
  if (melodie==6){
for (int thisNote = 0; thisNote < pocetnot6; thisNote++) {
  int noteDuration6 = 1200 / noteDurations6[thisNote];
   tone(9, melody6[thisNote], noteDuration6);
   pin8=digitalRead(8); 
   if (pin8==1 and test==0) {noTone(9); jeAlarm=70; thisNote=99;};
   if (test==1 and thisNote>15) {noTone(9); thisNote=99;};
   int pauseBetweenNotes = noteDuration6 * 1.30; delay(pauseBetweenNotes); noTone(9); };
  }
if (melodie==7){};
  
  delay(100); displej.setBrightness(jas);
}

void kontrolaVstupu() {
pin8=digitalRead(8); //kontroluje vypinac - pokud je vypnut je HIGH
 pin12=digitalRead(12); //kontrola senzor spinace - pokud je v klidu je LOW
 pot1Value = analogRead(A0); // funkce 0az4 rozsahy 0,1,2,3,4 hodnoty a rozsahy f0 0-205, f1 206-411, f2 412-617, f3 618-823, f4 824-1023 
 pot2Value = analogRead(A1);
 pot3Value = analogRead(A2);
}

void kontrolaFunkce() {
// funkce - 0 - Bezi cas, nic se nenastavuje, funguje budik pokud je zapnuty
// funkce - 1 - Nastavuje se alarm, pokud neni zapnut D8. Pokud je D8 zapnuty, nic se nedeje.
// funkce - 2 - Nastavuje se  cas. Pokud je D8 zapnuto, nic se nedeje.
// funkce - 3 - Nastavuje se opakovani stejneho dne a cas opakovani v minutach. Pokud je D8 zapnuto, nic se nedeje.
// funkce - 4 - Nastavuje se melodie alarmu. Pokud je D8 zapnuto, nic se nedeje.
if (pot1Value > -1 and pot1Value < 206) {funkce=0; digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, LOW);};
if (pot1Value > 205 and pot1Value < 412) {funkce=1; digitalWrite(4, HIGH); digitalWrite(5, LOW); digitalWrite(6,LOW);};
if (pot1Value > 411 and pot1Value < 618) {funkce=2; digitalWrite(4, LOW); digitalWrite(5, HIGH); digitalWrite(6, LOW);};
if (pot1Value > 617 and pot1Value < 824) {funkce=3; digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, HIGH);};
if (pot1Value > 823 and pot1Value < 1024) {funkce=4; digitalWrite(4, HIGH); digitalWrite(5, HIGH); digitalWrite(6, LOW);}; 
}

void kontrolaBudiku() // Zobrazi cas alarmu a dalsi podrobnosti 
{
if (pin12 == 1 and funkce==0) {displej.setSegments(SEG_ALARM);
 delay(1500);displej.clear();
 displej.showNumberDec(a1hodina,false,2,0);displej.showNumberDec(a1minuta,false,2,2);
 delay(2000); pin12=digitalRead(12);
 displej.setSegments(SEG_OPAK); delay(2000); pin12=digitalRead(12); 
 if (opakovanialarmu==0) { displej.setSegments(SEG_NENE);};
 if (opakovanialarmu==1) { displej.clear ();displej.showNumberDec(pozadavekposunutiminuty,false,2,2);}; 
 delay(2000); pin12=digitalRead(12);
displej.clear(); displej.setSegments(SEG_MELO); delay(1500); pin12=digitalRead(12);
displej.clear(); displej.showNumberDec(melodie, false, 4,0); 
if (pin12 == 1) { test=1; tedHraje(); test=0; pin12=digitalRead(12);} else {delay(2000);};
 };
}

void checkAlarms()
{
  RTCAlarmTime a1;  
  RTCAlarmTime a2;

  if (clock.isArmed1())
  {
    a1 = clock.getAlarm1();

    switch (clock.getAlarmType1())
    {
      case DS3231_EVERY_SECOND:
        break;
      case DS3231_MATCH_S:
        break;
      case DS3231_MATCH_M_S:
        break;
      case DS3231_MATCH_H_M_S:
        budetobudit = 1; 
        break;
      case DS3231_MATCH_DT_H_M_S:
        break;
      case DS3231_MATCH_DY_H_M_S:
        break;
      default: 
        Serial.println("UNKNOWN RULE");
        break;
    }
  } else
  {  }
 }

void loop()
{

nastaveniJasu(); displej.setBrightness(jas); 
kontrolaVstupu(); kontrolaFunkce(); 


// val = map(val, 0, 1023, 0, 255); ukazka funkce premapovani

displej.setBrightness(jas);
dt = clock.getDateTime();

sekunda=(dt.second);
hodina=(dt.hour); minuta=(dt.minute); 
rok=(dt.year); denvtydnu=(dt.dayOfWeek); 
den=(dt.day); mesic=(dt.month);  
a1hodina=(clock.getAlarm1().hour);
a1minuta=(clock.getAlarm1().minute);

// Nacte teplotu do pameti
clock.forceConversion();
teplota = (clock.readTemperature())-1;

pripravLED(); nastaveniJasu(); displej.setBrightness(jas); 

// PIN4 HIGH - nastavuji alarm
// PIN5 HIGH - nastavuji cas
// PIN6 HIGH - nastavuji opakovani
// PIN7 HIGH - sviti pokud to bude budit
// PIN9 levy reproduktor
// PIN10 pravy reproduktor

kontrolaVstupu();
kontrolaBudiku();
kontrolaVstupu(); kontrolaFunkce(); 
if (pin8 == 0 ) {budetobudit=1; zap=zap+1; vyp=0;} ; //vypinac je zapnuty  

if (zap == 1 and pot1Value > 205 and pot1Value < 412) {displej.clear(); displej.setSegments(SEG_ULOZ); 
clock.setAlarm1(0, (casalarmuhod),(casalarmumin), 0, DS3231_MATCH_H_M_S);digitalWrite(7,LOW);delay(2000);}; //ulozen cas alarmu hod,min,0 sekund
 
if (zap == 1 and pot1Value > 411 and pot1Value < 618) {displej.clear(); displej.setSegments(SEG_ULOZ); 
clock.setDateTime(rok,mesic,den,t1hodina,t1minuta,0);digitalWrite(7,LOW);delay(2000);}; //ulozena hodina, minuta, sekunda je 0

if (zap == 1 and pot1Value > 617 and pot1Value < 824) {displej.clear(); displej.setSegments(SEG_ULOZ); 
opakovanialarmu=pozadavekopakovanialarmu; pozadavekposunutiminuty = pozadavek; EEPROM.put(1, opakovanialarmu); 
EEPROM.put(3, pozadavekposunutiminuty); digitalWrite(7,LOW);delay(2000);}; // ulozen pozadavek na posunuti alarmu

if (zap == 1 and pot1Value > 823 ) {displej.clear(); displej.setSegments(SEG_ULOZ); melodie=pozadavekmelodie; EEPROM.put(5, melodie);    
digitalWrite(7,LOW);delay(2000);}; // konec nastavovani vyzvaneci melodie


if (zap > 10) {zap=10;};
if (pin8 == 1 ) {budetobudit=0; vyp=vyp+1; zap=0; }; //vypinac je vypnuty - nebudime
if (vyp > 10) {vyp=10;};
if (budetobudit == 1) {digitalWrite(7, HIGH);} else {digitalWrite(7, LOW);}; //dioda D7 sviti pokud je alarm zapnuty

uint8_t vypis[] = { 0, 0, 0, 0 };
vypis[0] = displej.encodeDigit(c1);
vypis[1] = displej.encodeDigit(c2);
vypis[2] = displej.encodeDigit(c3);
vypis[3] = displej.encodeDigit(c4);
displej.setSegments(vypis);
nastaveniJasu(); displej.setBrightness(jas); 

if (clock.isAlarm1() and budetobudit==1)
{ tedHraje(); }; //ted probiha alarm
 
kontrolaVstupu(); kontrolaFunkce(); 
 if (jeAlarm > 60) {jeAlarm=1; digitalWrite(7, LOW); budetobudit=0; delay(100) ; } ; // Alarm skoncil vyprsel cas nebo bylo vypnuto vypinacem 
  delay(200);

if (budetobudit==1 and opakovanialarmu==1) {minutaposunutehoalarmu=a1minuta+pozadavekposunutiminuty; hodinaposunutehoalarmu=a1hodina;};
if (minutaposunutehoalarmu>59) {minutaposunutehoalarmu=minutaposunutehoalarmu-60; hodinaposunutehoalarmu=hodinaposunutehoalarmu+1;};
if (hodinaposunutehoalarmu>23) {hodinaposunutehoalarmu=0;};

if (budetobudit==1 and opakovanialarmu==1 and hodina==hodinaposunutehoalarmu and minuta==minutaposunutehoalarmu and sekunda==0) {
 test=0; displej.setBrightness(7); tedHraje();};  // tento radek spousti posunuty budik

kontrolaVstupu(); kontrolaFunkce(); 

// uint8_t vypis[] = { 0, 0, 0, 0 };
vypis[0] = displej.encodeDigit(c1);
vypis[1] = displej.encodeDigit(c2);
vypis[2] = displej.encodeDigit(c3);
vypis[3] = displej.encodeDigit(c4);
displej.setSegments(vypis);
delay(195); 
kontrolaVstupu(); 
kontrolaFunkce(); 

vypis[1] = vypis[1]+128;
displej.setSegments(vypis); digitalWrite(13, HIGH);
delay(195);
kontrolaVstupu(); 
kontrolaFunkce();
kontrolaBudiku(); 
if (funkce>0) delay(200);

while (pot1Value > 205 and pot1Value < 412 and pin8 == 1) {kontrolaVstupu(); casalarmuhod=pot2Value/44;  pot3Value = analogRead(A2); 
casalarmumin=pot3Value/17.5; displej.showNumberDec(casalarmuhod,false,2,0);
displej.showNumberDec(casalarmumin,false,2,2); digitalWrite(4, HIGH);digitalWrite(5, LOW); digitalWrite(6,LOW);}; // funkce c.1 nastavuji alarm

while (pot1Value > 411 and pot1Value < 618 and pin8 == 1) {kontrolaVstupu(); t1hodina=pot2Value/44;  pot3Value = analogRead(A2); 
t1minuta=pot3Value/17.5; displej.showNumberDec(t1hodina,false,2,0);
digitalWrite(4, LOW); digitalWrite(5, HIGH); digitalWrite(6, LOW);
displej.showNumberDec(t1minuta,false,2,2);}; // funkce c.2 nastavuji cas - hodiny a minuty

// funkce c.3 - Opakovani budiku v den buzeni   ano/ne  pokud ano pot2 nastavuje prodlevu buzeni v minutach  
while (pot1Value > 617 and pot1Value < 824 and pin8 == 1) {kontrolaVstupu();
if (pot2Value < 514) {pozadavekopakovanialarmu=0;};
if (pot2Value > 513) {pozadavekopakovanialarmu=1;}; 
displej.showNumberDec(pozadavekopakovanialarmu,false,2,0);
pozadavek = ((pot3Value+1)/50)+2; 
digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, HIGH);
displej.showNumberDec(pozadavek,false,2,2);delay(10);}; // funkce c.3 

kontrolaVstupu(); kontrolaBudiku(); kontrolaFunkce();
while (pot1Value > 823 and pin8==1) {pot1Value = analogRead(A0); digitalWrite(4, HIGH); digitalWrite(5, HIGH); digitalWrite(6, LOW);  
kontrolaVstupu(); delay(90); pozadavekmelodie=(pot3Value/100); displej.showNumberDec(pozadavekmelodie, true,4); delay(90); 
if (pot1Value > 1022 and pot2Value > 1022 and pot3Value>1022 and pin12==1) {
displej.clear(); displej.setSegments(SEG_VER); delay(2000);
displej.clear(); displej.showNumberDec(verzeDen, true); delay(2000);
displej.clear(); displej.showNumberDec(verzeRok, true); delay(2000);
clock.forceConversion(); teplota = (clock.readTemperature()); 
displej.clear(); displej.setSegments(SEG_TEPLOTA); delay(1500);
displej.clear(); displej.showNumberDec((teplota)-1); delay(1500);}; 
};  // funkce c.4

nastaveniJasu(); displej.setBrightness(jas); 
kontrolaVstupu(); kontrolaBudiku(); 

if (hodina==12 and minuta==00 and sekunda==1) {for (int thisNote = 0; thisNote < pocetnot3; thisNote++) {
   int noteDuration3 = 1000 / noteDurations3[thisNote];
    tone(10, melody3[thisNote], noteDuration3);
    int pauseBetweenNotes = noteDuration3 * 1.30;
    delay(pauseBetweenNotes); };
    noTone(10);  };   // V poledne zahraje melodii 
if (c4==5 and sekunda==30 and funkce==0 and test==0) {
  clock.forceConversion(); teplota = (clock.readTemperature()); 
displej.clear(); displej.setSegments(SEG_TEPLOTA); delay(1500);
displej.clear(); displej.showNumberDec((teplota)-1); delay(1500);
};
if (c4==1 and sekunda==30 and funkce==0 and test==0) {
clock.forceConversion(); teplota = (clock.readTemperature()); 
displej.clear(); displej.setSegments(SEG_TEPLOTA); delay(1500);
displej.clear(); displej.showNumberDec((teplota)-1); delay(1500);
};
if (sekunda==30 and c4==2 and hodina>5 and hodina<22 and funkce==0) {
displej.clear(); displej.setSegments(SEG_DEN); delay(1500);  
displej.clear(); displej.showNumberDec(den); delay(1500); 
displej.clear(); displej.setSegments(SEG_MESIC); delay(1500);
displej.clear(); displej.showNumberDec(mesic); delay(1500); 
displej.clear(); displej.setSegments(SEG_ROK); delay(1500);
displej.clear(); displej.showNumberDec(rok); delay(1500);
};
digitalWrite(13, LOW);
kontrolaVstupu();  
nastaveniJasu(); displej.setBrightness(jas); 
}
