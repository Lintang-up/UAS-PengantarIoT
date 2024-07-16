#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WifiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
WiFiServer server(80);
String header;   
String output5State = "off";
String output4State = "off";
const int output5 = 5;
const int output4 = 4;

//tempalte_id, device_ide dan token dari aplikasi blynk
#define BLYNK_TEMPLATE_ID "TMPL6cDbeN8Pt"
#define BLYNK_TEMPLATE_NAME "Sensor Arus Listrik"
#define BLYNK_AUTH_TOKEN "xSVCA073wIQlIn9QMPb5x0ipveNO16Dj"

//Library Blink
#include <BlynkSimpleEsp8266.h>

//inialisasi Sensor Arus
PZEM004Tv30 SensorArus(12,13); //12=D6 (Tx), 13=D7 (Rx)

//Variabel Sensor
float Power, Energy, Voltase, Current, Estimasi;

void setup() {
  Serial.begin(9600);
  WiFi.begin("","");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("WIFI Disconnect!");
//WifiManager
    pinMode(output5, OUTPUT);
    pinMode(output4, OUTPUT);
    digitalWrite(output5, LOW);
    digitalWrite(output4, LOW);
    Serial.print("\n");
    WiFiManager wifiManager;
    wifiManager.autoConnect("ArusListrik");
  }
  Serial.println("\nWIFI Connect!");
  
//koneksi ke blynk
  Blynk.begin(BLYNK_AUTH_TOKEN," "," ");
  Serial.println("BLYNK Terkoneksi\n");
  delay(3000);
}

void loop() {
//WifiManager
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
//Close WifiManager

//Cek Wifi
  WiFi.begin("","");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("WIFI Disconnect!");
  }
 
//baca nilai power (kW)
  Power = SensorArus.power();
  if(isnan(Power))
  {
    Serial.println("\nGagal membaca power");
  }
  else
  {
    Serial.print("\nPower\t : ");
    Serial.print(Power);
    Serial.println("W");
  }

//baca nilai energy (kWh)
  Energy = SensorArus.energy();
  if(isnan(Energy))
  {
    Serial.println("Gagal membaca energy");
  }
  else
  {
    Serial.print("Energy\t : ");
    Serial.print(Energy);
    Serial.println("kWh");
  }

//baca nilai energy voltase (Volt)
  Voltase = SensorArus.voltage();
  if(isnan(Voltase))
  {
    Serial.println("Gagal membaca voltase");
  }
  else
  {
    Serial.print("Voltase\t : ");
    Serial.print(Voltase);
    Serial.println("V");
  }

//baca nilai energy current (A)
  Current  = SensorArus.current();
  if(isnan(Current))
  {
    Serial.println("Gagal membaca current");
  }
  else
  {
    Serial.print("Current\t : ");
    Serial.print(Current);
    Serial.println("A");
  }

//Memberikan Estimasi
 Estimasi = Power * 24 / 1000 * 1,352;
   if(isnan(Estimasi))
  {
    Serial.println("Gagal Menghitung Estimasi");
  }
  else
  {
    Serial.print("Estimasi : Rp ");
    Serial.print(Estimasi);
    Serial.println("/Hari");
  }

// Mengirim data ke Blynk
// Power/Watt, Energy/kWh, Voltase, Current/Ampere
   Blynk.virtualWrite(V0, Power);
   Blynk.virtualWrite(V1, Energy);
   Blynk.virtualWrite(V2, Voltase);
   Blynk.virtualWrite(V3, Current);
   Blynk.virtualWrite(V4, Estimasi);
   Blynk.run();
  
delay(1000);
}
# lp/aruslistrik
