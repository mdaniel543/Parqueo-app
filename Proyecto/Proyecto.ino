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
}

void loop() {
  if (tenSeconds == 11) {
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
  } else if (tenSeconds == 10) {
    delay(1000);
    lcd.clear();
    tenSeconds++;
  } else if (tenSeconds == 12) {
    if (Serial.available() > 0) {
      if (entrada == 'h') {
        lcd.clear();
        digitalWrite(buz, LOW);
        generarToken();
        conectarse();
      }else if(entrada == 'u'){
        tokenConexion = "";
        tokenIngresado = "";
        intentosToken = 0;
        tenSeconds = 11;
        delay(5000);
      }
    }
  }
  else if (tenSeconds == 13) {
    if (Serial.available() > 0) {
      if (entrada == 'n') {
        lcd.clear();
        sesionIniciada();
      }
    }
  } else if (tenSeconds < 10) {
    tenSeconds++;
    delay(1000);
  }
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
          Serial.println('V');
          Serial1.println('V');
          tenSeconds++;
        } else {
          intentosToken++;
          if (intentosToken == 3) {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Bloqueado");
            digitalWrite(buz, HIGH);
            delay(8000);
            escuchar = false;
          } else {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Incorrecto");
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
        digitalWrite(buz, HIGH);
        delay(8000);
        escuchar = false;
      }
      else {
        tokenIngresado += tecla;
        lcd.print(tecla);
      }
    }
  }
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

void sesionIniciada() {

}

void login() {

}

void seguridad() {
  char tecla = teclado.getKey();
  if (tecla) {
    if (estadoIngre == 0) {
      if (contrasena.length() < 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Usuario");
        lcd.setCursor(0, 1);
      }
      if (tecla != '*') {
        contrasena += tecla;
        lcd.print(tecla);
        digitosIngresados++;
        delay(15);
      } else {
        usuTempo = contrasena;
        contrasena = "";
        estadoIngre = 1;
        delay(15);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Codigo");
        lcd.setCursor(0, 1);
      }
    } else {
      if (contrasena.length() < 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Codigo");
        lcd.setCursor(0, 1);
      }
      if (tecla != '*') {
        contrasena += tecla;
        lcd.print('*');
        digitosIngresados++;
        delay(15);
        estadoIngre = 3;
      } else {
        if (clave == contrasena) {  // correcto
          contrasena = "";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("BIENVENIDO");
          lcd.setCursor(0, 1);
          lcd.print("AL PARQUEO");
          contrasena = "";
          delay(200);
          //etapa2 = 1;
        } else {  // incorrecto
          contrasena = "";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("ERROR en Contrasena");
          lcd.setCursor(2, 1);
          contrasena = "";
        }
        contrasena = "";
        digitosIngresados = 0;
        estadoIngre = 0;
        delay(100);
      }
    }
  }
  if (tecla == '#') {
    lcd.clear();
    lcd.setCursor(2, 0);  //Set cursor to character 2 on line 0
    lcd.print("CASA ACYE1");
    lcd.setCursor(2, 1);  //Move cursor to character 2 on line 1
    lcd.print("A-G4-S2");
    estadoIngre = 0;
    contrasena = "";
    delay(20);
  }
}
