#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


byte candado[] = {
  B00100,
  B01010,
  B10001,
  B10001,
  B11111,
  B11011,
  B11111,
  B11111
};

int tenSeconds;
int buz = 13;
int led = 10;
char entrada;

/******** teclado ***********/
const byte FILAS = 4;     // Filas
const byte COLUMNAS = 4;  // Columnas
int estadoIngre = 0;
// Matriz de las teclas
char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}
};
// Asignación de las teclas a los pines de arduino
byte filaPines[FILAS] = {26, 27, 28, 29};
byte columnaPines[COLUMNAS] = {22, 23, 24, 25};
String clave = "1234";
String usuario = "admin";
// Instancia teclado
Keypad teclado = Keypad(makeKeymap(teclas), filaPines, columnaPines, FILAS, COLUMNAS);
String contrasena = "";
String usuTempo = "";
int digitosIngresados = 0;
/*******FIN TECLADO**********/

/***TOKEN**///
String tokenConexion;
String tokenIngresado;
int intentosToken;


/*****EEPROM******/

int punteroEE; /// donde va la memoria eeprom
int punteroIni; // se logueo y donde esta en la eeprom

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); //16 Columnas 2 filas
  lcd.createChar(0, candado);
  lcd.clear();
  lcd.home();
  lcd.setCursor(2, 0);
  lcd.write(byte(0));
  lcd.setCursor(3, 0);
  lcd.print("Bienvenido");
  lcd.setCursor(13, 0);
  lcd.write(byte(0));
  lcd.setCursor(1, 1);
  lcd.print("Grupo5-SeccionA");
  Serial.begin(9600);
  Serial1.begin(9600);
  tenSeconds = -1;
  tokenConexion = "";
  tokenIngresado = "";
  randomSeed(analogRead(A0));
  intentosToken = 0;
  pinMode(buz, OUTPUT);
  pinMode(led, OUTPUT);
  
  for(int i = 0; i < 1024; i++){
    if(255 == EEPROM.read(i)) punteroEE = i; 
  }
  Serial1.println(punteroEE);
}

void loop() {
  if (tenSeconds == 11) { ///// Esperando Conexion
    lcd.setCursor(0, 0);
    lcd.print("Esperando");
    lcd.setCursor(0, 1);
    lcd.print("Conexion");
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'C') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Conectado");
        tenSeconds++;
      }
    }
  } else if (tenSeconds == 10) { /// solo me limpia la pantalla una vez
    delay(1000);
    lcd.clear();
    tenSeconds++;
  } else if (tenSeconds == 12) { /// va esperar el login o regristrar
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'l') { // login
        lcd.clear();
        login();
      } else if (entrada == 'r') { // registro
        lcd.clear();
        registro();
      }
    }
  } else if (tenSeconds == 13) {  /// ya entro con su usuario
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'h') {  /// va verificar el token de ingreso de nuevo (Porque cuando se logueo ya se mando un token)
        digitalWrite(buz, LOW);
        tokenConexion = "";
        tokenIngresado = "";
        intentosToken = 0;
        generarToken();
        conectarse();
      } else if (entrada == 'a') { // apartar el estacionamiento

      } else if (entrada == 'p') { /// ocupar el estacionamiento

      }
      else if (entrada == 'u') { /// salida
        tenSeconds = 11;
        delay(2000);
      }
    }
  }
  else if (tenSeconds < 10) { /// solo es un contador
    tenSeconds++;
    delay(1000);
  }
}



void registro() {
  String cadena;
  while(Serial.available()){
    entrada = Serial.read();
    cadena += entrada;
    if(entrada == '*'){
      
      StringToEEPROM(cadena);
    }
  }
}


void login() {

}


void verificarUsuario(){
  
}

void generarToken() {
  int r;
  for (int i = 0; i < 4; i++) {
    tokenConexion += (String)random(0, 9);
  }
  for (int i = 0; i < 2; i++) {
    r = random(65, 68);
    tokenConexion += char(r);
  }
  Serial.println(tokenConexion);
  Serial1.println(tokenConexion);
  lcd.setCursor(0, 0);
  lcd.print("Codigo...");
  lcd.setCursor(0, 1);
}


void conectarse() {
  bool escuchar = true;
  while (escuchar) {
    char tecla = teclado.getKey();
    if (tecla) {
      if (tecla == '*') {
        if (tokenIngresado == tokenConexion) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Ingreso");
          lcd.setCursor(0, 1);
          lcd.print("Correctamente");
          intentosToken = 0;
          tokenIngresado = "";
          escuchar = false;
          Serial.println("correcto");
          tenSeconds++;
        } else {
          intentosToken++;
          if (intentosToken == 3) {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Bloqueado");
            Serial.println("bloqueado");
            digitalWrite(buz, HIGH);
            delay(5000);
            escuchar = false;
          } else {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Incorrecto");
            Serial.println("incorrecto");
            delay(8000);
            lcd.clear();
            tokenIngresado = "";
            lcd.setCursor(0, 0);
            lcd.print("Codigo...");
            lcd.setCursor(0, 1);
            delay(15);
          }
        }
      } else if (tecla == '#') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Ingreso");
        lcd.setCursor(0, 1);
        lcd.print("Cancelado");
        Serial.println("cancelado");
        digitalWrite(buz, HIGH);
        delay(5000);
        escuchar = false;
      }
      else {
        tokenIngresado += tecla;
        lcd.print(tecla);
      }
    }
  }
}

void StringToEEPROM(String str) {
  byte len = str.length();
  for (int i = 0; i < len; i++)
    EEPROM.write(punteroEE + i, str[i]);
  punteroEE = punteroEE + len + 1;
}

String readStringFromEEPROM(int offset) {
  int len = EEPROM.read(offset);
  int i;
  char data[len + 1];
  for (i = 0; i < punteroEE; i++)
    data[i] = EEPROM.read(offset + i);
  return String(data);
}
