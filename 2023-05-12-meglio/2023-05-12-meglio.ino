#include <Servo.h>

Servo myservo; 
const int pinIndietro = 2;
const int pwmMotore = 3;  //PWM control for motor outputs 1 and 2 
const int pinAvanti = 4;  //direction control for motor outputs 1 and 2  
const int trigSx = 8;
const int echoSx = 7;
const int trigDx = 10;
const int echoDx = 9;
const int trigFr = 12;
const int echoFr = 11;

int muroImportante = 0;

float distSx,distDx, oldSx = 1, oldDx = 1, distFr;
int errore;



void setup()
{
  pinMode(pwmMotore, OUTPUT);  //Set control pins to be outputs
  pinMode(pinAvanti, OUTPUT);
  pinMode(pinIndietro, OUTPUT);
  pinMode(trigSx, OUTPUT);
  pinMode(echoSx, INPUT);
  pinMode(trigDx, OUTPUT);
  pinMode(echoDx, INPUT);
  pinMode(trigFr, OUTPUT);
  pinMode(echoFr, INPUT);
  Serial.begin(9600);
  myservo.attach(5);
  /*for(int i=150; i<255;i++){
    vai(i);
   Serial.println(i);
    delay(100);
  }
  delay(100000);*/
  stopMotore();
  raddrizza();
    
}

void loop()
{
  // while(1) {
  //   curva(0);
  // }
  vai(255);
  avanti();
  float temp = misura(trigSx,echoSx);
  if(temp != oldSx && temp > 1 && temp < 1000) {
    oldSx = distSx;
    distSx = temp; 
  }
  temp = misura(trigDx,echoDx);
  if(temp != oldDx && temp > 1 && temp < 1000){
    oldDx = distDx;
    distDx = temp;
  }

  distFr = misura(trigFr,echoFr);

  errore=isDritto(distSx,distDx);
  Serial.print("sinistra:");
  Serial.print(distSx);
  Serial.print("   destra: ");
  Serial.print(distDx);
  Serial.print("   avanti: ");
  Serial.print(distFr);
  Serial.print("errore: ");
  Serial.println(errore);
  if (muroImportante == 0 && distDx > 100 ) {
    muroImportante = 1;
    }
  else if(muroImportante == 0 && distSx > 100 ) {
    muroImportante = -1;
    }
  else if(distDx > 100 && muroImportante == 1)
  {
    curva(40);
    while(distFr < 70) {
      temp = misura(trigFr,echoFr);
      if (temp < 1000)
        distFr = temp; 
    }
  }
  else if(distSx > 100 && muroImportante == -1)
  {
    curva(-40);
    while(distFr < 70) {
      temp = misura(trigFr,echoFr);
      if (temp < 1000)
        distFr = temp; 
    }
  }
  else{
    curva(errore);
 }
}

void stopMotore(){
  analogWrite(pwmMotore, 0);  
}
void vai(int velocita){
  analogWrite(pwmMotore, velocita); 
}

void avanti(){
  digitalWrite(pinAvanti, HIGH);   
  digitalWrite(pinIndietro, LOW);  
}

void indietro(){
  digitalWrite(pinAvanti, LOW);   
  digitalWrite(pinIndietro, HIGH);  
}
void raddrizza(){
  myservo.write(90);
  delay(100);
}

void curva(int gradi){
  gradi=90+gradi;
  myservo.write(gradi);
}

float misura(int trigPin,int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH, 200000L);
  return (duration*.0343)/2;
  delay(100);
}

int isDritto(int distSx,int distDx)
{
  if (muroImportante == 1) {
    distSx = distSx / 1.2;
  }
  else if (muroImportante == -1) {
    distDx = distDx / 1.2;
  }

  int differenza=((distDx-distSx)/1.5); //calcolo dell'errore (per ora ho fatto una semplice sottrazione dovremo poi vedere come farlo bene)
  if (differenza > 40)
    differenza=40;
  else if(differenza<-40)
    differenza=-40;

  if (muroImportante == 1 && differenza < -30) {
    differenza = -30;
  } else if(muroImportante == -1 && differenza > 30) {
    differenza = 30;
  }
  return differenza;
}