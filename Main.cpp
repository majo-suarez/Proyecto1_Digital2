//Universidad del Valle de Guatemala
//Proyecto 1 Electrónica Digital 2
//María José Suárez 21121

// Se incluyen librerías
#include <Arduino.h>
#include <driver/ledc.h>
#include <display7.h>
#include <config.h>

//Definición de pines de segmentos de Display
#define pA 32
#define pB 33
#define pC 25
#define pD 26
#define pE 27
#define pF 14
#define pG 12
#define pdP 13

//Pines de cada display
#define dis1 5
#define dis2 17
#define dis3 16

//Sensor de temperatura
#define ADC_VREF_mV 3300.0    // minivolts
#define ADC_RESOLUTION 4096.0 // Resolución del ADC
#define pinsensor 36          // ESP32 pin GPIO36 (ADC0) conectado a LM35

#define botpin 18             // Pin de botón

#define ledv 23 // Led verde
#define leda 22 // Led amarilla
#define ledr 21  // Led roja
#define servo 19 // Servo motor

// Canales y frecuencias de LEDs y Servo
#define ledvChannel 1
#define ledaChannel 2
#define ledrChannel 3
#define servoChannel 4
#define freqPWM1 1000
#define freqPWM2 1000
#define freqPWM3 1000
#define resolution 8

// Variables globales
float tempC;
float milliVolt;
bool banderabot = false;

// this int will hold the current count for our sketch
volatile float temperatura = tempC;

// set up the 'counter' feed
AdafruitIO_Feed *temperaturaCanal = io.feed("Temperatura");

// Funciones 
void configurarPWM();
void moverServo(int angulo);
void mostrarTemperaturaEnDisplay(float tempC);
void convertirTemp();

void setup()
{
  //*******************ADAFRUIT*****************************************//
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

//**************************DISPLAYS**********************************//
  configurarDisplay7(pA, pB, pC, pD, pE, pF, pG, pdP);

  pinMode(dis1, OUTPUT);
  pinMode(dis2, OUTPUT);
  pinMode(dis3, OUTPUT);

  digitalWrite(dis1, LOW);
  digitalWrite(dis2, LOW);
  digitalWrite(dis3, LOW);
  
  //Configuracion leds y servo
  pinMode(botpin, INPUT_PULLUP);
  pinMode(ledv, OUTPUT);
  pinMode(leda, OUTPUT);
  pinMode(ledr, OUTPUT);


  // Apagar todos los LEDs al principio
  ledcWrite(ledvChannel, 0);
  ledcWrite(ledaChannel, 0);
  ledcWrite(ledrChannel, 0);

  //void PWM
  configurarPWM();
}

void loop()
{
  // Mostrar la temperatura en los displays de 7 segmentos
  mostrarTemperaturaEnDisplay(tempC);

  //Convertir la temperatura a digital
  convertirTemp();

  //Condiciones del boton
  int leerbot = digitalRead(botpin);
  if (leerbot == LOW && !banderabot)
  {
    //**************************ADAFRUIT**************************//
    banderabot = true;
    io.run();

    // save count to the 'counter' feed on Adafruit IO
    Serial.print("sending -> ");
    Serial.println(tempC);
    temperaturaCanal->save(tempC);

    // (1000 milliseconds == 1 second) during each loop.
    delay(3000);

    //************************************************************//

    // Control de LEDs en función de la temperatura
    if (tempC < 37.0)
    {
      ledcWrite(ledvChannel, 255); // Encender LED verde
      ledcWrite(ledaChannel, 0);
      ledcWrite(ledrChannel, 0);
      moverServo(0); // Mover el servo a 0 grados
      Serial.println("Caso 37");
    }
    else if (tempC < 37.5)
    {
      ledcWrite(ledvChannel, 0);
      ledcWrite(ledaChannel, 255); // Encender LED amarilla
      ledcWrite(ledrChannel, 0);
      moverServo(90); // Mover el servo a 90 grados
      Serial.println("Caso 37.5");
    }
    else if (tempC >= 37.5)
    {
      ledcWrite(ledvChannel, 0);
      ledcWrite(ledaChannel, 0);
      ledcWrite(ledrChannel, 255); // Encender LED roja
      moverServo(180); // Mover el servo a 180 grados
      Serial.println("Caso 38");
    }

    // Imprimir en monitor serial
    Serial.print("Temperatura: ");
    Serial.print(tempC); // Imprime temperatura en °C
    Serial.println(" °C");   
  }

  //Otra condicion de boton
  if (leerbot == HIGH)
  {
    banderabot = false;
  }
}

//Funcion que controla pulsos de LEDs y Servo
void configurarPWM()
{
  ledcSetup(ledvChannel, freqPWM1, resolution);
  ledcSetup(ledaChannel, freqPWM2, resolution);
  ledcSetup(ledrChannel, freqPWM3, resolution);
  ledcSetup(servoChannel, 500, 8);

  ledcAttachPin(ledv, ledvChannel);
  ledcAttachPin(leda, ledaChannel);
  ledcAttachPin(ledr, ledrChannel);
  ledcAttachPin(servo, servoChannel);
}

//Funcion de mapeo de angulo de Servo
void moverServo(int angulo)
{
  int dutyCycle = map(angulo, 0, 180, 40, 115); // Mapear ángulo a ciclo de trabajo
  ledcWrite(servoChannel, dutyCycle);
}

//Funcion de conversion de temperatura para displays
void mostrarTemperaturaEnDisplay(float tempC)
{
  int temperaturaEntera, tempa, decenas, unidades, centenas;

  // Convertir temperatura a un valor entero para mostrar en los displays
  temperaturaEntera = int(tempC);
  tempa = int(temperaturaEntera*10);
  decenas = tempa / 100;
  unidades = (tempa - (decenas*100))/10;
  centenas = tempa - (unidades*10);

  // Mostrar la temperatura en los tres displays
  digitalWrite(dis1, HIGH);
  digitalWrite(dis2, LOW);
  digitalWrite(dis3, LOW);
  desplegarValor(decenas); // Decenas

  digitalWrite(dis1, LOW);
  digitalWrite(dis2, HIGH);
  digitalWrite(dis3, LOW);
  desplegarValor(unidades); // Unidades
  desplegarPunto(true);

  
  digitalWrite(dis1, LOW);
  digitalWrite(dis2, LOW);
  digitalWrite(dis3, HIGH);
  desplegarValor(centenas); // Centenas
  desplegarPunto(false);
}

//Funcion de conversion de temperatura a analogico
void convertirTemp()
{
  // Leer valor ADC del sensor de temperatura
    int adcVal = analogRead(pinsensor);
    //Serial.print(adcVal);
    // Convertir ADC a voltaje minivolts
    milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
    // Convertir voltaje a temperatura en °C
    tempC = milliVolt / 10.0;
}
