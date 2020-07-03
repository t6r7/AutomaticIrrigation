#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <dht11.h>

#define BLYNK_DEBUG
#define BLYNK_PRINT Serial
#define DHT11PIN 13
#define DHTPOWER 12
#define RELAYPIN 5
#define BUTTONPIN 14
#define SOIL_PIN A0
#define SOIL_POWER 4

dht11 DHT11;

int output_value;
bool buttonPressed;
bool openState;



// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "sE_Tb5VzbHFmEASI1BPkTWdwT9k_5niI";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "mi casa";
char pass[] = "wanda213";

BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
    readSoilMoisture();
    readDHT();
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
}

void setup() {
  // put your setup code here, to run once:
  openState = false;
  buttonPressed = false;
  pinMode(SOIL_POWER, OUTPUT);
  pinMode(DHTPOWER, OUTPUT);
  pinMode(RELAYPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
  digitalWrite(SOIL_POWER, LOW); 
  digitalWrite(DHTPOWER,LOW); 
  digitalWrite(RELAYPIN,LOW);
  Serial.begin(9600);
 
 
  WiFi.begin(ssid, pass);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  
  Blynk.config(auth);  // in place of Blynk.begin(auth, ssid, pass);
  Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
  Serial.println("Connecting to Blynk");
  while (Blynk.connect() == false) {
    // Wait until connected
    Serial.print(".");
  }
  Serial.println("Connected to Blynk server");
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(10000L, myTimerEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  readButton();

}

void readButton(){
    int val = digitalRead(BUTTONPIN);
    if(!val){
      if(!buttonPressed){
        changeState();
        buttonPressed = true;
      }
    }
    else{
      buttonPressed = false;
    }
}

void changeState(){
  if(openState){
    closeRelay();
    openState = false;
  }
  else{
    openRelay();
    openState = true;
  }
}
void openRelay(){
  Serial.println("Opening Relay");
  digitalWrite(RELAYPIN,HIGH);
}

void closeRelay(){
  Serial.println("Closing Relay");
  digitalWrite(RELAYPIN,LOW);
}

int readSoilMoisture(){
  digitalWrite(SOIL_POWER, HIGH);
  delay(500);
  output_value = analogRead(SOIL_PIN);
//  output_value = map(output_value,550,0,0,100);
  Serial.print("Moisture: ");
  Serial.println(output_value);
  digitalWrite(SOIL_POWER, LOW);
}

void readDHT(){
  Serial.println("Reading Humidity and Temp"); 
  digitalWrite(DHTPOWER, HIGH);   
  delay(500);
  int chk = DHT11.read(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
    Serial.println("OK"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("Checksum error"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("Time out error"); 
    break;
    default: 
    Serial.println("Unknown error"); 
    break;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Temperature (°C): ");
  Serial.println((float)DHT11.temperature, 2);
  digitalWrite(DHTPOWER, LOW); 
}
