#include <DHT.h>
#include <LiquidCrystal.h>

#define BAUDIOS 9600
#define DHTPIN 7
#define DHTTYPE DHT11

const unsigned long TIEMPO_RIEGO_ACTIVO = 5000;
const unsigned long TIEMPO_RIEGO_ESPERA = 10000;
unsigned long tiempoRiegoActual = 0;

const unsigned long TIEMPO_FOCO_ACTIVO = 4000;
const unsigned long TIEMPO_FOCO_ESPERA = 10000;
unsigned long tiempoFocoActual = 0;

const byte TEMPERATURA_MINIMA = 24;
const byte TEMPERATURA_MAXIMA = 28;

const byte HUMEDAD_MINIMA = 80;
const byte HUMEDAD_MAXIMA = 85;

const byte BOTON_CALEFACCION = 3;
const byte BOTON_GOTEO = 4;
const byte BOTON_FOCO = 8;
const byte BOTON_RIEGO = 9;
const byte BOTON_INICIO = 12;

const byte RELAY_CALEFACCION = 10;
const byte RELAY_GOTEO = 11;
const byte RELAY_FOCO = 8;
const byte RELAY_RIEGO = 9;

byte temperaturaActual = 0;
byte humedadActual = 0;

boolean estadoSistema = true;
bool estadoCalefaccion = false;
bool estadoGoteo = false;
bool estadoFoco = false;
bool estadoRiego = false;

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
    if (estadoRiego) {
      if(tiempo - tiempoRiegoActual > TIEMPO_RIEGO_ACTIVO){
        estadoRiego = false;
        tiempoRiegoActual = tiempo;
        digitalWrite(RELAY_RIEGO, LOW);
      }
    } else {
      if (tiempo - tiempoRiegoActual > TIEMPO_RIEGO_ESPERA) {
        estadoRiego = true;
        tiempoRiegoActual = tiempo;
        digitalWrite(RELAY_RIEGO, HIGH);
      }
    }
    if (estadoFoco) {
      if(tiempo - tiempoFocoActual > TIEMPO_FOCO_ACTIVO){
        estadoFoco = false;
        tiempoFocoActual = tiempo;
        digitalWrite(RELAY_FOCO, LOW);
      }
    } else {
      if (tiempo - tiempoFocoActual > TIEMPO_FOCO_ESPERA) {
        estadoFoco = true;
        tiempoFocoActual = tiempo;
        digitalWrite(RELAY_FOCO, HIGH);
      }
    }
  } else {
    if (digitalRead(BOTON_INICIO)) {
      delay(100);
      estadoSistema = !estadoSistema;
    }
  }
}
