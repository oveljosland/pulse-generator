#include <Arduino.h>
#include <TimerOne.h>

const char SET_PERIOD_HEADER = 'p';
const char SET_FREQUENCY_HEADER = 'f';
const char SET_PULSE_WIDTH_HEADER = 'w';
const char SET_DUTY_CYCLE_HEADER = 'c';

const int out_pin = 9; // Use pin 11-13 on Arduino Mega

long period = 40; // 40 μs period (25 kHz)
int duty = 512; // Duty is a range from 0 to 1023, 512 is 50% duty cycle

void setup() {
  Serial.begin(9600);
  pinMode(out_pin, OUTPUT);
  Timer1.initialize(period); // Initialise timer1, 1000 μs
  Timer1.pwm(out_pin, duty); // PWM on output pin
  instructions();
}

void loop() {
  process_serial();
}

void process_serial() {
  static long val = 0;

  if (Serial.available()) {
    val = Serial.parseInt(); // Find the first number in the serial buffer
    if (val) {
      char ch = Serial.read();
      switch(ch) {
        case SET_PERIOD_HEADER:
          period = val;
          Serial.print("Setting period to ");
          Serial.print(period);
          Timer1.setPeriod(period);
          Timer1.pwm(out_pin, duty);
          show();
          break;
        case SET_FREQUENCY_HEADER:
          if(val > 0) {
            Serial.print("Setting frequency to ");
            Serial.print(val);
            period = 1000000 / val;
            Timer1.setPeriod(period);
            Timer1.pwm(out_pin, duty);
          }
          show();
          break;
        case SET_PULSE_WIDTH_HEADER:
          if(val < period && val > 0) {
            long width = val;
            Serial.print("Setting pulse width to ");
            Serial.print(width);
            duty = map(width, 0, period, 0, 1023);
            Timer1.pwm(out_pin, duty);
          } else {
            Serial.print("Pulsewidth too long for current period");
            show();
            break;
          }
        case SET_DUTY_CYCLE_HEADER:
          if(val > 0 && val < 100) {
            Serial.print("Setting duty cycle to ");
            Serial.print(val);
            duty = map(val, 0, 99, 0, 1023);
            Timer1.pwm(out_pin, duty);
            show();
          }
      }
    }
  }
}

void instructions() {
  Serial.println("Send values followed by one of the following characters:");
  Serial.println(" p - Sets period in microseconds");
  Serial.println(" f - Sets frequency in Hz");
  Serial.println(" w - Sets pulse width in microseconds");
  Serial.println(" c - Sets duty cycle in percent");
  Serial.println("\n(duty cycle can have one decimal place)\n");
}

void show() {
  Serial.print("The period is ");
  Serial.print(period);
  Serial.print("Duty cycle is ");
  Serial.print(map(duty, 0, 1023, 0, 99));
  Serial.print("%");
  Serial.println();
}