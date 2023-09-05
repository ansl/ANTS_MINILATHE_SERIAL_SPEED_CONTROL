# ANTS_MINILATHE_SERIAL_SPEED_CONTROL

The purpose of this project is to substitute the stock Spidle speed control of a minilathe CJ0618 with a digital controlled alternative. For this specific project the minilathe includes a "CT_MOTOR@191125" control board, that uses a 4.7k potentiometer to regulate the speed
![image](https://github.com/ansl/ANTS_MINILATHE_SERIAL_SPEED_CONTROL/assets/5759139/cdd021ea-77be-46bc-a8f5-d472df9c004e)

Asuming the speed control cicuit to be a black box uisng a single input the potentiomenter we took as a main hipothesis that teh potentiomenter operates a as voltaje divisor being the voltaje  in terminal P2 the input to the speed control circuit.

To substitute the potentiometer we have used an arduino mini pro 16MHz 5V  to generate  a PWM signal (any other dev board capable of generating PWM signals  can work) and a RC low pass filter to smooth the output voltaje in the board terminal P2. So difining the duty of the PWM signal we get a variable constant voltaje between 0 and 5V. It is important to take into account that this specific motor driver stops working what hte voltage on P2 decays below 300mV is this for taht the minimun duty lays arround 650(derived from experimentation) is waht in the code is called ZERO POINT and needs to be adjusted for each driver for a safe use.


## Wiring Diagram:
![image](https://github.com/ansl/ANTS_MINILATHE_SERIAL_SPEED_CONTROL/assets/5759139/bfa4ec04-6c8e-44ce-8f5f-e29173b5767b)



For the PWM signal we use the timer2 that has 16 bits (65535) configured as a fast PWM with a prescaler of 1 and a Period of 10000(1.6KHz). This has proved to work smoothly, because it provides a good balance between duty resolution (0 to 10000) and frequency ( the higher the frequency the smoother the oputput voltaje after the RC filter). 
For the RC filter we used a 105 ceramic capacitor  and 10k Pot as a rheostate to adjust the ripple and the response.

## Spindle direction
The direction of the Spindle can be adjusted manually or you can also control it with the arduino. For the last you need
 1. - Disconect the direction switch and jump the safety wire that switch off the power while changing the direction. **IMPORTANT do not operate the lathe without this safety system changing the spindle direction while the motor is under load might destroy your lathe and a risk for your persona!!!**
 1. - Conect the motor wires to a 2way realy to conmute wires
 1. - Conect pin2 (external interrupt0) to pin3 (Spindle direction output to 2 way relay). This substitutes the safety cutoff of the direction switch. In summary what is does is to monitor the Status of PIN3 and in case of flange compare the value  with the expected stare of the pin. If ther is a missaligment becuase  anything has disturbed the state of the pin the MCU shuts down the spindle  power .


 ## Software configuration (ongoing)


 ## How to comunicate (ongoing)
 




