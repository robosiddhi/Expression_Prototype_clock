/*
Code Name: Expression Clock
Author: RoboSiddhi

⚠ Warning

This project and its code are shared for learning and personal use only.  
Commercial use, redistribution, or selling (in any form) of this code or
project is not allowed without proper permission.  

                                                                                 .                 
              !^           .   ..   ..            
              ??^~~~^.      ....:....             
             :P5J?7?JY5.   ...^:::^..             
            .BG5JY75&JYG   ...^:::^..             
            .BBBP57?YYYG.   ....::::              
             :YBBGYJ??Y?   .  .^ YGGY.            
              :!BBBBY!:       .G??5BP^            
            ~GP5P5??77!7:    .^J55Y!              
           ^#57!7#5777!?57~:~77JG~                
           ?#G5PB#57777!B5GG5YPJ.                 
          .BGYY&&BPY???J!!Y5Y7:                   
          :7!~!7777!!!!^     
                               
Description: Expression Clock is an simple prototype. Which change the display 
acording to the user expression and also give a reminder to drink water and 
different types of works.It uses Microcontroller,Oled display and a Touchsensor.
Great for
learning robotics, DIY projects, and basic STEM applications.
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // common for most 0.96" OLEDs

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ----- pins -----
#define TOUCH_PIN 2
#define BUZZER_PIN 8   // optional

// ----- timings -----
const unsigned long AUTO_RETURN_MS = 20000;     // back to clock after 20 s
const unsigned long DEBOUNCE_MS = 400;
const unsigned long REMINDER_MS = 50000;        // 10 s demo

// ----- variables -----
int mode = 0;            // 0 = clock, 1..5 = emojis
int emojiIndex = 1;
bool showingTip = false;

unsigned long lastTouch = 0;
unsigned long lastAction = 0;
unsigned long lastReminder = 0;
unsigned long lastTick = 0;

// software clock
int hh = 12, mm = 0, ss = 0;

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 init failed! Check address/wiring.");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.println("Smart Emotion Clock");
  display.display();
  delay(1500);
  displayClock();
}

void loop() {
  unsigned long now = millis();

  // software clock tick
  if (now - lastTick >= 1000) {
    lastTick = now;
    ss++;
    if (ss >= 60) { ss = 0; mm++; }
    if (mm >= 60) { mm = 0; hh++; }
    if (hh >= 24) hh = 0;
    if (mode == 0) displayClock();
  }

  // read touch
  int touch = digitalRead(TOUCH_PIN);
  if (touch == HIGH && now - lastTouch > DEBOUNCE_MS) {
    lastTouch = now;
    lastAction = now;

    if (mode == 0) {
      mode = 1; emojiIndex = 1; showingTip = false;
      displayEmoji(emojiIndex);
    } else {
      if (!showingTip) { showingTip = true; showTip(); }
      else {
        emojiIndex++; if (emojiIndex > 5) emojiIndex = 1;
        showingTip = false; displayEmoji(emojiIndex);
      }
    }
  }

  // auto return
  if (mode != 0 && now - lastAction > AUTO_RETURN_MS) {
    mode = 0; showingTip = false; displayClock();
  }

  // reminder
  if (now - lastReminder > REMINDER_MS) {
    lastReminder = now;
    showReminder();
  }
}

// -------------------------------------------------

void displayClock() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 18);
  if (hh < 10) display.print('0'); display.print(hh);
  display.print(':');
  if (mm < 10) display.print('0'); display.print(mm);
  display.setTextSize(1);
  display.setCursor(40, 48);
  display.print("Clock Mode");
  display.display();
}

// -------------------------------------------------

void displayEmoji(int idx) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  switch (idx) {
    case 1: drawHappy(); break;
    case 2: drawNeutral(); break;
    case 3: drawSad(); break;
    case 4: drawAngry(); break;
    case 5: drawCool(); break;
  }

  display.setTextSize(1);
  display.setCursor(4, 52);
  display.print("Touch again");
  display.display();
}

void showTip() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  switch (emojiIndex) {
    case 1: display.setCursor(8, 20); display.println("Smile today!"); break;
    case 2: display.setCursor(8, 20); display.println("Take a short walk."); break;
    case 3: display.setCursor(8, 20); display.println("Relax & breathe."); break;
    case 4: display.setCursor(8, 20); display.println("Calm down slowly."); break;
    case 5: display.setCursor(8, 20); display.println("Stay confident!"); break;
  }
  display.display();
}

// -------------------------------------------------

void showReminder() {
  tone(BUZZER_PIN, 1000, 100);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("  Drink  Water           Reminder!");
  display.setCursor(20, 38);
  display.println("Stay hydrated!");
  display.display();
  delay(1500);
  if (mode == 0) displayClock();
}

// -------------------------------------------------

void drawHappy() {
  display.drawCircle(64, 28, 22, SSD1306_WHITE);
  display.fillCircle(54, 22, 3, SSD1306_WHITE);
  display.fillCircle(74, 22, 3, SSD1306_WHITE);
  display.drawLine(52, 38, 76, 38, SSD1306_WHITE);
  display.drawLine(52, 38, 64, 46, SSD1306_WHITE);
  display.drawLine(64, 46, 76, 38, SSD1306_WHITE);
}

void drawNeutral() {
  display.drawCircle(64, 28, 22, SSD1306_WHITE);
  display.fillCircle(54, 22, 3, SSD1306_WHITE);
  display.fillCircle(74, 22, 3, SSD1306_WHITE);
  display.drawLine(52, 42, 76, 42, SSD1306_WHITE);
}

void drawSad() {
  display.drawCircle(64, 28, 22, SSD1306_WHITE);
  display.fillCircle(54, 22, 3, SSD1306_WHITE);
  display.fillCircle(74, 22, 3, SSD1306_WHITE);
  display.drawLine(52, 44, 64, 36, SSD1306_WHITE);
  display.drawLine(64, 36, 76, 44, SSD1306_WHITE);
}

void drawAngry() {
  display.drawCircle(64, 28, 22, SSD1306_WHITE);
  display.drawLine(50, 18, 58, 24, SSD1306_WHITE);
  display.drawLine(78, 18, 70, 24, SSD1306_WHITE);
  display.fillCircle(54, 26, 3, SSD1306_WHITE);
  display.fillCircle(74, 26, 3, SSD1306_WHITE);
  display.drawLine(52, 44, 76, 44, SSD1306_WHITE);
}

void drawCool() {
  display.drawCircle(64, 28, 22, SSD1306_WHITE);
  display.fillRect(52, 18, 24, 6, SSD1306_WHITE);
  display.drawLine(52, 42, 76, 42, SSD1306_WHITE);
}
