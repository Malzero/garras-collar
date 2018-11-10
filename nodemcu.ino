#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <SocketIoClient.h>
#include <Ticker.h>


Ticker flipper;

//  VARIABLES
int blinkPin = 15;                // pin to blink led at each beat
int fadePin = 12;                 // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin


// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, must be seeded! 
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
char tiempo[100];
char datos[100];


void event(const char * payload, size_t length) {
	USE_SERIAL.printf("got message: %s\n", payload);
}

void conectado(const char * payload, size_t length) {
	USE_SERIAL.printf("conectado al socket\n", payload);
}

void setup() {
	USE_SERIAL.begin(115200);
	
	mlx.begin();
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for (uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP("nodemcu", "nodemcuv3");

	while (WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	webSocket.on("event", event);
	webSocket.on("connect", conectado);
	webSocket.begin("192.168.43.46", 4000);

	
	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("username", "password");
		   // pin that will fade to your heartbeat!
         // we agree to talk fast!115200
	interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
	 // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE, 
	 // AND APPLY THAT VOLTAGE TO THE A-REF PIN
	 //analogReference(EXTERNAL);   

}

void loop() {
	
	/*
	Serial.print("TO = ");*/
	//Serial.print(mlx.readObjectTempC());
	//Serial.print(",");
	/*
	Serial.print("*C----");
	Serial.print("TA = ");*/
	//Serial.println(mlx.readAmbientTempC());
	//Serial.print(",");
	/*
	Serial.print("*C");
	Serial.println();*/

	sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
	if (QS == true) {                       // Quantified Self flag is true when arduino finds a heartbeat
		fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
		sendDataToProcessing('B', BPM);   // send heart rate with a 'B' prefix
		sendDataToProcessing('Q', IBI);   // send time between beats with a 'Q' prefix
		QS = false;                      // reset the Quantified Self flag for next time    
	}

	//webSocket.emit("DATA", "{\"x\": 10, \"y\" : 11 ,}");// the data to send culminating in a carriage return
	delay(17);
	//webSocket.loop();

}

void sendDataToProcessing(char symbol, int data) {
	//Serial.print(symbol); // symbol prefix tells Processing what type of data is coming
	//itoa((int)(millis() / 1000), tiempo, 10);
	//itoa(data, datos, 10);
	Serial.println(data);     
	
	//Serial.print(analogRead(A0));
}