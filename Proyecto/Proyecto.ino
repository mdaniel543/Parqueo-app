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

  /*byte dato;
    for (int direccion = 0; direccion < 1024; direccion++) {
    dato = EEPROM.read(direccion);
    Serial1.print(direccion);
    Serial1.print(" = ");
    Serial1.print(dato);
    Serial1.println();
    }*/

  /*for (int i = 0; i <= 50; i++) {
    EEPROM.write(i, 255); /// para resetear la eeprom
    }*/

  for (int i = 16; i < 1024; i++) { /// para reconocer el puntero de la eeprom dejando los 16 espacios
    if (255 == EEPROM.read(i)) {
      punteroEE = i;
      break;
    }
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
      Serial1.println(entrada);
      if (entrada == 'l') { // login
        lcd.clear();
        delay(10);
        login();
      } else if (entrada == 'r') { // registro
        lcd.clear();
        delay(10);
        registro();
      }
    }
  } else if (tenSeconds == 13) {  /// ya entro con su usuario
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'h') {  /// va verificar el token de ingreso de nuevo (Porque cuando se logueo ya se mando un token)
        lcd.clear();
        digitalWrite(buz, LOW);
        tokenConexion = "";
        tokenIngresado = "";
        intentosToken = 0;
        generarToken();
        delay(10);
        conectarse();
      } else if (entrada == 'u') { /// salida
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Salida");
        lcd.setCursor(0, 1);
        lcd.print("con Exito");
        tenSeconds = 12;
        delay(2000);
      }
    }
  } else if (tenSeconds == 14) { // cuando ya ingreso
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'a') { // apartar el estacionamiento

      } else if (entrada == 'p') { /// ocupar el estacionamiento

      }
      else if (entrada == 'u') { /// salida
        tenSeconds = 12;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Salida");
        lcd.setCursor(0, 1);
        lcd.print("con Exito");
        delay(2000);
      }
    }

  }
  else if (tenSeconds < 10) { /// solo es un contador
    tenSeconds++;
    delay(10);// despues se agrega el 1000
  }
}

void registro() {
  char aux;
  String cadena;
  bool existe = true;
  bool pas_us = true;
  int inservible = 0;
  while (Serial.available()) {
    aux = Serial.read();
    if (existe) {
      cadena += aux;
      if (aux == '*') {
        if (pas_us) {
          if (existeUsuario(cadena, inservible)) {
            existe = false;
          } else { // creo un nuevo usuario
            EEPROM.write(punteroEE, 1);//numero de veces que ingreso
            punteroEE++;
            EEPROM.write(punteroEE, 0);//numero de parqueo por el momento 0
            punteroEE++;
            EEPROM.put(punteroEE, '~');// inico de nombre de usuario
            punteroEE++;
            StringToEEPROM(cadena);
            cadena = "";
            pas_us = false;
          }
        } else {
          StringToEEPROM(cadena);
        }
      }
    }
  }
  if (existe) {
    Serial.print("b");
    Serial1.println("bien");
    lcd.setCursor(0, 0);
    lcd.print("Registro");
    lcd.setCursor(0, 1);
    lcd.print("Exitoso");
    tenSeconds++;
  }
  else {
    Serial.print("m");
    Serial1.println("mal");
    lcd.setCursor(0, 0);
    lcd.print("Registro");
    lcd.setCursor(0, 1);
    lcd.print("Denegado");
  }
}


void login() {
  char aux;
  String cadena;
  bool existe = true;
  bool pas_us = true;
  int miPuntero = 0;
  while (Serial.available()) {
    aux = Serial.read();
    if (existe) {
      cadena += aux;
      if (aux == '*') {
        if (pas_us) {
          if (!existeUsuario(cadena, miPuntero)) {
            existe = false;
          } else {
            pas_us = false;
            delay(10);
          }
          cadena = "";
        } else {
          if (!suPass(cadena, miPuntero)) {
            existe = false;
          }
        }
      }
    }
  }
  if (existe) {
    Serial.print("b");
    Serial1.println("bien");
    lcd.setCursor(0, 0);
    lcd.print("login");
    lcd.setCursor(0, 1);
    lcd.print("Exitoso");
    tenSeconds++;
  }
  else {
    Serial.print("m");
    Serial1.println("mal");
    lcd.setCursor(0, 0);
    lcd.print("login");
    lcd.setCursor(0, 1);
    lcd.print("Denegado");
  }
}


bool existeUsuario(String user, int &puntero) {
  String cadena;
  char aux;
  for (int i = 0; i < punteroEE; i++) {
    if ('~' == char(EEPROM.read(i))) {
      for (int j = i + 1; j < punteroEE; j++) {
        aux = char(EEPROM.read(j));
        cadena += aux;
        if (aux == '*') {
          Serial1.println(cadena);
          Serial1.println(user);
          break;
        }
      }
      if (cadena == user) {
        puntero = i + cadena.length() + 1; //seria de probar,si va el 1 o no
        punteroIni = i - 2; // me coloco en el bit del usuario
        return true;
      }else {
        cadena = "";
      }
    }
  }
  return false;
}

bool suPass(String pass, int indice) {
  Serial1.println(pass);
  Serial1.println(pass.length());
  Serial1.println(indice);
  String cadena;
  char aux;
  for (int i = indice; i < indice + pass.length(); i++) {
    aux = char(EEPROM.read(i));
    Serial1.println(aux);
    cadena += aux;
    if (aux == '*') {
      Serial1.println(cadena);
      if (cadena == pass)
        return true;
    }
  }
  return false;
}

void StringToEEPROM(String str) {
  byte len = str.length();
  for (int i = 0; i < len; i++)
    EEPROM.write(punteroEE + i, str[i]);
  punteroEE = punteroEE + len;
}

String readStringFromEEPROM(int offset) {
  int len = EEPROM.read(offset);
  int i;
  char data[len + 1];
  for (i = 0; i < punteroEE; i++)
    data[i] = EEPROM.read(offset + i);
  return String(data);
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
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'u') { /// salida
        tenSeconds = 12;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Salida");
        lcd.setCursor(0, 1);
        lcd.print("con Exito");
        delay(2000);
        escuchar = false;
      }
    }
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
          Serial.print("correcto");
          tenSeconds++;
        } else {
          intentosToken++;
          if (intentosToken == 3) {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Bloqueado");
            Serial.print("bloqueado");
            digitalWrite(buz, HIGH);
            escuchar = false;
          } else {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Incorrecto");
            Serial.print("incorrecto");
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
        Serial.print("cancelado");
        digitalWrite(buz, HIGH);
        escuchar = false;
      }
      else {
        tokenIngresado += tecla;
        lcd.print(tecla);
      }
    }
  }
}
