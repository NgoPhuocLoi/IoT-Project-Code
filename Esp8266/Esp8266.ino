#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#include <Firebase_ESP_Client.h>

#define WIFI_SSID "Wifi."
#define WIFI_PASSWORD "123456789"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDsjIVc8t7bAefJ2F4h9Bx-fBg-v2mxGmU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://iot-project-4aade-default-rtdb.asia-southeast1.firebasedatabase.app"

#define RX_PIN 4 // D2
#define TX_PIN 5 // D1

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

SoftwareSerial mySerial(RX_PIN, TX_PIN);

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

String temp = "";
String moisture = "";
int servoSignal;

void setup(void){
  Serial.begin(115200);
  mySerial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  // config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.charAt(i) == separator || i == maxIndex) {
          found++;
          strIndex[0] = strIndex[1] + 1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
      }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/Servo")) {
      if (fbdo.dataType() == "int") {
        servoSignal = fbdo.intData();
        mySerial.write(servoSignal);
        Serial.println(servoSignal);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  
  }

  if (mySerial.available()) { // Nếu có dữ liệu từ cổng nối tiếp mềm
      
    String data = mySerial.readString(); // Đọc chuỗi dữ liệu
    Serial.println(data); // In ra chuỗi dữ liệu
    temp = getValue(data, '|', 0); // Tách phần tử thứ 0 theo ký tự phân cách ';'
    moisture = getValue(data, '|', 1); // Tách phần tử thứ 1 theo ký tự phân cách ';'
    Serial.println("Temperature:"+temp); // In r phần tử thứ 0
    Serial.println("Humidity:"+moisture); // In ra phần tử thứ 1
    int a;
    float b;
    sscanf(moisture.c_str(), "%d", &a);
    sscanf(temp.c_str(), "%f", &b);

    if (Firebase.ready() && signupOK){
        if (Firebase.RTDB.setInt(&fbdo, "Moisture/", a)){
          Serial.println("PASSED");
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }

        if (Firebase.RTDB.setFloat(&fbdo, "Temp/", b)){
          Serial.println("PASSED");
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
    }
  }
}
