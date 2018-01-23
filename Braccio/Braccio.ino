#include <Arduino.h>
#include <Braccio.h>
#include <Servo.h>

/* COMMANDS FROM KEYBOARD */
#define BASE_KEY_INC        '4'
#define BASE_KEY_DEC        '6'
#define SHOULDER_KEY_INC    '2'
#define SHOULDER_KEY_DEC    '8'
#define ELBOW_KEY_INC       's'
#define ELBOW_KEY_DEC       'w'
#define WRIST_ROT_KEY_INC   'd'
#define WRIST_ROT_KEY_DEC   'a'
#define WRIST_VER_KEY_INC   'f'
#define WRIST_VER_KEY_DEC   'r'
#define GRIPPER_KEY_INC     '5'
#define GRIPPER_KEY_DEC     '0'

#define DEG_INFO_KEY        'i'
#define RESET_POSITION_KEY  ' '
#define HELP_KEY            'h'
#define VENTILATION_KEY     'v'

/* PININ/PINOUT */
#define FAN_PIN      2

/* BOUNDS FOR SERVOS LIMITS */
#define BASE_MAX_DEG        180
#define SHOULDER_MAX_DEG    170
#define ELBOW_MAX_DEG       180
#define WRIST_ROT_MAX_DEG   200
#define WRIST_VER_MAX_DEG   180
#define GRIPPER_MAX_DEG     73

#define BASE_MIN_DEG        0
#define SHOULDER_MIN_DEG    90
#define ELBOW_MIN_DEG       90
#define WRIST_ROT_MIN_DEG   0
#define WRIST_VER_MIN_DEG   0
#define GRIPPER_MIN_DEG     0

/* VERTICAL POSITION DEFINITIONS */
#define STEP_DELAY_VERTICAL 10
#define BASE_VERTICAL       90
#define SHOULDER_VERTICAL   96
#define ELBOW_VERTICAL      94
#define WRIST_ROT_VERTICAL  90
#define WRIST_VER_VERTICAL  90
#define GRIPPER_VERTICAL    100

/* MISC */
#define DEG_OFFSET          1 

/* DECLARED AS EXTERN IN Braccio.h */
Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

/* GLOBAL VARIABLES */
int step_delay;
int count;
int areazioneAttiva;

/* GLOBAL VARIABLES FOR SERVOS DEGREES */
int base_deg;
int shoulder_deg;
int elbow_deg;
int wrist_rot_deg;
int wrist_ver_deg;
int gripper_deg;

void printCommands()
{
  Serial.println("");
  Serial.println("Lista dei comandi del braccio. I valori indicano i tasti da premere sulla tastiera per far effettuare un movimento al braccio.");
  Serial.print("*********** ATTENZIONE *********** Tutte le volte che ti trovi in difficolta con i comandi, puoi premere ");
  Serial.print(HELP_KEY); 
  Serial.println(" per avere la lista dei comandi *********** ATTENZIONE ***********\n");
  
  Serial.print( "Movimento base:    ");                Serial.print(BASE_KEY_INC);       Serial.print(" & ");  Serial.print(BASE_KEY_DEC);      Serial.println(" (sinistra & destra)");
  Serial.print( "Movimento spalla:  ");                Serial.print(SHOULDER_KEY_INC);   Serial.print(" & ");  Serial.print(SHOULDER_KEY_DEC);  Serial.println(" (giu & su)");
  Serial.print( "Movimento gomito:  ");                Serial.print(ELBOW_KEY_INC);      Serial.print(" & ");  Serial.print(ELBOW_KEY_DEC);     Serial.println(" (giu & su)");
  Serial.print( "Rotazione polso:   ");                Serial.print(WRIST_ROT_KEY_DEC);  Serial.print(" & ");  Serial.print(WRIST_ROT_KEY_INC); Serial.println(" (sinistra & destra)");
  Serial.print( "Movimento polso:   ");                Serial.print(WRIST_VER_KEY_INC);  Serial.print(" & ");  Serial.print(WRIST_VER_KEY_DEC); Serial.println(" (giu & su)");
  Serial.print( "Prendi/Rilascia:   ");                Serial.print(GRIPPER_KEY_INC);    Serial.print(" & ");  Serial.print(GRIPPER_KEY_DEC);   
  
  Serial.println("\n");
  
  Serial.print( "Info sui gradi (per admin): ");     Serial.println(DEG_INFO_KEY);
  Serial.print( "Resetta in posizione verticale: "); Serial.println("Spazio");
  Serial.print( "Per attivare la ventilazione: ");   Serial.println(VENTILATION_KEY);
  Serial.print( "Aiuto comandi: ");                  Serial.println(HELP_KEY);
}

void printServosDegrees()
{
    Serial.println("Servos degrees *******************\n");

    Serial.print("Base: ");
    Serial.println(base_deg);

    Serial.print("Spalla: ");
    Serial.println(shoulder_deg);

    Serial.print("Gomito: ");
    Serial.println(elbow_deg);

    Serial.print("Rotazione polso: ");
    Serial.println(wrist_rot_deg);

    Serial.print("Movimento verticale del polso: ");
    Serial.println(wrist_ver_deg);

    Serial.print("Mano: ");
    Serial.println(gripper_deg);
}

void setToVerticalPosition()
{
    base_deg =      BASE_VERTICAL;
    shoulder_deg =  SHOULDER_VERTICAL;
    elbow_deg =     ELBOW_VERTICAL;
    wrist_rot_deg = WRIST_ROT_VERTICAL;
    wrist_ver_deg = WRIST_VER_VERTICAL;
    gripper_deg =   GRIPPER_VERTICAL;
    Serial.println("Set in vertical position");
}

void setup() 
{  
    Braccio.begin();
    Serial.begin(9600);

    /* INITIZATION */
    step_delay = STEP_DELAY_VERTICAL;
    setToVerticalPosition();
    
    count = 0;
    areazioneAttiva = 0;

    /* Sets the Braccio in the VERTICAL position */
    /*                    (step delay,          M1,         M2,            M3,      M4,             M5,             M6);    */
    Braccio.ServoMovement(step_delay,         base_deg, shoulder_deg, elbow_deg, wrist_ver_deg, wrist_rot_deg, gripper_deg);  

    Serial.println("Sono pronto.");
    printCommands();

    pinMode(FAN_PIN,OUTPUT);
    digitalWrite(FAN_PIN,LOW);
}

char readSerialData()
{
    char result;

    if(Serial.available())
    {
        result = Serial.read();

        Serial.print("(");
        Serial.print(count);
        Serial.print(") ");
        count++;
    }

    return result;
}

//buchedessantimà ghelellelle addessellemmaccumascedde
/**
 * degrees: the variable i want to modify
 * bound:   the max/min limit
 * incDec:  if 0 increases degrees in a range between the bound range
 *          if 1 decreases degrees in a range between the bound range
*/
void alimu(int *degrees, int bound, int incDec)
{

    if(incDec == 0)
    {
        if(*degrees < bound)
        {
            *degrees+=DEG_OFFSET;
            Serial.println(*degrees);
        }
        else
        {
            Serial.println("Non posso muovermi ancora!");
        }
    }
    else 
    if (incDec == 1)
    {
        if(*degrees > bound)
        {
            *degrees-=DEG_OFFSET;
            Serial.println(*degrees);
        }
        else
        {
            Serial.println("Non posso muovermi ancora!");
        }
    }
}

void areazione()
{
  if(areazioneAttiva==1)
  {
    digitalWrite(FAN_PIN,LOW);
    areazioneAttiva=0;
  }
  else
  if(areazioneAttiva==0)
  {
    digitalWrite(FAN_PIN,HIGH);
    areazioneAttiva=1;
  }
}

void loop() 
{
    char c;

    c = readSerialData();

    /* M1 */
    if(c == BASE_KEY_INC)
    {
        alimu(&base_deg, BASE_MAX_DEG, 0);
    }

    if(c == BASE_KEY_DEC)
    {
        alimu(&base_deg, BASE_MIN_DEG, 1);
    }

    /* M2 */
    if(c == SHOULDER_KEY_INC)
    {
        alimu(&shoulder_deg, SHOULDER_MAX_DEG, 0);
    }

    if(c == SHOULDER_KEY_DEC)
    {
        alimu(&shoulder_deg, SHOULDER_MIN_DEG, 1);
    }

    /* M3 */
    if(c == ELBOW_KEY_INC)
    {
        alimu(&elbow_deg, ELBOW_MAX_DEG, 0);
    }

    if(c == ELBOW_KEY_DEC)
    {
        alimu(&elbow_deg, ELBOW_MIN_DEG, 1);
    }

    /* M4 */
    if(c == WRIST_ROT_KEY_INC)
    {
        alimu(&wrist_rot_deg, WRIST_ROT_MAX_DEG, 0);
    }

    if(c == WRIST_ROT_KEY_DEC)
    {
        alimu(&wrist_rot_deg, WRIST_ROT_MIN_DEG, 1);
    }

    /* M5 */
    if(c == WRIST_VER_KEY_INC)
    {
        alimu(&wrist_ver_deg, WRIST_VER_MAX_DEG, 0);
    }

    if(c == WRIST_VER_KEY_DEC)
    {
        alimu(&wrist_ver_deg, WRIST_VER_MIN_DEG, 1);
    }

    /* M6 */
    if(c == GRIPPER_KEY_INC)
    {
        alimu(&gripper_deg, GRIPPER_MAX_DEG, 0);
    }

    if(c == GRIPPER_KEY_DEC)
    {
        alimu(&gripper_deg, GRIPPER_MIN_DEG, 1);
    }

    /* WRITE INFORMATION ABOUT SERVOS DEGREES */
    if(c == DEG_INFO_KEY)
    {
        printServosDegrees();
    }

    /* RESET TO THE DEFAULT POSITION */
    if(c == RESET_POSITION_KEY)
    {
        setToVerticalPosition();    // Vertical
    }
  
    if(c == HELP_KEY)
    {
      printCommands();
    } 

  if(c == VENTILATION_KEY)
  {
    areazione();
  }

    // Move the Braccio in selected position
  Braccio.ServoMovement(step_delay, base_deg, shoulder_deg, elbow_deg, wrist_ver_deg, wrist_rot_deg, gripper_deg);  
}
