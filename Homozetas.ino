#include <DHT.h>
#include <LiquidCrystal.h>

#define BAUDIOS 9600
#define DHTPIN 7
#define DHTTYPE DHT11

unsigned long TIEMPO_RIEGO_ACTIVO = 5000;
unsigned long TIEMPO_RIEGO_ESPERA = 10000;
unsigned long tiempoRiegoActual = 0;

unsigned long TIEMPO_FOCO_ACTIVO = 4000;
unsigned long TIEMPO_FOCO_ESPERA = 10000;
unsigned long tiempoFocoActual = 0;

byte TEMPERATURA_MINIMA = 24;
byte TEMPERATURA_MAXIMA = 28;

byte HUMEDAD_MINIMA = 80;
byte HUMEDAD_MAXIMA = 85;

const byte BOTON_CALEFACCION = 3;
const byte BOTON_GOTEO = 4;
const byte BOTON_FOCO = 8;
const byte BOTON_RIEGO = 9;
const byte BOTON_INICIO = 12;

const byte RELAY_CALEFACCION = 10;
const byte RELAY_GOTEO = 11;
const byte RELAY_FOCO = 8;
const byte RELAY_RIEGO = 9;

const unsigned lond DIA = 86400000;

byte temperaturaActual = 0;
byte humedadActual = 0;

boolean estadoSistema = true;
bool estadoCalefaccion = false;
bool estadoGoteo = false;
bool estadoFoco = false;
bool estadoRiego = false;

byte nuevaHumedadMinima = 255;
byte nuevaHumedadMaxima = 255;
byte nuevaTemperaturaMinima = 255;
byte nuevaTemperaturaMaxima = 255;
unsigned long nuevoFoco = 0;
unsigned long nuevoRiego = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  Serial.begin(BAUDIOS);
  pinMode(RELAY_CALEFACCION, OUTPUT);
  pinMode(RELAY_GOTEO, OUTPUT);
  pinMode(RELAY_FOCO, OUTPUT);
  pinMode(RELAY_RIEGO, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long tiempo = millis();
  if (estadoSistema) {
    temperaturaActual = dht.readTemperature();
    humedadActual = dht.readHumidity();
    if (isnan(temperaturaActual) || isnan(humedadActual)) {
      // Hubo un error al tomar la lectura, problema del sensor
      Serial.println("ERROR AL LEER DEL SENSOR, POR FAVOR, VERIFIQUE");
    }
    Serial.println("TEMPERATURA " + String(temperaturaActual) + " " + String(TEMPERATURA_MINIMA) + " " + String(TEMPERATURA_MAXIMA));
    Serial.println("HUMEDAD " + String(humedadActual) + " " + String(HUMEDAD_MINIMA) + " " + String(HUMEDAD_MAXIMA));
    Serial.println("GOTEO " + String(estadoGoteo));
    Serial.println("CALEFACCION " + String(estadoCalefaccion));
    Serial.println("RIEGO " + String(estadoRiego));
    Serial.println("FOCO " + String(estadoFoco));

    if (temperaturaActual < TEMPERATURA_MINIMA || humedadActual >= HUMEDAD_MAXIMA) {
      // Se enciende calefacción
      estadoGoteo = false;
      estadoCalefaccion = true;
    } else if (temperaturaActual >= TEMPERATURA_MAXIMA || humedadActual < HUMEDAD_MINIMA) {
      // Apaga la calefacción y enciende el Goteo
      estadoCalefaccion = false;
      estadoGoteo = true;
    } else {
      estadoCalefaccion = false;
      estadoGoteo = false;
    }
    digitalWrite(RELAY_CALEFACCION, (estadoCalefaccion) ? HIGH : LOW);
    digitalWrite(RELAY_GOTEO, (estadoGoteo) ? HIGH : LOW);

    // SI ESTÁ PRENDIDO EL RIEGO
    if (estadoRiego) {
      if (tiempo - tiempoRiegoActual >= TIEMPO_RIEGO_ACTIVO) {
        estadoRiego = false;
        tiempoRiegoActual = tiempo;
        digitalWrite(RELAY_RIEGO, LOW);
      }
    } else {
      if (tiempo - tiempoRiegoActual >= TIEMPO_RIEGO_ESPERA) {
        estadoRiego = true;
        tiempoRiegoActual = tiempo;
        digitalWrite(RELAY_RIEGO, HIGH);
      }
    }
    
    // SI ESTÁ PRENDIDO O APAGADO EL FOCO
    if (estadoFoco) {
      if (tiempo - tiempoFocoActual >= TIEMPO_FOCO_ACTIVO) {
        estadoFoco = false;
        tiempoFocoActual = tiempo;
        digitalWrite(RELAY_FOCO, LOW);
      }
    } else {
      if (tiempo - tiempoFocoActual >= TIEMPO_FOCO_ESPERA) {
        estadoFoco = true;
        tiempoFocoActual = tiempo;
        digitalWrite(RELAY_FOCO, HIGH);
      }
    }
    
    // BOTONES
    if (digitalRead(BOTON_CALEFACCION)) {
      delay(150);
      estadoCalefaccion = !estadoCalefaccion;
    }
    if (digitalRead(BOTON_GOTEO)) {
      delay(150);
      estadoGoteo = !estadoGoteo;
    }
    if (digitalRead(BOTON_FOCO)) {
      delay(150);
      estadoFoco = !estadoFoco;
      tiempoFocoActual = tiempo;
    }
    if (digitalRead(BOTON_RIEGO)) {
      delay(150);
      estadoRiego = !estadoRiego;
      tiempoRiegoActual = tiempo;
    }

    // LEE LOS EVENTOS DEL SERIAL ENVIADOS DESDE EL SERVIDOR PARA GUARDAR NUEVOS VALORES O 
    if (Serial.available() != 0) {
      String s = Serial.readString();
      if (s.equals("CALEFACCION")) {
        estadoCalefaccion = !estadoCalefaccion;
      } else if (s.equals("GOTEO")) {
        estadoGoteo = !estadoGoteo;
      } else if (s.equals("FOCO")) {
        estadoFoco = !estadoFoco;
        tiempoFocoActual = tiempo;
      } else if (s.equals("RIEGO")) {
        estadoRiego = !estadoRiego;
        tiempoRiegoActual = tiempo;
      } else {
        byte inicio = s.indexOf(" ");
        byte fin = s.lastIndexOf(" ");
        if (s.startsWith("RIEGO")) {
          nuevoRiego = s.substring(inicio, fin).toInt();
          String intervalo = s.substring(fin);
          switch (intervalo.charAt(intervalo.length() - 1)) {
            case 'H':
              nuevoRiego *= 36000000;
              break;
            case 'M':
              nuevoRiego *= 60000;
              break;
            case 'S':
              nuevoRiego *= 1000;
              break;
          }
        } else if (s.startsWith("TEMPERATURA")) {
          nuevaTemperaturaMinima = s.substring(inicio, fin).toInt();
          nuevaTemperaturaMaxima = s.substring(fin).toInt();
        } else if (s.startsWith("HUMEDAD")) {
          nuevaHumedadMinima = s.substring(inicio, fin).toInt();
          nuevaHumedadMaxima = s.substring(fin).toInt();
        } else if (s.startsWith("FOCO")) {
          nuevoFoco = s.substring(inicio, fin).toInt();
          String intervalo = s.substring(fin);
          switch (intervalo.charAt(intervalo.length() - 1)) {
            case 'H':
              nuevoFoco *= 36000000;
              break;
            case 'M':
              nuevoFoco *= 60000;
              break;
            case 'S':
              nuevoFoco *= 1000;
              break;
          }
        }
      }
    }
    // RESET AL DIA SIGUIENTE SI SE HICIERON CAMBIOS EN LOS TIEMPOS, TEMPERATURA O HUMEDAD
    if (tiempo % DIA == 0) {
      if (nuevoRiego != 0) {
        TIEMPO_RIEGO_ACTIVO = nuevoRiego;
        TIEMPO_RIEGO_ESPERA = DIA - TIEMPO_RIEGO_ACTIVO;
        nuevoRiego = 0;
      }
      if (nuevoFoco != 0) {
        TIEMPO_FOCO_ACTIVO = nuevoFoco;
        TIEMPO_FOCO_ESPERA = DIA - TIEMPO_FOCO_ACTIVO;
        nuevoFoco = 0;
      }
      if (nuevaTemperaturaMinima != 255 && nuevaTemperaturaMinima != 255) {
        TEMPERATURA_MINIMA = nuevaTemperaturaMinima;
        TEMPERATURA_MAXIMA = nuevaTemperaturaMaxima;
        nuevaTemperaturaMinima = nuevaTemperaturaMaxima = 255;
      }
      if (nuevaHumedadMinima != 255 && nuevaHumedadMinima != 255) {
        HUMEDAD_MINIMA = nuevaHumedadMinima;
        HUMEDAD_MAXIMA = nuevaHumedadMaxima;
        nuevaHumedadMinima = nuevaHumedadMaxima = 255;
      }
    }
  } else {
    if (digitalRead(BOTON_INICIO)) {
      delay(100);
      estadoSistema = !estadoSistema;
    }
  }
}
