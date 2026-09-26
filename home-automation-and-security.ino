#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

/* ---------- OLED Configuration ---------- */
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_RESET  -1
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

/* ---------- Pin Assignments ---------- */
#define temp_sens  A0
#define gas_sens   A1
#define pir_sens   A2
#define fire_sens  A3

#define fire_led     1
#define warning_led  13

/* ---------- Motors ---------- */
#define m1_clock     9
#define m1_anticlock 10
#define m2_clock     11
#define m2_anticlock 12

/* ---------- Keypad ---------- */
const byte ROWS = 4;
const byte COLS = 3;

bool accessGranted = false;
unsigned long accessTime = 0;
const unsigned long ACCESS_TIMEOUT = 10000; // 10 seconds

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {5, 6, 7, 8};
byte colPins[COLS] = {4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* ---------- Password ---------- */
String password = "1234";
String input = "";

/* ================================================== */
void setup() {
  pinMode(fire_led, OUTPUT);
  pinMode(warning_led, OUTPUT);

  pinMode(m1_clock, OUTPUT);
  pinMode(m1_anticlock, OUTPUT);
  pinMode(m2_clock, OUTPUT);
  pinMode(m2_anticlock, OUTPUT);

  pinMode(pir_sens, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);

  showBootScreen();
}

/* ================================================== */
void loop() {

  /* ---------- Sensor Read ---------- */
  int tempRaw = analogRead(temp_sens);
  float temperatureC = ((tempRaw * 5.0 / 1023.0) - 0.5) * 100.0;

  int gas  = analogRead(gas_sens);
  int fire = analogRead(fire_sens);
  int pir  = digitalRead(pir_sens);

  /* ---------- OLED Update ---------- */
  updateOLED(temperatureC, gas, pir, fire, input);

  /* ---------- FIRE Detection ---------- */
  if (fire > 450) {
    digitalWrite(fire_led, HIGH);
    digitalWrite(warning_led, HIGH);
    openDoor();
    windowOpen();
  } else {
    //digitalWrite(fire_led, LOW);
  }

  /* ---------- PIR Intrusion Logic ---------- */
  if (pir == HIGH) {
    if (!accessGranted || (millis() - accessTime > ACCESS_TIMEOUT)) {
      digitalWrite(warning_led, HIGH);
      displayAlert("ALERT!!!");
    }
  }

  /* ---------- GAS Detection ---------- */
  if (gas > 450) {
    digitalWrite(warning_led, HIGH);
    windowOpen();
  } else {
    //digitalWrite(warning_led, LOW);
  }

  /* ---------- Keypad ---------- */
  char key = keypad.getKey();

  if (key) {
    if (key == '*') {
      input = "";
    } else {
      input += key;
    }

    if (input.length() == password.length()) {
      if (input == password) {
        displayAlert("AUTHORIZED");
        accessGranted = true;
        accessTime = millis();
        digitalWrite(warning_led, LOW);
        openDoor();
        delay(500);
        closeDoor();
      } else {
        displayAlert("WRONG PIN");
        digitalWrite(warning_led, HIGH);
      }
      input = "";
    }
  }
}

/* ================================================== */
/* OLED FUNCTIONS */

void updateOLED(float t, int g, int p, int f, String keyInput) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.println("HOME SECURITY SYSTEM");
  display.println("--------------------");
  display.print("TEMP: ");
  display.print(t, 1);
  display.println(" C");
  display.print("GAS : ");
  display.println(g > 450 ? "Gas Leak" : "No Gas");
  display.print("PIR : ");
  display.println(p ? "Motion" : "Clear");
  display.print("FIRE: ");
  display.println(f > 450 ? "YES" : "NO");
  display.print("KEY : ");
  display.println(keyInput);
  display.display();
}

void displayAlert(String msg) {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.println(msg);
  display.display();
  delay(1500);
}

void showBootScreen() {
  display.clearDisplay();
  display.setCursor(10, 20);
  display.setTextSize(2);
  display.println("SYSTEM ON");
  display.display();
  delay(1500);
}

/* ================================================== */
/* MOTOR FUNCTIONS */

void openDoor() {
  digitalWrite(m1_clock, HIGH);
  digitalWrite(m1_anticlock, LOW);
}

void closeDoor() {
  digitalWrite(m1_clock, LOW);
  digitalWrite(m1_anticlock, HIGH);
}

void windowOpen() {
  digitalWrite(m2_clock, HIGH);
  digitalWrite(m2_anticlock, LOW);
}

void windowClose() {
  digitalWrite(m2_clock, LOW);
  digitalWrite(m2_anticlock, HIGH);
}