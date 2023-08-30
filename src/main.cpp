#include <Arduino.h>
#include <CRC32.h>

const byte numChars = 24;

char receivedChars[numChars]; //STM32|-1000 1090785098\n

char MSG_in[13];
char checksum_in[10];
char DATA_in[6];
char crc_test[5]="hola";
void recvWithEndMarker();
void showNewData();
boolean newData = false;

uint16_t val=0;
int8_t dir=1;
CRC32 crc;
uint32_t checksum=0;



uint32_t crc32b(char *message, size_t l)
{
   size_t i, j;
   unsigned int crc, msb;

   crc = 0xFFFFFFFF;
   for(i = 0; i < l; i++) {
      // xor next byte to upper bits of crc
      crc ^= (((unsigned int)message[i])<<24);
      for (j = 0; j < 8; j++) {    // Do eight times.
            msb = crc>>31;
            crc <<= 1;
            crc ^= (0 - msb) & 0x04C11DB7;
      }
   }
   return crc;         // don't complement crc on output
}

void setup() {
  // put your setup code here, to run once:
pinMode(10,OUTPUT);// salida pwm

   //////////////////////////////////////////////////////////////////////////////////////
  //INTERRUPTS
  cli();
  //inicializo timer1/pwm

  TCCR1A=0;         //Inicializo los control registers del timer1 tanto el A como el B
  TCCR1B=0;
  TCCR1C=0;
  TIMSK1=0;

  //TCCR1A |=(1 << COM1A1);   //defino la salida en el OCR1A en toogle mode #pin9
  TCCR1A |=(1 << COM1B1);   //defino la salida en el OCR1B en toogle mode #pin10

  TCCR1A |=(1 << WGM10);    //WGMxx Lo define como fast pwm con el TOP en OCR1A /la comparaci?n en el OC1B
  TCCR1A |=(1 << WGM11);
  TCCR1B |=(1 << WGM12);
  TCCR1B |=(1 << WGM13);

  TCCR1B |=(1 << CS10);   //Seteo el prescaling en 1

  TIMSK1 |=(1 << OCIE1B);
  TIMSK1 |=(1 << TOIE1);

  OCR1A=10000;        
  OCR1B=0;

   
  sei();
Serial.begin(115200);//Serial com
Serial.println("<ANTS Serial SPEED CONTROLLER is ready>");
checksum= crc32b(crc_test, sizeof(crc_test));
// checksum=~checksum;
Serial.println(checksum);
}

void loop() {
   recvWithEndMarker();
   showNewData();
   delay(1);
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            // sscanf(receivedChars,"%s",MSG_in);
            // sscanf(receivedChars,"%*s %s",checksum_in);
            // sscanf(receivedChars,"STM32|%s",DATA_in);
            // Serial.println(receivedChars);
            // Serial.println(MSG_in);
            // Serial.println(checksum_in);
            // Serial.println(DATA_in);
             ndx = 0;
            // checksum = crc.calculate(MSG_in,  sizeof(MSG_in));

            // if(checksum==atol(checksum_in)){
               newData = true;
            //   val=atoi(DATA_in);
            //   dir=val/abs(val);
            // }

            // else {
            //   Serial.println("DATA_CORRUPTED");
            // }
            val=atol(receivedChars);
            dir=val/abs(val);      

        }
    }
}

void showNewData() {
    if (newData == true) {
        // Serial.print("CRC32:  ");
        // Serial.print(checksum);
        Serial.print("  TARGET VAL:  ");
        //Serial.println(receivedChars);
        Serial.print(abs(val));
        Serial.print("  TARGET DIR:  ");
        //Serial.println(receivedChars);
        Serial.println(dir);
        newData = false;
    }
}

ISR(TIMER1_OVF_vect){
 OCR1B=constrain(abs((int16_t)(10000.0*val/2500.0)),0,10000);
}
ISR(TIMER1_COMPB_vect){

}
