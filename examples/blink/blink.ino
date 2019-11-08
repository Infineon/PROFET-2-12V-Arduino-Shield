/***** FILE PROFET_2_12V_Shield_Starting_Software_Blink1.ino  ***********
*
* Revison: 001.00
*
* AUTHOR   :     Gerhard A. Wulz
* (c)      :     Copyright by Infineon
*
* USE      :     PROFET+2 12V Arduino Shield (HIGH CURRENT PROFET Shield)-> BTS7002/7004/7006/7008-1EPP  
*                Infineon XMC 1100 Bootkit or XMC 4700/4800 Relax Kit or ShieldBuddyTC275 – AURIX™ 
*                or any Arduino compatible µC board 
*                If the IS sense functionality of the board will be used the µC board must be able to handle 5V at the ADC inputs    
*
* COMPILE  :     with compiler from Arduino version 1.8.10
*
* LINK     :     Arduino development environment
*
* DESCRIPE :     PROFET+2 12V Arduino shield - 4 LEDs and 4 OUTPUTS blink demo 
*                Simple demo to demonstarte how to control the 4 LEDs and the 4 Outputs of the Profet+2 Shield
*                Level: beginner
*
* Functions:     setup(),loop(),
*
*
* HISTORY  :     08.08.2019 [Wulz] -> Start of Initial Version (001.00)
*
* LAST UPDATE :  23.10.2019 [Wulz]
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


// --------------------------------------------------------------------------------------
// OUTPUT PINs DECLARATION of the PROFET+2 12V Arduino Shield 
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
// ANALOG INPUTS 
// --------------------------------------------------------------------------------------
const int A0_pushButton   = A0; // select the analog input for the push button via Analog Input A0 (if jumper J2 is ON and if the push-button, RS_1=82k and C_S1J3=100nF is assembled)
const int A1_Vbb          = A1; // select the analog input for measuring Vbb (typical 12V) via voltage divider R9=47k and R11=10k
const int A2_Isense_1_2   = A2; // select the analog input for measuring ISense from U1 or U2, Rsense RIS1 = 1k, low pass filter: C1=1nF, RSense1 = 4.7k  
const int A3_Isense_3_4   = A3; // select the analog input for measuring ISense from U3 or U4, Rsense RIS2 = 1k, low pass filter: C2=1nF, RSense3 = 4.7k

const unsigned int stepDelay  = 1000;  // delay in ms -> 1000msec = 1sec


// ---------------------------------------------------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);        // Serial board Baud Rate = 115200 bps (bits per second); "standard" baud rates are 1200, 2400, 4800, 9600, 19200, 38400, 57600, and 115200 bps    

    pinMode(IN_1, OUTPUT);       // Initialize IN1 pin as an output (Profet+2 device U1 -> IN1 
    pinMode(IN_2, OUTPUT);       // Initialize IN2 pin as an output (Profet+2 device U2 -> IN2
    pinMode(IN_3, OUTPUT);       // Initialize IN3 pin as an output (Profet+2 device U3 -> IN3
    pinMode(IN_4, OUTPUT);       // Initialize IN4 pin as an output (Profet+2 device U4 -> IN4

    pinMode(LED_1, OUTPUT);      // Initialize LED_1 pin as an output (LED1, green)
    pinMode(LED_2, OUTPUT);      // Initialize LED_2 pin as an output (LED2, green)
    pinMode(LED_3, OUTPUT);      // Initialize LED_3 pin as an output (LED3, green)
    pinMode(LED_4, OUTPUT);      // Initialize LED_4 pin as an output (LED4, green)

    pinMode(DEN1_DEN3, OUTPUT);  // Initialize DEN1_DEN3 pin as an output 
    pinMode(DEN2_DEN4, OUTPUT);  // Initialize DEN2_DEN4 pin as an output
    pinMode(OLOFF, OUTPUT);      // Initialize OLOFF pin as an output
    
    delay(2000);

    Serial.println("-----------------------------------------------------");
    Serial.println(" Infineon PROFET+2 12V Arduino Shield HW Rev 5.0     ");
    Serial.println(" Software Version 1.00                               ");
    Serial.println(" PROFET+2 BTS7002-1EPP                               ");
    Serial.println(" PROFET+2 BTS7004-1EPP                               ");
    Serial.println(" PROFET+2 BTS7006-1EPP                               ");
    Serial.println(" PROFET+2 BTS7008-1EPP                     2019-10-23");
    Serial.println("-----------------------------------------------------");

    // PROFET+2 Arduino Shield -> PROFET+2 U1, U2, U3, U4 initialization
    digitalWrite(DEN1_DEN3, LOW);     // switch diagnostic enable for Profet+2 device U1 and U3 off  
    digitalWrite(DEN2_DEN4, LOW);     // switch diagnostic enable for Profet+2 device U2 and U4 off
    digitalWrite(OLOFF, LOW);         // switch open load detection in state OFF for alle four devices off
    
    Serial.println("OUT1 OUT2 OUT3 OUT4   LED1 LED2 LED3  LED4");
}

// ***********************************************************************
// Main Loop  
// ***********************************************************************
void loop() 
{   
    delay( stepDelay );
    digitalWrite(IN_1, HIGH);         // switch the output of PROFET U1 ON
    digitalWrite(IN_2, HIGH);         // switch the output of PROFET U2 ON
    digitalWrite(IN_3, HIGH);         // switch the output of PROFET U3 ON
    digitalWrite(IN_4, HIGH);         // switch the output of PROFET U4 ON
    
    digitalWrite(LED_1, HIGH);        // switch LED1 on the PROFET+2 12V Arduino shield ON
    digitalWrite(LED_2, HIGH);        // switch LED2 ON the PROFET+2 12V Arduino shield ON
    digitalWrite(LED_3, HIGH);        // switch LED3 ON the PROFET+2 12V Arduino shield ON
    digitalWrite(LED_4, HIGH);        // switch LED4 ON the PROFET+2 12V Arduino shield ON

    Serial.println("   x    x    x    x      x    x    x     x");
    
    delay( stepDelay );
    digitalWrite(IN_1, LOW);          // switch the output of PROFET U1 OFF
    digitalWrite(IN_2, LOW);          // switch the output of PROFET U2 OFF
    digitalWrite(IN_3, LOW);          // switch the output of PROFET U3 OFF
    digitalWrite(IN_4, LOW);          // switch the output of PROFET U4 OFF

    digitalWrite(LED_1, LOW);         // switch LED1 on the PROFET+2 12V Arduino shield OFF
    digitalWrite(LED_2, LOW);         // switch LED2 on the PROFET+2 12V Arduino shield OFF
    digitalWrite(LED_3, LOW);         // switch LED3 on the PROFET+2 12V Arduino shield OFF
    digitalWrite(LED_4, LOW);         // switch LED4 on the PROFET+2 12V Arduino shield OFF
    
    Serial.println("   -    -    -    -      -    -    -     -");
        
    delay( stepDelay );
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);

    digitalWrite(LED_1, HIGH);
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_4, LOW);

    Serial.println("   x    -    -    -      x    -    -     -");
        
    delay( stepDelay );
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
    
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, HIGH);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_4, LOW);

    Serial.println("   -    x    -    -      -    x    -     -"); 
       
    delay( stepDelay );
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);

    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_3, HIGH);
    digitalWrite(LED_4, LOW);

    Serial.println("   -    -    x    -      -    -    x     -");
        
    delay( stepDelay );
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);

    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_4, HIGH);

    Serial.println("   -    -    -    x      -    -    -     x");
        
    delay( stepDelay );
    digitalWrite(IN_4, LOW);
    digitalWrite(LED_4, LOW);

    Serial.println("   -    -    -    -      -    -    -     -");

}

// *****************************************************************
