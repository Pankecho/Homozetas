#include <DHT.h>
#include <LiquidCrystal.h>

#define BAUDIOS 9600
#define DHTPIN 7
#define DHTTYPE DHT11

const byte BOTON_CALEFACCION = 2;
const byte BOTON_GOTEO = 3;
const byte BOTON_FOCO = 4;
const byte BOTON_RIEGO = 5;
const byte BOTON_INICIO = 24;

const byte RELAY_CALEFACCION = 8;
const byte RELAY_GOTEO = 9;
const byte RELAY_FOCO = 11;
const byte RELAY_RIEGO = 13;

const byte RS = 22;
const byte ENABLE = 24;
const byte D4 = 30;
const byte D5 = 32;
const byte D6 = 34;
const byte D7 = 36;

const unsigned long DIA = 86400000;
const unsigned long PRESIONADO = 200;

const String TEMPERATURA = String("Temp: ");
const String CENTIGRADOS = String("C");
const String HUMEDAD = String("Hum: ");
const String PORCENTAJE = String("%");


unsigned long TIEMPO_RIEGO_ACTIVO = 5000;
unsigned long TIEMPO_RIEGO_ESPERA = 10000;
unsigned long tiempoRiegoActual = 0;

unsigned long TIEMPO_FOCO_ACTIVO = 4000;
unsigned long TIEMPO_FOCO_ESPERA = 10000;
unsigned long tiempoFocoActual = 0;

unsigned long BOTON_CALEFACCION_TIEMPO = 0;
unsigned long BOTON_GOTEO_TIEMPO = 0;
unsigned long BOTON_FOCO_TIEMPO = 0;
unsigned long BOTON_RIEGO_TIEMPO = 0;
unsigned long BOTON_INICIO_TIEMPO = 0;

unsigned long TIEMPO_DISPLAY_ESPERA = 3000;
unsigned long TIEMPO_DISPLAY_ACTUAL = 0;

byte TEMPERATURA_MINIMA = 24;
byte TEMPERATURA_MAXIMA = 26;

byte HUMEDAD_MINIMA = 40;
byte HUMEDAD_MAXIMA = 85;

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
LiquidCrystal lcd(RS, ENABLE, D4, D5, D6, D7);

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  lcd.begin(16, 2);
  Serial.begin(BAUDIOS);
  pinMode(BOTON_CALEFACCION, INPUT);
  pinMode(BOTON_GOTEO, INPUT);
  pinMode(BOTON_FOCO, INPUT);
  pinMode(BOTON_RIEGO, INPUT);
  pinMode(BOTON_INICIO, INPUT);

  pinMode(RELAY_CALEFACCION, OUTPUT);
  pinMode(RELAY_GOTEO, OUTPUT);
  pinMode(RELAY_FOCO, OUTPUT);
  pinMode(RELAY_RIEGO, OUTPUT);
  digitalWrite(RELAY_CALEFACCION, HIGH);
  digitalWrite(RELAY_GOTEO, HIGH);
  digitalWrite(RELAY_FOCO, HIGH);
  digitalWrite(RELAY_RIEGO, HIGH);
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
    printData();
    checaTemperaturaHumedad();
    riego(tiempo);
    foco(tiempo);
    checkButtons(tiempo);
    serialEvents(tiempo);
    reset(tiempo);
  } else {
    if (digitalRead(BOTON_INICIO) && (tiempo - BOTON_INICIO_TIEMPO) >= PRESIONADO) {
      estadoSistema = !estadoSistema;
      BOTON_INICIO_TIEMPO = tiempo;
    }
  }
}

void printData() {
  Serial.println("TEMPERATURA " + String(temperaturaActual) + " " + String(TEMPERATURA_MINIMA) + " " + String(TEMPERATURA_MAXIMA));
  Serial.println("HUMEDAD " + String(humedadActual) + " " + String(HUMEDAD_MINIMA) + " " + String(HUMEDAD_MAXIMA));
  Serial.println("GOTEO " + String(estadoGoteo));
  Serial.println("CALEFACCION " + String(estadoCalefaccion));
  Serial.println("RIEGO " + String(estadoRiego));
  Serial.println("FOCO " + String(estadoFoco));
}

void checaTemperaturaHumedad() {
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
  digitalWrite(RELAY_CALEFACCION, (estadoCalefaccion) ? LOW : HIGH);
  digitalWrite(RELAY_GOTEO, (estadoGoteo) ? LOW : HIGH);
}

void riego(unsigned long tiempo) {
  // SI ESTÁ PRENDIDO EL RIEGO
  if (estadoRiego) {
    if (tiempo - tiempoRiegoActual >= TIEMPO_RIEGO_ACTIVO) {
      estadoRiego = false;
      tiempoRiegoActual = tiempo;
      digitalWrite(RELAY_RIEGO, HIGH);
    }
  } else {
    if (tiempo - tiempoRiegoActual >= TIEMPO_RIEGO_ESPERA) {
      estadoRiego = true;
      tiempoRiegoActual = tiempo;
      digitalWrite(RELAY_RIEGO, LOW);
    }
  }
}

void foco(unsigned long tiempo) {
  // SI ESTÁ PRENDIDO O APAGADO EL FOCO
  if (estadoFoco) {
    if (tiempo - tiempoFocoActual >= TIEMPO_FOCO_ACTIVO) {
      estadoFoco = false;
      tiempoFocoActual = tiempo;
      digitalWrite(RELAY_FOCO, HIGH);
    }
  } else {
    if (tiempo - tiempoFocoActual >= TIEMPO_FOCO_ESPERA) {
      estadoFoco = true;
      tiempoFocoActual = tiempo;
      digitalWrite(RELAY_FOCO, LOW);
    }
  }
}

void checkButtons(unsigned long tiempo) {
  if (digitalRead(BOTON_CALEFACCION) && (tiempo - BOTON_CALEFACCION_TIEMPO) >= PRESIONADO) {
    estadoCalefaccion = !estadoCalefaccion;
    BOTON_CALEFACCION_TIEMPO = tiempo;
  }
  if (digitalRead(BOTON_GOTEO) && (tiempo - BOTON_GOTEO_TIEMPO) >= PRESIONADO) {
    estadoGoteo = !estadoGoteo;
    BOTON_GOTEO_TIEMPO = tiempo;
  }
  if (digitalRead(BOTON_FOCO) && (tiempo - BOTON_FOCO_TIEMPO) >= PRESIONADO) {
    estadoFoco = !estadoFoco;
    tiempoFocoActual = tiempo;
    BOTON_FOCO_TIEMPO = tiempo;
    digitalWrite(RELAY_FOCO, (estadoFoco) ? LOW : HIGH);
  }
  if (digitalRead(BOTON_RIEGO) && (tiempo - BOTON_RIEGO_TIEMPO) >= PRESIONADO) {
    estadoRiego = !estadoRiego;
    tiempoRiegoActual = tiempo;
    BOTON_RIEGO_TIEMPO = tiempo;
    digitalWrite(RELAY_RIEGO, (estadoRiego) ? LOW : HIGH);
  }
}

void serialEvents(unsigned long tiempo) {
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
}

void reset(unsigned long tiempo) {
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
}
