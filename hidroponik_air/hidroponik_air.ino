#include <SoftwareSerial.h>
SoftwareSerial espSerial(2, 3);//TX,RX
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

 
#include <EEPROM.h>
#include "GravityTDS.h"

#define TdsSensorPin A0
GravityTDS gravityTds;

#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 0
#define Offset -6.75              //deviation compensate
unsigned long int avgValue;

float temperature = 25,tdsValue = 0;
#define ONE_WIRE_BUS 4
//#define SENSOR A0
int val;
static float sensorph;
static float kekeruhan, hasil;
static float teg;
int adcPH;
float voltage, voltage1, turbidity;
//byte pHValue;
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches, kotak, ketinggian;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

String ssid="AB";
String password ="maulana99";


boolean DEBUG=true;
void showResponse(int waitTime) {
  long t=millis();
  char c;
  while (t+waitTime>millis()) {
    if (espSerial.available()) {
      c=espSerial.read();
      if (DEBUG) {
        Serial.print(c);
      }
    }
  }
}

boolean dataWrite() {
  String cmd = "AT+CIPSTART=\"TCP\",\""; // TCP connection
  cmd += "192.168.100.5"; // hostname
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
    
  String getStr = "GET /simpan1/simpan.php?data1="; 
  getStr +=sensors.getTempCByIndex(0);
  getStr +="&data2=";
  getStr += sensorph;
  getStr +="&data3=";
  getStr += tdsValue;
  getStr +="&data4=";
  getStr += ketinggian;
  getStr += " HTTP/1.1\r\nHost:192.168.100.5\r\n\r\n";
  
 
  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">") || espSerial.find("OK") || espSerial.find("CONNECT")) {
    espSerial.print(getStr);
    if (DEBUG) Serial.print(getStr);
  }
  else {
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG) Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}


void setup() {
  DEBUG=true; // enable debug serial
  Serial.begin(115200);
  espSerial.begin(115200); // Baud untuk esp
  espSerial.println("AT+RST"); 
  showResponse(1000);
  //espSerial.println("AT+UART_DEF=115200,8,1,0,3"); 
  //showResponse(1000);
  espSerial.println("AT+CWMODE=1"); // untuk mode client nya
  showResponse(1000);
  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\""); 
  showResponse(1000);
  if (DEBUG) Serial.println("Setup completed");
  // put your setup code here, to run once:
  // Start serial communication for debugging purposes
  
  // Start up the library
  sensors.begin();
  lcd.backlight();
  lcd.begin();

  
  //pinMode(SENSOR,INPUT);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
}

void loop() {
  // put your main code here, to run repeatedly:
  tds();
  ph();
  suhu();
  ketinggian_air();
  
  lcd.clear();
  // pH Sensor
  // put your main code here, to run repeatedly:
  int buf[10];                //buffer for read analog
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        int temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue + Offset; 
  
    /*int sensorValue = analogRead(A2);
    voltage = sensorValue * (5.0 / 1023.0);
    sensorph = voltage * (14.000 / 3.148);

    /*voltage1=0.004888*analogRead(SENSOR);  //in V
    turbidity=-1120.4*voltage1*voltage1+5742.3*voltage1-4352.9;  //in NTU
    if((voltage1>=2.5)&(turbidity>=0))
    {
      
    }*/

    

    
    lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("PH     :");
      lcd.setCursor(9,0);
      lcd.print(phValue, 2);
      lcd.setCursor(0,1);
      lcd.print("Nutrisi:");
      lcd.setCursor(9,1);
      lcd.print(tdsValue);
      lcd.setCursor(16,1);
      lcd.print("PPM");
      lcd.setCursor(0,2);
      lcd.print("Suhu   :");
      lcd.setCursor(9,2);
      lcd.print(sensors.getTempCByIndex(0));
      lcd.setCursor(15,2);
      lcd.print("C");
      lcd.setCursor(0,3);
      lcd.print("Ketinggian:");
      lcd.setCursor(12,3);
      lcd.print(ketinggian);
      lcd.setCursor(15,3);
      lcd.print("CM");
    dataWrite();
    delay(1000);
}

void tds() {
  //temperature = readTemperature();  //add your temperature sensor and read it
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate 
  tdsValue = gravityTds.getTdsValue();  // then get the value
  
}

void ph() {
  
}

void suhu() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
}

void ketinggian_air() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  kotak = 50;
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  ketinggian = 50 - cm;
}
