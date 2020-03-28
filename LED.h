
#ifndef _LED_H_
#define _LED_H_

#define LED_ANIM_LOOP       { 254, 0 }
#define LED_ANIM_END        { 253, 0 }
#define LED_ANIM_NEXT(anim) { 255, (uint32_t) anim }


typedef struct 
{
  uint8_t pct;
  uint32_t delay;
} led_anim_t;

typedef struct 
{
  uint32_t pin;
  uint32_t pos;
  uint32_t last_time;
  led_anim_t *anim;
} led_anim_state_t;


extern led_anim_t led_anim_ble_connected[];

extern led_anim_t led_anim_shortblink[];
extern led_anim_t led_anim_slowshortblink[];
extern led_anim_t led_anim_shortdoubleblinkloop[];
extern led_anim_t led_anim_shortdoubleblink[];
extern led_anim_t led_anim_emergency[];
extern led_anim_t led_anim_idle[];
extern led_anim_t led_anim_none[];
extern led_anim_t led_anim_mediumblink[];
extern led_anim_t led_anim_fastblink[];

void led_anim_loop(led_anim_state_t *state);
void led_anim_start(led_anim_state_t *state, led_anim_t *anim, bool force = false);
void led_anim_red(led_anim_t *anim);
void led_anim_blue(led_anim_t *anim);

#endif

