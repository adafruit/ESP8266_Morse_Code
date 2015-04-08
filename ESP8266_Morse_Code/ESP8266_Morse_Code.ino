// ESP8266 Morse Code Webserver Example
// Display a web page with a form and blink out the submitted
// message in morse code on an LED!
// Author: Tony DiCola
// License: MIT License
//
// Note you need to have a LED connected to pin 13 to see the
// morse code messages.

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "index_html.h"


// Details of AP to connect to on boot. 
const char* ssid = "tony-2.4";
const char* password = "101tdicola";

// Pin connected to the LED.
const int led = 13;

// Global web server instance.
ESP8266WebServer server(80);

// Number of milliseconds for a dot in morse code.
const int morse_dot_ms = 100;
// Dash is 3 times the length of a dot.
const int morse_dash_ms = morse_dot_ms*3;
// Delay between parts of a character is the length of a dot.
const int morse_char_delay_ms = morse_dot_ms;
// Delay between letters in a word is the length three dots.
const int morse_letter_delay_ms = morse_dot_ms*3;
// Delay between words is seven dots.
const int morse_word_delay_ms = morse_dot_ms*7;

// Signal to turn the LED on and off.  By default the code
// expects a HIGH signal is LED on and LOW signal is LED off,
// but you can reverse things if your circuit is different.
const int led_on  = HIGH;
const int led_off = LOW;

// Table of character to morse code mapping.
// Each string represents the dashes and dots for a character.
const char* morse_codes[36] = {
  ".-",     // A
  "-...",   // B
  "-.-.",   // C
  "-..",    // D
  ".",      // E
  "..-.",   // F
  "--.",    // G
  "....",   // H
  "..",     // I
  ".---",   // J
  "-.-",    // K
  ".-..",   // L
  "--",     // M
  "-.",     // N
  "---",    // O
  ".--.",   // P
  "--.-",   // Q
  ".-.",    // R
  "...",    // S
  "-",      // T
  "..-",    // U
  "...-",   // V
  ".--",    // W
  "-..-",   // X
  "-.--",   // Y
  "--..",   // Z
  "----",   // 0
  ".----",  // 1
  "..---",  // 2
  "...--",  // 3
  "....-",  // 4
  ".....",  // 5
  "-....",  // 6
  "--...",  // 7
  "---..",  // 8
  "----."   // 9
};


void setup(void)
{
  Serial.begin(115200);
  
  // Setup LED as an output and turn it off.
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // Connect to wifi AP.
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection to finish and print details.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Configure web server root handler to return the main HTML page.
  // Note the funny []() { .. code .. } syntax is a C++11 lambda function.
  server.on("/", []() {
    // Server up the main HTML page.
    server.send(200, "text/html", index_html);
  });
  
  // Configure a handler for the /morse endpoint.
  server.on("/morse", []() {
    // Blink out the message in morse code, then respond with the root 
    // HTML page again.
    blink_morse(led, server.arg("message").c_str());
    server.send(200, "text/html", index_html);
  });
  
  server.begin();
  Serial.println("Morse code HTTP server started!");
}
 
void loop(void)
{
  // Take care of handling any web server requests.
  server.handleClient();
} 

void blink_morse_char(int led_pin, char c) {
  // Blink out the morse code for the specified character.
  // Unrecognized characters are ignored.

  // First look up the string of morse code for the character.
  const char* code = NULL;
  if ((c >= 'A') && (c <= 'Z')) {
    // Get the code for an alphabet character.
    code = morse_codes[c-'A'];
  }
  else if ((c >= '0') && (c <= '9')) {
    // Get the code for a number.
    code = morse_codes[c-'0'+26];
  }
  else {
    // Unknown character, ignore it!
    return;
  }
  
  // Now loop through the components of the code and display them.
  // Be careful to not delay after the last character.
  for (int i = 0; i < strlen(code)-1; ++i) {
     // Turn the LED on, delay for the appropriate dot / dash time,
     // then turn the LED off again.
     digitalWrite(led_pin, led_on);
     delay(code[i] == '.' ? morse_dot_ms : morse_dash_ms);
     digitalWrite(led_pin, led_off);
     // Delay between characters.
     delay(morse_char_delay_ms);
  }
  // Handle the last character without any delay after it.
  digitalWrite(led_pin, led_on);
  delay(code[strlen(code)-1] == '.' ? morse_dot_ms : morse_dash_ms);
  digitalWrite(led_pin, led_off);
}

void blink_morse(int led_pin, const char* message) {
  // Blink out the morse code version of the message on the LED.
  
  // Process each character in the message and send them out as morse code.
  // Keep track of the previously seen character to find word boundaries.
  char old = toupper(message[0]);
  blink_morse_char(led_pin, old);
  for (int i = 1; i < strlen(message); ++i) {
    char c = toupper(message[i]);
    // Delay for word boundary if last char is whitespace and new char
    // is alphanumeric.
    if (isspace(old) && isalnum(c)) {
      delay(morse_word_delay_ms); 
    }
    // Delay for letter boundary if both last and new char are alphanumeric.
    else if (isalnum(old) && isalnum(c)) {
      delay(morse_letter_delay_ms);
    }
    // Else do nothing for other character boundaries.
    
    // Blink out the current character and move on to the next.
    blink_morse_char(led_pin, c);
    old = c;
  }
}

