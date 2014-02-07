#include "loggerHardware.h"
#include "loggerConfig.h"
#include "board.h"
#include "accelerometer.h"
#include "ADC.h"
#include "timer.h"
#include "CAN.h"
#include "sdcard.h"
#include "constants.h"
#include "memory.h"
#include "spi.h"
#include "printk.h"

static unsigned int GetGPIOBits(void){
	return AT91F_PIO_GetInput(AT91C_BASE_PIOA);
}

static void ClearGPIOBits(unsigned int portBits){
	AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, portBits );
}

static void SetGPIOBits(unsigned int portBits){
	AT91F_PIO_SetOutput( AT91C_BASE_PIOA, portBits );
}

void InitLoggerHardware(){
	init_spi_lock();
	LoggerConfig *loggerConfig = getWorkingLoggerConfig();
	InitWatchdog(WATCHDOG_TIMEOUT_MS);
	accelerometer_init();
	ADC_init();
	timer_init(loggerConfig);
	InitGPIO(loggerConfig);
	InitPWM(loggerConfig);
	InitLEDs();
	InitPushbutton();
	InitSDCard();
	CAN_init(CAN_BAUD_500K);
}

void ResetWatchdog(){
	AT91F_WDTRestart(AT91C_BASE_WDTC);
}

void InitWatchdog(int timeoutMs){
	 int counter= AT91F_WDTGetPeriod(timeoutMs);
	 AT91F_WDTSetMode(AT91C_BASE_WDTC, AT91C_WDTC_WDRSTEN | AT91C_WDTC_WDRPROC | counter | (counter << 16));
	 AT91F_WDTC_CfgPMC();
}

void InitGPIO(LoggerConfig *loggerConfig){


//	AT91F_PIO_CfgInput(AT91C_BASE_PIOA, GPIO_MASK);
//	AT91C_BASE_PIOA->PIO_PPUDR = GPIO_MASK; //disable pullup
//	AT91C_BASE_PIOA->PIO_IFER = GPIO_MASK; //enable input filter
//	AT91C_BASE_PIOA->PIO_MDER = GPIO_MASK; //enable multi drain

	GPIOConfig * gpios = loggerConfig->GPIOConfigs;
	if (gpios[0].mode == CONFIG_GPIO_IN){
		AT91F_PIO_CfgInput(AT91C_BASE_PIOA, GPIO_1);
		AT91C_BASE_PIOA->PIO_PPUDR = GPIO_1; //disable pullup
		AT91C_BASE_PIOA->PIO_IFER = GPIO_1; //enable input filter
	}
	else{
	    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, GPIO_1 );
	    AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, GPIO_1 );
	}
	if (gpios[1].mode == CONFIG_GPIO_IN){
		AT91F_PIO_CfgInput(AT91C_BASE_PIOA, GPIO_2);
		AT91C_BASE_PIOA->PIO_PPUDR = GPIO_2; //disable pullup
		AT91C_BASE_PIOA->PIO_IFER = GPIO_2; //enable input filter
	}
	else{
	    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, GPIO_2 );
	    AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, GPIO_2 );
	}
	if (gpios[2].mode == CONFIG_GPIO_IN){
		AT91F_PIO_CfgInput(AT91C_BASE_PIOA, GPIO_3);
		AT91C_BASE_PIOA->PIO_PPUDR = GPIO_3; //disable pullup
		AT91C_BASE_PIOA->PIO_IFER = GPIO_3; //enable input filter
	}
	else{
	    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, GPIO_3 );
	    AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, GPIO_3 );
	}
}

void InitSDCard(void){
	AT91F_PIO_CfgInput(AT91C_BASE_PIOA, SD_CARD_DETECT | SD_WRITE_PROTECT);
	AT91C_BASE_PIOA->PIO_PPUER = SD_CARD_DETECT | SD_WRITE_PROTECT; //enable pullup
	AT91C_BASE_PIOA->PIO_IFER = SD_CARD_DETECT | SD_WRITE_PROTECT; //enable input filter
	InitFSHardware();
}

int isCardPresent(void){
	return (GetGPIOBits() & SD_CARD_DETECT) == 0;
}

int isCardWritable(void){
	return (GetGPIOBits() & SD_WRITE_PROTECT) == 0;
}

int isButtonPressed(void){
	return (GetGPIOBits() & PIO_PUSHBUTTON_SWITCH) != 0;
}

void setGpio(unsigned int channel, unsigned int state){
	unsigned int gpioBits = 0;
	switch (channel){
		case 0:
			gpioBits = GPIO_1;
			break;
		case 1:
			gpioBits = GPIO_2;
			break;
		case 2:
			gpioBits = GPIO_3;
			break;
	}
	if (state){
		SetGPIOBits(gpioBits);
	} else{
		ClearGPIOBits(gpioBits);
	}
}

int readGpio(unsigned int channel){
	unsigned int gpioStates = AT91F_PIO_GetInput(AT91C_BASE_PIOA);
	int value = 0;
	switch (channel){
		case 0:
			value = ((gpioStates & GPIO_1) != 0);
			break;
		case 1:
			value = ((gpioStates & GPIO_2) != 0);
			break;
		case 2:
			value = ((gpioStates & GPIO_3) != 0);
			break;
		default:
			break;
	}
	return value;
}

void readGpios(unsigned int *gpio1, unsigned int *gpio2, unsigned int *gpio3){
	unsigned int gpioStates = AT91F_PIO_GetInput(AT91C_BASE_PIOA);
	*gpio1 = ((gpioStates & GPIO_1) != 0);
	*gpio2 = ((gpioStates & GPIO_2) != 0);
	*gpio3 = ((gpioStates & GPIO_3) != 0);
}

void SetFREQ_ANALOG(unsigned int freqAnalogPort){
	AT91F_PIO_SetOutput( AT91C_BASE_PIOA, freqAnalogPort );	
}

void ClearFREQ_ANALOG(unsigned int freqAnalogPort){
	AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, freqAnalogPort );
}

///////////////////PWM Functions

static unsigned int MapPwmHardwareChannel(unsigned int softwareChannel){
	switch(softwareChannel){
	case 0:
		return 3;
		break;
	case 1:
		return 0;
		break;
	case 2:
		return 1;
		break;
	case 3:
	default:
		return 2;
		break;
	}
}

//------------------------------------------------------------------------------
/// Finds a prescaler/divisor couple to generate the desired frequency from
/// MCK.
/// Returns the value to enter in PWMC_MR or 0 if the configuration cannot be
/// met.
/// \param frequency  Desired frequency in Hz.
/// \param mck  Master clock frequency in Hz.
//------------------------------------------------------------------------------
static unsigned short PWM_GetClockConfiguration(
    unsigned int frequency,
    unsigned int mck)
{
    const unsigned int divisors[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    unsigned char divisor = 0;
    unsigned int prescaler;

    // Find prescaler and divisor values
    prescaler = (mck / divisors[divisor]) / frequency;
    while ((prescaler > 255) && (divisor < 11)) {

        divisor++;
        prescaler = (mck / divisors[divisor]) / frequency;
    }

    // Return result
    if (divisor < 11) {
        return prescaler | (divisor << 8);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
/// Configures PWM clocks A & B to run at the given frequencies. This function
/// finds the best MCK divisor and prescaler values automatically.
/// \param clka  Desired clock A frequency (0 if not used).
/// \param clkb  Desired clock B frequency (0 if not used).
/// \param mck  Master clock frequency.
//------------------------------------------------------------------------------
static void PWM_ConfigureClocks(unsigned int clka, unsigned int clkb, unsigned int mck){

    unsigned int mode = 0;
    unsigned int result;

    // Clock A
    if (clka != 0) {
        result = PWM_GetClockConfiguration(clka, mck);
        mode |= result;
    }

    // Clock B
    if (clkb != 0) {
        result = PWM_GetClockConfiguration(clkb, mck);
        mode |= (result << 16);
    }

    // Configure clocks
    AT91C_BASE_PWMC->PWMC_MR = mode;
}

//------------------------------------------------------------------------------
/// Configures PWM a channel with the given parameters.
/// The PWM controller must have been clocked in the PMC prior to calling this
/// function.
/// \param channel  Channel number.
/// \param prescaler  Channel prescaler.
/// \param alignment  Channel alignment.
/// \param polarity  Channel polarity.
//------------------------------------------------------------------------------
static void PWM_ConfigureChannel(
    unsigned int channel,
    unsigned int prescaler,
    unsigned int alignment,
    unsigned int polarity)
{
	unsigned int hardwareChannel = MapPwmHardwareChannel(channel);
    // Disable channel
    AT91C_BASE_PWMC->PWMC_DIS = 1 << hardwareChannel;
    // Configure channel
    AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CMR = prescaler | alignment | polarity;
}

static void InitPWMChannel(unsigned int channel, PWMConfig *config){
    // Configure PWMC channel (left-aligned)
    PWM_ConfigureChannel(channel, AT91C_PWMC_CPRE_MCKA, 0, AT91C_PWMC_CPOL);
    PWM_SetPeriod(channel, config->startupPeriod);
    PWM_SetDutyCycle(channel, config->startupDutyCycle);
    StartPWM(channel);
}

void InitPWM(LoggerConfig *loggerConfig){
	
	//Configure PWM Clock
	PWM_ConfigureClocks(loggerConfig->PWMClockFrequency * MAX_PWM_DUTY_CYCLE, 0, BOARD_MCK);

	//Configure PWM ports
	/////////////////////////////////////////
	//PWM0
	///////////////////////////////////////// 
 	//Configure Peripherials for PWM outputs
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			0, 				// mux function A
			AT91C_PIO_PA23); // mux funtion B

	/////////////////////////////////////////
	//PWM1
	///////////////////////////////////////// 
 	//Configure Peripherials for PWM outputs
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			0, 				// mux function A
			AT91C_PIO_PA24); // mux funtion B
	
	/////////////////////////////////////////
	//PWM2
	///////////////////////////////////////// 
 	//Configure Peripherials for PWM outputs
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			0, 				// mux function A
			AT91C_PIO_PA25); // mux funtion B

	/////////////////////////////////////////
	//PWM3
	///////////////////////////////////////// 
 	//Configure Peripherials for PWM outputs
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			0, 				// mux function A
			AT91C_PIO_PA7); // mux funtion B

	InitPWMChannel(0,&(loggerConfig->PWMConfigs[0]));
	InitPWMChannel(1,&(loggerConfig->PWMConfigs[1]));
	InitPWMChannel(2,&(loggerConfig->PWMConfigs[2]));
	InitPWMChannel(3,&(loggerConfig->PWMConfigs[3]));
	StartAllPWM();
}

void StartAllPWM(){
	AT91F_PWMC_StartChannel(AT91C_BASE_PWMC,(1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
}

void StopAllPWM(){
	AT91F_PWMC_StopChannel(AT91C_BASE_PWMC,(1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
}

void StartPWM(unsigned int channel){
	AT91F_PWMC_StartChannel(AT91C_BASE_PWMC,1 << channel);
}

void StopPWM(unsigned int channel){
	if (channel <= 3) AT91F_PWMC_StopChannel(AT91C_BASE_PWMC,1 << channel);
}

//------------------------------------------------------------------------------
/// Sets the period value used by a PWM channel. This function writes directly
/// to the CPRD register if the channel is disabled; otherwise, it uses the
/// update register CUPD.
/// \param channel  Channel number.
/// \param period  Period value.
//------------------------------------------------------------------------------
void PWM_SetPeriod(unsigned int channel, unsigned short period)
{
	unsigned int hardwareChannel = MapPwmHardwareChannel(channel);
    // If channel is disabled, write to CPRD
    if ((AT91C_BASE_PWMC->PWMC_SR & (1 << hardwareChannel)) == 0) {
        AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CPRDR = period;
    }
    // Otherwise use update register
    else {
        AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CMR |= AT91C_PWMC_CPD;
        AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CUPDR = period;
    }
}

unsigned short PWM_GetPeriod(unsigned int channel){
	unsigned int hardwareChannel = MapPwmHardwareChannel(channel);
	return AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CPRDR;
}

//------------------------------------------------------------------------------
/// Sets the duty cycle used by a PWM channel. This function writes directly to
/// the CDTY register if the channel is disabled; otherwise it uses the
/// update register CUPD.
/// Note that the duty cycle must always be inferior or equal to the channel
/// period.
/// \param channel  Channel number.
/// \param duty  Duty cycle value.
//------------------------------------------------------------------------------
void PWM_SetDutyCycle(unsigned int channel, unsigned short duty){

	unsigned int hardwareChannel = MapPwmHardwareChannel(channel);
	//duty cycle of zero freaks out the PWM controller
	//seems to invert the polarity. fix this up until we understand better.
	if (duty < 1 ) duty = 1;
	if (duty > 100) duty = 100;

    // If channel is disabled, write to CDTY
    if ((AT91C_BASE_PWMC->PWMC_SR & (1 << hardwareChannel)) == 0) {
        AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CDTYR = duty;
    }
    // Otherwise use update register
    else {
        AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CMR &= ~AT91C_PWMC_CPD;
        AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CUPDR = duty;
    }
}

unsigned short PWM_GetDutyCycle(unsigned int channel){
	unsigned int hardwareChannel = MapPwmHardwareChannel(channel);
	return AT91C_BASE_PWMC->PWMC_CH[hardwareChannel].PWMC_CDTYR;
}



void InitLEDs(void){
    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, LED_MASK ) ;
   //* Clear the LED's.
    AT91F_PIO_SetOutput( AT91C_BASE_PIOA, LED_MASK ) ;
}

void InitPushbutton(void){
	AT91F_PIO_CfgInput(AT91C_BASE_PIOA, PIO_PUSHBUTTON_SWITCH);
	AT91C_BASE_PIOA->PIO_PPUER = PIO_PUSHBUTTON_SWITCH; //enable pullup
	AT91C_BASE_PIOA->PIO_IFER = PIO_PUSHBUTTON_SWITCH; //enable input filter
	AT91C_BASE_PIOA->PIO_MDER = PIO_PUSHBUTTON_SWITCH; //enable multi drain
}

void enableLED(unsigned int Led){
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, Led );
}

void disableLED(unsigned int Led){
        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, Led );
}

void toggleLED (unsigned int Led){
    if ( (AT91F_PIO_GetInput(AT91C_BASE_PIOA) & Led ) == Led )
    {
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, Led );
    }
    else
    {
        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, Led );
    }
}


int flashLoggerConfig(){
	void * savedLoggerConfig = getSavedLoggerConfig();
	void * workingLoggerConfig = getWorkingLoggerConfig();

	return flashWriteRegion(savedLoggerConfig, workingLoggerConfig, sizeof (LoggerConfig));
}
