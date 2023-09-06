#include <Arduino.h>


#define PWM 10
#define DIR_PIN 3
#define DIR_CHANGE_PIN 2
#define PWM_ZERO 600

const byte numChars = 24;
char receivedChars[numChars]; // STM32|-1000 1090785098\n

bool halt_flag = false;
boolean newData = false;

uint8_t CHECKSUM_CALC(char *buff, uint8_t n);
void recvWithEndMarker();
void spindle_calc();

volatile uint16_t val = PWM_ZERO;
volatile int8_t dir = 1;
uint8_t checksum = 0;

void setup()
{
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);                  // salida pwm
    pinMode(PWM, OUTPUT);                  // salida pwm
    pinMode(DIR_PIN, OUTPUT);              // salida pwm
    pinMode(DIR_CHANGE_PIN, INPUT_PULLUP); // salida pwm
                                           //////////////////////////////////////////////////////////////////////////////////////
    // INTERRUPTS
    cli();
    // inicializo timer1/pwm

    TCCR1A = 0; // Inicializo los control registers del timer1 tanto el A como el B
    TCCR1B = 0;
    TCCR1C = 0;
    TIMSK1 = 0;

    // TCCR1A |=(1 << COM1A1);   //defino la salida en el OCR1A en toogle mode #pin9
    TCCR1A |= (1 << COM1B1); // defino la salida en el OCR1B en toogle mode #pin10

    TCCR1A |= (1 << WGM10); // WGMxx Lo define como fast pwm con el TOP en OCR1A /la comparaci?n en el OC1B
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << WGM13);

    TCCR1B |= (1 << CS10); // Seteo el prescaling en 1

    TIMSK1 |= (1 << OCIE1B);
    TIMSK1 |= (1 << TOIE1);

    OCR1A = 10000;
    OCR1B = PWM_ZERO;

    // inicializo external interrupt
    EICRA = 0;
    EICRA |= (1 << ISC00); // Enable relay change alarm interrupt for raising/falling edges

    EIMSK = 0;
    EIMSK |= (1 << INT0); // Enable relay change alarm interrupt

    sei();
    Serial.begin(115200);
    delay(10);
    //Serial.println("<ANTS Serial SPEED CONTROLLER is ready>");
}

void loop()
{
    recvWithEndMarker();
    spindle_calc();

    // if (halt_flag){
    //     Serial.println("HALT");
    //     delay(300);
    // }
    delay(1);
}

void recvWithEndMarker()
{
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();
        
        if (rc != endMarker)
        {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars)
            {
                ndx = numChars - 1;
            }
        }
        else
        {
            ndx++;
            receivedChars[ndx] = '\n';
            
            if (!halt_flag)
            {
                if (sscanf(receivedChars, "ANTS_S|%d|%d|", &val, &dir) == 2 && receivedChars[16] == CHECKSUM_CALC(receivedChars, 16) )//&& sscanf(receivedChars, "ANTS_S|%d|%d|", &val, &dir) == 2)
                {
                    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
                    newData = true;
                     //int n=sscanf(receivedChars,"ANTS_S|%d|%d|",&val,&dir);
                    Serial.print("$N#\n");
                }
                else
                {
                    Serial.print("$C#\n");
                    
                }
            }
            else if (halt_flag && strstr(receivedChars, "UNHALT") != NULL)
            {
                halt_flag = false;
                val=PWM_ZERO;
                Serial.print("$I#\n");
            }
            ndx = 0;
        }
    }
}

void spindle_calc()
{
    if (newData == true)
    {
        if (dir >= 0)
        {
            digitalWrite(DIR_PIN, HIGH);
        }

        else
        {
            digitalWrite(DIR_PIN, LOW);
        }

        newData = false;
    }
}

ISR(TIMER1_OVF_vect)
{
    OCR1B = val;
}
ISR(TIMER1_COMPB_vect)
{
}

ISR(INT0_vect)
{ // ENC_BUTTON

    if (dir != (digitalRead(DIR_PIN) * 2 - 1))
    {
        // OCR1B = PWM_ZERO;
        val = PWM_ZERO;
        halt_flag = true;
        Serial.print("$H#\n");
    }
}

uint8_t CHECKSUM_CALC(char *buff, uint8_t n)
{

    uint8_t checksum = 0;

    for (int i = 0; i < n; i++)
    {
        checksum ^= buff[i];
    }

    return checksum;
}
