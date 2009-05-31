#include <common.h>
#include <asm/arch/hardware.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/clk.h>
#include <asm/arch/io.h>

int arch_cpu_init(void)
{
#ifdef AT91_MAIN_CLOCK
	return at91_clock_init(AT91_MAIN_CLOCK);
#else
	return at91_clock_init(0);
#endif
}

#if defined(CONFIG_DISPLAY_CPUINFO)
int print_cpuinfo(void)
{
	char buf[32];

	printf("CPU: %s\n", AT91_CPU_NAME);
	printf("Crystal frequency: %8s MHz\n",
					strmhz(buf, get_main_clk_rate()));
	printf("CPU clock        : %8s MHz\n",
					strmhz(buf, get_cpu_clk_rate()));
	printf("Master clock     : %8s MHz\n",
					strmhz(buf, get_mck_clk_rate()));

	return 0;
}
#endif
