/***** FILE PROFET_2_12V_Shield_Starting_Software_Button2.ino  ***********
*
* Revison: 001.00
*
* AUTHOR   :     Gerhard A. Wulz
* (c)      :     Copyright by Infineon
*
* USE      :     PROFET+2 12V Arduino Shield (HIGH CURRENT PROFET Shield)-> BTS7002/7004/7006/7008-1EPP  
*                Infineon XMC 1100 Bootkit or XMC 4700/4800 Relax Kit or ShieldBuddyTC275 – AURIX™ 
*                or any Arduino compatible µC board 
*                If the IS sense functionality of the board will be used the used µC board must be able to handle 5V at the ADC inputs!    
*
* COMPILE  :     with compiler from Arduino version 1.8.10
*
* LINK     :     Arduino development environment
*
* DESCRIPE :     PROFET+2 12V Arduino shield - button demo 2
*                Simple demo to demonstrate how to use the optional mounted push-button to switch the LEDs and outputs on the PROFET+2 Arduino shield 
*                Configuration: a: digital mode -> J3 closed, J2 open   -> push button connected to digital GPIO D2
*                               b: analog mode  -> J3 open,   J3 closed -> push button connected to analog Input A0
*                The demo supports both modes
*                
*                Level: beginner, rookie
*
* HISTORY  :     23.10.2019 [Wulz] -> Start of Initial Version (001.00)
*
* LAST UPDATE :  24.10.2019 [Wulz]
*
* OVERVIEW of the available PROFET+2 Arduino Shields:
* BTS7002-1EPP:   Package: PG-TSDSO-14
*                 Nominal load current (TA = 85 °C)           IL(NOM)       =    21 A
*                 Minimum overload detection current          IL(OVL0)-40   =   120 A
*                 Typical current sense ratio at IL = IL(NOM) kILIS         = 22700
*                 Vbb                                                       = max 28V
* 
* BTS7004-1EPP:   Package: PG-TSDSO-14
*                 Nominal load current (TA = 85 °C)           IL(NOM)       =    15 A
*                 Minimum overload detection current          IL(OVL0)-40   =   107 A
*                 Typical current sense ratio at IL = IL(NOM) kILIS         = 20000
*                 Vbb                                                       = max 28V
* 
* BTS7006-1EPP:   Package: PG-TSDSO-14
*                 Nominal load current (TA = 85 °C)           IL(NOM)       =    12.5 A
*                 Minimum overload detection current          IL(OVL0)-40   =    81   A
*                 Typical current sense ratio at IL = IL(NOM) kILIS         = 17700
*                 Vbb                                                       = max 28V
* 
* BTS7008-1EPP:   PackagePG-TSDSO-14
*                 Nominal load current (TA = 85 °C)           IL(NOM)       =    11 A
*                 Minimum overload detection current          IL(OVL0)-40   =    69 A
*                 Typical current sense ratio at IL = IL(NOM) kILIS         = 14500
*                 Vbb                                                       = max 28V 
* 
*                 
* 
***********************************************************************/

#define BAUDRATE 115200          // Serial board Baud Rate = 115200 baud; "standard" baud rates are 1200, 2400, 4800, 9600, 19200, 38400, 57600, and 115200 bps    

// --------------------------------------------------------------------------------------
// OUTPUT PINs DECLARATION of the PROFET+2 12V Arduino Shield             ** CONSTANTS **
// --------------------------------------------------------------------------------------

const int IN_1           = 9;    // select the pin to switch the output on BTS700x U1 
const int IN_2           = 10;   // select the pin to switch the output on BTS700x U2
const int IN_3           = 11;   // select the pin to switch the output on BTS700x U3
const int IN_4           = 3;    // select the pin to switch the output on BTS700x U4

const int LED_1           = 4;   // select the pin for LED 1 (green LED on the PROFET+2 shield)
const int LED_2           = 5;   // select the pin for LED 2 (green LED on the PROFET+2 shield)
const int LED_3           = 12;  // select the pin for LED 3 (green LED on the PROFET+2 shield)
const int LED_4           = 13;  // select the pin for LED 4 (green LED on the PROFET+2 shield)

const int DEN1_DEN3       = 6;   // select the pin to switch the diagnostic enable (ISense) for U1 and U3 on -> U1 can be measured at Analog Input A2, U3 can be measured at Analog Input A3 
const int DEN2_DEN4       = 8;   // select the pin to switch the diagnostic enable (ISense) for U2 and U4 on -> U2 can be measured at Analog Input A2, U4 can be measured at Analog Input A3
const int OLOFF           = 7;   // select the pin for the OPEN LOAD in OFF detection functionality for all 4 channels

// --------------------------------------------------------------------------------------
// PUSH BUTTON ON ARDUINO SHIELD
// --------------------------------------------------------------------------------------

const int pushButton       =  2;        // PIN for reading the push button via Digital Input D2 (if jumper J3 is ON, the push-button and RS_1=82k and C_S1J3=100nF must be assembled)
const int pushButtonAnalog = A0;        // PIN for reading the push button via Analog  Input A0 (if jumper J2 is ON, the push-button and RS_1=82k and C_S1J3=100nF must be assembled)
int pushButtonAnalogVal    =  0;        // variable for reading the Analog pushbutton value (ADC value = 0-1024)

const unsigned int timeout  = 1000;     //  timeout to trigger the status update in ms -> 1000msec = 1sec
unsigned long time_start;               //  variable to hold the timer start time             


// --------------------------------------------------------------------------------------
// ANALOG INPUTS       A0=14 A1=15 A2=16 A3=17 A4=18 A5=19
// --------------------------------------------------------------------------------------
// A0 already reserved for the pushbutton
const int A1_Vbb          = A1;       // select the analog input for measuring Vbb (typical 12V) via voltage divider R9=47k and R11=10k
const int A2_Isense_1_2   = A2;       // select the analog input for measuring ISense from U1 or U2, Rsense RIS1 = 1k, low pass filter: C1=1nF, RSense1 = 4.7k  
const int A3_Isense_3_4   = A3;       // select the analog input for measuring ISense from U3 or U4, Rsense RIS2 = 1k, low pass filter: C2=1nF, RSense3 = 4.7k

// VARIABLES
bool buttonState            = 1;    // button state -> reading of the push button -> pressed or not pressed
bool lastButtonState        = 1;    // last button state 
bool buttonFlag             = 0;    // button toggle flag   
unsigned int  countSM       = 0;    // counter for the number of button presses

struct ArduinoShield                // data structure to remember the output status of each PROFET U1-U4 and the LEDs status on the Arduino Shield  
{
  bool status_In_U1;                // PROFET U1 IN status
  bool status_In_U2;                // PROFET U2 IN status
  bool status_In_U3;                // PROFET U3 IN status
  bool status_In_U4;                // PROFET U4 IN status

  bool status_LED1;                 // LED1 status
  bool status_LED2;                 // LED2 status
  bool status_LED3;                 // LED3 status
  bool status_LED4;                 // LED4 status
};

ArduinoShield Shield = {0,0,0,0,0,0,0,0};    // Initialize the PROFET+2 12V High Side Switch Data Object

typedef enum {                      // the PushButton can be configured to digital or analog mode
  digital = 0,
  analog  = 1,
} PushButton;


// ---------------------------------------------------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(BAUDRATE);           // set the baud rate of the default serial board    

    pinMode(IN_1, OUTPUT);            // Initialize IN1 pin as an output (Profet+2 device U1 -> IN1 
    pinMode(IN_2, OUTPUT);            // Initialize IN2 pin as an output (Profet+2 device U2 -> IN2
    pinMode(IN_3, OUTPUT);            // Initialize IN3 pin as an output (Profet+2 device U3 -> IN3
    pinMode(IN_4, OUTPUT);            // Initialize IN4 pin as an output (Profet+2 device U4 -> IN4

    pinMode(LED_1, OUTPUT);           // Initialize LED_1 pin as an output (LED1, green)
    pinMode(LED_2, OUTPUT);           // Initialize LED_2 pin as an output (LED2, green)
    pinMode(LED_3, OUTPUT);           // Initialize LED_3 pin as an output (LED3, green)
    pinMode(LED_4, OUTPUT);           // Initialize LED_4 pin as an output (LED4, green)

    pinMode(DEN1_DEN3, OUTPUT);       // Initialize DEN1_DEN3 pin as an output 
    pinMode(DEN2_DEN4, OUTPUT);       // Initialize DEN2_DEN4 pin as an output
    pinMode(OLOFF, OUTPUT);           // Initialize OLOFF pin as an output

    pinMode(pushButton, INPUT);       // Initialize the push-button pin as an digital input (if jumper J3 is ON, push-button, RS_1=82k and C_S1J3=100nF assembled)
    //pinMode(pushButtonAnalog, INPUT);       // Initialize the push-button pin as an digital input (if jumper J3 is ON, push-button, RS_1=82k and C_S1J3=100nF assembled)

    // PROFET+2 Arduino Shield -> PROFET+2 U1, U2, U3, U4 initialization
    digitalWrite(DEN1_DEN3, LOW);     // switch diagnostic enable for Profet+2 device U1 and U3 off  
    digitalWrite(DEN2_DEN4, LOW);     // switch diagnostic enable for Profet+2 device U2 and U4 off
    digitalWrite(OLOFF, LOW);         // switch open load detection in state OFF for alle four devices off

    // initialize PROFET+2 Arduino Shield -> switch the output of PROFET+2 U1, U2, U3, U4 OFF and switch LED1 LED2 LED3 LED4 OFF
    writeShieldIN(false, false, false, false);
    writeShieldLED(false, false, false, false);
    
    delay(2000);  // delay to get the possibility to start a serial terminal program after powering the µC board on 

    Serial.println("-----------------------------------------------------");
    Serial.println(" Infineon PROFET+2 12V Arduino Shield HW Rev 5.0     ");
    Serial.println(" Software Version 1.00                               ");
    Serial.println(" PROFET+2 BTS7002-1EPP                               ");
    Serial.println(" PROFET+2 BTS7004-1EPP                               ");
    Serial.println(" PROFET+2 BTS7006-1EPP                               ");
    Serial.println(" PROFET+2 BTS7008-1EPP                     2019-10-23");
    Serial.println("-----------------------------------------------------");

    time_start = millis(); // start the status update timer
}

// ***********************************************************************
// Main Loop  
// ***********************************************************************
void loop() 
{ 
  buttonState=readPushButton(digital);     // reads the push button -> parameter: (analog) or (digital)  default: (digital)

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) 
  {    
    if (buttonState == HIGH)        // if the state has changed, increment the counter
    {
      // if the current state is HIGH then the button changed from off to on:
      countSM++;
      Serial.print("Button pressed - State: ");
      Serial.println(countSM);
    }    
    delay(100);                     // delay a little bit to avoid push button bouncing
  }  
  lastButtonState = buttonState;    // save the current state as the last state for next time through the loop 

  switch(countSM)                   // switch the outputs based on the button counter
  {
    case 0:  // switch all outputs an LEDs on the Arduino Shield OFF 
      writeShieldLED(false, false, false, false);
      writeShieldIN (false, false, false, false);      
    break;  
    case 1:  // switch OUT1 and LED1 ON
      writeShieldLED(true, false, false, false);
      writeShieldIN (true, false, false, false);      
    break;
    case 2:  // switch OUT2 and LED2 ON
      writeShieldLED(false, true, false, false);
      writeShieldIN (false, true, false, false);
    break;
    case 3:  // switch OUT3 and LED3 ON
      writeShieldLED(false, false, true, false);
      writeShieldIN (false, false, true, false);
    break;
    case 4:  // switch OUT4 and LED4 ON
      writeShieldLED(false, false, false, true);
      writeShieldIN (false, false, false, true);
    break;  
    default: // switch all outputs an LEDs on the Arduino Shield OFF      
      countSM=0;
    break;  
  } 

 if(millis()-time_start > timeout) // timer to trigger sending the status 
  {
    printStatus();        // send the IN and LED status 
    time_start=millis();  // reset the timer     
  }
}

// ***********************************************************************
// Function to switch LED1 LED2 LED3 and LED4 on the Arduino Shield ON/OFF
// ***********************************************************************
void writeShieldLED(bool _LED1, bool _LED2, bool _LED3, bool _LED4)
{
  // switch LED1 ON or OFF
  if(_LED1==false)  { digitalWrite(LED_1, LOW);  } 
  else  { digitalWrite(LED_1, HIGH);  }

  // switch LED2 ON or OFF
  if(_LED2==false)  { digitalWrite(LED_2, LOW);  }
  else  { digitalWrite(LED_2, HIGH);  }

  // switch LED3 ON or OFF
  if(_LED3==false)  { digitalWrite(LED_3, LOW);  }
  else  { digitalWrite(LED_3, HIGH);  }

  // switch LED4 ON or OFF
  if(_LED4==false)  { digitalWrite(LED_4, LOW);  }
  else  { digitalWrite(LED_4, HIGH);  }   

  Shield.status_LED1 = _LED1;   // remember LED1 status
  Shield.status_LED2 = _LED2;   // remember LED" status
  Shield.status_LED3 = _LED3;   // remember LED3 status
  Shield.status_LED4 = _LED4;   // remember LED4 status 
}

// ********************************************************************************************
// Function to switch the outputs of the PROFETS U1, U2, U3 and U4 on the Arduino shield ON/OFF
// ********************************************************************************************
void writeShieldIN(bool _In1, bool _In2, bool _In3, bool _In4)
{
  // switch output 1 ON or OFF
  if(_In1==false)  { digitalWrite(IN_1, LOW);    }
  else  { digitalWrite(IN_1, HIGH);  }

  // switch output 2 ON or OFF
  if(_In2==false)  { digitalWrite(IN_2, LOW);  }
  else  { digitalWrite(IN_2, HIGH);  }

  // switch output 3 ON or OFF
  if(_In3==false)  { digitalWrite(IN_3, LOW);  }
  else  { digitalWrite(IN_3, HIGH);  }

  // switch output 4 ON or OFF
  if(_In4==false)  { digitalWrite(IN_4, LOW);  }
  else  { digitalWrite(IN_4, HIGH);  }   

  Shield.status_In_U1 = _In1; // remember PROFET U1 output status
  Shield.status_In_U2 = _In2; // remember PROFET U2 output status
  Shield.status_In_U3 = _In3; // remember PROFET U3 output status
  Shield.status_In_U4 = _In4; // remember PROFET U4 output status   
}


// ************************************************************************************************
// Function to read the analog or digital push button on the PROFET+2 Arduino shield
// Configuration:
// Digital reading of the push button: J3 closed, J2 open
// Analog reading of the push button:  J2 closed, J3 open
// ************************************************************************************************
bool readPushButton(PushButton button)
{ 
  bool ButtonState;

  pushButtonAnalogVal  = analogRead(pushButtonAnalog);    // read the analog push button state from the PROFET+2 Arduino Shield

  switch(button)
   {
    case 0:
      ButtonState   = digitalRead(pushButton);            // read the digital push button state from the PROFET+2 Arduino Shield
    break;
    case 1:
      if(pushButtonAnalogVal > 700)                       // if the ADC value is > 700 the pull up resistor provides 5V to the analog input  
         ButtonState = 1;                                 // analog button released - ADC value > 700 -> 5V
      else
         ButtonState = 0;                                 // analog button pressed  - ADC value < 700 -> 0V (connection to GND)
    break;
    default:
      ButtonState   = digitalRead(pushButton);            // read the digital push button state from the PROFET+2 Arduino Shield
    break;
   }   

 return (ButtonState);                                    // analog or digital pressed button detected  
}  

// ************************************************************************************************
// Function to send the status of the 4 LEDs and 4 Outputs of the Arduino shield to the serial port
// ************************************************************************************************
void printStatus(void)
{
 Serial.print("OUT1: ");
 Serial.print(Shield.status_In_U1);
 Serial.print("  OUT2: ");
 Serial.print(Shield.status_In_U2);
 Serial.print("  OUT3: ");
 Serial.print(Shield.status_In_U3);
 Serial.print("  OUT4: ");
 Serial.print(Shield.status_In_U4);

 Serial.print("      LED1: ");
 Serial.print(Shield.status_LED1);
 Serial.print("  LED2: ");
 Serial.print(Shield.status_LED2);
 Serial.print("  LED3: ");
 Serial.print(Shield.status_LED3);
 Serial.print("  LED4: ");
 Serial.print(Shield.status_LED4);
 Serial.print("    Button Analog - ADC 0: ");
 Serial.println(pushButtonAnalogVal); 
}
