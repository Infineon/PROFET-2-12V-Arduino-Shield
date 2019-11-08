/***** FILE PROFET_2_12V_Shield_Starting_Software_ADC1.ino  ***********
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
* DESCRIPE :     PROFET+2 12V Arduino shield - ADC demo (based on button demo 3)
*                Simple ADC sense measurment included, the measured data are available via the Arduino IDE Serial Monitor
*                Diagnostic not included
*                
*                Level: beginner 
*
* HISTORY  :     27.10.2019 [Wulz] -> Start of Initial Version (001.00)
*
* LAST UPDATE :  29.10.2019 [Wulz]
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

#define BAUDRATE 115200             // Serial board Baud Rate = 115200 baud; "standard" baud rates are 1200, 2400, 4800, 9600, 19200, 38400, 57600, and 115200 bps    

// Select the PROFET+2 Arduino shield ---------------------------------------------------
#define   BTS7002                 // Arduino Shield >PROFET+2 12V BTS7002-1EPP selected 
// #define   BTS7004                // Arduino Shield >PROFET+2 12V BTS7004-1EPP selected
// #define   BTS7006                // Arduino Shield >PROFET+2 12V BTS7006-1EPP selected 
// #define   BTS7008                // Arduino Shield >PROFET+2 12V BTS7008-1EPP selected

// --------------------------------------------------------------------------------------
// OUTPUT PINs DECLARATION of the PROFET+2 12V Arduino Shield             ** CONSTANTS **
// --------------------------------------------------------------------------------------

const int IN_1           = 9;       // select the pin to switch the output on BTS700x U1 
const int IN_2           = 10;      // select the pin to switch the output on BTS700x U2
const int IN_3           = 11;      // select the pin to switch the output on BTS700x U3
const int IN_4           = 3;       // select the pin to switch the output on BTS700x U4

const int LED_1           = 4;      // select the pin for LED 1 (green LED on the PROFET+2 shield)
const int LED_2           = 5;      // select the pin for LED 2 (green LED on the PROFET+2 shield)
const int LED_3           = 12;     // select the pin for LED 3 (green LED on the PROFET+2 shield)
const int LED_4           = 13;     // select the pin for LED 4 (green LED on the PROFET+2 shield)

const int DEN1_DEN3       = 6;      // select the pin to switch the diagnostic enable (ISense) for U1 and U3 on -> U1 can be measured at Analog Input A2, U3 can be measured at Analog Input A3 
const int DEN2_DEN4       = 8;      // select the pin to switch the diagnostic enable (ISense) for U2 and U4 on -> U2 can be measured at Analog Input A2, U4 can be measured at Analog Input A3
const int OLOFF           = 7;      // select the pin for the OPEN LOAD in OFF detection functionality for all 4 channels

// --------------------------------------------------------------------------------------
// PUSH BUTTON ON ARDUINO SHIELD
// --------------------------------------------------------------------------------------

const int pushButton       =  2;                // PIN for reading the push button via Digital Input D2 (if jumper J3 is ON, the push-button and RS_1=82k and C_S1J3=100nF must be assembled)
const int pushButtonAnalog = A0;                // PIN for reading the push button via Analog  Input A0 (if jumper J2 is ON, the push-button and RS_1=82k and C_S1J3=100nF must be assembled)
int pushButtonAnalogVal    =  0;                // variable for reading the Analog pushbutton value (ADC value = 0-1024)

const unsigned int timeout  = 1500;             //  timeout to trigger the status update in ms -> 1000msec = 1sec
unsigned long time_start;                       //  variable to hold the timer start time             


// --------------------------------------------------------------------------------------
// ANALOG INPUTS       A0=14 A1=15 A2=16 A3=17 A4=18 A5=19
// --------------------------------------------------------------------------------------
// A0 already reserved for the pushbutton
const int A1_Vbb                  = A1;        // select the analog input for measuring Vbb (typical 12V) via voltage divider R9=47k and R11=10k
const int A2_Isense_1_2           = A2;        // select the analog input for measuring ISense from U1 or U2, Rsense RIS1 = 1k, low pass filter: C1=1nF, RSense1 = 4.7k  
const int A3_Isense_3_4           = A3;        // select the analog input for measuring ISense from U3 or U4, Rsense RIS2 = 1k, low pass filter: C2=1nF, RSense3 = 4.7k

// VARIABLES
bool buttonState                  = 1;         // button state -> reading of the push button -> pressed or not pressed
bool lastButtonState              = 1;         // last button state 
bool buttonFlag                   = 0;         // button toggle flag   
unsigned int  countSM             = 0;         // counter for the number of button presses

// --------------------------------------------------------------------------------------
// PROFET+2 12V Arduino shield specific data
// --------------------------------------------------------------------------------------
struct ArduinoShield                            // data structure to remember the output status of each PROFET U1-U4 and the LEDs status on the Arduino Shield  
{
  bool status_In_U1                = 0;         // PROFET U1 IN status
  bool status_In_U2                = 0;         // PROFET U2 IN status
  bool status_In_U3                = 0;         // PROFET U3 IN status
  bool status_In_U4                = 0;         // PROFET U4 IN status

  bool status_LED1                 = 0;         // LED1 status
  bool status_LED2                 = 0;         // LED2 status
  bool status_LED3                 = 0;         // LED3 status
  bool status_LED4                 = 0;         // LED4 status

  float max_adc_reading             = 1024.0;   // ADC = 10 bit -> 0x400 -> 1024 -> 1 digit = 0.0048828125V resolution
  float adc_reference_voltage       = 5.0;      // ADC reference voltage = 5V  
  
  int A0_adc_raw_value_PushButton = 0;          // ADC raw value reading Analog Input A0 (push botton)
  int A1_adc_raw_value_Vbb        = 0;          // ADC raw value reading Analog Input A1 (supply voltage VS)
  int A2_adc_raw_value_IS_1       = 0;          // ADC raw value reading Analog Input A2 (IS sense PROFET+2 U1)
  int A2_adc_raw_value_IS_2       = 0;          // ADC raw value reading Analog Input A2 (IS sense PROFET+2 U2)
  int A3_adc_raw_value_IS_3       = 0;          // ADC raw value reading Analog Input A3 (IS sense PROFET+2 U3)
  int A3_adc_raw_value_IS_4       = 0;          // ADC raw value reading Analog Input A3 (IS sense PROFET+2 U4)

  // enter the calibration data if available
  float A1_Vbb_Offset             = 0.0;        // Vbb offset
  float A1_Vbb_Gain               = 1.045;       // Vbb gain

  // enter the calibration data if available
  float U1_Ampere_Offset          = 0.0;        // PROFET+2 U1 offset -> A2_IS_1_Ampere_Offset
  float U1_Ampere_Gain            = 1.08;       // PROFET+2 U1 gain   -> A2_IS_1_Ampere_Gain
  float U2_Ampere_Offset          = 0.0;        // PROFET+2 U2 offset -> A2_IS_2_Ampere_Offset
  float U2_Ampere_Gain            = 1.08;       // PROFET+2 U2 gain   -> A2_IS_2_Ampere_Gain
  float U3_Ampere_Offset          = 0.0;        // PROFET+2 U3 offset -> A3_IS_3_Ampere_Offset
  float U3_Ampere_Gain            = 1.08;       // PROFET+2 U3 gain   -> A3_IS_3_Ampere_Gain
  float U4_Ampere_Offset          = 0.0;        // PROFET+2 U4 offset -> A3_IS_4_Ampere_Offset
  float U4_Ampere_Gain            = 1.08;       // PROFET+2 U4 gain   -> A3_IS_4_Ampere_Gain

  
  float Vbb_Resistor_1            = 47000.0;    // 47kOhm -> check in the schematic resistor R9  (user manual v1.00 page 15 -> 2.5.3 and page 23 area D2/D3/E2/E3)
  float Vbb_Resistor_2            = 10000.0;    // 10kOhm -> check in the schematic resistor R11
  float A1_Vbb_ADC_Voltage        = 0.0;        // Vbb raw voltage data measured from ADC A1 (0-5V)
  float A1_Vbb_Real_Voltage       = 0.0;        // Vbb voltage (e.g. 12V)
  float A1_Vbb_Real_Voltage_Filter= 0.0;        // Vbb voltage with Digital Filter

  float U1_IS_Voltage             = 0.0;        // PROFET+2 U1 measured voltage -> A2_ADC_Voltage_IS_1
  float U2_IS_Voltage             = 0.0;        // PROFET+2 U2 measured voltage -> A2_ADC_Voltage_IS_2 
  float U3_IS_Voltage             = 0.0;        // PROFET+2 U3 measured voltage -> A3_ADC_Voltage_IS_3
  float U4_IS_Voltage             = 0.0;        // PROFET+2 U4 measured voltage -> A3_ADC_Voltage_IS_4
  
  float U1_IS_Ampere              = 0.0;        // PROFET+2 U1 calcualted ampere -> A2_IS_1_Ampere
  float U2_IS_Ampere              = 0.0;        // PROFET+2 U2 calcualted ampere -> A2_IS_2_Ampere
  float U3_IS_Ampere              = 0.0;        // PROFET+2 U3 calcualted ampere -> A3_IS_3_Ampere
  float U4_IS_Ampere              = 0.0;        // PROFET+2 U4 calcualted ampere -> A3_IS_4_Ampere

  float U1_IS_Ampere_Average      = 0.0;        // PROFET+2 U1 calcualted ampere average -> A2_IS_1_Ampere_Average
  float U2_IS_Ampere_Average      = 0.0;        // PROFET+2 U2 calcualted ampere average -> A2_IS_2_Ampere_Average
  float U3_IS_Ampere_Average      = 0.0;        // PROFET+2 U3 calcualted ampere average -> A3_IS_3_Ampere_Average
  float U4_IS_Ampere_Average      = 0.0;        // PROFET+2 U4 calcualted ampere average -> A3_IS_4_Ampere_Average
  
  #ifdef BTS7002 
  const int kilis                 = 22700;      // typical kilis for the BTS7002
  #endif  

  #ifdef BTS7004 
  const int kilis                 = 20000;      // typical kilis for the BTS7004
  #endif 

  #ifdef BTS7006 
  const int kilis                 = 17700;      // typical kilis for the BTS7006
  #endif 

  #ifdef BTS7008 
  const int kilis                 = 14500;      // typical kilis for the BTS7008
  #endif 
};

ArduinoShield Shield;                           // Initialize the PROFET+2 12V High Side Switch Data Object

typedef enum {                                  // the PushButton can be configured to digital or analog mode
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
    Serial.println(" Software Version 1.00                SW Button3     ");
    Serial.println(" PROFET+2 BTS7002-1EPP                               ");
    Serial.println(" PROFET+2 BTS7004-1EPP                               ");
    Serial.println(" PROFET+2 BTS7006-1EPP                               ");
    Serial.println(" PROFET+2 BTS7008-1EPP                     2019-10-29");    
    Serial.println("-----------------------------------------------------");

    time_start = millis(); // start the status update timer
}

// ***********************************************************************
// Main Loop  
// ***********************************************************************
void loop() 
{ 

  if(CheckSerialCommand())                 // check if a permissible character from serial terminal received, usable commands are: '1' or '+' or 'p'
     countSM++;                            // switch to next output pattern
     
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

 read_ADC();  // read PROFET+2 sense signal from device U1, U2, U3, and U4

 if(millis()-time_start > timeout) // timer to trigger sending the status 
  {
    printStatus();        // send the IN and LED status 
    printADC();
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

// ************************************************************************************************
// Function to check if there is a cmd/character from serial port aivable
// ************************************************************************************************
bool CheckSerialCommand(void)
{
 bool cmd=false;
 
 if(Serial.available())
  {
    char incomingChar = Serial.read();              // Reads incoming serial data, returns the first byte of incoming data available
    switch(incomingChar)                            // checks permissible incoming characters
    {
     case '+':
       Serial.println("received command: +");
       cmd=true;
     break;
     case '1':
       Serial.println("received command: 1");
       cmd=true;
     break;
     case 'p':
       Serial.println("received command: p");
       cmd=true;
     break;
     default:
       Serial.println("received command: unknown");
       cmd=false;
     break;
    }
  }
 return (cmd);
} 

// ************************************************************************************************
// Function to read the ADC values from channel A0 - A3 and to calculate the VS [V] and IS1-4 [A]
// ************************************************************************************************
void read_ADC(void)
{
    //Serial.println("=====================================");
    //Serial.print("Vbb ");
    Shield.A0_adc_raw_value_PushButton = analogRead(pushButtonAnalog);  // push putton analog value
    Shield.A1_adc_raw_value_Vbb = analogRead(A1_Vbb);                   // Vbb raw value
     
    digitalWrite(DEN1_DEN3, HIGH);                                      // Select sense signal from PROFET+2 device U1 and U3 (DEN1_DEN3= high,DEN2_DEN4 = low) 
    digitalWrite(DEN2_DEN4, LOW);                                       // Select sense signal from PROFET+2 device U1 and U3
    digitalWrite(OLOFF, LOW);                                           // open load in state OFF diagnosis disabled

    // Attention: notice the PROFET+2 12V switching / sense timing (e.g. datasheet BTS7002-1EPP -> page 21, 25, 43, 44  
    // Switch-ON Delay                                       tON(DELAY)     : 130µs
    // Switch-ON Time                                        tON            : 210µs
    // Switch-OFF Delay                                      tOFF(DELAY)    : 160µs
    // Switch-OFF Time                                       tOFF           : 220µs
    //
    // SENSE Open Load in OFF Delay Time                     tIS(OLOFF)_D   : 300µs
    // SENSE Settling Time with Nominal Load Current Stable  tsIS(ON)       :  40µs
    // SENSE Disable Time                                    tsIS(OFF)      :  20µs
    // SENSE Settling Time after Load Change                 tsIS(CL)       :  20µs   
    // tsIS(DIAG) <= 3 x (tON_max + tsIS(ON)_max)
    // Attention: if diagnosis of open load in OFF is used -> consider the SENSE Open Load in OFF Delay Time
    
    delay(1);                                                           // generously delay of 1ms to give the PROFET+2 time to provide the sense signal 
                                                                        // tsIS(DIAG) <= 3 x (tON_max + tsIS(ON)_max) = 3 x (210µs + 40) = 750µs!
                                                                        
    Shield.A2_adc_raw_value_IS_1 = analogRead(A2_Isense_1_2);           // read analog value from analog input A2 -> device U1 -> sense IS1 
    
    delay(1);                                                           // generously delay of 1ms to give the the PROFET+2 time to provide the sense signal  
    Shield.A3_adc_raw_value_IS_3 = analogRead(A3_Isense_3_4);           // read analog value from analog input A3 -> device U3 -> sense IS3
    
    digitalWrite(DEN1_DEN3, LOW);                                       // Select sense signal from PROFET+2 device U2 and U4 (DEN1_DEN3= low, DEN2_DEN4 = high)
    digitalWrite(DEN2_DEN4, HIGH);                                      // Select sense signal from PROFET+2 device U2 and U4

    delay(1);                                                           // generously delay of 1ms to give the PROFET+2 time to provide the sense signal  
    Shield.A2_adc_raw_value_IS_2 = analogRead(A2_Isense_1_2);           // read analog value from analog input A2 -> device U2 -> sense IS2 

    delay(1);                                                           // generously delay of 1ms to give the PROFET+2 time to provide the sense signal  
    Shield.A3_adc_raw_value_IS_4 = analogRead(A3_Isense_3_4);           // read analog value from analog input A3 -> device U4 -> sense IS4 
        
    // Calculate ADC voltage for analog input A1 -> Vbb
    Shield.A1_Vbb_ADC_Voltage = float (Shield.A1_adc_raw_value_Vbb / Shield.max_adc_reading * Shield.adc_reference_voltage);                    // e.g. 1024/1024*5 -> 5V max
    Shield.A1_Vbb_ADC_Voltage = (Shield.A1_Vbb_ADC_Voltage - Shield.A1_Vbb_Offset) * Shield.A1_Vbb_Gain;                                        // e.g. (5V - 0V) * 1 = 5V 
    Shield.A1_Vbb_Real_Voltage = float ((Shield.A1_Vbb_ADC_Voltage * (Shield.Vbb_Resistor_1 + Shield.Vbb_Resistor_2)) / Shield.Vbb_Resistor_2); // e.g. (5V *(47k + 10k))/10k = 28.5V max measurable VS
    
    // Calculate ADC voltage for analog input A2 -> ISense 1 
    Shield.U1_IS_Voltage = Shield.A2_adc_raw_value_IS_1 / Shield.max_adc_reading * Shield.adc_reference_voltage;                                // e.g. 1024/1024*5 -> 5V
    Shield.U1_IS_Ampere = float ((Shield.U1_IS_Voltage * Shield.kilis) / 1000);                                                                 // e.g. (5*22700)/1000 -> 113.5 A max
    Shield.U1_IS_Ampere = (Shield.U1_IS_Ampere - Shield.U1_Ampere_Offset) * Shield.U1_Ampere_Gain;                                              // (113.5 - 0) * 1 = 113.5A    

    // Calculate ADC voltage for analog input A2 -> ISense 2 
    Shield.U2_IS_Voltage = Shield.A2_adc_raw_value_IS_2 / Shield.max_adc_reading * Shield.adc_reference_voltage;    // e.g. calculation for 10A // e.g. (91/1024)*5 = 0,4443359375 
    Shield.U2_IS_Ampere = float ((Shield.U2_IS_Voltage * Shield.kilis) / 1000);                                                                 // e.g. (0,4443359375*22700)/1000 = 10,086 A
    Shield.U2_IS_Ampere = (Shield.U2_IS_Ampere - Shield.U2_Ampere_Offset) * Shield.U2_Ampere_Gain;                                              // e.g. (10,086-0) * 1 = 10,086A
 
    // Calculate ADC voltage for analog input A3 -> ISense 3 
    Shield.U3_IS_Voltage = Shield.A3_adc_raw_value_IS_3 / Shield.max_adc_reading * Shield.adc_reference_voltage;   // e.g. calculation for 2.2A // e.g. (20/1024)*5 = 0,09765625
    Shield.U3_IS_Ampere = float ((Shield.U3_IS_Voltage * Shield.kilis) / 1000);                                                                 // (0,09765625*22700)/1000 = 2,216796 A
    Shield.U3_IS_Ampere = (Shield.U3_IS_Ampere - Shield.U3_Ampere_Offset) * Shield.U3_Ampere_Gain;                                              // e.g. (2,2167-0) * 1 = 2,2167A
      
    // Calculate ADC voltage for analog input A3 -> ISense 4 
    Shield.U4_IS_Voltage = Shield.A3_adc_raw_value_IS_4 / Shield.max_adc_reading * Shield.adc_reference_voltage;
    Shield.U4_IS_Ampere = float ((Shield.U4_IS_Voltage * Shield.kilis) / 1000);
    Shield.U4_IS_Ampere = (Shield.U4_IS_Ampere - Shield.U4_Ampere_Offset) * Shield.U4_Ampere_Gain;

    digitalWrite(OLOFF, LOW); 
}

// ************************************************************************************************
// Function to send the ADC measurement of the Arduino shield to the serial port
// ************************************************************************************************
void printADC(void)
{
        Serial.println("===================================================================");
         Serial.print(" Vbb   - ");
         Serial.print(" ADC raw value: ");
         Serial.print(Shield.A1_adc_raw_value_Vbb);
         Serial.print("  ADC-Voltage (0-5V): ");
         Serial.print(Shield.A1_Vbb_ADC_Voltage);
         Serial.print("  Vbb-Voltage         : ");
         Serial.println(Shield.A1_Vbb_Real_Voltage); 
   
         //Serial.println("=====================================================");     
         Serial.print(" Out 1 - ");
         Serial.print(" ADC raw value: ");
         Serial.print(Shield.A2_adc_raw_value_IS_1);
         Serial.print("  ADC-Voltage (0-5V): ");
         Serial.print(Shield.U1_IS_Voltage);
         Serial.print("  Sense current OUT 1 : ");
         Serial.println(Shield.U1_IS_Ampere); 
    
         Serial.print(" Out 2 - ");
         Serial.print(" ADC raw value: ");
         Serial.print(Shield.A2_adc_raw_value_IS_2);
         Serial.print("  ADC-Voltage (0-5V): ");
         Serial.print(Shield.U2_IS_Voltage);
         Serial.print("  Sense current OUT 2 : ");
         Serial.println(Shield.U2_IS_Ampere); 
    
         Serial.print(" Out 3 - ");
         Serial.print(" ADC raw value: ");
         Serial.print(Shield.A3_adc_raw_value_IS_3);
         Serial.print("  ADC-Voltage (0-5V): ");
         Serial.print(Shield.U3_IS_Voltage);
         Serial.print("  Sense current OUT 3 : ");
         Serial.println(Shield.U3_IS_Ampere); 
    
         Serial.print(" Out 4 - ");
         Serial.print(" ADC raw value: ");
         Serial.print(Shield.A3_adc_raw_value_IS_4);
         Serial.print("  ADC-Voltage (0-5V): ");
         Serial.print(Shield.U4_IS_Voltage);
         Serial.print("  Sense current OUT 4 : ");
         Serial.println(Shield.U4_IS_Ampere); 
}
