#include <LiquidCrystal.h>
#include <Key.h>
#include <Keypad.h>

//PRUEBA

// Configuración del teclado
const byte FILAS = 4;     // Filas
const byte COLUMNAS = 4;  // Columnas

int estadoIngre=0;
// Matriz de las teclas
char teclas[FILAS][COLUMNAS] = {
    {'1', '2', '3','A'}, {'4', '5', '6','B'}, {'7', '8', '9','C'}, {'*', '0', '#','D'}};

// Asignación de las teclas a los pines de arduino
byte filaPines[FILAS] = {26, 27, 28, 29};
byte columnaPines[COLUMNAS] = {22, 23, 24, 25};
String clave = "1234";
String usuario= "admin";
// Instancia teclado
Keypad teclado = Keypad(makeKeymap(teclas), filaPines, columnaPines, FILAS, COLUMNAS);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
String contrasena = "";
String usuTempo ="";
int digitosIngresados = 0;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); //16 Columnas 2 filas
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido");
  lcd.setCursor(0, 1);
  lcd.print("Grupo5-SeccionA");
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  seguridad();
}
void seguridad() {
  
  //lcd.clear();
    char tecla = teclado.getKey();
    
    //Serial.print(tecla);
    //Verificamos el estado de ingreso si estamos en usuario o en contra
    // usuario ---- ingreso ===0
    // contra ---- ingreso ===1
    if (tecla) {
        if(estadoIngre==0){
        if(contrasena.length()<1){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Usuario");
            lcd.setCursor(0, 1);
        }
        if(tecla!='*'){
        contrasena += tecla;
        lcd.print(tecla);
        digitosIngresados++;
        delay(15);
        }else{
          usuTempo = contrasena;
        contrasena="";
        estadoIngre=1;
        delay(15);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Codigo");
          lcd.setCursor(0, 1);
        
        }
        }else{
          if(contrasena.length()<1){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Codigo");
            lcd.setCursor(0, 1);
        }
        if(tecla!='*'){
        contrasena += tecla;
        lcd.print('*');
        digitosIngresados++;
        delay(15);
        estadoIngre=3;
        }else{
          if (clave == contrasena) {  // correcto
            contrasena="";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("BIENVENIDO");
            lcd.setCursor(0, 1);
            lcd.print("AL PARQUEO");
            contrasena="";
            delay(200);
           
            //etapa2 = 1;
        } else {  // incorrecto
            contrasena="";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ERROR EN Contrasena");
            lcd.setCursor(2,1); 
            contrasena="";
        }
        contrasena = "";
        digitosIngresados = 0;
        estadoIngre=0;
        delay(100);
        }
          
        }
    }

     if(tecla=='#'){ 
      lcd.clear();
      lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
      lcd.print("CASA ACYE1");
      lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
      lcd.print("A-G4-S2");
      estadoIngre=0;
      contrasena ="";
      delay(20);
    }
}
