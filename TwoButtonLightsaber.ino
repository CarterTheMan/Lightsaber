#include <FastLED.h>
#include <EEPROM.h>

#define LED_PIN     3
#define POWER_PIN  4
#define COLOR_PIN 5

#define BRIGHTNESS  64
#define NUM_LEDS    250
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

// Setup all the colors
typedef struct {
  unsigned int color;    // EEPROM 0
  unsigned int r;      // EEPROM 1
  unsigned int g;      // EEPROM 2
  unsigned int b;      // EEPROM 3
} colorDictionary;
const colorDictionary colors[] {
    {0, 255, 0, 0},     // Red
    {1, 255, 127, 0},   // Orange
    {2, 255, 255, 0},   // Yellow
    {3, 0, 255, 0},     // Green
    {4, 0, 0, 255},     // Blue
    {5, 148, 0, 211},   // Purple
    {6, 255, 255, 255}  // White
};
int numColors = 7;

CRGB leds[NUM_LEDS];
bool activated = false;
int ledsPerFrame = 1;

const int ledPin = 13;       // Pin number for the LED (built-in LED on most Arduino boards)

void setup() {
  pinMode(POWER_PIN, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(COLOR_PIN, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);          // Turn off the LED initially

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  unsigned int off[3] = {0, 0, 0};
  setWholeSaberToColor(off);
}

void loop() {
  // If the power button may have been pressed
  if (digitalRead(POWER_PIN) == LOW) {
    delay(100);   // Debounce to wait for double_check

    // If that change is correct
    if (digitalRead(POWER_PIN) == LOW) {
      digitalWrite(ledPin, HIGH);  // Turn on the LED
      
      if (activated) {
          DeactivateSaber();
      }
      else {
          ActivateSaber();
      }
    }

  // If the color button may have been pressed
  } else if (digitalRead(COLOR_PIN) == LOW) {
    delay(100);   // Debounce to wait for double_check

    // If that change is correct
    if (digitalRead(COLOR_PIN) == LOW) {
      digitalWrite(ledPin, LOW);  // Turn on the LED

      if (activated) {
        changeColor();  
      }
    }
  }
}

void setWholeSaberToColor(unsigned int color[3]) {
    for (int led = 0; led < NUM_LEDS; led++) {
      leds[led] = CRGB( color[0], color[1], color[2]);
    }
    FastLED.show();
}

void ActivateSaber() {
  unsigned int r = EEPROM.read(1);
  unsigned int g = EEPROM.read(2);
  unsigned int b = EEPROM.read(3);
  for (int led = 0; led <= NUM_LEDS / 2; led += ledsPerFrame) {
      for (int i = 0; i <= led; i++) {
          leds[i] = CRGB(r, g, b);
          leds[NUM_LEDS-i] = CRGB(r, g, b);
      }
      FastLED.show();
  }
  activated = true;
}

void DeactivateSaber() {
  for (int led = 0; led <= NUM_LEDS / 2; led += ledsPerFrame) {
      for (int i = 0; i <= led; i++) {
          leds[NUM_LEDS/2-i] = CRGB(0, 0, 0);
          leds[NUM_LEDS/2+i] = CRGB(0, 0, 0);
      }
      FastLED.show();
  }
  activated = false;
}

void changeColor() {
  // Get the next color in the sequence
  int color = EEPROM.read(0);
  color = color + 1;
  if (color >= numColors) {
    color = 0;
  }
  unsigned int r = colors[color].r;
  unsigned int g = colors[color].g;
  unsigned int b = colors[color].b;
  
  // Save everything for that color
  EEPROM.write(0, color);
  EEPROM.write(1, r);
  EEPROM.write(2, g);
  EEPROM.write(3, b);
  
  // Update the color
  unsigned int colorChange[3] = {r, g, b};
  setWholeSaberToColor(colorChange);
}
