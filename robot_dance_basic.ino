// A chatbot-responsive robot that responds to chat commands

#include <ESP32Servo.h>
#include <WiFi.h>
#include <FastLED.h>

// WiFi Setup

const char* ssid = "your SSID";
const char* password = "your password";
const int PORT = 8090;
WiFiServer server(PORT);

// Variable to store the HTTP request
String in_message;
unsigned long currentTime_client = 0;
unsigned long previousTime_client = 0;
unsigned long timeoutTime_client = 1000;

// LED setup
#define NUM_LEDS 9
#define LED_PIN 5
#define LED_PIN_eyes 18
#define NUM_LEDS_eyes 2

uint8_t hue = 0;
CRGB leds[NUM_LEDS];
CRGB leds_eyes[NUM_LEDS_eyes];


// Servo setup
#define SERVO_PIN_1 19
#define SERVO_PIN_2 21
#define SERVO_PIN_3 22
#define SERVO_PIN_4 23

Servo myservo1;  // head
Servo myservo2;  // right arm
Servo myservo3;  // waist
Servo myservo4;  // left arm   

int servo_number_1 = 1;
int servo_number_2 = 2;
int angle_1 = 90;
int angle_2 = 150;
int angle_3 = 90;
int angle_4 = 30;
int angle_servo_3 = 90;
int angle_min = 30;
int angle_max = 150;

void reset_servos(int angle=90) {
    angle_1 = 90;
    angle_2 = 150;
    angle_3 = 90;
    angle_4 = 30;
    myservo1.write(angle);              // 90
    myservo2.write((angle + 60) % 180); // 150
    myservo3.write(angle);              // 90
    myservo4.write((angle - 60) % 180); // 30
}

void setup() {
    Serial.begin(115200);
    delay(200);

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.addLeds<WS2812, LED_PIN_eyes, GRB>(leds_eyes, NUM_LEDS_eyes);
    FastLED.setBrightness(1); // brightness ranges from 0-255
    
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    fill_solid(leds_eyes, NUM_LEDS_eyes, CRGB::Red);
    FastLED.show();
    
    Serial.println();
    Serial.println("===== Robot Dance =====");
    Serial.println();
       
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    fill_solid(leds, NUM_LEDS, CRGB::Blue);
    fill_solid(leds_eyes, NUM_LEDS_eyes, CRGB::Blue);
    FastLED.show();

    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.println(PORT);
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI());

    server.begin();

    // Servo setup
    // Allow allocation of all timers for servos
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);  
    
    myservo1.setPeriodHertz(50);
    myservo2.setPeriodHertz(50);
    myservo3.setPeriodHertz(50);
    myservo4.setPeriodHertz(50); 
    myservo1.attach(SERVO_PIN_1, 500, 2500); 
    myservo2.attach(SERVO_PIN_2, 500, 2500);
    myservo3.attach(SERVO_PIN_3, 500, 2500);
    myservo4.attach(SERVO_PIN_4, 500, 2500);
    
    reset_servos();
    delay(500);
    myservo2.write(90);
    myservo4.write(90);
    delay(500);
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    fill_solid(leds_eyes, NUM_LEDS_eyes, CRGB::Green);
    FastLED.show();
    delay(1000);
    reset_servos();
    FastLED.clear();
    FastLED.show();
}


void loop() {
    WiFiClient client = server.available();   // Listen for incoming clients
    
    if (client) {
        currentTime_client = millis();
        previousTime_client = currentTime_client;
        Serial.println("\nNew client");

        while (client.connected() && currentTime_client - previousTime_client < timeoutTime_client) {
            if (client.available()) {
                char c = client.read();
                in_message += c;
            }
            currentTime_client = millis();
        }
        
        client.write("Robot Dance received");
        client.stop();
        Serial.println("Client disconnected");

        Serial.print("Dance number: ");
        Serial.println(in_message);
        Serial.println();
        
        int spaceIndex = in_message.indexOf(' '); // find the index of the space character
        if (spaceIndex == -1) {
            if (in_message == "reset"){
                reset_servos();
            }
            else if (in_message == "1"){
                dance_1();
            }
            else {
                Serial.println("Invalid dance command");
            }
        }
        else {
            String servoNumberString = in_message.substring(0, spaceIndex); // extract the servo number substring
            String angleString = in_message.substring(spaceIndex + 1); // extract the angle substring
            int servo_number = servoNumberString.toInt(); // convert servo number substring to int
            int angle = angleString.toInt(); // convert angle substring to int
            if (angle > 180) {
                angle = 180;
            } else if (angle < 0) {
                angle = 0;
            }

            if (servo_number == 1) {
                myservo1.write(angle);
            } else if (servo_number == 2) {
                myservo2.write(angle);
            } else if (servo_number == 3) {
                myservo3.write(angle);
            } else if (servo_number == 4) {
                myservo4.write(angle);
            } else {
                Serial.println("Invalid servo number. Please enter a number between 1 and 4.");
            }
        }
        
        in_message = "";
    }
}


// random dancing
void dance_1() {
    Serial.println("Dance 1 started - random");
    int num_beats = 32;
    for (int i=0; i<num_beats; i++){
        angle_1 = random(angle_min, angle_max);
        angle_2 = random(angle_min, angle_max);

        servo_number_1 = random(1, 5);
        while (servo_number_2 == servo_number_1){
            servo_number_2 = random(1, 5);
        }

        switch (servo_number_1) {
            case 1:
                myservo1.write(angle_1);
                break;
            case 2:
                myservo2.write(angle_1);
                break;
            case 3:
                angle_servo_3 = map(angle_1, angle_min, angle_max, 70, 110);
                myservo3.write(angle_servo_3);
                break;
            case 4:
                myservo4.write(angle_1);
                break;
        }
        
        switch (servo_number_2) {
            case 1:
                myservo1.write(angle_2);
                break;
            case 2:
                myservo2.write(angle_2);
                break;
            case 3:
                angle_servo_3 = map(angle_2, angle_min, angle_max, 70, 110);
                myservo3.write(angle_servo_3);
                break;
            case 4:
                myservo4.write(angle_2);
                break;
        }

        hue = random(0, 255);
        for (int i = 0; i < NUM_LEDS; i++) {
            hue = hue+10;
            leds[i] = CHSV(hue, 255, 255);
        }
        FastLED.show();

        delay(343);
    }
    FastLED.clear();
    FastLED.show();

    reset_servos();

    Serial.println("Dance 1 done");
}