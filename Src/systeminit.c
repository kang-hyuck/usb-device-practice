#include <stdint.h>  // uint32_t
#include "system_stm32f4xx.h"  // FLASH, RCC
#include "stm32f4xx.h"  // SET_BIT(), MODIFY_REG()
#include "Helpers/logger.h"

#define MHZ(x) (x*1000*1000)

LogLevel system_log_level = LOG_LEVEL_DEBUG;
uint32_t SystemCoreClock = MHZ(72);  // 72 MHz

/*
 * HCLK = 72 MHz
 * PLL: (M=4, N=72, P=2, Q=3)
 * AHB Prescaler = 1
 * APB Prescaler1 = 2
 * APB Prescaler2 = 1
 * MCO1 Prescaler = 2
 */

static void configure_clock()
{
	/**********************  Flash  **********************/
	/*
	 * To correctly read data from the Flash memory,
	 * set latency according to the CPU clock (HCLK)
	 */
    MODIFY_REG(FLASH->ACR,
    		FLASH_ACR_LATENCY,
			_VAL2FLD(FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS)); /* (Mask & Shift) Bit field values */

    /**********************  HSE  **********************/
    /* enable High Speed External Oscillator (HSE) */
    SET_BIT(RCC->CR, RCC_CR_HSEON);

    /* waits until HSE is stable */
    while (!READ_BIT(RCC->CR, RCC_CR_HSERDY));

    /**********************  PLL  **********************/
    /* configures PLL */
    MODIFY_REG(RCC->PLLCFGR,
    		RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLSRC,
    		_VAL2FLD(RCC_PLLCFGR_PLLM, 4) | _VAL2FLD(RCC_PLLCFGR_PLLN, 72) | _VAL2FLD(RCC_PLLCFGR_PLLQ, 3) | RCC_PLLCFGR_PLLSRC_HSE); /* set (M,N,P,Q,source) / P=2(register value: 0x0) */

    /* enable PLL */
    SET_BIT(RCC->CR, RCC_CR_PLLON);

    /* waits until PLL is stable */
    while (!READ_BIT(RCC->CR, RCC_CR_PLLRDY));

    /**********************  System Clock  **********************/
    /* set System Clock to PLL */
    MODIFY_REG(RCC->CFGR,
    		RCC_CFGR_SW,
			_VAL2FLD(RCC_CFGR_SW, RCC_CFGR_SW_PLL));

    /* set APB Prescaler */
    MODIFY_REG(RCC->CFGR,
    		RCC_CFGR_PPRE1,
			_VAL2FLD(RCC_CFGR_PPRE1, 4));  /* APB Pre1=2 / APB Pre2=1(default) / ABP Pre=1(default) */

    /* waits until System Clock is set to PLL */
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); /* check status register value == PLL */

    /**********************  ETC  **********************/
    CLEAR_BIT(RCC->CR, RCC_CR_HSION);  /* turn off HSI after setting HSE */
}

void SystemInit(void)
{
	configure_clock();
}

