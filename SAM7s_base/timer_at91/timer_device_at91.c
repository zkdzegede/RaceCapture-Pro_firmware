#include "timer_device.h"
#include "board.h"

//Timer/Counter declarations
#define TIMER0_INTERRUPT_LEVEL 	5
#define TIMER1_INTERRUPT_LEVEL 	5
#define TIMER2_INTERRUPT_LEVEL 	5

#define MAX_TIMER_VALUE			0xFFFF
#define TIMER_CHANNELS			3

extern void ( timer0_irq_handler )( void );
extern void ( timer1_irq_handler )( void );
extern void ( timer2_irq_handler )( void );
extern void ( timer0_slow_irq_handler )( void );
extern void ( timer1_slow_irq_handler )( void );
extern void ( timer2_slow_irq_handler )( void );

unsigned int g_timer0_overflow;
unsigned int g_timer1_overflow;
unsigned int g_timer2_overflow;
unsigned int g_timer_counts[TIMER_CHANNELS];

static unsigned int timer_clock_from_divider(unsigned short divider){
	switch (divider){
		case 2:
			return AT91C_TC_CLKS_TIMER_DIV1_CLOCK;
		case 8:
			return AT91C_TC_CLKS_TIMER_DIV2_CLOCK;
		case 32:
			return AT91C_TC_CLKS_TIMER_DIV3_CLOCK;
		case 128:
			return AT91C_TC_CLKS_TIMER_DIV4_CLOCK;
		case 1024:
			return AT91C_TC_CLKS_TIMER_DIV5_CLOCK;
		default:
			return AT91C_TC_CLKS_TIMER_DIV5_CLOCK;
	}
}

static void init_timer_0(size_t divider, unsigned int slowTimerMode){

	g_timer0_overflow = 1;
	g_timer_counts[0] = 0;

	// Set PIO pins for Timer Counter 0
	//TIOA0 connected to PA0
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,0,AT91C_PA0_TIOA0);

	 //enable pullup (optoisolator input)
	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PA0_TIOA0; //disable pullup
	AT91C_BASE_PIOA->PIO_IFER = AT91C_PA0_TIOA0; //enable input filter

	/// Enable TC0's clock in the PMC controller
	AT91F_TC0_CfgPMC();

	AT91F_TC_Open (
	AT91C_BASE_TC0,

	timer_clock_from_divider(divider) |
	AT91C_TC_ETRGEDG_FALLING |
	AT91C_TC_ABETRG |
	AT91C_TC_LDRA_RISING|
	AT91C_TC_LDRB_FALLING

	,AT91C_ID_TC0
	);

	if (slowTimerMode){
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC0, TIMER0_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, timer0_slow_irq_handler);
		AT91C_BASE_TC0->TC_IER = AT91C_TC_LDRBS | AT91C_TC_COVFS;  // IRQ enable RB loading and overflow
	}
	else{
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC0, TIMER0_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, timer0_irq_handler);
		AT91C_BASE_TC0->TC_IER = AT91C_TC_COVFS;  // IRQ enable overflow
	}
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC0);
}

static void init_timer_1(size_t divider, unsigned int slowTimerMode){

	g_timer1_overflow = 1;
	g_timer_counts[1] = 0;

	// Set PIO pins for Timer Counter 1
	// TIOA1 mapped to PA15
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,0,AT91C_PA15_TIOA1);

	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PA15_TIOA1; //disable pullup
	AT91C_BASE_PIOA->PIO_IFER = AT91C_PA15_TIOA1; //enable input filter

	// Enable TC0's clock in the PMC controller
	AT91F_TC1_CfgPMC();

	AT91F_TC_Open (
	AT91C_BASE_TC1,

	timer_clock_from_divider(divider) |
	AT91C_TC_ETRGEDG_FALLING |
	AT91C_TC_ABETRG |
	AT91C_TC_LDRA_RISING|
	AT91C_TC_LDRB_FALLING

	,AT91C_ID_TC1
	);

	if (slowTimerMode){
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC1, TIMER1_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, timer1_slow_irq_handler);
		AT91C_BASE_TC1->TC_IER = AT91C_TC_LDRBS | AT91C_TC_COVFS;  // IRQ enable RB loading and overflow
	}
	else{
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC1, TIMER1_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, timer1_irq_handler);
		AT91C_BASE_TC1->TC_IER = AT91C_TC_COVFS;  // IRQ enable overflow
	}
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC1);
}

static void init_timer_2(size_t divider, unsigned int slowTimerMode){

	g_timer2_overflow = 1;
	g_timer_counts[2] = 0;
	// Set PIO pins for Timer Counter 2
	// TIOA2 mapped to PA26
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,0,AT91C_PA26_TIOA2);

	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PA26_TIOA2; //disable pullup
	AT91C_BASE_PIOA->PIO_IFER = AT91C_PA26_TIOA2; //enable input filter

	// Enable TC0's clock in the PMC controller
	AT91F_TC2_CfgPMC();

	AT91F_TC_Open (
	AT91C_BASE_TC2,

	timer_clock_from_divider(divider) |
	AT91C_TC_ETRGEDG_FALLING |
	AT91C_TC_ABETRG |
	AT91C_TC_LDRA_RISING|
	AT91C_TC_LDRB_FALLING

	,AT91C_ID_TC2
	);

	if (slowTimerMode){
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC2, TIMER2_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, timer2_slow_irq_handler);
		AT91C_BASE_TC2->TC_IER =AT91C_TC_LDRBS | AT91C_TC_COVFS;  // IRQ enable RB loading and overflow
	}
	else{
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC2, TIMER2_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, timer2_irq_handler);
		AT91C_BASE_TC2->TC_IER = AT91C_TC_COVFS;  // IRQ enable overflow
	}
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC2);
}

static unsigned int getTimer0Period(){
	return g_timer0_overflow ? 0 : AT91C_BASE_TC0->TC_RB;
}

static unsigned int getTimer1Period(){
	return g_timer1_overflow ? 0 : AT91C_BASE_TC1->TC_RB;
}

static unsigned int getTimer2Period(){
	return g_timer2_overflow ? 0 : AT91C_BASE_TC2->TC_RB;
}

int timer_device_init(size_t channel, unsigned int divider, unsigned int slowTimerMode){
	switch(channel){
		case 0:
			init_timer_0(divider, slowTimerMode);
			return 1;
		case 1:
			init_timer_1(divider, slowTimerMode);
			return 1;
		case 2:
			init_timer_2(divider, slowTimerMode);
			return 1;
		default:
			return 0;
	}
}

void timer_device_get_all_periods(unsigned int *t0, unsigned int *t1, unsigned int *t2){
	*t0 = getTimer0Period();
	*t1 = getTimer1Period();
	*t2 = getTimer2Period();
}

void timer_device_reset_count(unsigned int channel){
	if (channel >= 0 && channel < TIMER_CHANNELS){
		g_timer_counts[channel] = 0;
	}
}

unsigned int timer_device_get_count(unsigned int channel){
	if (channel >= 0 && channel < TIMER_CHANNELS){
		return g_timer_counts[channel];
	}
	else{
		return 0;
	}
}

unsigned int timer_device_get_period(unsigned int channel){
	switch (channel){
		case 0:
			return getTimer0Period();
		case 1:
			return getTimer1Period();
		case 2:
			return getTimer2Period();
	}
	return 0;
}


