
#include "usbd_driver.h"

static void initialize_gpio()
{
	/**********************  Clock enable for GPIO  **********************/
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

	/**********************  GPIO  **********************/
	/* set alternate function for PB14(D-),PB15(D+): AF12(=0xC)*/
	MODIFY_REG(GPIOB->AFR[1],
			GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15,
			_VAL2FLD(GPIO_AFRH_AFSEL14, 0xC) | _VAL2FLD(GPIO_AFRH_AFSEL15, 0xC));

	/* set PB14,PB15 on alternate function mode */
	MODIFY_REG(GPIOB->MODER,
			GPIO_MODER_MODER14 | GPIO_MODER_MODER15,
			_VAL2FLD(GPIO_MODER_MODER14, 2) | _VAL2FLD(GPIO_MODER_MODER15, 2));
}

