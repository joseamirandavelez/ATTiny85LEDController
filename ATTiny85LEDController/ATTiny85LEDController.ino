/*
  ATtiny85 Button-Controlled LED Strip

  This sketch cycles through different lighting effects
  on a WS2182B (NeoPixel) strip each time a button is pressed.

  REQUIRED LIBRARIES:
  - Adafruit NeoPixel (you should already have this)
*/

#include <Adafruit_NeoPixel.h>

// --- Configuration ---
// PIN ASSIGNMENTS HAVE BEEN SWAPPED
#define LED_PIN 0     // Data pin for LED strip (Physical Pin 5, PB0)
#define BUTTON_PIN 2  // Button pin (Physical Pin 7, PB2)
#define NUM_LEDS 38   // Number of LEDs in your strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- Global Variables for State ---
// We now have 20 effects (0-19)
int effectMode = 0;
const int totalEffects = 21;

// Variables for button debounce
long lastButtonPress = 0;
// Increased delay to handle more button bounce
const long debounceDelay = 150;  // 150 milliseconds

bool effectNeedsReset = true;
const int MASTER_BRIGHTNESS = 40;

void setup() {
  strip.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Set button pin with internal pull-up resistor
  randomSeed(analogRead(A1));         // Seed random number generator

  // POWER SAVING: Brightness is the #1 way to save power.
  strip.setBrightness(MASTER_BRIGHTNESS);
  strip.show();  // Initialize all pixels to 'off'
  turnOn();
}

void loop() {
  // --- Button Reading Logic ---
  if (buttonPressed()) {  // Check if button is pressed
    if (millis() - lastButtonPress > debounceDelay) {
      lastButtonPress = millis();

      // Before switching away from the OFF mode, restore the brightness
      if (effectMode == 0) {
        strip.setBrightness(MASTER_BRIGHTNESS);
      }

      effectMode++;
      if (effectMode >= totalEffects) {
        effectMode = 0;  // Wrap around
      }
      effectNeedsReset = true;  // Signal that the new effect needs to be set up
    }
  }

  // --- Effect Switching Logic ---
  switch (effectMode) {
    case 0:
      turnOff();
      break;
    case 1:
      staticColor(255, 255, 255);
      break;
    case 2:
      rainbowCycle(20);
      break;
    case 3:
      theaterChase(strip.Color(255, 0, 0), 50);
      break;
    case 4:
      colorWipe(50);
      break;
    case 5:
      staticColor(0, 0, 255);
      break;
    case 6:
      cylonScanner(strip.Color(255, 0, 0), 30, 8);
      break;
    case 7:
      breathingEffect(strip.Color(128, 0, 255), 10);
      break;
    case 8:
      theaterChaseRainbow(50);
      break;
    case 9:
      sparkle(strip.Color(255, 255, 255), 50);
      break;
    case 10:
      policeLights(100);
      break;
    case 11:
      comet(30);
      break;
    case 12:
      confetti(10);
      break;
    case 13:
      runningLights(strip.Color(255, 255, 0), 40);  // Yellow
      break;
    case 14:
      strobe(50);
      break;
    case 15:
      flame(55);
      break;
    case 16:
      alternatingColors(strip.Color(255, 0, 0), strip.Color(0, 0, 255), 500);  // Red & Blue
      break;
    case 17:
      fadeInOut(strip.Color(0, 255, 0), 10);  // Green
      break;
    case 18:
      colorTwinkle(50);
      break;
    case 19:
      gradientWipe(strip.Color(255, 0, 0), strip.Color(0, 0, 255), 50);  // Red to Blue
      break;
    case 20:
      bouncingDot(strip.Color(0, 255, 255), 20);  // Cyan
      break;
  }
}

bool buttonPressed() {
  return digitalRead(BUTTON_PIN) == LOW;
}


// --- Animation Functions ---

void turnOn() {
  for (int j = 0; j <= 255; j++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, j, 0));
    }
    strip.show();
  }
  delay(500);
  for (int j = 254; j >= 0; j--) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, j, 0));
    }
    strip.show();
  }
  effectNeedsReset = false;
}

void turnOff() {
  if (effectNeedsReset) {
    // Make all leds red
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    strip.show();
    delay(500);

    // Fade out the current colors
    for (int i = MASTER_BRIGHTNESS; i >= 0; i--) {
      strip.setBrightness(i);
      strip.show();
      delay(15);  // Controls the speed of the fade
    }
    strip.clear();  // Ensure all pixel data is cleared
    strip.show();
    effectNeedsReset = false;
  }
}

void staticColor(uint32_t r, uint32_t g, uint32_t b) {
  if (effectNeedsReset) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
      if (buttonPressed()) { return; }
    }
    strip.show();
    effectNeedsReset = false;
  }
}

void colorWipe(uint8_t wait) {
  uint32_t r = random(0, 255);
  uint32_t g = random(0, 255);
  uint32_t b = random(0, 255);
  if (effectNeedsReset) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
      strip.show();
      if (buttonPressed()) { return; }
      delay(wait);
    }
    effectNeedsReset = true;
  }
}

void rainbowCycle(uint8_t wait) {
  effectNeedsReset = false;

  static uint16_t j_tracker = 0;

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j_tracker) & 255));
    if (buttonPressed()) { return; }
  }
  strip.show();
  j_tracker++;
  if (j_tracker >= 256 * 5) j_tracker = 0;  // Slow down the cycle a bit
  delay(wait);
}


void theaterChase(uint32_t c, uint8_t wait) {
  effectNeedsReset = false;

  static int q_tracker = 0;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();

    strip.clear();

    for (int i = q_tracker; i < strip.numPixels(); i = i + 3) {
      strip.setPixelColor(i, c);
      if (buttonPressed()) { return; }
    }
    strip.show();

    q_tracker++;
    if (q_tracker >= 3) q_tracker = 0;
  }
}

void cylonScanner(uint32_t c, uint8_t wait, int width) {
  effectNeedsReset = false;

  static int position = 0;
  static bool movingForward = true;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();
    
    strip.clear(); 
    
    // Draw the scanner bar, centered on 'position'
    for (int i = 0; i < width; i++) {
      int pixelPos = position + i - (width / 2);
      if(pixelPos >= 0 && pixelPos < strip.numPixels()) {
        strip.setPixelColor(pixelPos, c);
      }
    }
    strip.show();
    
    if (movingForward) {
      position++;
      if (position >= strip.numPixels()) {
        position = strip.numPixels() - 2;
        movingForward = false;
      }
    } else {
      position--;
      if (position < 0) {
        position = 1;
        movingForward = true;
      }
    }
  }
}

void breathingEffect(uint32_t c, uint8_t wait) {
  effectNeedsReset = false;

  static int brightness = 0;
  static bool increasing = true;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();

    if (increasing) {
      brightness++;
      if (brightness >= 150) {
        brightness = 150;
        increasing = false;
      }
    } else {
      brightness--;
      if (brightness <= 0) {
        brightness = 0;
        increasing = true;
      }
    }

    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8) & 0xFF;
    uint8_t b = c & 0xFF;

    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;

    uint32_t newColor = strip.Color(r, g, b);

    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, newColor);
      if (buttonPressed()) { return; }
    }
    strip.show();
  }
}

void theaterChaseRainbow(uint8_t wait) {
  effectNeedsReset = false;
  static uint16_t j_tracker = 0;
  static int q_tracker = 0;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();
    strip.clear();
    for (int i = q_tracker; i < strip.numPixels(); i = i + 3) {
      strip.setPixelColor(i, Wheel((i + j_tracker) % 255));
    }
    strip.show();

    q_tracker++;
    if (q_tracker >= 3) q_tracker = 0;

    j_tracker++;
    if (j_tracker >= 256) j_tracker = 0;
  }
}

void sparkle(uint32_t c, uint8_t wait) {
  if (effectNeedsReset) {
    strip.clear();
    effectNeedsReset = false;
  }

  static long last_update = 0;
  if (millis() - last_update > wait) {
    last_update = millis();
    strip.clear();  // Creates a single jumping pixel
    int pixel = random(strip.numPixels());
    strip.setPixelColor(pixel, c);
    strip.show();
  }
}

void policeLights(uint8_t wait) {
  if (effectNeedsReset) {
    strip.clear();
    effectNeedsReset = false;
  }

  static long last_update = 0;
  static bool state = false;

  if (millis() - last_update > wait) {
    last_update = millis();
    state = !state;

    int half = strip.numPixels() / 2;

    if (state) {  // State 1: Red on left
      for (int i = 0; i < half; i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
        if (buttonPressed()) { return; }
      }
      for (int i = half; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
        if (buttonPressed()) { return; }
      }
    } else {  // State 2: Blue on right
      for (int i = 0; i < half; i++) {
        strip.setPixelColor(i, 0);
        if (buttonPressed()) { return; }
      }
      for (int i = half; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 255));
        if (buttonPressed()) { return; }
      }
    }
    strip.show();
  }
}

void comet(uint8_t wait) {
  effectNeedsReset = false;
  static int head = 0;
  static long last_update = 0;
  const byte tailLength = 5;

  if (millis() - last_update > wait) {
    last_update = millis();

    // Fade all pixels
    for (int i = 0; i < strip.numPixels(); i++) {
      uint32_t color = strip.getPixelColor(i);
      uint8_t r = (uint8_t)(color >> 16);
      uint8_t g = (uint8_t)(color >> 8);
      uint8_t b = (uint8_t)color;
      r = (r <= 10) ? 0 : r - 10;
      g = (g <= 10) ? 0 : g - 10;
      b = (b <= 10) ? 0 : b - 10;
      strip.setPixelColor(i, r, g, b);
      if (buttonPressed()) { return; }
    }

    // Draw head
    strip.setPixelColor(head, strip.Color(255, 255, 255));
    strip.show();

    head++;
    if (head >= strip.numPixels()) {
      head = 0;
    }
  }
}

void confetti(uint8_t wait) {
  effectNeedsReset = false;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();

    // Fade all pixels
    for (int i = 0; i < strip.numPixels(); i++) {
      uint32_t color = strip.getPixelColor(i);
      uint8_t r = ((uint8_t)(color >> 16)) / 2;
      uint8_t g = ((uint8_t)(color >> 8)) / 2;
      uint8_t b = ((uint8_t)color) / 2;
      strip.setPixelColor(i, r, g, b);
      if (buttonPressed()) { return; }
    }

    // Add a new random pixel
    int pos = random(strip.numPixels());
    strip.setPixelColor(pos, Wheel(random(255)));
    strip.show();
  }
}

void runningLights(uint32_t c, uint8_t wait) {
  effectNeedsReset = false;
  static int pos = 0;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();
    strip.clear();
    for (int i = 0; i < 3; i++) {
      strip.setPixelColor((pos + i) % strip.numPixels(), c);
      if (buttonPressed()) { return; }
    }
    strip.show();
    pos++;
    if (pos >= strip.numPixels()) pos = 0;
  }
}

void strobe(uint8_t wait) {
  effectNeedsReset = false;
  static long last_update = 0;
  static bool on = true;

  if (millis() - last_update > wait) {
    last_update = millis();
    if (on) {
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 255));
        if (buttonPressed()) { return; }
      }
    } else {
      strip.clear();
    }
    strip.show();
    on = !on;
  }
}

void flame(uint8_t wait) {
  effectNeedsReset = false;
  static long last_update = 0;

  if (millis() - last_update > wait) {
    last_update = millis();
    for (int i = 0; i < strip.numPixels(); i++) {
      int flicker = random(0, 150);
      int r = 100 + flicker;
      int g = flicker / 2;
      int b = 0;
      strip.setPixelColor(i, strip.Color(r, g, b));
      if (buttonPressed()) { return; }
    }
    strip.show();
  }
}

void alternatingColors(uint32_t c1, uint32_t c2, uint8_t wait) {
  effectNeedsReset = false;
  static long last_update = 0;
  static bool state = false;

  if (millis() - last_update > wait) {
    last_update = millis();
    state = !state;
    for (int i = 0; i < strip.numPixels(); i++) {
      if ((i % 2) == state) {
        strip.setPixelColor(i, c1);
      } else {
        strip.setPixelColor(i, c2);
      }
    }
    strip.show();
  }
}

void fadeInOut(uint32_t c, uint8_t wait) {
  breathingEffect(c, wait);  // This is just breathing, which is a fade in/out loop
}

void colorTwinkle(uint8_t wait) {
  if (effectNeedsReset) {
    strip.clear();
    effectNeedsReset = false;
  }

  static long last_update = 0;
  if (millis() - last_update > wait) {
    last_update = millis();
    strip.setPixelColor(random(strip.numPixels()), Wheel(random(255)));
    strip.show();
    delay(wait);
    strip.clear();
    strip.show();
  }
}

void gradientWipe(uint32_t c1, uint32_t c2, uint8_t wait) {
  if (effectNeedsReset) {
    uint8_t r1 = (uint8_t)(c1 >> 16), g1 = (uint8_t)(c1 >> 8), b1 = (uint8_t)c1;
    uint8_t r2 = (uint8_t)(c2 >> 16), g2 = (uint8_t)(c2 >> 8), b2 = (uint8_t)c2;
    for (int i = 0; i < strip.numPixels(); i++) {
      uint8_t r = r1 + (r2 - r1) * i / (strip.numPixels() - 1);
      uint8_t g = g1 + (g2 - g1) * i / (strip.numPixels() - 1);
      uint8_t b = b1 + (b2 - b1) * i / (strip.numPixels() - 1);
      strip.setPixelColor(i, strip.Color(r, g, b));
      strip.show();
      if (buttonPressed()) { return; }
      delay(wait);
    }
    delay(500);
    for (int i = strip.numPixels() - 1; i >= 0; i--) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
      if (buttonPressed()) { return; }
      delay(wait);
    }
    delay(500);
    effectNeedsReset = true;
  }
}

void bouncingDot(uint32_t c, uint8_t wait) {
  cylonScanner(c, wait, 1);  // This is the same logic as Cylon, just with a different name/color
}

// --- Helper Functions ---

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
