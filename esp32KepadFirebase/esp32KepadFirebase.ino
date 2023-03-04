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

#include <Keypad.h>
#include <map>

std::map<String, float> myItems;

#define ROW_NUM     4 // four rows
#define COLUMN_NUM  4 // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'2', '5', '6', 'B'},
  {'3', '8', '9', 'C'},
  {'4', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM]      = {19,18,5,17}; // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
byte pin_column[COLUMN_NUM] = {16,4,2,15};   // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// Insert your network credentials
#define WIFI_SSID ".............."
#define WIFI_PASSWORD "............."

// Insert Firebase project API Key
#define API_KEY ".........................."

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL ".................................." 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
int intValue;
float floatValue;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


void WriteDataFirebase(String item, float newBalance){
  static unsigned long sendDataPrevMillisWrite = 0; // separate variable for WriteDataFirebase
  String uid= "wMfLuU4BBWMNnnQEfRL5l7rILPH3";
   
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillisWrite > 15000 || sendDataPrevMillisWrite == 0)){
    sendDataPrevMillisWrite = millis();
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, ("/users/"+uid+"/balance/").c_str() ,float(newBalance) )){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}
}

float ReadData(){
  static unsigned long sendDataPrevMillisRead = 0; // separate variable for ReadData
  String uid= "wMfLuU4BBWMNnnQEfRL5l7rILPH3";
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillisRead > 15000 || sendDataPrevMillisRead == 0)) {
    sendDataPrevMillisRead = millis();
    
    if (Firebase.RTDB.getFloat(&fbdo, ("/users/"+uid+"/balance/").c_str())) {
      if (fbdo.dataType() == "float") {
        return floatValue = fbdo.floatData();
      }
    }
    else {
       return floatValue = 1.0;
      Serial.println(fbdo.errorReason());
    }
  }
  return floatValue; // return the value outside the if block
}

void setup(){
  Serial.begin(115200);
  
  myItems["water"] = 50.0;
  myItems["milk"] = 60.0;
  myItems["kitkat"] = 100.0;
  myItems["dairymilk"] = 150.0;
  
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

void loop(){
  char key = keypad.getKey();
  float balance;
  
  if (key) {
    Serial.println(key);
    switch (key) {
    case '1':  
      Serial.println("Water chosen");
      balance = ReadData();
      balance= balance- myItems["water"];
      Serial.println(balance);
      WriteDataFirebase("water", balance);
      break;
    case '2':  
      Serial.println("Milk chosen");
      balance = ReadData();
      balance= balance-  myItems["milk"];
      Serial.println(balance);
      WriteDataFirebase("milk", balance);
      break;
    case '3':  
      Serial.println("kitkat chosen");
      balance = ReadData();
      balance= balance- myItems["kitkat"];
      Serial.println(balance);
      WriteDataFirebase("kitkat", balance);
      break;
    case '4':  
      Serial.println("dairy milk");
      balance = ReadData();
      balance= balance-  myItems["dairymilk"];
      Serial.println(balance);
      WriteDataFirebase("dairymilk", balance);
      break;
    default:
      Serial.print("invalid input");
  }
     
  }
}
