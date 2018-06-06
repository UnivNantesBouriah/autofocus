#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>


#define MAX_CHAR 30

SoftwareSerial mySerial(D5,D7); // RX, TX ->8,7

const char* ssid = "TP-Link_Guest_710C";           // Identifiant WiFi
const char* password = "azertyuiop";  // Mot de passe WiFi
WiFiServer server(80);         // On instancie un serveur qui ecoute sur le port 80
char bufRecu[MAX_CHAR];
int nbCharRecu=0;
 
const int _OUT_LEDG = D8; // -> 0
const int _OUT_LEDD = D6; // -> 1
const int _OUT_MOT_SENS = D2; // -> 2

const int _IN_MOT_ACK = D1; // -> 5 

int ledGOn=0;
int ledDOn=0;
int motSens=0;
int motOn=0;
float duree=0.;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  // on attend d'etre connecte au WiFi avant de continuer
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
  // on affiche l'adresse IP attribuee pour le serveur DSN
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // on demarre le serveur web 
  server.begin();
  pinMode(_OUT_LEDG, OUTPUT);     // Initialise la broche "led" comme une sortie - Initialize the "LED" pin as an output
  pinMode(_OUT_LEDD, OUTPUT);     // Initialise la broche "led" comme une sortie - Initialize the "LED" pin as an output
  pinMode(_OUT_MOT_SENS, OUTPUT);

  pinMode(_IN_MOT_ACK, INPUT);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}
 
// Cette boucle s'exécute à l'infini - the loop function runs over and over again forever
void loop()
{  
/*  digitalWrite(led, LOW);   // Eteint la Led - Turn the LED OFF 
  delay(1000);              // Attendre 1 seconde - Wait for a second
  digitalWrite(led, HIGH);  // Allume la Led - Turn the LED off by making the voltage HIGH
  delay(3000);              // Pause de 2 secondes - Wait 2 secondes
*/  
  // listen for incoming clients
  WiFiClient client= server.available();
  if (client) 
  {
//    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
//        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          int sensorReading = digitalRead(_OUT_LEDG);
          client.print("Pointeur Gauche is : ");
          client.print(sensorReading);client.print(ledGOn);
          client.println("<br />");
          sensorReading = digitalRead(_OUT_LEDD);client.print(ledDOn);
          client.print("Pointeur Droit is : ");
          client.print(sensorReading);
          client.println("<br />");
          client.println("</html>");
          break;
        }
        else if (c == '\n') 
        {
/*          Serial.println("TOP: ");
          Serial.println(nbCharRecu);
          for(int i=0;i<nbCharRecu;i++)
            Serial.print(bufRecu[i]);
          Serial.println("");
*/            
          // you're starting a new line
          currentLineIsBlank = true;       
          if(bufRecu[0]=='L')
          {
            // Une commande pour les pointeurs est reçue "LED:E:P\r\n"
            int etat=LOW;
            if((nbCharRecu>=4)&&(bufRecu[4]=='1')) 
            {
              etat = HIGH;
            }
            if(nbCharRecu>=6)
              if(bufRecu[6]=='1')              
                ledDOn=etat;
              else
                ledGOn=etat;
            nbCharRecu=0;
//            Serial.print("LED G : ");
//            Serial.println(ledGOn);
//            Serial.print("LED D : ");
//            Serial.println(ledDOn);
          }
          if(bufRecu[0]=='M')
          {
            // Une commande pour le moteur est reçue Moteur:S:D\r\n
            motSens=0;
            if((nbCharRecu>=7)&&(bufRecu[7]=='1'))
              motSens=1;
            duree=0;
            if(nbCharRecu>=9)
            {
              char cDuree[MAX_CHAR];
              int i=0;
              while(i+9<nbCharRecu)
              {
                cDuree[i]=bufRecu[i+9];
                i++;
              }
//              Serial.println(cDuree);
              duree=atof(cDuree);//*1000;
            }
            motOn=1;
//            Serial.println(motSens);
//            Serial.println(duree);
            nbCharRecu=0;
          }
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          bufRecu[nbCharRecu]=c;
          nbCharRecu++;
          if(nbCharRecu>MAX_CHAR)
            nbCharRecu=0;
        }        
      }
      //Prise en compte des données reçues:
      digitalWrite(_OUT_LEDG,ledGOn);
      digitalWrite(_OUT_LEDD,ledDOn);
      if(motOn==1)
      {
        digitalWrite(_OUT_MOT_SENS, motSens); //envoyer commande moteur
        mySerial.print(duree,4);
        Serial.println(duree,4);
        digitalWrite(BUILTIN_LED, LOW);
//        delay(duree*1000);
//        analogWrite(_OUT_MOT_DUREE, duree*1023./2.); //envoyer commande moteur
//        delay(1);
//        Serial.println(duree*1023./2.);
//        int iDuree = analogRead(_IN_MOT_DUREE);
//        delay(1);
//        Serial.print("Duree lue : ");Serial.println(iDuree); 
//        motOn=2;
//        digitalWrite(_OUT_MOT_ON, LOW); //envoyer commande moteur
//        digitalWrite(BUILTIN_LED, HIGH);
        motOn=2;
      }
      else if((motOn==2) && (digitalRead(_IN_MOT_ACK)))
      {
//        digitalWrite(_OUT_MOT_ON, LOW); //envoyer commande moteur
        digitalWrite(BUILTIN_LED, HIGH);
        delay(1);
        Serial.println("Moteur ON Acq");    
        motOn=0;
      }
  //    digitalWrite(_OUT_MOT_ON, LOW); //envoyer commande moteur
  //  Serial.println("Moteur OFF");
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
//    Serial.println("client disonnected");
  }
}

