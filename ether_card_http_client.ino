#include <Keypad.h>
#include <TM1637Display.h>
/*
Nümerik tuş takımından iki sayı alıp sunucuya gönderir. Enter karakteri için "#" kullanılmıştır.
Sonucu sunucudan aldıktan sonra tekrar ekrana yazdırır.
*/
const int CLK = A1; //TM1637Display CLK pini
const int DIO = A2; //TM1637Display DIO pini
TM1637Display display(CLK, DIO); //TM1637Display Oluşturulması
const byte SATIR = 4; //Numeric takımın satir ayarı
const byte SUTUN = 3; //Numeric takımın sütun ayarı
String bir = "";
String iki = "";
int netice = 0;
int durum = 1;

/*Ethernet Bas*/
#include <EtherCard.h>
static byte mymac[] = { 
  0x74,0x69,0x69,0x2D,0x30,0x3B };
byte Ethernet::buffer[700];
static uint32_t timer;
boolean cagir = false;
const char website[] = "192.168.1.244";
byte hisip[] = {
  192,168,1,244};
/*Ethernet Bitis*/

static void getReq (byte status, word off, word len) {
  Ethernet::buffer[off+300] = 0;
  String neticeMhll = (const char*) Ethernet::buffer + off;
  neticeMhll += "bitis";
  String charSet = "<meta charset='UTF-8'/>";
  int charSetBas = neticeMhll.indexOf(charSet); 
  String toplam = neticeMhll.substring(charSetBas + charSet.length(), neticeMhll.indexOf("bitis")  ); 
  netice =   toplam.toInt();

  delay(1000);
  display.showNumberDec(netice);
  Serial.print("Netice: ");
  Serial.println(toplam);
  durumuBasaAl();  
}

char keys[SATIR][SUTUN] = {
  {
    '1', '2', '3'                          }  
  ,
  {
    '4', '5', '6'                          }  
  ,

  {
    '7', '8', '9'                          }  
  ,

  {
    '*', '0', '#'                          }
};

byte rowPins[SATIR] = { 
  5, 4, 3, 2 };
byte colPins[SUTUN] = { 
  8, 7, 6 };

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, SATIR, SUTUN );

void ethernetSetup(){
  Serial.println(F("\n[istemci]"));
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println( "Ethernet controller a ulaşılamıyor!");
  if (!ether.dhcpSetup())
    Serial.println("DHCP'den IP alınamadı.");
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  ether.copyIp(ether.hisip, hisip);
  ether.printIp("SRV: ", ether.hisip);
  display.showNumberDec(0);

}

void setup()
{
  display.setBrightness(0x0a); //Parlaklık ayarı
  Serial.begin(9600);
  ethernetSetup();
}

void sifirla(){
  bir = "";
  iki = "";
  netice = 0;
  durum = 1;
  display.showNumberDec(0);
}

void talepYap(String pBir, String pIki){
  ether.packetLoop(ether.packetReceive());
  int millll = millis();
  if (millll > timer) 
  {
    timer = millll + 5000;
    char urlStr[50];
    String aStupidWasteOfResource = "?sayi1="+ pBir +"&sayi2=" + pIki; 
    //aStupidWasteOfResource = "?sayi1=22&sayi2=15";
    aStupidWasteOfResource.toCharArray(urlStr, 50);
    Serial.print("webAPI....: ");
    Serial.print(String(website));

    Serial.print("/");    
    Serial.println(urlStr);
    ether.browseUrl(PSTR("/"), urlStr, website, getReq);
  }  
}

void durumuBasaAl(){
  durum = 1;
  bir = "";
  iki = "";
  cagir = false;
}

void loop()
{
  if(cagir){
    talepYap(bir, iki);
  }
  char key = kpd.getKey();
  if (key) {

    if(key=='*')
    {
      Serial.println(".: Temizle :.");
      sifirla();
    }
    else
    {
      if (key == '#')
      {
        Serial.println(".:Enter:.: ");
        if (durum == 1) {
          Serial.print("Sayi1...: "); 
          Serial.println(bir);
        }
        if (durum == 2) {
          Serial.print("Sayi2...: "); 
          Serial.println(iki);
        }
        durum++; 

        if (durum >= 3)
        {
          cagir = true;          
        }        
      }
      else
      {
        //Serial.println(key);
        if (durum == 1) {
          bir += key;
          display.showNumberDec(bir.toInt());
        }
        if (durum == 2) {
          iki += key;
          display.showNumberDec(iki.toInt());
        }
      }
    }
    /*
    Serial.print("bir: ");
     Serial.println( bir);
     Serial.print("iki: ");
     Serial.println( iki);*/
  }
}
