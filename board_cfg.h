#ifndef BOARD_CFG_H_
#define BOARD_CFG_H_

// Generic constants
#define N_SERVOS                3

// Arm controls (outputs)
#define PORT_SERVO_ROT          GPIOD
#define PIN_SERVO_ROT           GPIO_PIN_4

#define PORT_SERVO_MAIN_ARM     GPIOD
#define PIN_SERVO_MAIN_ARM      GPIO_PIN_5

#define PORT_SERVO_TIP_ARM      GPIOD
#define PIN_SERVO_TIP_ARM       GPIO_PIN_6

#define PORT_PUMP_INVERTED      GPIOA
#define PIN_PUMP_INVERTED       GPIO_PIN_3

// Joystick (inputs)
#define PORT_MAIN_ARM_UP        GPIOD
#define PIN_MAIN_ARM_UP         GPIO_PIN_3

#define PORT_MAIN_ARM_DOWN      GPIOD
#define PIN_MAIN_ARM_DOWN       GPIO_PIN_2

#define PORT_TIP_ARM_UP         GPIOC
#define PIN_TIP_ARM_UP          GPIO_PIN_7

#define PORT_TIP_ARM_DOWN       GPIOC
#define PIN_TIP_ARM_DOWN        GPIO_PIN_6

#define PORT_ROT_LEFT           GPIOC
#define PIN_ROT_LEFT            GPIO_PIN_5

#define PORT_ROT_RIGHT          GPIOC
#define PIN_ROT_RIGHT           GPIO_PIN_4

#define PORT_PUMP_BUTTON        GPIOC
#define PIN_PUMP_BUTTON         GPIO_PIN_3

#endif // BOARD_CFG_H_