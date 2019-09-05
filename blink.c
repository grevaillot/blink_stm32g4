#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/lptimer.h>
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

void lptim1_isr(void)
{
//	gpio_toggle(GPIOA, GPIO5);
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_SYSCFG);
	rcc_periph_clock_enable(RCC_LPTIM1);

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

	pwr_disable_backup_domain_write_protect();
	rcc_osc_on(RCC_LSE);
	rcc_wait_for_osc_ready(RCC_LSE);
	pwr_enable_backup_domain_write_protect();

	systick_set_frequency(1000, rcc_ahb_frequency);
	systick_interrupt_enable();
	systick_counter_enable();

	rcc_set_peripheral_clk_sel(LPTIM1, RCC_CCIPR1_LPTIM1SEL_LSI);

	rcc_periph_clock_enable(RCC_LPTIM1);
	rcc_periph_reset_pulse(RST_LPTIM1);

	lptimer_set_internal_clock_source(LPTIM1);
	lptimer_enable_trigger(LPTIM1, LPTIM_CFGR_TRIGEN_SW);
	lptimer_set_prescaler(LPTIM1, LPTIM_CFGR_PRESC_2);

	lptimer_enable(LPTIM1);

	lptimer_set_period(LPTIM1, 0xffff);
	lptimer_set_compare(LPTIM1, 0xffff);

	lptimer_clear_flag(LPTIM1, LPTIM_ICR_ARRMCF | LPTIM_ICR_CMPMCF);
	lptimer_enable_irq(LPTIM1, LPTIM_IER_ARRMIE | LPTIM_IER_CMPMIE);
	nvic_enable_irq(NVIC_LPTIM1_IRQ);

	lptimer_start_counter(LPTIM1, LPTIM_CR_CNTSTRT);

	rcc_set_mcopre(RCC_CFGR_MCOPRE_DIV16);
	rcc_set_mco(RCC_CFGR_MCO_SYSCLK);

	while (1) {
	}

	return 0;
}

