#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include "sh1106.h"
#include <time.h>  // standard C timing functions
#include "button.h"

const byte ledPin = 4;
const byte interruptPin = 2;
const byte ledstatePin = 5;
const byte ledstatePin1 = 6;
const byte ledstatePin2 = 7;

Button btn1;
Button btn2;
Button btn3;


void SH1106_begin();
void SH1106_display(uint8_t* buffer);
void SH1106_clear(uint8_t* buffer);
void SH1106_pixel(int x,int y,char color, uint8_t* buffer);
void SH1106_bitmap(uint8_t x,uint8_t y,const uint8_t *pBmp, uint8_t chWidth, uint8_t chHeight, uint8_t* buffer);
void SH1106_char1616(uint8_t x,uint8_t y,uint8_t chChar, uint8_t* buffer);
void SH1106_char3216(uint8_t x, uint8_t y, uint8_t chChar, uint8_t* buffer);
void SH1106_string(uint8_t x, uint8_t y, const char *pString, uint8_t Size, uint8_t Mode, uint8_t* buffer);
void SPIWrite(uint8_t *buffer, int bufferLength);
void command(uint8_t cmd);


#define VCCSTATE SH1106_SWITCHCAPVCC
#define WIDTH     128
#define HEIGHT     64
#define NUM_PAGE    8  /* number of pages */

#define OLED_RST    9 
#define OLED_DC     8
#define OLED_CS    10
#define SPI_MOSI   11    /* connect to the DIN pin of OLED */
#define SPI_SCK    13     /* connect to the CLK pin of OLED */


uint8_t oled_buf[WIDTH * HEIGHT / 8];

int contador=0;
int dezena_de_segundos=0;
int unidade_de_minutos=0;
int dezena_de_minutos=0;
int unidade_de_horas=0;
int dezena_de_horas=0;

int buttonState = 0; 
int buttonState1 = 0; 
int buttonState2 = 0; 
int estate = 0;
int memled = 0;
int estate1 = 0;
int memled1 = 0;
int estate2 = 0;
int memled2 = 0;

 int acerto_dh =0;
 int acerto_uh =0;
 int acerto_dm =0;
 int acerto_um =0;



char ano[3][12] = { "2022", "2023", "2024" };
char mes[16][12] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "12" };
char mes_desc[12][12] = {"Janeiro","Fevereiro","Março","Abril","Maio","Junho","Julho","Agosto","Setembro","Outubro","Novembro","Dezembro"};
char dia [31][3] =  {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31"};
char hora [24][3]=  {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23"};
int minuto = 0;
int segundo = 0;
 

RTC_DS1307 rtc;
int mode_index = 0;
Ds1307SqwPinMode modes[] = { DS1307_OFF, DS1307_ON, DS1307_SquareWave1HZ, DS1307_SquareWave4kHz, DS1307_SquareWave8kHz, DS1307_SquareWave32kHz};

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabado"};

char numero[10] = {'0','1','2','3','4','5','6','7','8','9'};

// rtc.adjust(DateTime(ano, mes, dia, hora, minuto, segundo));


//Rotina de interrupção 
//pino 2(tick())
//pino 3( )
//________________________________

void tick(){
  
  contador=contador+1;
 
}


//Serve para iniciar o output de onda quadrada que vai gerar interrupções
//_______________________________

void print_mode() {
  Ds1307SqwPinMode mode = rtc.readSqwPinMode();

  Serial.print("Sqw Pin Mode: ");
  switch(mode) {
  case DS1307_OFF:              Serial.println("OFF");       break;
  case DS1307_ON:               Serial.println("ON");        break;
  case DS1307_SquareWave1HZ:    Serial.println("1Hz");       break;
  case DS1307_SquareWave4kHz:   Serial.println("4.096kHz");  break;
  case DS1307_SquareWave8kHz:   Serial.println("8.192kHz");  break;
  case DS1307_SquareWave32kHz:  Serial.println("32.768kHz"); break;
  default:                      Serial.println("UNKNOWN");   break;
  }
}


//Acerto do relogio no RTC
//____________________________________________

void acerto_relogio(){
; 
    Serial.println(estate); 

   switch(estate){

   case 1 :{

    SH1106_clear(oled_buf);
    SH1106_string(0, 30, "Acrt HORA: ", 16, 1, oled_buf); 
    SH1106_char1616(96, 30, acerto_dh, oled_buf);
    SH1106_char1616(110, 30, acerto_uh, oled_buf);
    SH1106_display(oled_buf); 
    break;}

    case 2 :{
    
    SH1106_clear(oled_buf);
    SH1106_string(0, 30, "Acrt MINUT: ", 16, 1, oled_buf); 
    SH1106_char1616(96, 30, acerto_dm, oled_buf);
    SH1106_char1616(110, 30, acerto_um, oled_buf);
    SH1106_display(oled_buf); 
    break; }

    default :  break;
   }
  // secção do Botão numero dois ____BTN2__________________________________________


  if (btn2.debounce()) {
  SH1106_clear(oled_buf);
  SH1106_display(oled_buf); 
  digitalWrite(ledPin,  HIGH);
  delay(100);
  estate = estate+1;
  estate1 = estate1+1;
  } else {
      digitalWrite(ledPin, LOW);  
  }

  if(estate1>=2){
    digitalWrite(ledstatePin1, HIGH);
    estate1 = 0;
    memled1 = memled1+1;
  }

  if (memled1>=2){
    memled1 = 0;
    digitalWrite(ledstatePin1, LOW);
  }
  // secção do Botão numero três _BTN3_______________________________________________

  if (btn3.debounce()) {
     digitalWrite(ledPin,  HIGH);
      delay(100);
              if(estate==1){
                ++acerto_uh;
                      if(acerto_uh>9){
                        ++acerto_dh;
                      }
              }
              if(estate==2){
                ++acerto_um;
                      if(acerto_um>9){
                        ++acerto_dm;
                      }
              }
      
      estate2 = 1;
  } else {

      digitalWrite(ledPin, LOW);  
  }

  if(estate2>=1){
    digitalWrite(ledstatePin2, HIGH);
    estate2 = 0;
    memled2 = memled2+1;
  }

  if (memled2>=2){
    memled2 = 0;
     digitalWrite(ledstatePin2, LOW);
  }
 
// rtc.adjust(DateTime(ano, mes, dia, hora, minuto, segundo));

}




void setup () 
{
 
  Serial.begin(9600);
  delay(1000); // wait for console opening

     // interrupt service routine.
  btn1.begin(A0);
  btn2.begin(A1);
  btn3.begin(A2);

   
 
  pinMode(ledPin, OUTPUT);
  pinMode( ledstatePin, OUTPUT);
  digitalWrite(ledstatePin, LOW);
  pinMode( ledstatePin1, OUTPUT);
  digitalWrite(ledstatePin1, LOW);
  pinMode( ledstatePin2, OUTPUT);
  digitalWrite(ledstatePin2, LOW);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tick, FALLING);

   /* display an image of bitmap matrix */
  SH1106_begin();
  SH1106_clear(oled_buf);
  SH1106_bitmap(0, 0, Waveshare12864, 128, 64, oled_buf);
  SH1106_display(oled_buf);
  delay(2000);
  SH1106_clear(oled_buf);

  SH1106_char3216(0, 16, numero[0], oled_buf);
  SH1106_char3216(16, 16, '0', oled_buf);
  SH1106_char3216(32, 16, ':', oled_buf);
  SH1106_char3216(48, 16, '0', oled_buf);
  SH1106_char3216(64, 16, '0', oled_buf);
  SH1106_char3216(80, 16, ':', oled_buf);
  SH1106_char3216(96, 16, '0', oled_buf);
  SH1106_char3216(112, 16, '0', oled_buf);

  SH1106_display(oled_buf); 



  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }


  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, lets set the time!");
  
  // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //fica com a hora que é compilado 

        for (mode_index=0 ; mode_index < 3 ; mode_index++){
          rtc.writeSqwPinMode(modes[mode_index]);
        }
  
    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
   // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }
    
     DateTime now = rtc.now();


    int h = now.hour();
    unidade_de_horas=(h%10);
    dezena_de_horas=((h-(h%10))/10);

    int m =now.minute();
    unidade_de_minutos=(m%10);
    dezena_de_minutos=((m-(m%10))/10);

    int s =now.second();
    contador=(s%10);
    dezena_de_segundos=((s-(s%10))/10);


    delay(100);

    // print_mode();

    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //fica com a hora que é compilado 
  
}






void loop () 
{
  // quando prime o botão1(A0) (manter premido durante algum tempo efeito "Debounce")  ligado ao pino A0
  // acende o led amarelo ligado ao pino 3 mas apaga de seguida - indica que o botão foi premido com sucesso
  //o led vermelho ligado ao pino 5 fica ceso ate ser premido novamente o botão 1 efeito ON-OFF
 if (btn1.debounce()) {
     digitalWrite(ledPin,  HIGH);
     delay(100);
      estate = 1;
     
  } else {
      digitalWrite(ledPin, LOW);     
  }

  if(estate>=1){
    digitalWrite(ledstatePin, HIGH);
    memled = memled+1;     
    acerto_dh = numero[dezena_de_horas];
    acerto_uh = numero[unidade_de_horas];
    acerto_dm = numero[dezena_de_minutos];
    acerto_um = numero[unidade_de_minutos];

    // LOOP de acerto do relogio
    while (!btn1.debounce())
    {
    digitalWrite(ledstatePin1, HIGH);
    digitalWrite(ledPin, LOW);  
     
    //Chama a Rotina para acertar Horas
     acerto_relogio();
    }

    SH1106_clear(oled_buf);
    SH1106_display(oled_buf); 
    digitalWrite(ledstatePin1, LOW);
    estate = 0;
    delay(1000);
  }

  if (memled>=2){
    memled = 0;
    digitalWrite(ledstatePin, LOW);
  

    SH1106_clear(oled_buf);
    SH1106_display(oled_buf); 
    delay(1000);

      // rtc.adjust(DateTime(ano, mes, dia, hora, minuto, segundo));
  }


   if (contador >9){
     ++dezena_de_segundos;

        if( dezena_de_segundos > 5 ){
              ++unidade_de_minutos;

                 if(unidade_de_minutos>9){
                   ++dezena_de_minutos;

                        if(dezena_de_minutos>5){
                          ++unidade_de_horas;

                           if(unidade_de_horas>3){ 
                               if((dezena_de_horas>=2)){ 
                                 unidade_de_horas=0;
                                 dezena_de_horas=0;
                               }
                                if(unidade_de_horas>9){
                                    unidade_de_horas=0;
                                    ++dezena_de_horas;
                                  }
                           }
                          dezena_de_minutos=0;}
                   unidade_de_minutos=0;}  
              dezena_de_segundos=0;}
     contador=0;}
     
SH1106_char3216(0, 16, numero[dezena_de_horas], oled_buf);
SH1106_char3216(16, 16, numero[unidade_de_horas], oled_buf);
SH1106_char3216(32, 16, ':', oled_buf);
SH1106_char3216(48, 16,numero[dezena_de_minutos], oled_buf);   
SH1106_char3216(64, 16, numero[unidade_de_minutos], oled_buf);
SH1106_char3216(80, 16, ':', oled_buf);
SH1106_char3216(96, 16, numero[dezena_de_segundos], oled_buf);
SH1106_char3216(112, 16, numero[contador], oled_buf);
SH1106_display(oled_buf); 


}

//----------------------FIM do PROGRAMA-----------
//---
//------------NOTAS----------------



  // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
    // rtc.adjust(DateTime(ano, mes, dia, horas, minutos, segundos));
      // DateTime now = rtc.now();
      // Serial.println("Current Date & Time: ");
      // Serial.print((int)now.year(), DEC);
      // Serial.print('/');
      // Serial.print(now.month(), DEC);
      // Serial.print('/');
      // Serial.print(now.day(), DEC);
      // Serial.print(" (");
      // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      // Serial.print(") ");
      // Serial.print(now.hour());
      // Serial.print(':');
      // Serial.print(now.minute(), DEC);
      // Serial.print(':');
      // Serial.print(now.second(), DEC);
      // Serial.println();
      //int ano = 2021;
      // int mes = 12;
      // int dia = 26;
      // int hora = 12;
      // int minuto = 42;
      // int segundo = 0;