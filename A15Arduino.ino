#include <NoDelay.h>

int leds[] = {2, 4, 5, 18};
enum Animacion { OFF, LTR, RTL, FLASH };
Animacion animacionActual = OFF;

int pasoAnimacion = 0;
int flashCount = 0;
bool flashState = false;

noDelay timerLTR(500);
noDelay timerRTL(500);
noDelay timerFlash(250);

void setup() {
  Serial.begin(115200);
  for(int i=0; i<4; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void loop() {
  recibirComando();
  ejecutarAnimacion();
}

void recibirComando() {
  if(animacionActual == FLASH) {
    while(Serial.available() > 0) Serial.read();
    return;
  }

  if(Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();

    Serial.print("Comando: ");
    Serial.println(input);

    bool comandoValido = false;

    switch(animacionActual) {
      case OFF:
        if(input == "ltr") {
          iniciarAnimacion(LTR);
          comandoValido = true;
        }
        else if(input == "rtl") {
          iniciarAnimacion(RTL);
          comandoValido = true;
        }
        break;

      case LTR:
      case RTL:
        if(input == "off") {
          detenerAnimacion();
          comandoValido = true;
        }
        break;
    }

    if(!comandoValido && animacionActual != FLASH) {
      iniciarAnimacion(FLASH);
      Serial.println("Error: Comando bloqueado");
    }
  }
}



void iniciarAnimacion(Animacion nuevaAnimacion) {
  detenerAnimacion();
  animacionActual = nuevaAnimacion;
  pasoAnimacion = 0;
  flashCount = 0;
  flashState = false;
}

void detenerAnimacion() {
  animacionActual = OFF;
  apagar_leds();
}

void ejecutarAnimacion() {
  switch(animacionActual) {
    case LTR:
      animacionLTR();
      break;
    case RTL:
      animacionRTL();
      break;
    case FLASH:
      animacionFlash();
      break;
    default:
      break;
  }
}

void animacionLTR() {
  if (timerLTR.update()) {
    if (pasoAnimacion > 0) digitalWrite(leds[pasoAnimacion - 1], LOW);
    
    if (pasoAnimacion < 4) {
      digitalWrite(leds[pasoAnimacion], HIGH);
      pasoAnimacion++;
    } else {
      pasoAnimacion = 0;
    }
  }
}

void animacionRTL() {
  if (timerRTL.update()) {
    int ledActual = 3 - pasoAnimacion;
    
    if (pasoAnimacion > 0) digitalWrite(leds[ledActual + 1], LOW);
    
    if (pasoAnimacion < 4) {
      digitalWrite(leds[ledActual], HIGH);
      pasoAnimacion++;
    } else {
      pasoAnimacion = 0;
    }
  }
}

void animacionFlash() {
  if(timerFlash.update()) {
    flashState = !flashState;
    for(int j=0; j<4; j++) {
      digitalWrite(leds[j], flashState ? HIGH : LOW);
    }

    if(!flashState) {
      flashCount++;
      if(flashCount >= 10) {
        detenerAnimacion();
        Serial.println("Flash completado");
      }
    }
  }
}

void apagar_leds() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW);
  }
}