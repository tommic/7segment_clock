//We always have to include the library
#include "LedControl.h"
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiManager.h>
#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);



WiFiUDP ntpUDP;
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000); // Winterzeit 
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

/*
DataIn = D7
CLK = D5
LOAD = D4
*/
const int PinDataIn = 13;
const int PinClk = 14;
const int PinLoad = 2;
const int PinButton = 0;
LedControl lc=LedControl(PinDataIn, PinClk, PinLoad, 1);

void setup()
{
  Serial.begin(115200);

  //WiFiManager
  WiFiManager wifiManager;
  //wifiManager.resetSettings();  //reset settings - for testing
  // wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

// Taster
  pinMode(PinButton, INPUT_PULLUP);

  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  dht.begin();
  
  // Initialize the 3 MAX7219 devices
  for(int k=0; k<3; k++){
    lc.shutdown(k,false);  // Enable display
    lc.setIntensity(k,0); // Set brightness level (0 is min, 15 is max)
    lc.clearDisplay(k);    // Clear display register
  }
}
void loop()
{
  if(digitalRead(PinButton) == LOW)
  {
    modeDisplayTemp();
  }
  else
  {
    modeDisplayTime();
  }
  
  delay(100);
  //lc.clearDisplay(0);
}

void modeDisplayTime()
{
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());
  int h = (int) timeClient.getHours();
  int i = (int) timeClient.getMinutes();
  int s = (int) timeClient.getSeconds();
  
  displayTime(h,i,s);
}

void displayTime(int h, int i, int s)
{  
    int h1 = floor(h/10);
    int h2 = h%10;

    int i1 = floor(i/10);
    int i2 = i%10;
    
    int s1 = floor(s/10);
    int s2 = s%10;


     lc.setDigit(0,7,h1, false);  // Decimal point enabled
     lc.setDigit(0,6,h2, false);  // Decimal point enabled
     lc.setChar(0,5,'-', false);  // Decimal point enabled
     lc.setDigit(0,4,i1, false);  // Decimal point enabled
     lc.setDigit(0,3,i2, false);  // Decimal point enabled
     lc.setChar(0,2,'-', false);  // Decimal point enabled
     lc.setDigit(0,1,s1, false);  // Decimal point enabled
     lc.setDigit(0,0,s2, false);  // Decimal point enabled
  
}

void modeDisplayTemp()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    Serial.print('temp: ');
    Serial.println(t);
    Serial.print('feucht: ');
    Serial.println(h);
    
    displayTemp(t,h);
}

void displayTemp(float t, float h)
{  
    int t1 = floor(t/10);
    int t2 = (int)t %10;
    int t3 = floor((t-floor(t))*10);

    int h1 = floor(h/10);
    int h2 = (int) h%10;
    int h3 = floor((h-floor(h))*10);

     lc.setDigit(0,7,t1, false);  // Decimal point enabled
     lc.setDigit(0,6,t2, true);  // Decimal point enabled
     lc.setDigit(0,5,t3, false);  // Decimal point enabled
     lc.setChar(0,4,' ', false);  // Decimal point enabled
     lc.setChar(0,3,' ', false);  // Decimal point enabled
     lc.setDigit(0,2,h1, false);  // Decimal point enabled
     lc.setDigit(0,1,h2, true);  // Decimal point enabled
     lc.setDigit(0,0,h3, false);  // Decimal point enabled
  
}
