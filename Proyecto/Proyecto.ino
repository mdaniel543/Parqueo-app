#include <LiquidCrystal.h>


LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); //16 Columnas 2 filas
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("HOLA");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}
