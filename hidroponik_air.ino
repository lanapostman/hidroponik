#include <SoftwareSerial.h>
SoftwareSerial espSerial(2,3);//TX,RX
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

 

#define ONE_WIRE_BUS 4
#define SENSOR A0
int val;
static float sensorph;
static float kekeruhan, hasil;
static float teg;
int adcPH;
float voltage, voltage1, turbidity;
byte pHValue;
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches, kotak, ketinggian;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);



void setup(void) {
  // put your setup code here, to run once:
  // Start serial communication for debugging purposes
  
  // Start up the library
  sensors.begin();
  lcd.backlight();
  lcd.begin();
  Serial.begin (9600);
  pinMode(SENSOR,INPUT);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
}

void loop(void) {
  // put your main code here, to run repeatedly:
  tds();
  ph();
  suhu();
  ketinggian_air();
  
  lcd.clear();
    int sensorValue = analogRead(A2);
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
      lcd.print(sensorph);
      lcd.setCursor(0,1);
      lcd.print("Nutrisi:");
      lcd.setCursor(9,1);
      lcd.print(hasil);
      lcd.setCursor(17,1);
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
    
    delay(1000);
}

void tds() {
  val = analogRead(SENSOR);
  teg = val*(5.0/1024);
  kekeruhan = 100.00-(teg/4.16)*100.00;
  hasil = kekeruhan*50;
  
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
