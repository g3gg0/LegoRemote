
#include "analogWrite.h"
#include "LED.h"

extern bool lego_fault;
led_anim_state_t led_anim_state_red = { LED_STATUS, 0, 0, NULL };
led_anim_state_t led_anim_state_blue = { LED_ACTIVITY, 0, 0, NULL };

led_anim_t led_anim_shortblink[] =           { { 100, 100 }, {   0, 2000 }, LED_ANIM_LOOP };
led_anim_t led_anim_slowshortblink[] =       { {  10,  20 }, { 100,   20 }, {  10, 20 }, { 0, 5000 }, LED_ANIM_LOOP };
led_anim_t led_anim_shortdoubleblinkloop[] = { {  10,  20 }, { 100,   20 }, {  10, 20 }, { 0,  100 }, { 10, 20 }, { 100, 20 }, { 10, 20 }, { 0, 100 }, { 0, 750 }, LED_ANIM_LOOP };
led_anim_t led_anim_shortdoubleblink[] =     { {  10,  20 }, { 100,   20 }, {  10, 20 }, { 0,  100 }, { 10, 20 }, { 100, 20 }, { 10, 20 }, { 0, 100 }, { 0, 750 }, LED_ANIM_END };
led_anim_t led_anim_emergency[] =            { { 100,  20 }, {   0,  100 }, { 100, 20 }, { 0,  100 }, LED_ANIM_LOOP };
led_anim_t led_anim_idle[] =                 { {   1,  20 }, {   0, 5000 }, LED_ANIM_LOOP };
led_anim_t led_anim_mediumblink[] =          { {   1, 200 }, {   0,  200 }, LED_ANIM_LOOP };
led_anim_t led_anim_fastblink[] =            { {   1,  50 }, {   0,   50 }, LED_ANIM_LOOP };
led_anim_t led_anim_none[] =                 { {   0, 500 }, LED_ANIM_LOOP };

led_anim_t led_anim_ble_connected[] =        { { 100, 200 }, {   0,  20 }, { 100, 200 }, {   0,  20 }, { 100,   20 }, {   0,  20 }, { 100,  20 }, LED_ANIM_NEXT(led_anim_slowshortblink) };


void led_anim_loop(led_anim_state_t *state)
{
  if(!state || !state->anim)
  {
    return;
  }
  if(state->anim[state->pos].pct == 255)
  {
    led_anim_start(state, (led_anim_t *)state->anim[state->pos].delay, true);
    return;
  }
  else if(state->anim[state->pos].pct == 253)
  {
    return;
  }
  else if(state->anim[state->pos].pct > 100)
  {
    state->pos = 0;
  }
  
  analogWrite(state->pin, state->anim[state->pos].pct, 100);

  if(state->last_time + state->anim[state->pos].delay < millis())
  {
    state->last_time = millis();
    state->pos++;
  }
}

void led_anim_start(led_anim_state_t *state, led_anim_t *anim, bool force)
{
  if(state->anim == anim && !force)
  {
    return;
  }
  state->pos = 0;
  state->anim = anim;
  state->last_time = millis();

  led_anim_loop(state);
}

void led_anim_red(led_anim_t *anim)
{
  led_anim_start(&led_anim_state_red, anim);
}

void led_anim_blue(led_anim_t *anim)
{
  led_anim_start(&led_anim_state_blue, anim);
}

void led_setup()
{
  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_ACTIVITY, OUTPUT);

  digitalWrite(LED_STATUS, LOW);
  digitalWrite(LED_ACTIVITY, LOW);
}

bool led_loop()
{
  led_anim_loop(&led_anim_state_red);
  led_anim_loop(&led_anim_state_blue);
  
  return false;
}

