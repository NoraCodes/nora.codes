/*
 * geiger_counter.pde
 * Arduino (Uno / Adafruit Metro Mini) firmware for converting the MightyOhm Geiger Counter into a dosimeter.
 * Using the MightyOhm counter means this ONLY works for β and γ radiation, NOT α particles.
 * 
 * Connect a 4-character alphanumeric display on an i2c backpack (Adafruit p.n. 2159 with HT16K33) to SCL/SDA (A5/A4).
 * Connect the PULSE output of the Geiger counter to digital I/O pin 3 (PULSE_PIN).
 * Connect an LED to pin 13 (LED_PIN). This is already connected to the onboard LED on most Arduino boards. You can change this
 *  pin if you want to use an external LED. The LED is turned on when a pulse is received and off when the next latch() occurs.
 *  
 * If you have soldered onto the address pads on the i2c backpack, you can change DISPLAY_ADDRESS to the correct address.
 *  Otherwise, leave it be.
 *  
 * The maximum radiation measurable is STEPS_PER_SECOND * 255. You can adjust this, but it currently is operating right on the edge
 *  of what's sustainable as far as memory usage goes.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define LED_PIN 13
#define PULSE_PIN 3
#define DISPLAY_ADDRESS 0x70

#define STEPS_PER_SECOND 42
#define BUFFER_SECONDS 30
#define BUFFER_LEN STEPS_PER_SECOND * BUFFER_SECONDS

volatile byte current = 0;
volatile int value_buffer_position = 0;
volatile byte values[BUFFER_LEN] = {0};
volatile bool has_wrapped = false;
int avg = 0;

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  // Set up I/O
  alpha4.begin(DISPLAY_ADDRESS);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PULSE_PIN, INPUT);

  int interrupt = digitalPinToInterrupt(PULSE_PIN);
  attachInterrupt(interrupt, pulseIsr, RISING);
  
  // Display the introductory text on the character display
  alpha4.clear();
  alpha4.writeDigitAscii(0, 'N');
  alpha4.writeDigitAscii(1, 'O');
  alpha4.writeDigitAscii(2, 'R');
  alpha4.writeDigitAscii(3, 'A');
  alpha4.writeDisplay();
  delay(500);
}

byte num_loops = 0;
void loop() {
  // STEPS times per second
  delay(1000 / STEPS_PER_SECOND);
  // Save the current data
  latch();
  // Twice a second, update the display.
  if (num_loops % (STEPS_PER_SECOND / 2) == 0) {
    average();
  
    if (avg > 9999) {
      avg = 9999;
    }
  
    String avg_str = String(avg);
    alpha4.writeDigitAscii(0, nthDigit(avg, 3));
    alpha4.writeDigitAscii(1, nthDigit(avg, 2));
    alpha4.writeDigitAscii(2, nthDigit(avg, 1));
    alpha4.writeDigitAscii(3, nthDigit(avg, 0));
    alpha4.writeDisplay();
  }
}

void average() {
  float total = 0; // counts per sample
  for (int i = 0; i < BUFFER_LEN; i++) {
    total += values[i];
  }

  noInterrupts();
  // In case we don't have enough samples to fill the whole buffer,
  // extrapolate what we'd expect if getting the same values in the
  // unfilled time.
  if (!has_wrapped) {
    int samples_so_far = (value_buffer_position + 1);
    float avg_per_sample = total / (float)samples_so_far;
    float remaining_samples = (float)(BUFFER_LEN - value_buffer_position);
    total = total + (avg_per_sample * remaining_samples);
  }
  avg = round(total * 2.0);
  interrupts();
}

void latch() {
  noInterrupts();
  digitalWrite(LED_PIN, LOW);
  if (value_buffer_position < BUFFER_LEN) {
    values[value_buffer_position] = current;
    current = 0;
    value_buffer_position += 1;
  } else {
    has_wrapped = true;
    value_buffer_position = 0;
    values[0] = current;
    current = 0;
  }
  interrupts();
}

void pulseIsr() {
  current = current + 1;
  digitalWrite(LED_PIN, HIGH);
}

char nthDigit(int x, int n) {
  static int powersof10[] = {1, 10, 100, 1000, 10000};
  return ((x / powersof10[n]) % 10) + '0';
}
