#include <LiquidCrystal.h>
#include <DHT.h>

bool modeStandardCheck = true;        //Variable permettant d'identifier le mode en cours (1 correspond au mode standard et le 2 au mode configuration)

const static int portLEDVerte = 8;      //Port de la LED verte
const static int portLEDJaune = 10;     //Port de la LED jaune
const static int portCapteurDHT = 7;    //Port du capteur de température et d'humidité
const static int portButton = 2;      //Port du bouton qui permet de changer de mode

int timeDelayLCD = 250;             //Delay de rafraichissement du LCD

int intervalleMesure = 1000;        //Delay d'intervalle entre deux mesures

String modeText;              //Variable de texte qui permet à l'utilisateur de savoir quel mode est en court d'utilisation

LiquidCrystal lcd(12, 11, 6, 5, 4, 3);    //Déclaration de l'objet LiquidCrystal correspond à notre écran LCD

#define DHTTYPE DHT11           //On définit quel type de capteur est utilisé

DHT dht(portCapteurDHT, DHTTYPE);     //Déclaration de l'objet de type DHT correspondant à notre capteur de température et d'humidité

//*******************************************************************
//* Nom :         ON                                                *
//* Paramètre :   Port                                              *
//* Utilité :     Allumer une LED                                   *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void ON(int port)
{
  digitalWrite(port, 1);
}

//*******************************************************************
//* Nom :         OFF                                               *
//* Paramètre :   Port                                              *
//* Utilité :     Eteindre une LED                                  *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void OFF(int port)
{
  digitalWrite(port, 0);
}

//*******************************************************************
//* Nom :         modeStandard                                      *
//* Paramètre :   Aucun                                             *
//* Utilité :     mode de fonctionnement normal                     *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void modeStandard()
{
  OFF(portLEDJaune);
  ON(portLEDVerte);

  modeText = "SMODE";
  getData();
}

//*******************************************************************
//* Nom :         modeConfiguration                                 *
//* Paramètre :   Aucun                                             *
//* Utilité :     mode de configuration des paramètres              *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void modeConfiguration()
{
  OFF(portLEDVerte);
  ON(portLEDJaune);

  modeText = "CONFIGURATION";

  affichageLCD(0,0,"USE SERIAL PORT");
  affichageLCD(0,1,modeText);

  Serial.println(F("Mode configuration !"));
  Serial.println(F("Changement du temps d'intervalle entre deux mesures : "));

  int newIntervalle;
  String reponse;

  while (Serial.available() == 0){}     //Tant qu'aucune réponse n'est rentrée on ne continue pas le programme
  reponse = Serial.readString();
  newIntervalle = reponse.toInt();

  Serial.print(F("Le temps que vous venez de mettre est "));
  Serial.print(newIntervalle);
  Serial.println(F(" secondes."));

  intervalleMesure = newIntervalle * 1000;

  Serial.print(F("Retour en mode Standard !"));

  delay(500);

  modeStandardCheck = true;
}

//*******************************************************************
//* Nom :         getData                                           *
//* Paramètre :   Aucun                                             *
//* Utilité :     Obtenir des données                               *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void getData()
{
  String dataString1;
  String dataString2;

  dataString1 = "Temp: ";
  dataString1 += String((int)dht.readTemperature()) + " C ";
  dataString1 += modeText;

  dataString2 = "Humidity: ";
  dataString2 += String((int)dht.readHumidity()) + "%";

  affichageLCD(0,0,dataString1);
  affichageLCD(0,1,dataString2);

  delay(intervalleMesure);
}

//*******************************************************************
//* Nom :         interruptButton                                   *
//* Paramètre :   Aucun                                             *
//* Utilité :     Interrompre le programme                          *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void interruptionButton()
{
  if (digitalRead(portButton) == 1)
    {
      modeStandardCheck = !modeStandardCheck;
      Serial.println("interruption");
    }
}

//*******************************************************************
//* Nom :         initInterrupt                                     *
//* Paramètre :   Aucun                                             *
//* Utilité :     Déclrarer l'interruption                          *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void initInterrupt()
{
  attachInterrupt(digitalPinToInterrupt(portButton), interruptionButton, CHANGE);
}

//*******************************************************************
//* Nom :         initialisation                                    *
//* Paramètre :   Aucun                                             *
//* Utilité :     Initialisation de l'Arduino                       *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void initialisation()
{
  Serial.begin(9600);           //Initialisation de la fréquence de l'Arduino à 9600 bauds

  initInterrupt();

  ON(portLEDVerte);
  ON(portLEDJaune);
  delay(100);

  OFF(portLEDVerte);
  OFF(portLEDJaune);
  delay(100);

  ON(portLEDVerte);
  ON(portLEDJaune);
  delay(100);

  OFF(portLEDVerte);
  OFF(portLEDJaune);

  pinMode(portLEDVerte, OUTPUT);        //Déclaration de la LED Verte en tant que sortie
  pinMode(portLEDJaune, OUTPUT);        //Déclaration de la LED Rouge en tant que sortie
  pinMode(portLEDJaune, OUTPUT);      //Déclaration de la LED Jaune en tant que sortie
  pinMode(portButton, INPUT);       //Déclaration du bouton en tant qu'entrée

  lcd.begin(16,2);                  //Configuration du nombre de colonnes et de lignes sur l'écran LCD

  affichageLCD(4,0,"Starting");
  affichageLCD(5,1,"System");

  dht.begin();              //Lancement du capteur

  delay(1500);

  resetLCD();
}

//*******************************************************************
//* Nom :         affichageLCD                                      *
//* Paramètre :   colone, ligne, texteToDisp                        *
//* Utilité :     Afficher du texte sur l'écran LCD                 *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void affichageLCD(int colonne, int ligne, String texteToDisp)
{
  lcd.setCursor(colonne, ligne);        //Positionnement du texte à afficher
  lcd.print(texteToDisp);             //Affichage du texte
}

//*******************************************************************
//* Nom :         resetLCD                                          *
//* Paramètre :   Aucun                                             *
//* Utilité :     Nettoyer l'écran                                  *
//* Retour :      Aucun                                             *
//*                                                                 *
//*******************************************************************

void resetLCD()
{
  for (int y = 0; y < 2; y++)
  {
    for (int x = 0; x < 16; x++)
    {
      affichageLCD(x,y," ");
    }
  }
}

void setup()
{
  initialisation();
}

void loop()
{
  if (modeStandardCheck)
    modeStandard();
  else 
    modeConfiguration();
}
