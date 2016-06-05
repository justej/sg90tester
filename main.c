/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "board_cfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void GPIO_Config();
static void TIM2_Config(uint16_t);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Main entry point.
  * @param  None
  * @retval None
  */
void main(void) {
  uint16_t t = 3000; // central position
  
  GPIO_Config();
  TIM2_Config(t);
  
  while (42) {
    BitStatus buttonUpPressed = GPIO_ReadInputPin(PORT_BUTTON_UP, PIN_BUTTON_UP);
    BitStatus buttonDownPressed = GPIO_ReadInputPin(PORT_BUTTON_DOWN, PIN_BUTTON_DOWN);
    if (buttonUpPressed == RESET) {
      // decrease duty factor
    }
    if (buttonDownPressed == RESET) {
      // increase duty factor
    }
  }
}

/**
  * @brief  Configure GPIO for LEDs, buttons and other purpose available on the board.
  * Pins that don't act as a GPIO (as peripheral's pins) should be configured in
  * the peripheral configuration function.
  * @param  None
  * @retval None
  */
static void GPIO_Config() {
  GPIO_Init(PORT_SERVO_CONTROL, (GPIO_Pin_TypeDef)PIN_SERVO_CONTROL, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_BUTTON_UP, (GPIO_Pin_TypeDef)PIN_BUTTON_UP, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_BUTTON_DOWN, (GPIO_Pin_TypeDef)PIN_BUTTON_DOWN, GPIO_MODE_IN_PU_NO_IT);
}

static void TIM2_Config(uint16_t initialPeriod) {
  TIM2_DeInit();
  
  /*
     Period T = 20 ms, pulse width t0 = 1..2 ms, system clock f0 = 8 MHz.
     CNTR * PRESCALER = T * f0 = 20e-3 * 8e6 = 160e3.
     Minimal possible PRESCALER value in this case is 4, and CNTR = 160e3 / 4 = 40e3.
     T = t0 + t1   =>   CNTR = CNTR_0 + CNTR_1
     Thus:
       PRESCALER = 4
       CNTR_0 = 2000, CNTR_1 = 38000 <- t0 = 1 ms,   t1 = 19 ms
       CNTR_0 = 3000, CNTR_1 = 37000 <- t0 = 1.5 ms, t1 = 18.5 ms
       CNTR_0 = 4000, CNTR_1 = 36000 <- t0 = 2 ms,   t1 = 18 ms
  */
  TIM2_TimeBaseInit(TIM2_PRESCALER_4, 40000);
  TIM2_PrescalerConfig(TIM2_PRESCALER_4, TIM2_PSCRELOADMODE_IMMEDIATE);
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);

  /* TIM2 enable counter */
  TIM2_Cmd(ENABLE);
}