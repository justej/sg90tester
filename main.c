/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"
#include "board_cfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UPPER_LIMIT             ((uint16_t)4500)
#define LOWER_LIMIT             ((uint16_t)1500)
#define STEP                    ((uint16_t)100)
#define PERIOD                  ((uint16_t)40000)

#define F_CPU                   16000000ul
/* Private function prototypes -----------------------------------------------*/
static void CLK_Config();
static void GPIO_Config();
static void TIM2_Config(uint16_t, uint16_t);

static void pulse(uint16_t, uint16_t);
static void delay_ms(uint32_t);
/* Private functions ---------------------------------------------------------*/

static void pulse(uint16_t t0, uint16_t t1) {
  PORT_SERVO_CONTROL->ODR = PIN_SERVO_CONTROL;
  delay_ms(t0);
  PORT_SERVO_CONTROL->ODR = 0;
  delay_ms(t1);
  PORT_SERVO_CONTROL->ODR = PIN_SERVO_CONTROL;
  delay_ms(t0);
  PORT_SERVO_CONTROL->ODR = 0;
  delay_ms(t1);
  }

static void delay_ms(uint32_t ms) {
  while (ms-- > 0) {
    volatile uint16_t i;
    for (i = F_CPU / 13200; i > 0; i--) {
      // do nothing
    }
  }
}

/**
  * @brief Main entry point.
  * @param  None
  * @retval None
  */
void main(void) {
  uint16_t t = LOWER_LIMIT;
  
  CLK_Config();
  GPIO_Config();
  TIM2_Config(t, PERIOD - t);
  
  pulse(1, 19);
  delay_ms(2000);
  pulse(2, 18);
  delay_ms(2000);

  enableInterrupts();

  while (42) {
    delay_ms(100);
    t += STEP;
    if (t > UPPER_LIMIT) {
      t = LOWER_LIMIT;
    }
    updatePulseParameters(t, PERIOD - t);
  }
}

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config() {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

static void GPIO_Config() {
  GPIO_Init(PORT_SERVO_CONTROL, (GPIO_Pin_TypeDef)PIN_SERVO_CONTROL, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_BUTTON_UP, (GPIO_Pin_TypeDef)PIN_BUTTON_UP, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_BUTTON_DOWN, (GPIO_Pin_TypeDef)PIN_BUTTON_DOWN, GPIO_MODE_IN_PU_NO_IT);
}

static void TIM2_Config(uint16_t t0, uint16_t t1) {
  TIM2_DeInit();
  
  /*
     Period T = 20 ms, pulse width t0 = 1..2 ms, system clock f0 = 16 MHz.
     CNTR * PRESCALER = T * f0 = 20e-3 * 8e6 = 320e3.
     Minimal possible PRESCALER value in this case is 8, and CNTR = 320e3 / 8 = 40e3.
     T = t0 + t1   =>   CNTR = CNTR_0 + CNTR_1
     Thus:
       PRESCALER = 8
       CNTR_0 = 2000, CNTR_1 = 38000 <- t0 = 1 ms,   t1 = 19 ms
       CNTR_0 = 3000, CNTR_1 = 37000 <- t0 = 1.5 ms, t1 = 18.5 ms
       CNTR_0 = 4000, CNTR_1 = 36000 <- t0 = 2 ms,   t1 = 18 ms
  */
  updatePulseParameters(t0, t1);
  TIM2_TimeBaseInit(TIM2_PRESCALER_8, t0);
 // TIM2_PrescalerConfig(TIM2_PRESCALER_8, TIM2_PSCRELOADMODE_IMMEDIATE);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  /* TIM2 enable counter */
  TIM2_Cmd(ENABLE);
}