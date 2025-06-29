#include <Servo.h>
#include <SPI.h>
#include <Pixy2.h>

using namespace std;

Servo myservo_D;  // servo droit
Servo myservo_G;  // servo gauche
Pixy2 pixy; // caméra

// pins---------------------------
const int trigPin = 4; // pin emetteur
const int echoPin = 2; // pin recepteur
int PIN_VENT = 10; // pin ventilateur 
int PIN_SD = 7; // pin servo droit
int PIN_SG = 8; // pin servo gauche
//--------------------------------

// variables----------------------
int vit_D = 0;  // vitesse servo droit
int vit_G = 0;  // vitesse servo gauche
int distance; // distance entre le robot et un obstacle
bool diametre = 6.5; // diamètre des roues
bool circonf = 2 * 3.14 * 6.5 / 2; // vitesse de 180 rad/s
int lin = 0;
bool sucking;
char parcoursStored[] = "";//"AAAAAAAADAADAAGAAGAAS"; 
char parcoursDone[] = "";//"AAGAAGAADAADAAAAAAAAAA"; 
int current = 0;
int Block;
//--------------------------------

//fonctions-----------------------

void avancer() {
  myservo_D.write(0);
  myservo_G.write(180);
}//10cm/s au max

void arret() {
  myservo_D.write(90);
  myservo_G.write(90);
  Serial.println("ARREERTTT");
}


void tourner_D() {
  myservo_D.write(180);
  myservo_G.write(180);
}

void tourner_G() {
  myservo_D.write(0);
  myservo_G.write(0);
}// 1300 pour tourner 90

void mvt_rect() {
  avancer();
  tourner_D();
  avancer();
  tourner_D();

}

void parcours(char direction) {
  if (direction == 'A') {
    avancer();
    delay(500);
    }
  else if (direction == 'D') {
    tourner_D();
    delay(2200);
    }
  else if (direction = 'G') {
    tourner_G();
    delay(2200);
    }

  else if (direction = 'S') {
    arret();
    delay(1000);
    }
}
////--------------------------------

void setup() {
  myservo_D.attach(PIN_SD);  // on attache le servo droit à la pin 1
  myservo_G.attach(PIN_SG);  // on attache le servo gauche à la pin 2

  pinMode(trigPin, OUTPUT); // initialise le port lié à l'émetteur en sortie
  pinMode(echoPin, INPUT); // initialise le port lié au récepteur en entrée
  pinMode(PIN_VENT, OUTPUT); // initialise le port lié au ventilateur en sortie  
  
  pixy.init();
  Serial.begin(115200);
  Serial.print("Starting...\n");
}


void loop() {
  
  // mesure de la distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duree = pulseIn(echoPin, HIGH);
  distance = duree * 0.034 / 2;
  //--------------------------------

  // affichage de la distance
  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");
  //--------------------------------
  
  pixy.ccc.getBlocks();

  //mouvement-----------------------
  if (distance < 20) // si il y a un objet
  {
    myservo_D.write(90); // Arrêter le servo D
    myservo_G.write(90); // Arrêter le servo G
    delay(1000); // petite pause pour éviter les lectures rapides
    if (distance < 30) // si l'objet est toujours devant le robot
    {
      sucking = false;
      digitalWrite(PIN_VENT, LOW); // on éteint le ventilo
      current = 0;
    }
  }
  else // si il n'y a pas d'objets devant le robot
  {
    if (sucking == false) // si le robot n'est pas en train d'aspirer
    {
      if (pixy.ccc.numBlocks) // si la caméra voit un objet
      {
        sucking = true; 
        digitalWrite(PIN_VENT, HIGH); // on active le ventilo
        delay(5000);
        Block = pixy.ccc.numBlocks; // on stock le numéro de l'objet
        tourner_D();
        delay(00);
        tourner_D();
        delay(00);
      }
      else // si la caméra ne voit pas d'objet
      {
        // avancer(); 
        // delay(500);



        if (current < 0)//21)
          {
            parcours(parcoursStored[current]);
            //current++;
          }

        else 
        {
          arret();
          delay(1000);
        }

      }
    }
    else // si le robot est en train d'aspirer
    {
      if (current<=0) // si le robot se trouve à la base de récupération
      {
        arret();
        delay(1000); // petite pause pour éviter les lectures rapides
        Serial.print("Object captured : ");
        Serial.println(Block);
        if (Block == 1)
        {
          tourner_D();
          delay(1300);
          digitalWrite(PIN_VENT, LOW); // on éteint le ventilo
          delay(500);
          tourner_D();
          delay(1300);
        }
        else if (Block == 2)
        {
          digitalWrite(PIN_VENT, LOW); // on éteint le ventilo
          delay(500);
          tourner_D();
          delay(500);
          tourner_D();
          delay(5200);
        }
        else
        {
          tourner_G();
          delay(1300);
          digitalWrite(PIN_VENT, LOW); // on éteint le ventilo
          delay(500);
          tourner_G();
          delay(1300);
        }
        digitalWrite(PIN_VENT, LOW); // on éteint le ventilo
        sucking = false;

      }

      else // si le robot ne se trouve pas à la base de récupération
      {
        if (current > 0)
          {
            current--;
            parcours(parcoursDone[current]);
          }
      }
    }
  }


  //------------------------------------
}