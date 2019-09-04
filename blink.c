#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/assert.h>

volatile int cnt = 0;
volatile int tempo = 2000;

void sys_tick_handler(void)
{
	if (cnt++ == tempo) {
		cnt = 0;
		gpio_toggle(GPIOA, GPIO5);
	}
}

void exti15_10_isr(void)
{
	gpio_toggle(GPIOA, GPIO5);
	exti_reset_request(EXTI13);
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_SYSCFG);

	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO13);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO8);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,  GPIO8);

	exti_select_source(EXTI13, GPIOC);
	exti_set_trigger(EXTI13, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI13);

	nvic_enable_irq(NVIC_EXTI15_10_IRQ);

	rcc_clock_setup(&rcc_clock_config[RCC_CLOCK_CONFIG_HSE_24MHZ_PLL_170MHZ]);

	systick_set_frequency(1000, rcc_ahb_frequency);
	systick_interrupt_enable();
	systick_counter_enable();

	rcc_set_mcopre(RCC_CFGR_MCOPRE_DIV16);
	rcc_set_mco(RCC_CFGR_MCO_SYSCLK);

	while (1) {
	}

	return 0;
}

