/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm8s.h"
#include "stm8s_it.h"
#include "board_cfg.h"
#include "bit_utils.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SERVO_MAIN_LOWER_LIMIT      ((uint8_t)100)
#define SERVO_MAIN_UPPER_LIMIT      ((uint8_t)170)

#define SERVO_TIP_LOWER_LIMIT       ((uint8_t)50)
#define SERVO_TIP_UPPER_LIMIT       ((uint8_t)250)

#define SERVO_ROT_LOWER_LIMIT       ((uint8_t)100)
#define SERVO_ROT_UPPER_LIMIT       ((uint8_t)250)

#define STEP                        ((uint8_t)1)

#define PERIOD                      ((uint16_t)40000)
//#define BUMP_LIMIT                  (255 + SERVO_MAIN_LOWER_LIMIT)
#define KEY_POLLING_DELAY_MS        10
#define DISABLE_COUNTER_INIT_VALUE  500

#define F_CPU                       16000000ul
/* Private function prototypes -----------------------------------------------*/
static void CLK_Config();
static void GPIO_Config();
static void TIM2_Config(Pulse_t pulse[], uint8_t size);

static void delay_ms(uint32_t);

static void increasePulseWidth(Pulse_t pulse[], uint8_t size, CounterNumber_t n) {
  while (size > 0) {
    size--;
    if (pulse[size].n == n) {
      if (pulse[size].t < pulse[size].upperLimit) {
        pulse[size].t += STEP;
      }
      return;
    }
  }
}

static void decreasePulseWidth(Pulse_t pulse[], uint8_t size, CounterNumber_t n) {
  while (size > 0) {
    size--;
    if (pulse[size].n == n) {
      if (pulse[size].t > pulse[size].lowerLimit) {
        pulse[size].t -= STEP;
      }
      return;
    }
  }
}
/* Private functions ---------------------------------------------------------*/

static void delay_ms(uint32_t ms) {
  while (ms-- > 0) {
    volatile uint16_t i;
    for (i = F_CPU / 13200; i > 0; i--) {
      // do nothing
    }
  }
}

// Main entry point
void main(void) {
  Pulse_t pulse[N_SERVOS] = {
    {(SERVO_MAIN_LOWER_LIMIT + SERVO_MAIN_UPPER_LIMIT) / 2, COUNTER_SERVO_MAIN, SERVO_MAIN_LOWER_LIMIT, SERVO_MAIN_UPPER_LIMIT},
    {SERVO_TIP_LOWER_LIMIT, COUNTER_SERVO_TIP, SERVO_TIP_LOWER_LIMIT, SERVO_TIP_UPPER_LIMIT},
    {(SERVO_ROT_LOWER_LIMIT + SERVO_ROT_UPPER_LIMIT) / 2, COUNTER_SERVO_ROT, SERVO_ROT_LOWER_LIMIT, SERVO_ROT_UPPER_LIMIT} // Central position
  };
  Pulse_t pulseCopy[N_SERVOS];
  memcpy(pulseCopy, pulse, N_SERVOS * sizeof(Pulse_t));

  CLK_Config();
  GPIO_Config();
  TIM2_Config(pulseCopy, N_SERVOS);
  
  enableInterrupts();

  uint8_t updateRequired;
  uint8_t buttonPushed;
  uint16_t disableCounter = DISABLE_COUNTER_INIT_VALUE;
  while (42) {
    delay_ms(KEY_POLLING_DELAY_MS);
    
    updateRequired = FALSE;
    if (!testMask(PORT_ROT_LEFT->IDR, PIN_ROT_LEFT)) {
      increasePulseWidth(pulse, N_SERVOS, COUNTER_SERVO_ROT);
      updateRequired = TRUE;
    }
    if (!testMask(PORT_ROT_RIGHT->IDR, PIN_ROT_RIGHT)) {
      decreasePulseWidth(pulse, N_SERVOS, COUNTER_SERVO_ROT);
      updateRequired = TRUE;
    }
    if (!testMask(PORT_MAIN_ARM_UP->IDR, PIN_MAIN_ARM_UP)) {
      increasePulseWidth(pulse, N_SERVOS, COUNTER_SERVO_MAIN);
      updateRequired = TRUE;
    }
    if (!testMask(PORT_MAIN_ARM_DOWN->IDR, PIN_MAIN_ARM_DOWN)) {
      decreasePulseWidth(pulse, N_SERVOS, COUNTER_SERVO_MAIN);
      updateRequired = TRUE;
    }
    if (!testMask(PORT_TIP_ARM_UP->IDR, PIN_TIP_ARM_UP)) {
      increasePulseWidth(pulse, N_SERVOS, COUNTER_SERVO_TIP);
      updateRequired = TRUE;
    }
    if (!testMask(PORT_TIP_ARM_DOWN->IDR, PIN_TIP_ARM_DOWN)) {
      decreasePulseWidth(pulse, N_SERVOS, COUNTER_SERVO_TIP);
      updateRequired = TRUE;
    }
    
    if (updateRequired) {
      memcpy(pulseCopy, pulse, N_SERVOS * sizeof(Pulse_t));
      calculateCounterIncrements(pulseCopy, N_SERVOS);
      updateCounterIncrements(pulseCopy, N_SERVOS);
    }
    
    buttonPushed = !testMask(PORT_PUMP_BUTTON->IDR, PIN_PUMP_BUTTON);
    
    if (buttonPushed) {
      setBit(PORT_PUMP_INVERTED->ODR, PIN_PUMP_INVERTED);
    } else {
      resetBit(PORT_PUMP_INVERTED->ODR, PIN_PUMP_INVERTED);
    }
    
    // When no movement commands, if pump button is not pressed we stop sending pulses to the arm
    // after some delay (if pump button is pressed we continue sending commands in order to fix
    // the position with, possibly, some load). This should sovle servo heating problem
    if (buttonPushed || updateRequired) {
      disableCounter = DISABLE_COUNTER_INIT_VALUE;
      TIM2_Cmd(ENABLE);
    }
    
    if (disableCounter) {
      disableCounter--;
    } else {
      TIM2_Cmd(DISABLE);
    }
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
  // Configure outputs
  GPIO_Init(PORT_SERVO_ROT, (GPIO_Pin_TypeDef)PIN_SERVO_ROT, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_SERVO_MAIN_ARM, (GPIO_Pin_TypeDef)PIN_SERVO_MAIN_ARM, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_SERVO_TIP_ARM, (GPIO_Pin_TypeDef)PIN_SERVO_TIP_ARM, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_PUMP_INVERTED, (GPIO_Pin_TypeDef)PIN_PUMP_INVERTED, GPIO_MODE_OUT_PP_LOW_FAST);
  // Configure inputs
  GPIO_Init(PORT_MAIN_ARM_UP, (GPIO_Pin_TypeDef)PIN_MAIN_ARM_UP, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_MAIN_ARM_DOWN, (GPIO_Pin_TypeDef)PIN_MAIN_ARM_DOWN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_TIP_ARM_UP, (GPIO_Pin_TypeDef)PIN_TIP_ARM_UP, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_TIP_ARM_DOWN, (GPIO_Pin_TypeDef)PIN_TIP_ARM_DOWN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_ROT_LEFT, (GPIO_Pin_TypeDef)PIN_ROT_LEFT, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_ROT_RIGHT, (GPIO_Pin_TypeDef)PIN_ROT_RIGHT, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_PUMP_BUTTON, (GPIO_Pin_TypeDef)PIN_PUMP_BUTTON, GPIO_MODE_IN_PU_NO_IT);
}

static void TIM2_Config(Pulse_t pulse[], uint8_t size) {
  /*
     Period T = 20 ms, system clock f0 = 16 MHz.
     CNTR * PRESCALER = T * f0 = 20e-3 * 8e6 = 320e3.
     Minimal possible PRESCALER value in this case is 8, and CNTR = 320e3 / 8 = 40e3.
  */
  TIM2_DeInit();
  
  TIM2_TimeBaseInit(TIM2_PRESCALER_8, PERIOD);
  TIM2_PrescalerConfig(TIM2_PRESCALER_8, TIM2_PSCRELOADMODE_IMMEDIATE);
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  
  /*
    Pulse width t0 = 1..2 ms, system clock f0 = 16 MHz.
    CNTR * PRESCALER = T * f0 = 2e-3 * 8e6 = 32e3.
    Minimal possible PRESCALER value in this case is 128, and CNTR = 32e3 / 128 = 250.
    t0 => CNTR
    Thus:
      PRESCALER = 128
      CNTR = 125 <- t0 = 1 ms
      CNTR = 188 <- t0 = 1.5 ms
      CNTR = 250 <- t0 = 2 ms
  */
  TIM4_DeInit();
  
  calculateCounterIncrements(pulse, size);
  updateCounterIncrements(pulse, size);
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 1);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  // TIM2, TIM4 are NOT ENABLED yet
}