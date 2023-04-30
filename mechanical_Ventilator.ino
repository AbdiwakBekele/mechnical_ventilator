#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Stepper.h>
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.  


//  Variables
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value.

//Serial Communication Port Assign
#define BT_SERIAL_RX 0
#define BT_SERIAL_TX 1


//number of steps on your motor
#define STEPS 500

//LCD pin Configuration
const int rs = 13;
const int en = 12;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

// create an instance of the stepper class using the steps and pins
// create an Instace of LCD 
// create an Instance of SoftwareSerial
Stepper stepper(STEPS, 8, 9, 10, 11);

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial BluetoothSerial(BT_SERIAL_RX, BT_SERIAL_TX);
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

int rate = 0;
String valueRead;
int tidalVolume = 0; //for adult
String switchStatus = "OFF";

void setup() {
  BluetoothSerial.begin(9600);
  Serial.begin(9600);
  
  stepper.setSpeed(60);


  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   

  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    //Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Welcome");
  
}

void loop() {

int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now.
delay(20);

if(Serial.available()){
  valueRead = Serial.readString();
  Serial.print(valueRead);

  if(valueRead == "y"){
    switchStatus = "ON";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WELCOME");
    delay(500);
    lcd.clear();
  }
  else if(valueRead == "n"){
    switchStatus = "OFF";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("OFF");
    delay(500);
  }
  else{
    //String Function to Separate Rate and tidalVolume
    String strSplit = valueRead;

    int delimiter, delimiter_1, delimiter_2;
    delimiter = strSplit.indexOf("%");
    delimiter_1 = strSplit.indexOf("%", delimiter + 1);
    delimiter_2 = strSplit.indexOf("%", delimiter_1 +1);
    
    String rateStr = strSplit.substring(delimiter + 1, delimiter_1);
    String volumeStr = strSplit.substring(delimiter_1 + 1, delimiter_2);

//    Serial.print("Rate: ");
//    Serial.println(rate);
//    Serial.print("Tidal Volume:");
//    Serial.println(volume);

    int rate = rateStr.toInt();
    int tidalVolume = volumeStr.toInt(); 
    
  }
}

if(switchStatus == "ON"){
  lcd.setCursor(0,0);

  if (pulseSensor.sawStartOfBeat()) {            
     Serial.println(myBPM);
        lcd.setCursor(0,0);
       lcd.print("BPM: ");
       lcd.print(myBPM);
    }
  
  lcd.setCursor(0,1);
  lcd.print("R: ");
  lcd.print(rate);
  lcd.print(" ");

  lcd.print("T.Vol: ");
  lcd.print(tidalVolume);

  forward(rate, tidalVolume);
  delay(1000);
  backward(rate, tidalVolume);
  delay(1000);
   
}
else if(switchStatus == "OFF"){
  // Stop Working
  //Serial.println("Stop Working");
  
  }
}


void forward(int rate, int volume){
    for(int i=1; i<=20; i++){
    stepper.step(volume);
    delay(rate/4);
    }
  }

void backward(int rate, int volume){
    for(int i=1; i<=20; i++){
    stepper.step(-volume);
    delay(rate/4);
    }
  }



