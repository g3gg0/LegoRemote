
#include "analogWrite.h"
#include "LED.h"

#define LEGO_LED    27
#define LEGO_NSLEEP 26
#define LEGO_NFAULT 25
#define LEGO_AIN2   33
#define LEGO_AIN1   32
#define LEGO_BIN2   12
#define LEGO_BIN1   14

bool lego_fault = false;

uint8_t lego_in1[] = { LEGO_AIN1, LEGO_BIN1 };
uint8_t lego_in2[] = { LEGO_AIN2, LEGO_BIN2 };


void lego_motor_disable()
{
  digitalWrite(LEGO_NSLEEP, LOW);
  analogWrite(LEGO_AIN1, 0, 255);
  analogWrite(LEGO_AIN2, 0, 255);
  analogWrite(LEGO_BIN1, 0, 255);
  analogWrite(LEGO_BIN2, 0, 255);
}

void lego_led(uint8_t state)
{
  analogWrite(LEGO_LED, state, 255);
}

void lego_init()
{
  lego_motor_disable();
  
  analogWriteFrequency(30000);
  analogWriteResolution(8);
  
  pinMode(LEGO_LED, OUTPUT);
  digitalWrite(LEGO_LED, HIGH);
  
  pinMode(LEGO_NSLEEP, OUTPUT);
  pinMode(LEGO_NFAULT, INPUT_PULLUP);
  pinMode(LEGO_AIN1, OUTPUT);
  pinMode(LEGO_AIN2, OUTPUT);
  pinMode(LEGO_BIN1, OUTPUT);
  pinMode(LEGO_BIN2, OUTPUT);
  
  lego_motor(0, 0);
  lego_motor(1, 0);
  lego_led(0);

  delay(10);
  digitalWrite(LEGO_NSLEEP, HIGH);
  delay(10);
  
  led_anim_red(led_anim_none);
}

void lego_motor(uint8_t motor, uint8_t power, bool forward = true)
{
  /* swap motor if configured */
  if(current_config.swap)
  {
    motor = motor ? 0 : 1;
  }

  /* invert direction */
  forward ^= current_config.invert[motor];

  if(power == 0)
  {
    analogWrite(lego_in1[motor], 0, 255);
    analogWrite(lego_in2[motor], 0, 255);
  }
  else
  {
    uint8_t pwm = min(255.0f, sgn(power) * 0.2f * 255.0f + power * 0.8f);
    
    if(forward)
    {
      analogWrite(lego_in1[motor], 255 - pwm, 255);
      analogWrite(lego_in2[motor], 255, 255);
    }
    else
    {
      analogWrite(lego_in1[motor], 255, 255);
      analogWrite(lego_in2[motor], 255 - pwm, 255);
    }
  }
}

void lego_motor(uint8_t motor, float power)
{
  lego_motor(motor, min(1.0f,fabsf(power)) * 255, power >= 0);
}

void lego_motor_a(uint8_t motor, float power)
{
  lego_motor(0, power);
}

void lego_motor_b(uint8_t motor, float power)
{
  lego_motor(1, power);
}

bool lego_loop()
{
  if(digitalRead(LEGO_NFAULT) == LOW)
  {
    Serial.println("[E] DRV8833 reports nFAULT == LOW, disabling");
    lego_motor_disable();
    lego_fault = true;
    led_anim_red(led_anim_emergency);
  }
  
  return false;
}

