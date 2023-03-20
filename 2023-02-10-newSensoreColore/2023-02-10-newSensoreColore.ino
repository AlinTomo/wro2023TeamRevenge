#include <Servo.h>
#include <Wire.h>
#include <MPU6050.h>
#include "Adafruit_TCS34725.h"

//pin
#define OUT_PIN 6
#define pwmMotore 11       //PWM control for motor outputs 1 and 2 
#define motoreAvanti 12
#define motoreIndietro 10 

//COSTANTI
#define GRADI_CURVA 87

//OGGETTI
Servo myservo;            //oggetto servomotore
MPU6050 mpu;              //oggetto giroscopio
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X);


//VARIABILI
int velocita=255;         //velocità media in tutto il percorso
float colori[3],vecchi[3];  //array RGB di lettura del sensore 
                          //^^^aggiornati alla chiamata di updatergb^^^

int bluWhite=0;           //quantità di blu quando sei sul bianco
int redStandard=0;        //quantità di rosso quando sei sul bianco
                          //^^^aggiornati al setup^^^
                          
short dirTrovata=0;       //stabilisce se ha trovato una direzione
short verso=0;            //orario oppure antiorario

//NECESSARI ALLA FUNC giroscopio()
// Timers
unsigned long timer = 0;
float timeStep = 0.01;

//Yaw values
float yaw = 0;
float yawPrecedente = 0;
//variabile di stato
int stato=1;

//prototipi
void raddrizza();
void updateRGB();
void vai(int );
void destra();
void sinistra();
short direzione();
void giroscopio();
void orario();
void antiorario();

void setup(){
    Serial.begin(115200);

    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)){
      Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
      delay(500);
    }

      if (tcs.begin()) {
    //Serial.println("Found sensor");
      } else {
        Serial.println("No TCS34725 found ... check your connections");
        while (1); // halt!
      }
    
    mpu.calibrateGyro();
    mpu.setThreshold(3);
    
    pinMode(pwmMotore, OUTPUT);  //Set control pins to be outputs
    pinMode(motoreAvanti, OUTPUT);
    pinMode(motoreIndietro, OUTPUT);
    pinMode(OUT_PIN, INPUT);
    myservo.attach(9);
    raddrizza();
    vecchi[0]=colori[0];

    //salviamo il valore blu del bianco
    updateRGB();

    //colori[0]*=100;
    //colori[1]*=100;
    //colori[2]*=100;
    
    //bluWhite = colori[0];
    //salvo anche il rosso standard (quando è sul bianco)
    //redStandard = colori[2];
}

void loop(){
    updateRGB();
    
    switch(stato){
      case 0:
        stopMotore();
        break;
      case 1:                       //stato ricerca del verso
        vai(velocita);
        avanti();
        while(dirTrovata==0){
            updateRGB();
            verso = direzione();
            if(verso!=0){
              dirTrovata = 1;
              stato=2;
            }
        }
        break;
      case 2:                       //stato in cui si curva 
        giro();
        break;
      case 3:
        break;
    }

    giroscopio();
    
    vecchi[0]=colori[0]+20;
}

//funzioni per il movimento dei motori
void stopMotore(){
  analogWrite(pwmMotore, 0);  
  Serial.print("STOP");
}
void vai(int velocita){
  analogWrite(pwmMotore, velocita);  
}
void avanti(){
  digitalWrite(motoreAvanti, HIGH);  
  digitalWrite(motoreIndietro, LOW);
}
void indietro(){
  digitalWrite(motoreAvanti, LOW);  
  digitalWrite(motoreIndietro, HIGH);
}
void raddrizza(){
  myservo.write(88);
  Serial.println("raddrizza");
}
void destra(){
  myservo.write(120);
  Serial.println("destra");
}
void sinistra(){
  myservo.write(50);
  Serial.println("sinistra");
}

//funzione che aggiorna i dati del sensore di colore 
void updateRGB() {
  /*//Frequency 20%
  digitalWrite(S0_PIN, HIGH);
  digitalWrite(S1_PIN, LOW);
  delay(10);
  */
  colori[0]=0;
  colori[1]=0;
  colori[2]=0;

  tcs.getRGB(&colori[2], &colori[1], &colori[0]);
  Serial.print("B: ");
  Serial.print(colori[0]);
  Serial.print("G: ");
  Serial.print(colori[1]);
  Serial.print("R: ");
  Serial.println(colori[2]);
  delay(1000);
}

//funzione che rileva il corlore -1 per il rosso 0 per il bianco 1 per il blu
short direzione(){dz
      return -1;
    }else{
      if(colori[0]>=100)
        return 1;
      else
        return 0;
   }
}

void giroscopio(){
  timer = millis();
  
  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Yaw
  yaw = yaw + norm.ZAxis * timeStep;

  // Serial.print(" Yaw = ");
  Serial.println(yaw);

  // Wait to full timeStep period
  delay((timeStep*1000) - (millis() - timer));
}

void giro(){
    Serial.println("GIRO");
    if(direzione()==verso){
        vai(velocita);
        if(verso==1)
          sinistra();
        else 
          destra();
        while(true){
          giroscopio();
          if(valAssoluto(yaw)>GRADI_CURVA+yawPrecedente){
            yawPrecedente+=GRADI_CURVA;
            Serial.println(yawPrecedente);
            raddrizza();
            updateRGB();
            vai(velocita);
            break;
          }
          if(valAssoluto(yaw)>=GRADI_CURVA*(12))
            stato=0;
        }
    }
}

float valAssoluto(float x){
  if(x>0)
    return x;
  return x*(-1);
}
