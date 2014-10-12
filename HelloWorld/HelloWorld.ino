
#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <WiFi.h>

rgb_lcd lcd;

char ssid[] = "David"; //  your network SSID (name) 
char pass[] = "hellohello";    // your network password (use for WPA, or use as key for WEP)
IPAddress server(172,20,10,7);
WiFiClient client;
int status = WL_IDLE_STATUS;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;
const int buttonPin = 4;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int potentiometer = A0; 
  String type = "";
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
boolean pressed = false;
void setup() 
{
  Serial.begin(9600);    // set the serial communication frequency at 9600 bits per sec


  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

  } 
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
 

  pinMode(potentiometer, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setRGB(colorR, colorG, colorB);

  // Print a message to the LCD.
   client.connect(server, 4000);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT); 
  lcd.print("Workout Type:");
  delay(100);

}

void loop() 
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:


  buttonState = digitalRead(buttonPin);
  int value = analogRead(potentiometer);

  if (value > 500) {
    type = "Hammer";
    lcd.print("Hammer Curl  ");
  } 
  else {
    type = "Dumbbell";
    lcd.print("Dumbbell Curl");
  }  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    if (pressed) {
      digitalWrite(ledPin, HIGH);
      client.println(type + "Start");
    } 
    else {
      
      client.println(type + "End");
      digitalWrite(ledPin, LOW);
    }
    pressed = !pressed;  
  } 
  else {
  }
  delay(100);


}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/*********************************************************************************************************
 * END FILE
 *********************************************************************************************************/

