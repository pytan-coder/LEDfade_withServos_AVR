/*
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LED7 PC7

void toggleLED(float duty);
void runServos();

int main(void)
{

    unsigned int ElapsedSeconds = 0;
//    float multiplier = -0.1;
//    float duty_cycle = 1;
    int multiplier = 1;
    unsigned int duty_cycle = 1;

	// for LEDs
    //DDRB = 0x18;		// input for PB3-4, 9, 10
    DDRB = 0x18;
    DDRC |= (1 << LED7);	// input for PC7

	DDRD = 0x70;		// for servos at PD4-5

    TCCR1B |= ((1 << CS12) | (1 << CS10));      // 1024 prescale, for real-time clock

    TCCR0A = 0;
	TCCR0B = 0;
    TCCR0A |= ( 1<<COM0A1 | 1<<COM0B1 |	1<<WGM01 |	1<<WGM00); // fast pwm mode 7
    TCCR0B |= ( 1<<WGM02 | 1<<CS02 | 1<< CS00);         // prescale 1024

    TCCR2A = 0;
	TCCR2B = 0;
    TCCR2A |= ( 1<<COM2A1 | 1<<COM2B1 |	1<<WGM21 |	1<<WGM20); // fast pwm mode 7
    TCCR2B |= ( 1<<WGM22 | 1<<CS22 | 1<< CS21 | 1<< CS20);         // prescale 1024

    while(1)
    {
        toggleLED(duty_cycle);
        if (TCNT1 >= 15624) // at 16 MHZ and 1 HZ target, 1024 prescale
            {

                TCNT1 = 0;
                ElapsedSeconds++;
                if (ElapsedSeconds == 43200)
                    {
                if (((ElapsedSeconds % 90) == 0) && (duty_cycle>0))
                {
                    duty_cycle = duty_cycle + multiplier;
                }}

                if ((ElapsedSeconds % 86400) == 0)
                {
                    multiplier *= -1;
                    ElapsedSeconds = 0;
                }
                if ((ElapsedSeconds == 21600 | ElapsedSeconds == 42900))
                {
                if (((ElapsedSeconds % 5) == 0) &!((ElapsedSeconds == 21900 | ElapsedSeconds == 43200)))
				{
					runServos();		// servos at PD4,4 and PD5,5
				}
				}

            }
    };

    return 0;
}

void toggleLED(float duty)
{

    OCR0A = 156;		//~2mS HIGH == ALL RIGHT or 90deg
	OCR0B = 0 + (duty * 15);

	OCR2A = 156;		//~2mS HIGH == ALL RIGHT or 90deg
	OCR2B = 0 + (duty * 15);

}

void runServos()
{
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A |= ( 1<<COM1A1 |	1<<COM1A0 | 1<<COM1B1 |	1<<COM1B0 |	1<<WGM11 );		//fast PWM (14) , //non-inverted mode
	TCCR1B |= ( 1<<WGM12 | 1<<WGM13 | 1<<CS11 | 1<<CS10);		//prescale 64

	ICR1 = 4999;				//20ms at 16MHz/64 prescale

	OCR1A = ICR1 - 500;		//~2mS HIGH == ALL RIGHT or 90deg
	_delay_ms(1000);
	OCR1A = ICR1 - 250;		//~1mS HIGH == ALL LEFT or -90deg
	_delay_ms(1000);

	_delay_ms(1000);

	OCR1B = ICR1 - 500;
	_delay_ms(1000);
    OCR1B = ICR1 - 250;
    _delay_ms(1000);

	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= ((1 << CS12) | (1 << CS10));      // 1024 prescale, reset for real-time clock
}
