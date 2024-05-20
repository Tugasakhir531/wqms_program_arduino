#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// define rx tx 
#define RXp2 16
#define TXp2 17

// Insert your network credentials
#define WIFI_SSID "WQMS"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDMvshvjpAT2Di9qMOe1lc1iVCP1Q--fgM"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://wqms-81d27-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;


 int tds,kekeruhan;
 float ph;
 int suhu;
//variabel array 
String arrData[4];


// millis sebagai pengganti delay 
unsigned long previousMillis = 0 ;
const long interval = 2000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

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
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
void loop() {
  

  unsigned long currentMillis = millis(); 
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis ;
    String data2 = ""; 
    while(Serial2.available() > 0){

      data2 += char(Serial2.read());

    }
    // Serial.print(data2);

    data2.trim();

    if(data2 != ""){
      int index = 0 ;
      for (int i=0; i<=data2.length();i++){
        char delimiter = '#';

        if(data2[i] != delimiter ){
          arrData[index] += data2[i];
        }
        else{
          index++;
        }
      }

      if (index == 3){
        suhu = arrData[0].toInt();
        tds = arrData[1].toInt();
        ph = arrData[2].toFloat();
        kekeruhan = arrData[3].toInt();

        Serial.print("suhu : ");
        Serial.println(suhu);
        Serial.print("PH : ");
        Serial.println(ph);
        Serial.print("TDS : ");
        Serial.println(tds);
        Serial.print("Kekeruhan : ");
        Serial.println(kekeruhan);
        Serial.println("-------------------------------");
       
      }    
    }
    // minta data ke arduino 
    Serial2.println("Ya");
  }


   if (Firebase.ready() && signupOK ) {
    
          if (Firebase.RTDB.setInt(&fbdo, "WQMS/suhu",suhu)){
            Serial.print("suhu: ");
            Serial.println(suhu);
            
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
          
          // Write an Float number on the database path WQMS/ph
          if (Firebase.RTDB.setFloat(&fbdo, "WQMS/ph", ph)){
            Serial.print("ph: ");
            Serial.println(ph);
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }

          // send tds value
          if (Firebase.RTDB.setInt(&fbdo, "WQMS/tds", tds)){
            Serial.print("tds: ");
            Serial.println(tds);
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }

          if (Firebase.RTDB.setInt(&fbdo, "WQMS/kekeruhan", kekeruhan)){
            Serial.print("kekeruhan: ");
            Serial.println(kekeruhan);
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
        }
      arrData[0] = "";
      arrData[1] = "";
      arrData[2] = "";
      arrData[3] = "";

}