// M. Riski Aziz
// Muhammad A.M Hikar Syahrial
// Yusril Iskandar Harahap 

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
// #include <NTPClient.h>

#define WIFI_SSID "wytruii"
#define WIFI_PASSWORD "aa12345678"

#define MQTT_SERVER  "io.adafruit.com"
#define MQTT_PORT    1883
#define IO_USERNAME ""
#define IO_KEY ""

LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo1;

#define IR1 2
#define IR2 4

int Slot = 4; // total number of parking Slots
int nilai_harga = 0;

int flag1 = 0;
int flag2 = 0;

//inisialisasi server adafruit
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, IO_USERNAME, IO_KEY);

//Set up the feed you're publishing to (nodemcu -> adafruit)
Adafruit_MQTT_Publish Jml = Adafruit_MQTT_Publish(&mqtt,IO_USERNAME "/feeds/jml_slot");
Adafruit_MQTT_Publish harga = Adafruit_MQTT_Publish(&mqtt,IO_USERNAME "/feeds/harga");
// Adafruit_MQTT_Publish "--" = Adafruit_MQTT_Publish(&mqtt,IO_USERNAME "/feeds/--");

void setup()
{
  Serial.begin(9600); 
  
   //sambungkan nodemcu dengan wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
  Serial.print("Connecting to Wi-Fi"); 
  
  //loading koneksi dengan ..
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  /* //sambungkan timeClient dengan NTP server
  timeClient.begin(); */

  lcd.init();
  lcd.backlight();
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  
  myservo1.attach(13);
  myservo1.write(100);

  lcd.setCursor (0,0);
  lcd.print(" ITERA SMART ");
  lcd.setCursor (0,1);
  lcd.print(" PARKING SYSTEM ");
  delay (2000);
  lcd.clear();  
}

void loop()
{
  //hubungkan nodemcu dengan MQTT server adafruit
  MQTT_connect();
  
  
  if(digitalRead (IR1) == LOW && flag1 == 0)
  {
    if(Slot > 0)
    {
      flag1 = 1;
      if(flag2 == 0)
      {
        myservo1.write(0);
        Slot = Slot - 1;
        Jml.publish(Slot);
        
        nilai_harga = nilai_harga + 2000;
        harga.publish(nilai_harga);
      }
    }
    else
    {
      lcd.setCursor (0,0);
      lcd.print("    SORRY :(    ");
      lcd.setCursor (0,1);
      lcd.print("  Parking Full. "); 
      delay (3000);
      lcd.clear(); 
    }
  }

  if(digitalRead (IR2) == LOW && flag2 == 0)
  {
    flag2 = 1;
    if(flag1 == 0)
    {
      myservo1.write(0); 
      Slot = Slot + 1;
      Jml.publish(Slot);
  }
}

  if(flag1 == 1 && flag2 == 1)
  {
    delay (1000);
    myservo1.write(100);
    flag1 = 0, flag2 = 0;
  }

  lcd.setCursor (0,0);
  lcd.print("    WELCOME!    ");
  lcd.setCursor (0,1);
  lcd.print("Slot Left: ");
  lcd.print(Slot);
}

void MQTT_connect() 
{
  // Stop jika sudah terkoneksi
  if (mqtt.connected()){
    return;
  }
  mqtt.disconnect();
  mqtt.connect();
}
