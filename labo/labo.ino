#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin = 2;
const int sensorPin = A0;
const int ledPin = 8;

int joystickX = 0;
int joystickY = 0;

int vitesse = 0;
char direction = 'D';
bool page = false;

bool headlightOn = false;
unsigned long nameDisplayTime = 0;
unsigned long previousTime = 0;
unsigned long lastTime = 0;
unsigned long lastButtonPress = 0;

long etudiantID = 2413645;
int lastVitesse = -10;


byte customChar[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b10001,
  0b01110,
  0b00000
};

byte chiffre5[8] = {
  0b00100,
  0b01100,
  0b10100,
  0b11111,
  0b01111,
  0b00100,
  0b00100,
  0b00000,
};

byte chiffre3[8] = {
  0b00100,
  0b01100,
  0b10100,
  0b11111,
  0b01111,
  0b00001,
  0b01110,
  0b00000,
};

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.setCursor(0, 0);
  lcd.print("Audry Noupoue");
  lcd.setCursor(5, 1);
  lcd.write(byte(0));
  lcd.createChar(1, chiffre5);
  lcd.createChar(2, chiffre3);
  lcd.setCursor(14, 1);
  lcd.write(byte(1));
  lcd.setCursor(14, 1);
  lcd.write(byte(2));
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  nameDisplayTime = millis();
  while (millis() - nameDisplayTime < 3000) {}

  lcd.clear();
}

void loop() {
  joystickX = analogRead(A1);
  joystickY = analogRead(A2);

  calculerVitesse();
  calculerDirection();

  switch (page) {
    case true:
      afficherPage2();
      break;
    case false:
      surveillerLuminositeEtPhares();
      break;
  }

  envoyerDonneesSerie();

  if (digitalRead(buttonPin) == LOW && millis() - lastButtonPress > 300) {
    changerPage();
    lastButtonPress = millis();
  }
}

void calculerVitesse() {
  int yValue = analogRead(A2);
  if (yValue > 512 + 50) {
    vitesse = map(yValue, 512, 1023, 0, 120);
  } else if (yValue < 512 - 50) {
    vitesse = map(yValue, 512, 0, 0, -25);
  } else {
    vitesse = 0;
  }
}


void calculerDirection() {
  int angle = map(joystickX, 0, 1023, -90, 90);
  direction = (angle < 0) ? 'G' : 'D';
}

void surveillerLuminositeEtPhares() {
  int sensorValue = analogRead(A0);
  int luminosite = map(sensorValue, 0, 1023, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print("Luminosite: ");
  lcd.print(luminosite);
  lcd.print("%  ");

  if (luminosite < 50) {
    if (!headlightOn && millis() - lastTime >= 5000) {
      digitalWrite(ledPin, HIGH);
      headlightOn = true;
      lastTime = millis();
    }
  } else {
    if (headlightOn && millis() - lastTime >= 5000) {
      digitalWrite(ledPin, LOW);
      headlightOn = false;
      lastTime = millis();
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("Phares: ");
  lcd.print(headlightOn ? "ON  " : "OFF ");
}

void afficherPage2() {
  if (vitesse != lastVitesse) {
    
    lcd.setCursor(0, 0);
    if (vitesse < 0) {
      lcd.print("Recule ");
    } else {
      lcd.print("Avance ");
    }
    lcd.print(abs(vitesse));
    lcd.print(" km/h  ");
  }

  lcd.setCursor(0, 1);
  lcd.print("Direction: ");
  lcd.print(direction);
}

void changerPage() {
  page = !page;
  lcd.clear();
  if (page) {
    afficherPage2();
  } else {
    surveillerLuminositeEtPhares();
  }
}

void envoyerDonneesSerie() {
  long valX = analogRead(A1);
  long valY = analogRead(A2);
  int sys = headlightOn ? 1 : 0;

  unsigned long currutTime = millis();
  if (currutTime - previousTime > 100) {
    Serial.print("etd:");
  Serial.print(etudiantID);
  Serial.print(",x:");
  Serial.print(valX);
  Serial.print(",y:");
  Serial.print(valY);
  Serial.print(",sys:");
  Serial.println(sys);
  previousTime= currutTime;
  }
}
