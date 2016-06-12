#ifndef BOARD_CFG_H_
#define BOARD_CFG_H_

#define PORT_ADC_IN             GPIOD
#define PIN_ADC_IN              GPIO_PIN_3
#define CHANNEL_ADC_IN          ADC1_CHANNEL_4
#define SHMITTRIG_ADC_IN        ADC1_SCHMITTTRIG_CHANNEL4

#define PORT_SERVO_CONTROL      GPIOD
#define PIN_SERVO_CONTROL       GPIO_PIN_3

#define PORT_BUTTON_UP          GPIOC
#define PIN_BUTTON_UP           GPIO_PIN_7

#define PORT_BUTTON_DOWN        GPIOC
#define PIN_BUTTON_DOWN         GPIO_PIN_6

#endif // BOARD_CFG_H_