
#include <DHT.h>
#include <SoftwareSerial.h>

DHT dht;

int RELAY1 = 13;
int relay1_status = 0;
float THRESHHOLD_C = 10.0;

SoftwareSerial mySerial(3,2); // pin 2 = TX, pin 3 = RX (unused)
char buffer[20];

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600); // set up serial port for 9600 baud

  dht.setup(8); // temperature, humidity sensor
  
  relay1_status = 0;
  pinMode(RELAY1, OUTPUT);     

}

char * floatToString(char * outstr, double val, byte precision, byte widthp)
{
 char temp[20];
 byte i;

 // compute the rounding factor and fractional multiplier
 double roundingFactor = 0.5;
 unsigned long mult = 1;
 for (i = 0; i < precision; i++)
 {
   roundingFactor /= 10.0;
   mult *= 10;
 }
 
 temp[0]='\0';
 outstr[0]='\0';

 if(val < 0.0){
   strcpy(outstr,"-\0");
   val = -val;
 }

 val += roundingFactor;

 strcat(outstr, itoa(int(val),temp,10));  //prints the int part
 
 if( precision > 0) 
 {
   strcat(outstr, ".\0"); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   
   while(precision--)
     mult *=10;

   if(val >= 0)
     frac = (val - int(val)) * mult;
   else
     frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;

   while(frac1 /= 10)
     padding--;

   while(padding--)
     strcat(outstr,"0\0");

   strcat(outstr,itoa(frac,temp,10));
 }

 // generate space padding 
 if ((widthp != 0)&&(widthp >= strlen(outstr))){
   byte J=0;
   J = widthp - strlen(outstr);
   
   for (i=0; i< J; i++) {
     temp[i] = ' ';
   }

   temp[i++] = '\0';
   strcat(temp,outstr);
   strcpy(outstr,temp);
 }
 
 return outstr;
}

void loop()
{
  
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("C");
  
  mySerial.write(254); // move cursor to beginning of first line
  mySerial.write(128);

  mySerial.write("################"); // clear display
  mySerial.write("                ");

  mySerial.write(254); // move cursor to beginning of first line
  mySerial.write(128);
 
  mySerial.write(floatToString(buffer,temperature,2,6));
  mySerial.write('C');
  mySerial.write(floatToString(buffer,humidity,2,6));
  mySerial.write('%');//mySerial.write("% ");
  mySerial.write(' ');
  mySerial.write(relay1_status + '0');
  mySerial.write("Config: ");
  mySerial.write(floatToString(buffer,THRESHHOLD_C,2,6));
  mySerial.write('C');

  Serial.print("\tConfig:");
  Serial.print(THRESHHOLD_C, 1);
  Serial.println("C\t\t");

  if (temperature >= THRESHHOLD_C && relay1_status == 0)
  {
    digitalWrite(RELAY1, HIGH);   // turn the LED on (HIGH is the voltage level)
    relay1_status = 1;
  }
  else if (temperature < THRESHHOLD_C)
  {
    //Just wait for 10 secounds because it's very annoying switcing back within short time.
    delay(10000);
    digitalWrite(RELAY1, LOW);   // turn the LED on (HIGH is the voltage level)
    relay1_status = 0;
  }
  
}

