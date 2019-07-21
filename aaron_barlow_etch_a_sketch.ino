// Include low level code for Adafruit color display (by Adafruit)
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

// Input pins
const int x_pent = A0;
const int y_pent = A1;
const int reset_btn = 4;

// Color LCD output pins
const int cs_pin = 10;
const int reset_pin = 9;
const int dc_pin = 8;

// Red LED variables
const int red_pin = 6;
const int blink_delay = 500;
unsigned long blink_last_on = 0;
boolean led_on = false;

// Tracking pen values
int x_pos = 0;
int y_pos = 0;
uint16_t pen_color = ST7735_BLACK;
boolean erase = false;
const int delay_before_erase = 4000;
unsigned long erase_last_update = 0;

// Initialize the colorful LCD screen display
Adafruit_ST7735 tft = Adafruit_ST7735(cs_pin,  dc_pin, reset_pin);

void setup() {
  Serial.begin(9600);
  // Set the pin mode of the reset button and red LED
  pinMode(reset_btn, INPUT_PULLUP);
  pinMode(red_pin, OUTPUT);
  // Initialize the 1.44" tft ST7735S chip with a white background
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST7735_WHITE);
}

void loop() {
  // When the etch a sketch's state is in erase
  if (erase) {
    // Perform erase actions
    erase_sketch();
  } else { // Otherwise, we are drawing
    // Allow the user to sketch
    draw_sketch();
  }
}

// Map the potentiometer x and y values to valid coordinates on 
// The TFT display and then add a black pixel at the coordinates.
void draw_sketch() {
  int x_value = analogRead(x_pent);
  int y_value = analogRead(y_pent);

  int new_x_pos = map(x_value, 0, 1023, 0, tft.width());
  int new_y_pos = map(y_value, 0, 1023, 0, tft.height());

  x_pos = (new_x_pos + x_pos) / 2;
  y_pos = (new_y_pos + y_pos) / 2;

  // Draw a pixel at the given coordinates with the color black
  tft.drawPixel(x_pos, y_pos, pen_color);

  // Get the state of the reset button
  boolean reset_value = digitalRead(reset_btn);
  // When the button has been pressed (I use input_pullup)
  if (reset_value == LOW) {
    erase = true;
    blink_last_on = millis();
    erase_last_update = millis();
    // Turn on the LED on by default
    digitalWrite(red_pin, HIGH);
    // Prevent misreadings
    delay(350);
  }
}

// Erase the sketch after 3 seconds. That is, if the user does not change
// Their mind by pressing the reset button again within the alloted time.
// Also, during the erasing purgatory, have a blinking LED indicate to
// The user, that their beautiful is about to be meet everlasting oblivion.
void erase_sketch() {
  // When the LED's blinking state needs to change
  if (millis() - blink_delay > blink_last_on) {
    // Change the state of the LED
    led_on = !led_on;
    // Update the last blink timestamp
    blink_last_on = millis();
  } 
  
  // When the user has had 4 seconds to change their mind
  if (millis() - delay_before_erase > erase_last_update) {
    // Clear the screen
    tft.fillScreen(ST7735_WHITE);
    // We are done erasing now
    erase = false;
    // Ensure the red LED is off
    led_on = false;
  }

  // Get state of the reset button
  boolean reset_value = digitalRead(reset_btn);
  // When the button has been pressed (I use input_pullup)
  if (reset_value == LOW) {
    // Prevent erasing, the user changed their mind
    erase = false;
    // Ensure the red LED is off
    led_on = false;
    // Prevent misreadings
    delay(350);
  }
  
  // When the LED is set to on
  if(led_on) {
    // Ensure it is on
    digitalWrite(red_pin, HIGH);
  } else { // When the LED is off
    // Ensure it is off
    digitalWrite(red_pin, LOW);
  }
}

