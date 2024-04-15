#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h> 
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27,16,2); 

#define moisturePin A0
#define tmpPin A1
#define ledPin 4
#define buttonPin 5
#define servoPin 7

#define RX_PIN 10 // Chân RX của cổng nối tiếp mềm
#define TX_PIN 11 // Chân TX của cổng nối tiếp mềm

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Tạo một đối tượng cổng nối tiếp mềm

Servo myservo;

int sensorValue;
float reading;
float temp;
int signal;

void setup() {
  mySerial.begin(9600);
  Serial.begin(115200);
  lcd.init();                    
  lcd.backlight();

  pinMode(ledPin, OUTPUT);
  pinMode(tmpPin, INPUT);
  pinMode(buttonPin, INPUT);

  myservo.attach(servoPin);
  myservo.write(90);
}

unsigned long time = 0;  
 
void loop() {
  // put your main code here, to run repeatedly
  if(mySerial.available()){
    signal = mySerial.read();
    Serial.println(signal);
    if(signal == 1){
      myservo.write(0);
      turnOnLed();
    }else if (signal == 0){
      myservo.write(90);
     turnOffLed();
    }
  }
  reading = analogRead(tmpPin);
  temp = getTempValue(reading);

  sensorValue = analogRead(moisturePin);
  int moisture = getMoisture(sensorValue);

  displayLCD(temp, moisture);

  if (moisture < 40 && signal == 2)
  {
    myservo.write(0);
    turnOnLed();
  }
  else if(sensorValue >= 40 && signal == 2)
  {
    myservo.write(90);
    turnOffLed();
  }

  String Data = String(temp,1)  + "|" + String(moisture) ;
  mySerial.println(Data); 

  delay(1000);        
}

float getTempValue(float reading) {
  return (reading * 4.88 / 10.0);
}

int getMoisture(int reading){
  return 100 - map(reading, 0, 1023, 0, 100);
}


void turnOnLed()
{
  digitalWrite(ledPin, HIGH);
}

void turnOffLed()
{
  digitalWrite(ledPin, LOW);
}

void displayLCD(float temp, int moisture) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" do C");
  lcd.setCursor(0,1);
  lcd.print("Moisture: ");
  lcd.print(moisture);
  lcd.print(" %");
}


















