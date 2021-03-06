#include <LedControl.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LedControl mc = LedControl(51, 53, 52, 1);

float LDR, LDR1, LDR2, LDR3, LDR4, LDR5, LDR6, LDR7, LDR8, LDR9, LDR10, LDR11, LDR12, LDR13, LDR14, LDR15;

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
int buz = 9;
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

int punteroEE; /// donde va la memoria EEProm
int punteroIni; // se logueo y donde esta en la EEProm
bool parqueado;


/*****MOTORES****///

#define IN1  10
#define IN2  11
#define IN3  12
#define IN4  13
int contadorMotor1 = 0;
int contadorMotor2 = 0;
int estado;


// Secuencia de pasos (par máximo)
int paso [4][4] =
{
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};


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
  parqueado = false;
  estado = 1;

  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(34, OUTPUT);

  /*byte dato;
    for (int direccion = 0; direccion < 1024; direccion++) {
    dato = EEPROM.read(direccion);
    Serial1.print(direccion);
    Serial1.print(" = ");
    Serial1.print(dato);
    Serial1.println();
    }*/

  /*for (int i = 0; i <= 50; i++) {
    EEPROM.write(i, 255); /// para resetear la EEProm
    }*/
/*
  for (int i = 0; i <= 15; i++) {
    EEPROM.write(i, 0); /// para resetear la EEProm
    }

    for (int i = 16; i <= 100; i++) {
    EEPROM.write(i, 255); /// para resetear la EEProm
    }*/

  mc.shutdown(0, false);
  mc.setIntensity(0, 15);
  mc.clearDisplay(0);

  actualizarMatrix();

  for (int i = 16; i < 1024; i++) { /// para reconocer el puntero de la EEProm dejando los 16 espacios
    if (255 == EEPROM.read(i)) {
      punteroEE = i;
      break;
    }
  }
  punteroIni = 16;
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
        reservar();
        delay(15);
      } else if (entrada == 'p') { /// ocupar el estacionamiento
        parquearCarro();
        delay(15);
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
      else if (entrada == 's') { /// salida del parqueo
        
          EEPROM.write(EEPROM.read(punteroIni + 1) - 1, 0);
          EEPROM.write(punteroIni + 1, 0);
          revisarLeds();
          while (estado == 1) {
            motor1();
          }
          delay(5000);
          while (estado == 2) {
            motor2();
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Salida del");
          lcd.setCursor(0, 1);
          lcd.print("Parqueo");
        
        actualizarMatrix();
        delay(1000);
      }
    }
  } else if (tenSeconds == 15) {
    if (Serial.available() > 0) {
      entrada = Serial.read();
      if (entrada == 'm') {
        enlistar();
        delay(1000);
      } else if (entrada == 'e') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Salida");
        lcd.setCursor(0, 1);
        lcd.print("Administrador");
        tenSeconds = 12;
      }
    }
  }
  else if (tenSeconds < 10) { /// solo es un contador
    tenSeconds++;
    delay(1000);// despues se agrega el 1000
  }

}


void enlistar() {
  if (punteroEE == 16) {
    return;
  }
  String cadena;
  char aux;
  String lista = "";
  for (int i = 16; i < punteroEE; i++) {
    if ('~' == char(EEPROM.read(i))) {
      lista += EEPROM.read(i - 2);
      lista += " -  ";
      for (int j = i + 1; j < punteroEE; j++) {
        aux = char(EEPROM.read(j));
        if (aux == '*') {
          lista += cadena;
          lista += ",";
          cadena = "";
          break;
        } else {
          cadena += aux;
        }
      }
    }
  }
  Serial.print(lista);
  Serial1.print(lista);
}

void registro() {
  char aux;
  String cadena;
  bool existe = true;
  bool pas_us = true;
  int inservible = 0;
  int limite = 0;
  while (Serial.available()) {
    aux = Serial.read();
    if (existe) {
      limite++;
      cadena += aux;
      delay(100);
      if (aux == '*') {
        if (pas_us) {
          if (limite > 9){
            existe = false;
            Serial1.println(limite);
            continue;
          } 
          
          if (existeUsuario(cadena, inservible)) {
            existe = false;
          } else { // creo un nuevo usuario
            punteroIni = punteroEE;
            EEPROM.write(punteroEE, 1);//numero de veces que ingreso
            punteroEE++;
            EEPROM.write(punteroEE, 0);//numero de parqueo por el momento
            punteroEE++;
            EEPROM.put(punteroEE, '~');// inico de nombre de usuario
            punteroEE++;
            StringToEEPROM(cadena);
            cadena = "";
            pas_us = false;
            limite = 0;
          }
        } else {
          if (limite > 6) existe = false;
          else StringToEEPROM(cadena);
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
    delay(2000);
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
  bool admin = false;
  while (Serial.available()) {
    aux = Serial.read();
    if (existe) {
      cadena += aux;
      if (aux == '*') {
        if (cadena == "admin*") {
          admin = true;
          cadena = "";
          continue;
        }
        if (admin){
          if(cadena != "1234*")admin = false; 
          break;
        }
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
  if (admin) {
    Serial.print("admin");
    lcd.setCursor(0, 0);
    lcd.print("Bienvenido");
    lcd.setCursor(0, 1);
    lcd.print("Administrador");
    tenSeconds = 15;
    delay(500);
  }
  else if (existe) {
    Serial.print("b");
    Serial1.println("bien");
    lcd.setCursor(0, 0);
    lcd.print("login");
    lcd.setCursor(0, 1);
    lcd.print("Exitoso");
    delay(2000);
    int t = EEPROM.read(punteroIni);
    EEPROM.write(punteroIni, t + 1);
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
  for (int i = 16; i < punteroEE; i++) {
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
      } else {
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


void generarToken() {
  int r;
  for (int i = 0; i < 4; i++) {
    tokenConexion += (String)random(0, 9);
  }
  for (int i = 0; i < 2; i++) {
    r = random(65, 68);
    tokenConexion += char(r);
  }
  Serial.print(tokenConexion);
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
          tenSeconds++;
          String reg = "correcto," + (String)EEPROM.read(punteroIni + 1);
          Serial.print(reg);
        } else {
          intentosToken++;
          if (intentosToken == 3) {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Bloqueado");
            Serial.print("bloqueado,0");
            digitalWrite(buz, HIGH);
            escuchar = false;
          } else {
            lcd.setCursor(0, 0);
            lcd.print("Ingreso");
            lcd.setCursor(0, 1);
            lcd.print("Incorrecto");
            Serial.print("incorrecto,0");
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
        Serial.print("cancelado,0");
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

void parquearCarro() {
  bool vacio = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parqueando...");
  int temp =  EEPROM.read(punteroIni + 1);
  if (temp != 0) {

    while (estado == 1) {
      motor1();
    }
    delay(5000);
    while (estado == 2) {
      motor2();
    }
    revisarLeds();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parqueado en ");
    lcd.setCursor(0, 1);
    lcd.print("Reservado: ");
    lcd.setCursor(11, 1);
    lcd.print(temp);
    Serial.print(temp);
    parqueado = true;
    return;
  }
  for (int x = 0; x < 16; x++) {
    if ( EEPROM.read(x) == 0) {
      vacio = true;
      break;
    }
  }
  if (vacio) {
    while (estado == 1) {
      motor1();
    }
    delay(5000);
    while (estado == 2) {
      motor2();
    }
    parqueado = true;
    int regreso = revisarLeds();
    int suma = regreso + 1;
    String envio = (String)suma;
    Serial1.println(regreso + 1);
    EEPROM.write(regreso, 1);
    EEPROM.write(punteroIni + 1, suma);
    Serial.print(envio);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parqueado en");
    lcd.setCursor(7, 1);
    lcd.print(envio);

  } else {
    //No hay espacios vacios
    Serial1.println("Vacio");
    Serial.print("no");
  }
  actualizarMatrix();
}

void reservar() {
  bool vacio = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reservando...");
  delay(1000);
  int x;
  for (x = 0; x < 16; x++) {
    if (EEPROM.read(x) == 0) {
      vacio = true;
      break;
    }
  }
  if (vacio) {
    int suma = x + 1;
    String envio = (String)suma;
    Serial1.println(x + 1);
    EEPROM.write(x, 1);
    EEPROM.write(punteroIni + 1, suma);
    Serial.print(envio);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Reservado en");
    lcd.setCursor(7, 1);
    lcd.print(envio);
  } else {
    Serial1.println("Vacio");
    Serial.print("no");
  }
  actualizarMatrix();
}

int revisarLeds() {
  bool aux = true;
  int vector1[16];
  int vector2[16];
  int retorno = -1;

  LDR = analogRead(A0);
  LDR1 = analogRead(A1);
  LDR2 = analogRead(A2);
  LDR3 = analogRead(A3);
  LDR4 = analogRead(A4);
  LDR5 = analogRead(A5);
  LDR6 = analogRead(A6);
  LDR7 = analogRead(A7);
  LDR8 = analogRead(A8);
  LDR9 = analogRead(A9);
  LDR10 = analogRead(A10);
  LDR11 = analogRead(A11);
  LDR12 = analogRead(A12);
  LDR13 = analogRead(A13);
  LDR14 = analogRead(A14);
  LDR15 = analogRead(A15);

  if (LDR < 600) {
    vector1[0] = 1;
  } else {
    vector1[0] = 0;
  }
  if (LDR1 < 600) {
    vector1[1] = 1;
  } else {
    vector1[1] = 0;
  }
  if (LDR2 < 600) {
    vector1[2] = 1;
  } else {
    vector1[2] = 0;
  }
  if (LDR3 < 600) {
    vector1[3] = 1;
  } else {
    vector1[3] = 0;
  }
  if (LDR4 < 600) {
    vector1[4] = 1;
  } else {
    vector1[4] = 0;
  }
  if (LDR5 < 600) {
    vector1[5] = 1;
  } else {
    vector1[5] = 0;
  }
  if (LDR6 < 600) {
    vector1[6] = 1;
  } else {
    vector1[6] = 0;
  }
  if (LDR7 < 600) {
    vector1[7] = 1;
  } else {
    vector1[7] = 0;
  }
  if (LDR8 < 600) {
    vector1[8] = 1;
  } else {
    vector1[8] = 0;
  }
  if (LDR9 < 600) {
    vector1[9] = 1;
  } else {
    vector1[9] = 0;
  }
  if (LDR10 < 600) {
    vector1[10] = 1;
  } else {
    vector1[10] = 0;
  }
  if (LDR11 < 600) {
    vector1[11] = 1;
  } else {
    vector1[11] = 0;
  }
  if (LDR12 < 600) {
    vector1[12] = 1;
  } else {
    vector1[12] = 0;
  }
  if (LDR13 < 600) {
    vector1[13] = 1;
  } else {
    vector1[13] = 0;
  }
  if (LDR14 < 600) {
    vector1[14] = 1;
  } else {
    vector1[14] = 0;
  }
  if (LDR15 < 600) {
    vector1[15] = 1;
  } else {
    vector1[15] = 0;
  }
  while (aux) {
    LDR = analogRead(A0);
    LDR1 = analogRead(A1);
    LDR2 = analogRead(A2);
    LDR3 = analogRead(A3);
    LDR4 = analogRead(A4);
    LDR5 = analogRead(A5);
    LDR6 = analogRead(A6);
    LDR7 = analogRead(A7);
    LDR8 = analogRead(A8);
    LDR9 = analogRead(A9);
    LDR10 = analogRead(A10);
    LDR11 = analogRead(A11);
    LDR12 = analogRead(A12);
    LDR13 = analogRead(A13);
    LDR14 = analogRead(A14);
    LDR15 = analogRead(A15);

    if (LDR < 600) {
      vector2[0] = 1;
    } else {
      vector2[0] = 0;
    }
    if (LDR1 < 600) {
      vector2[1] = 1;
    } else {
      vector2[1] = 0;
    }
    if (LDR2 < 600) {
      vector2[2] = 1;
    } else {
      vector2[2] = 0;
    }
    if (LDR3 < 600) {
      vector2[3] = 1;
    } else {
      vector2[3] = 0;
    }
    if (LDR4 < 600) {
      vector2[4] = 1;
    } else {
      vector2[4] = 0;
    }
    if (LDR5 < 600) {
      vector2[5] = 1;
    } else {
      vector2[5] = 0;
    }
    if (LDR6 < 600) {
      vector2[6] = 1;
    } else {
      vector2[6] = 0;
    }
    if (LDR7 < 600) {
      vector2[7] = 1;
    } else {
      vector2[7] = 0;
    }
    if (LDR8 < 600) {
      vector2[8] = 1;
    } else {
      vector2[8] = 0;
    }
    if (LDR9 < 600) {
      vector2[9] = 1;
    } else {
      vector2[9] = 0;
    }
    if (LDR10 < 600) {
      vector2[10] = 1;
    } else {
      vector2[10] = 0;
    }
    if (LDR11 < 600) {
      vector2[11] = 1;
    } else {
      vector2[11] = 0;
    }
    if (LDR12 < 600) {
      vector2[12] = 1;
    } else {
      vector2[12] = 0;
    }
    if (LDR13 < 600) {
      vector2[13] = 1;
    } else {
      vector2[13] = 0;
    }
    if (LDR14 < 600) {
      vector2[14] = 1;
    } else {
      vector2[14] = 0;
    }
    if (LDR15 < 600) {
      vector2[15] = 1;
    } else {
      vector2[15] = 0;
    }
    for (int i = 0 ; i < 16; i++) {
      /*Serial1.print(i);
        Serial1.print(". ");
        Serial1.println(vector2[i]);
        Serial1.println("*********");
        Serial1.print(i);
        Serial1.print(". ");
        Serial1.println(vector1[i]);*/
      if (vector1[i] != vector2[i]) {
        aux = false;
        retorno = i;
        break;
      }
    }
  }
  return retorno;
}


void actualizarMatrix() {
  for (int i = 0 ; i < 8; i++) {
    if (1 == EEPROM.read(i)) {
      mc.setLed(0, 0, i, true);
    } else {
      mc.setLed(0, 0, i, false);
    }
  }
  for (int i = 8 ; i < 16; i++) {
    if (1 == EEPROM.read(i)) {
      mc.setLed(0, 1, i - 8, true);
    }
    else {
      mc.setLed(0, 0, i, false);
    }
  }
}

void motor1() {
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(IN1, paso[i][0]);
    digitalWrite(IN2, paso[i][1]);
    digitalWrite(IN3, paso[i][2]);
    digitalWrite(IN4, paso[i][3]);
    delay(500);
    contadorMotor1++;
    if (contadorMotor1 == 20) {
      estado = 2;
      contadorMotor1 = 0;
    }
  }
}
void motor2() {
  digitalWrite(31, LOW);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);
  digitalWrite(34, HIGH);
  delay(500);
  contadorMotor2++;
  digitalWrite(31, LOW);
  digitalWrite(32, LOW);
  digitalWrite(33, HIGH);
  digitalWrite(34, LOW);
  delay(500);
  contadorMotor2++;
  digitalWrite(31, LOW);
  digitalWrite(32, HIGH);
  digitalWrite(33, LOW);
  digitalWrite(34, LOW);
  delay(500);
  contadorMotor2++;
  digitalWrite(31, HIGH);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);
  digitalWrite(34, LOW);
  delay(500);
  contadorMotor2++;
  if (contadorMotor2 == 20) {
    estado = 1;
    contadorMotor2 = 0;
  }
}
