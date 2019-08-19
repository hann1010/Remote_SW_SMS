/*******************************
Remote SW with ArdUnio board and Sim900 GSM shield
Hanna Päivästö 2018
*******************************/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>

/*-----( Declare Variables )-----*/
char ReadTmp;
String inputString = "";
String PhoneNro = "";
int col = 0;
int IndicationState;  //For input read output
//int ManualSWState;

/*-----( Declare Constants )-----*/
const int Relay = 7;   // Relay pin 7
const int Indication = 6;   // Relay indication pin 6
const int ManualSW = 5;   // Manual switch pin 5 Manual SW not implement yet!

/*-----( Declare objects )-----*/
SoftwareSerial mySerial(3, 2); // 3=TX 2=RX in GSM shield side



void setup()  /*----( SETUP: RUNS ONCE )----*/
{
  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(5000);          // Waiting for Sim900 booting up
  mySerial.println("AT+CPIN=\"9009\"\r"); // Set Pin code
  Serial.println("Recieve Setup... Done ");

  mySerial.println("AT+CMGF=1"); // turn to text mode
  mySerial.println("AT+CMGD=1,1"); // Delete all read SMS from Sim card
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  Serial.println("SMS Setup... Done ");
 
    
  pinMode(Relay, OUTPUT);    // initialize the digital pin as an output.
  digitalWrite(Relay, HIGH);
  pinMode(Indication, INPUT_PULLUP); // initialize the digital pins to input and pullup modes
  pinMode(ManualSW, INPUT_PULLUP);
  Serial.println("All Setup... Done ");
}


void loop()  /*----( LOOP: RUNS CONSTANTLY )----*/
{
//  IndicationState = digitalRead(Indication);
//  ManualSWState = digitalRead(ManualSW);

   if (Serial.available()>0)
    switch(Serial.read())
    {
    
    case 'l':  // Print out last phone number
      LastNumber();
      break;
    }

  if (mySerial.available()>0)
  {
    ReadTmp=(mySerial.read());
    inputString += ReadTmp;
    if (ReadTmp == '\n') 
    {
    for (col = 0 ;col < 200; col++) 
        { 
            if (inputString.substring(col,(col+3)) == "#On") 
                {
                    Serial.println("Set On...   "); 
                    digitalWrite(Relay, LOW);
                    SendMessage();
                } 
            if (inputString.substring(col,(col+4)) == "#Off") 
                {
                    Serial.println("Set Off...   "); 
                    digitalWrite(Relay, HIGH);
                    SendMessage();
                } 
            if (inputString.substring(col,(col+7)) == "#Status") 
                {
                    Serial.println("Send Status...   ");
                    SendMessage();
                    
                } 
            if (inputString.substring(col,(col+4)) == "+358") // Reading phone number, only finnish number
                {
                  Serial.println("Phone nro...   "); 
                  PhoneNro = "";
                  PhoneNro = inputString.substring(col,(col+13)); // Read Phone Nro from the string
                  Serial.print(PhoneNro);
                } 
        
        }
        
        
      Serial.println(inputString);
      // clear the string:
      inputString = "";
    }
 
  }
}


void SendMessage()
{
  Serial.print("Sending message... ");
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
 // mySerial.println("AT+CMGS=\"x\"\r"); // Replace x with mobile number
  mySerial.print("AT+CMGS="); // Set phone nro 
  mySerial.print(char(34));  //***  ASCII code of "
  mySerial.print(PhoneNro);  //***
  mySerial.println(char(34));// End of setting phone nro
  delay(1000);
  mySerial.println("GSM SW Box status is...  ");// The SMS text you want to send
  delay(1000);
  
  IndicationState = digitalRead(Indication); // read the indication pin state
//  ManualSWState = digitalRead(ManualSW);

  if (IndicationState == LOW)
    {
        mySerial.println("SW On");// The SMS text you want to send
        Serial.println("SW On");// The Local text you want to send
    }
  else
    {
        mySerial.println("SW Off");// The SMS text you want to send
        Serial.println("SW Off");// The Local text you want to send
    }
    
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
 
 
}


void LastNumber() // Read last phone number!
{
  Serial.println("Last GSM number ");
  Serial.print(PhoneNro);
  delay(1000);
}
// EOF
