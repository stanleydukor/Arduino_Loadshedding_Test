 /*
Measuring AC Current Using ACS712
*/
#include <LiquidCrystal.h> // Including the LCD display library

const int sensorIn1 = A0; // Current Sensor for load 1
const int sensorIn2 = A1; // Current Sensor for load 2
const int sensorIn3 = A2; // Current Sensor for load 3

const int LED=12; // Indicator LED 
const int BUTTON=2; // Button to start varying Frequency
boolean lastButton = LOW; // Variable containing the previous button state
boolean currentButton = LOW; // Variable containing the current button state
  
int relay1 = 9; // Relay for load 1
int relay2 = 10; // Relay for load 2
int relay3 = 11; // Relay for load 3

const float POT=A3; // Potentiometer pin (Acting as frequency variator
const int LOWER_BOUND=50; // Lower Threshold
const int UPPER_BOUND=900; // Upper Threshold
float val = 0; // Variable to hold analog reading

int mVperAmp1 = 66; // use 100 for 20A Module and 66 for 30A Module (Load 1)
int mVperAmp2 = 100; // use 100 for 20A Module and 66 for 30A Module (Load 2)
int mVperAmp3 = 100; // use 100 for 20A Module and 66 for 30A Module (Load 3)
int maxValue1 = 0;          // store max value here (Load 1)
int minValue1 = 1024;          // store min value here (Load 1)
int maxValue2 = 0;          // store max value here (Load 2)
int minValue2 = 1024;          // store min value here (Load 2)
int maxValue3 = 0;          // store max value here (Load 3)
int minValue3 = 1024;          // store min value here (Load 3)

double Voltage1 = 0; // Load 1
double Voltage2 = 0; // Load 2
double Voltage3 = 0; // Load 3

double AmpsRMS1 = 0; // Load 1
double AmpsRMS2 = 0; // Load 2
double AmpsRMS3 = 0; // Load 3
 
double VRMS1 = 0; // Load 1
double VRMS2 = 0; // Load 2
double VRMS3 = 0; // Load 3

double totalAmps1 = 0; // Load 1
double totalAmps2 = 0; // Load 2
double totalAmps3 = 0; // Load 3

double a; // Load 1
double b; // Load 2
double c; // Load 3

double pay1 = 0; // Load 1
double pay2 = 0; // Load 2
double pay3 = 0; // Load 3

double equi1; // Load 1
double equi2; // Load 2
double equi3; // Load 3

//Initialize the library with the numbers of the interface pins 
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup()
{ 
  pinMode(LED, OUTPUT); // Set the LED pin as an output
  pinMode(BUTTON, INPUT); // Set button as input (not required)
  
  pinMode(relay1, OUTPUT); // Set the Relay pin as an output
  pinMode(relay2, OUTPUT); // Set the Relay pin as an output 
  pinMode(relay3, OUTPUT); // Set the Relay pin as an output
  
  digitalWrite(relay1, HIGH); //Initialize Relay is High 
  digitalWrite(relay2, HIGH); //Initialize Relay is High 
  digitalWrite(relay3, HIGH); //Initialize Relay is High 

  lcd.begin(16, 2); //LCD's number of rows and columns
  lcd.print("Load Shedding Prototype");
  
  Serial.begin(9600);
}

void sortNum(double &, double &, double &);

boolean debounce(boolean last)
{
  boolean current = digitalRead(BUTTON); //Read the button state
  if (last != current) //if it's different…
  {
    delay(50); //wait 5ms
    current = digitalRead(BUTTON); //read it again
    return current; //return the current value
  }
}

void loop() 
{   
  for (int i=0; i<4; i++)
  {
    Voltage1 = getVPP1(); // Vmax for Load 1
    Voltage2 = getVPP2(); // Vmax for Load 2
    Voltage3 = getVPP3(); // Vmax for Load 1
    VRMS1 = (Voltage1/2.0) * 0.707; // Vrms for Load 1 
    VRMS2 = (Voltage2/2.0) * 0.707; // Vrms for Load 2
    VRMS3 = (Voltage3/2.0) * 0.707;  // Vrms for Load 3
    AmpsRMS1 = (VRMS1 * 1000)/mVperAmp1; // Irms for Load 1
    AmpsRMS2 = (VRMS2 * 1000)/mVperAmp2; // Irms for Load 2
    AmpsRMS3 = (VRMS3 * 1000)/mVperAmp3; // Irms for Load 3
    
    Serial.print("1st: "); //Prints on the Serial COM screen
    Serial.print(AmpsRMS1); 
    Serial.println(" Amps RMS");

    Serial.print("2nd: "); //Prints on the Serial COM screen
    Serial.print(AmpsRMS2);
    Serial.println(" Amps RMS");

    Serial.print("3rd: "); //Prints on the Serial COM screen
    Serial.print(AmpsRMS3);
    Serial.println(" Amps RMS");
    
    totalAmps1 = AmpsRMS1 + totalAmps1;
    totalAmps2 = AmpsRMS2 + totalAmps2;
    totalAmps3 = AmpsRMS3 + totalAmps3;

    if (i==3) 
    {
      a = (totalAmps1/(60*60)) * 220 * 2000 * 0.033; // Converts to cost per usage 
      b = (totalAmps2/(60*60)) * 220 * 2000 * 0.033; // Converts to cost per usage
      c = (totalAmps3/(60*60)) * 220 * 2000 * 0.033; // Converts to cost per usage
      
      Serial.println("");
      
      Serial.println("User 1");
      Serial.print("Total Current: ");
      Serial.print(totalAmps1);
      Serial.println(" Amps");
      Serial.print("You are to pay N");
      Serial.println(a);

      Serial.println("User 2");
      Serial.print("Total Current: ");
      Serial.print(totalAmps2);
      Serial.println(" Amps");
      Serial.print("You are to pay N");
      Serial.println(b);

      Serial.println("User 3");
      Serial.print("Total Current: ");
      Serial.print(totalAmps3);
      Serial.println(" Amps");
      Serial.print("You are to pay N");
      Serial.println(c);
      
      Serial.println("Input amount to pay. (user1,user2,user3)");
    
      while (Serial.available() == 0){} // Wait until user inputs payment
  
      if (Serial.available()>0) 
      {
        pay1 = Serial.parseFloat();
        pay2 = Serial.parseFloat();
        pay3 = Serial.parseFloat();

        if (Serial.read() == '\n') //Done transmitting
        {
          Serial.println(pay1);
          Serial.println(pay2);
          Serial.println(pay3);
          equi1 = (pay1/a) * 100;
          equi2 = (pay2/b) * 100;
          equi3 = (pay3/c) * 100;
          
          Serial.print("User 1 paid ");
          Serial.print(equi1);
          Serial.println(" %");

          Serial.print("User 2 paid ");
          Serial.print(equi2);
          Serial.println(" %");

          Serial.print("User 3 paid ");
          Serial.print(equi3);
          Serial.println(" %");
        }
      }
      sortNum(equi1, equi2, equi3);
    }   
  }
  delay(10000);
}

float getVPP1() // Gets Vmax for Load 1
{
  float result1; 
  int readValue1;             //value read from the sensor
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue1 = analogRead(sensorIn1);
       // see if you have a new maxValue
       if (readValue1 > maxValue1) 
       {
           /*record the maximum sensor value*/
           maxValue1 = readValue1;
       }
       if (readValue1 < minValue1) 
       {
           /*record the maximum sensor value*/
           minValue1 = readValue1;
       }
   }
   
   // Subtract min from max
   result1 = ((maxValue1 - minValue1) * 5.0)/1024.0;
      
   return result1;
 }

 float getVPP2() // Gets Vmax for Load 2
{
  float result2; 
  int readValue2;             //value read from the sensor
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue2 = analogRead(sensorIn2);
       // see if you have a new maxValue
       if (readValue2 > maxValue2) 
       {
           /*record the maximum sensor value*/
           maxValue2 = readValue2;
       }
       if (readValue2 < minValue2) 
       {
           /*record the maximum sensor value*/
           minValue2 = readValue2;
       }
   }
   
   // Subtract min from max
   result2 = ((maxValue2 - minValue2) * 5.0)/1024.0;
      
   return result2;
 }

 float getVPP3() // Gets Vmax for Load 3
{
  float result3; 
  int readValue3;             //value read from the sensor
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue3 = analogRead(sensorIn3);
       // see if you have a new maxValue
       if (readValue3 > maxValue3) 
       {
           /*record the maximum sensor value*/
           maxValue3 = readValue3;
       }
       if (readValue3 < minValue3) 
       {
           /*record the maximum sensor value*/
           minValue3 = readValue3;
       }
   }
   
   // Subtract min from max
   result3 = ((maxValue3 - minValue3) * 5.0)/1024.0;
      
   return result3;
 }







 
// sortNum function definition
void sortNum (double &equi1, double &equi2, double &equi3)
{
  // declare the lowest, middle, and highest variables
  double lowest, middle, highest;
  
  if (equi1 < equi2 && equi2 < equi3)
  {
    lowest = equi1;
    middle = equi2;
    highest = equi3;

    Serial.println("Arrangement in order of payment in percentage (%)");
    Serial.print("3rd User: ");
    Serial.println(highest);
    Serial.print("2nd User: ");
    Serial.println(middle);
    Serial.print("1st User: ");
    Serial.println(lowest);

  while (true) {
    currentButton = debounce(lastButton); //read deboucned state
    if (lastButton == LOW && currentButton == HIGH) 
    {
      lastButton = currentButton; 
      break; 
    }
    if (lastButton != currentButton) lastButton = currentButton; 
  }

    digitalWrite(LED, true); //change the LED state
    Serial.println("LED is on"); 
    
    while (true) {
      currentButton = debounce(lastButton); //read deboucned state

      val = analogRead(POT); 
      val = map(val, LOWER_BOUND, UPPER_BOUND, 0, 50.5);
      val = constrain(val, 0, 50.5);  
      
      if (val <= 50.5 && val > 49)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }      
      else if (val <= 49 && val > 47)
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 47 && val > 46)
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, HIGH);
      }
      else
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }
      
      lcd.setCursor(0,1);   
      lcd.print(val);    
  
      if (lastButton == LOW && currentButton == HIGH) 
      {
        lastButton = currentButton; 
        break; 
      }
      if (lastButton != currentButton) lastButton = currentButton; 
    }
    
    digitalWrite(LED, false); 
    Serial.println("LED is off");  
  }
  
  else if (equi2 < equi1 && equi1 < equi3)
  {
    lowest = equi2;
    middle = equi1;
    highest = equi3;
    
    Serial.println("Arrangement in order of payment in percentage (%)");
    Serial.print("3rd User: ");
    Serial.println(highest);
    Serial.print("1st User: ");
    Serial.println(middle);
    Serial.print("2nd User: ");
    Serial.println(lowest);

  while (true) {
    currentButton = debounce(lastButton); //read deboucned state
    if (lastButton == LOW && currentButton == HIGH) 
    {
      lastButton = currentButton; 
      break; 
    }
    if (lastButton != currentButton) lastButton = currentButton; 
  }

    digitalWrite(LED, true); //change the LED state
    Serial.println("LED is on"); 
    
    while (true) {
      currentButton = debounce(lastButton); //read deboucned state

      val = analogRead(POT); 
      val = map(val, LOWER_BOUND, UPPER_BOUND, 0, 50.5);
      val = constrain(val, 0, 50.5);  
      
      if (val <= 50.5 && val > 49)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 49 && val > 47)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 47 && val > 46)
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, HIGH);
      }
      else
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }

      lcd.setCursor(0,1);   
      lcd.print(val);   
    
      if (lastButton == LOW && currentButton == HIGH) 
      {
        lastButton = currentButton; 
        break; 
      }
      if (lastButton != currentButton) lastButton = currentButton; 
    }
    
    digitalWrite(LED, false); 
    Serial.println("LED is off"); 
  }
  
  else if (equi3 < equi2 && equi2 < equi1)
  {
    lowest = equi3;
    middle = equi2;
    highest = equi1;
  
    Serial.println("Arrangement in order of payment in percentage (%)");
    Serial.print("1st User: ");
    Serial.println(highest);
    Serial.print("2nd User: ");
    Serial.println(middle);
    Serial.print("3rd User: ");
    Serial.println(lowest);

  while (true) {
    currentButton = debounce(lastButton); //read deboucned state
    if (lastButton == LOW && currentButton == HIGH) 
    {
      lastButton = currentButton; 
      break; 
    }
    if (lastButton != currentButton) lastButton = currentButton; 
  }

    digitalWrite(LED, true); //change the LED state
    Serial.println("LED is on"); 
    
    while (true) {
      currentButton = debounce(lastButton); //read deboucned state

      val = analogRead(POT); 
      val = map(val, LOWER_BOUND, UPPER_BOUND, 0, 50.5);
      val = constrain(val, 0, 50.5);  
      
      if (val <= 50.5 && val > 49)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 49 && val > 47)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, LOW);
      }
      else if (val <= 47 && val > 46)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }
      else
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }
            
      lcd.setCursor(0,1);   
      lcd.print(val); 
      
      if (lastButton == LOW && currentButton == HIGH) 
      {
        lastButton = currentButton; 
        break; 
      }
      if (lastButton != currentButton) lastButton = currentButton; 
    }
    
    digitalWrite(LED, false); 
    Serial.println("LED is off");  
  }
  
  else if (equi3 < equi1 && equi1 < equi2)
  {
    lowest = equi3;
    middle = equi1;
    highest = equi2;
  
    Serial.println("Arrangement in order of payment in percentage (%)");
    Serial.print("2nd User: ");
    Serial.println(highest);
    Serial.print("1st User: ");
    Serial.println(middle);
    Serial.print("3rd User: ");
    Serial.println(lowest);

  while (true) {
    currentButton = debounce(lastButton); //read deboucned state
    if (lastButton == LOW && currentButton == HIGH) 
    {
      lastButton = currentButton; 
      break; 
    }
    if (lastButton != currentButton) lastButton = currentButton; 
  }

    digitalWrite(LED, true); //change the LED state
    Serial.println("LED is on"); 
    
    while (true) {
      currentButton = debounce(lastButton); //read deboucned state

      val = analogRead(POT); 
      val = map(val, LOWER_BOUND, UPPER_BOUND, 0, 50.5);
      val = constrain(val, 0, 50.5);
      
      if (val <= 50.5 && val > 49)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 49 && val > 47)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, LOW);
      }
      else if (val <= 47 && val > 46)
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, LOW);
      }
      else
     {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }

      lcd.setCursor(0,1);   
      lcd.print(val); 
      
      if (lastButton == LOW && currentButton == HIGH) 
      {
        lastButton = currentButton; 
        break; 
      }
      if (lastButton != currentButton) lastButton = currentButton; 
    }
    
    digitalWrite(LED, false); 
    Serial.println("LED is off"); 
  }
  
  else if (equi2 < equi3 && equi3 < equi1)
  {
    lowest = equi2;
    middle = equi3;
    highest = equi1;
  
    Serial.println("Arrangement in order of payment in percentage (%)");
    Serial.print("1st User: ");
    Serial.println(highest);
    Serial.print("3rd User: ");
    Serial.println(middle);
    Serial.print("2nd User: ");
    Serial.println(lowest);

  while (true) {
    currentButton = debounce(lastButton); //read deboucned state
    if (lastButton == LOW && currentButton == HIGH) 
    {
      lastButton = currentButton; 
      break; 
    }
    if (lastButton != currentButton) lastButton = currentButton; 
  }

    digitalWrite(LED, true); //change the LED state
    Serial.println("LED is on"); 
    
    while (true) {
      currentButton = debounce(lastButton); //read deboucned state

      val = analogRead(POT); 
      val = map(val, LOWER_BOUND, UPPER_BOUND, 0, 50.5);
      val = constrain(val, 0, 50.5);  
      
      if (val <= 50.5 && val > 49)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 49 && val > 47)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 47 && val > 46)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }
      else
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }

      lcd.setCursor(0,1);   
      lcd.print(val); 
      
      if (lastButton == LOW && currentButton == HIGH) 
      {
        lastButton = currentButton; 
        break; 
      }
      if (lastButton != currentButton) lastButton = currentButton; 
    }
    
    digitalWrite(LED, false); 
    Serial.println("LED is off");
  }
  
  else if (equi1 < equi3 && equi3 < equi2)
  {
    lowest = equi1;
    middle = equi3;
    highest = equi2;
  
    Serial.println("Arrangement in order of payment in percentage (%)");
    Serial.print("2nd User: ");
    Serial.println(highest);
    Serial.print("3rd User: ");
    Serial.println(middle);
    Serial.print("1st User: ");
    Serial.println(lowest);

  while (true) {
    currentButton = debounce(lastButton); //read deboucned state
    if (lastButton == LOW && currentButton == HIGH) 
    {
      lastButton = currentButton; 
      break; 
    }
    if (lastButton != currentButton) lastButton = currentButton; 
  }

    digitalWrite(LED, true); //change the LED state
    Serial.println("LED is on"); 
    
    while (true) {
      currentButton = debounce(lastButton); //read deboucned state

      val = analogRead(POT); 
      val = map(val, LOWER_BOUND, UPPER_BOUND, 0, 50.5);
      val = constrain(val, 0, 50.5);  
      
      if (val <= 50.5 && val > 49)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 49 && val > 47)
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      }
      else if (val <= 47 && val > 46)
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, LOW);
      } 
      else
      {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
      }

      lcd.setCursor(0,1);   
      lcd.print(val);
        
      if (lastButton == LOW && currentButton == HIGH) 
      {
        lastButton = currentButton; 
        break; 
      }
    if (lastButton != currentButton) lastButton = currentButton; 
  }
    
    digitalWrite(LED, false); 
    Serial.println("LED is off"); 
  }
}
