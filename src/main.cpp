/**
INCUBADORA TATO V1.0
Usa las siguientes bibliotecas Arduino:
- DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
- Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
- Sin conectividad
**/

#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <DHT.h>
#include <EEPROM.h>

#define SUBE A2
#define BAJA A3
#define LAMPARA 4
#define FAN 5
#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display

void set_temperatura();
void set_humedad();
void sube_baja_humedad_seteada(); 
void sube_baja_temperatura_seteada(); 

float temperatura_seteada = 0.00f;
float humedad_seteada = 0.00f;
float h = 0.00f;
float t = 0.00f;
float f = 0.00f;
float aux = 0.00f;
int show = -1;

int dir_ee_temperatura = 0;
int dir_ee_humedad = 8;
int dir_ee_token = 16;

void setup()
{
  int error;
  float token = 0.0;

  pinMode(LAMPARA, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  Serial.begin(9600);

  // Configura LCD =============================================
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD Ok.");
    show = 0;
    lcd.begin(16, 2); // inicializa el lcd

  } else {
    Serial.println(": LCD no encontrado.");
  } 
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     Control    ");
  lcd.setCursor(0, 1);
  lcd.print("    Incubadora  ");
  delay(3000);

// Verifica EEPROM ================================
  EEPROM.get(dir_ee_token, token);
  if ( token == 12.4 ){
    lcd.setCursor(0, 0);
    lcd.print(" Comprobando..  ");
    lcd.setCursor(0, 1);
    lcd.print(" NVRAM  OK      ");
    EEPROM.get(dir_ee_temperatura, temperatura_seteada);
    EEPROM.get(dir_ee_humedad, humedad_seteada);
 //   temperatura_seteada = 37.5f;
 //   humedad_seteada = 50.0f;
}
  else {
    lcd.setCursor(0, 0);
    lcd.print(" Comprobando..  ");
    lcd.setCursor(0, 1);
    lcd.print(" Fallo en NVRAM ");
    token = 12.4f;
    temperatura_seteada = 37.5f;
    humedad_seteada     = 50.0f;
    EEPROM.put(dir_ee_token, token);
    EEPROM.put(dir_ee_temperatura,temperatura_seteada);
    EEPROM.put(dir_ee_humedad, humedad_seteada);
  }

  delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("Temp. set: ");
    lcd.print(temperatura_seteada);
    lcd.setCursor(0, 1);
    lcd.print("Hum. set:  ");
    lcd.print(humedad_seteada);
  delay(3000);    

  lcd.setCursor(0,0);
  lcd.print("Temperatura:    ");
  lcd.setCursor(0,1);
  lcd.print("Humedad    :    ");
//========================================================

  Serial.println(F("DHTxx test!"));
  dht.begin();
}

void loop() 
{
  if(digitalRead(SUBE)==LOW){
    set_temperatura();  
  }
  if(digitalRead(BAJA)==LOW){
    set_humedad();  
  }
  
   
 
  delay(2000);

  h = dht.readHumidity();
  t = dht.readTemperature();
  t++;
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  lcd.setCursor(12,0);
  lcd.print(t);
  lcd.setCursor(12,1);
  lcd.print(h);
  if(t > temperatura_seteada) digitalWrite(LAMPARA, HIGH);
    else digitalWrite(LAMPARA, LOW);
  if(h > humedad_seteada) digitalWrite(FAN, LOW);
    else digitalWrite(FAN, HIGH);

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));  Serial.print(h);
  Serial.print(F("%  Temperature: ")); Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}
//============================================================
void set_temperatura()
{
  int contador = 0;
  for(register int tiem=0; tiem<100; tiem++){
      delay(50);
      if( digitalRead(SUBE)==LOW )contador++;
      else break;
  }
  if(contador == 100) sube_baja_temperatura_seteada();    
 
}
//============================================================
void sube_baja_temperatura_seteada()
{
  int bucles = 0;

  lcd.setCursor(0,0);
  lcd.print("Setea ->   :");
  lcd.print(temperatura_seteada);
  while(digitalRead(SUBE)==LOW);

  while(bucles<400){
    delay(10);
    bucles++;
    if(digitalRead(BAJA)==LOW) {
      while(digitalRead(BAJA)==LOW);
      bucles=0;
      temperatura_seteada-=0.1;
      lcd.setCursor(12,0);
      lcd.print(temperatura_seteada);
      }
    if(digitalRead(SUBE)==LOW) {
      while(digitalRead(SUBE)==LOW);
      bucles=0;
      temperatura_seteada+=0.1;
      lcd.setCursor(12,0);
      lcd.print(temperatura_seteada);
      }
  }
  EEPROM.put(dir_ee_temperatura, temperatura_seteada);
  EEPROM.get(dir_ee_temperatura,aux);
  if (aux == temperatura_seteada){
  lcd.setCursor(0,0);
  lcd.print("    GUARDADO    ");
    delay(2000);
  }
  
  
  lcd.setCursor(0,0);
  lcd.print("Temperatura:");
  lcd.print(t);

}
//============================================================
void set_humedad()
{
  int contador = 0;
  for(register int tiem=0; tiem<100; tiem++){
      delay(50);
      if( digitalRead(BAJA)==LOW )contador++;
      else break;
  }
  if(contador == 100) sube_baja_humedad_seteada();    

}
//============================================================
void sube_baja_humedad_seteada()
{
  int bucles = 0;

  lcd.setCursor(0,1);
  lcd.print("Setea ->   :");
  lcd.print(humedad_seteada);
  while(digitalRead(BAJA)==LOW);

  while(bucles<400){
    delay(10);
    bucles++;
    if(digitalRead(BAJA)==LOW) {
      while(digitalRead(BAJA)==LOW);
      bucles=0;
      humedad_seteada-=0.1;
      lcd.setCursor(12,1);
      lcd.print(humedad_seteada);
      }
    if(digitalRead(SUBE)==LOW) {
      while(digitalRead(SUBE)==LOW);
      bucles=0;
      humedad_seteada+=0.1;
      lcd.setCursor(12,1);
      lcd.print(humedad_seteada);
      }
  }
  EEPROM.put(dir_ee_humedad, humedad_seteada);
  EEPROM.get(dir_ee_humedad,aux);
  if (aux == humedad_seteada){
  lcd.setCursor(0,1);
  lcd.print("    GUARDADO    ");
    delay(2000);
  }
  lcd.setCursor(0,1);
  lcd.print("Humedad    :");
  lcd.print(h);
}

//============================================================