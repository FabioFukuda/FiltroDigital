#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "tx_api.h"

#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"

#include "math_helper.h"
#include "arm_math.h"

#include "menu.h"

#define STACK_SIZE         1024
#define BYTE_POOL_SIZE     9120
#define QUEUE_SIZE         100
#define PWM_FREQ  				 1000000
#define SAMPLE_GEN_FREQ		 40000
#define DEMO_QUEUE_SIZE 	 2

TX_THREAD								thread_filter_calc;
TX_THREAD								thread_menu;
TX_THREAD								thread_filter;
TX_EVENT_FLAGS_GROUP    event_flags;
TX_MUTEX                mutex;

TX_BYTE_POOL            byte_pool_0;
UCHAR                   byte_pool_memory[BYTE_POOL_SIZE];

uint32_t sysClock;
uint32_t pwd_ticks;
uint32_t sample;

void coeficientes(int ordem, float fc1, float fc2, float fs, int tipo, float* coeficientes);
void thread_filter_entry(ULONG thread_input);
void thread_filter_calc_entry(ULONG thread_input);
void thread_menu_entry(ULONG thread_input);

void gpio_init(void);
void pwm_init(void);
void adc_init(void);
void uart_init(void);

#define BUFFER_LEN 256
#define M_PI 3.14159265358979323846264338327950288

float buffer_1[BUFFER_LEN];
float buffer_2[BUFFER_LEN];
float* buffer_in = buffer_1;
float* full_buffer = buffer_2;
uint8_t buffer_in_id = 1;
uint32_t cur_index = 0;

bool flag = false;

float buffer_out_1[BUFFER_LEN];
float buffer_out_2[BUFFER_LEN];
float* buffer_out = buffer_out_1;
float* full_buffer_out = buffer_out_2;
uint8_t buffer_out_id = 1;
uint32_t cur_index_out = 0;

/*Parâmetros Teste do Filtro*/

int filter_order = MAX_ORDER;

bool on;
int type =1;
int fc1 = 1000;
int fc2 = 10000;

float coef[MAX_ORDER+1];

arm_fir_instance_f32 S;
float32_t firStateF32[BUFFER_LEN + MAX_ORDER - 1];

extern void menu_init();
extern void userInput(int jY,int b1,int b2);
extern void updateMenu();

int main(void){
	sysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
																	 SYSCTL_OSC_MAIN |
																		SYSCTL_USE_PLL |
																	 SYSCTL_CFG_VCO_240), 120000000);
	
	coeficientes(MAX_ORDER, fc1, fc2, SAMPLE_GEN_FREQ, type,coef);
	arm_fir_init_f32(&S, MAX_ORDER+1, (float32_t *)&coef[0], firStateF32, BUFFER_LEN);
	
	menu_init();
	uart_init();
	gpio_init();
	pwm_init();
	adc_init();
	
	tx_kernel_enter();
}

void    tx_application_define(void *first_unused_memory){
	CHAR    *pointer = TX_NULL;
	tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, BYTE_POOL_SIZE);
	
	tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, STACK_SIZE, TX_NO_WAIT);
	tx_thread_create(&thread_filter, "thread filter", thread_filter_entry, 0,  pointer, STACK_SIZE, 0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);
	
	tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, STACK_SIZE, TX_NO_WAIT);
	tx_thread_create(&thread_filter_calc, "thread filter calculator", thread_filter_calc_entry, 0,  pointer, STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
	
	tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, STACK_SIZE, TX_NO_WAIT);
	tx_thread_create(&thread_menu, "thread menu", thread_menu_entry, 0,  pointer, STACK_SIZE, 2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
	
	tx_event_flags_create(&event_flags, "event flags");
	tx_mutex_create(&mutex, "mutex", TX_NO_INHERIT);
	
}

void thread_filter_entry(ULONG thread_input){
	ULONG   actual_flags;
	ULONG 	received_message;
	UINT    status;
	
	while(1){
		tx_event_flags_get(&event_flags, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
		tx_mutex_get(&mutex, TX_WAIT_FOREVER);
		if (on)
			arm_fir_f32(&S, full_buffer, buffer_out , BUFFER_LEN);
		else
			for(int i=0;i<BUFFER_LEN;i++) buffer_out[i] = full_buffer[i];
		
		tx_mutex_put(&mutex);
		flag = true;
	}
}

void    thread_filter_calc_entry(ULONG thread_input)
{
	ULONG   actual_flags;
	while(1){		
		tx_event_flags_get(&event_flags, 0x2, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
		tx_mutex_get(&mutex, TX_WAIT_FOREVER);
		
		flag = false;
		
		memset(firStateF32,0,BUFFER_LEN+MAX_ORDER-1);
		memset(full_buffer,0,BUFFER_LEN);
		memset(buffer_out,0,BUFFER_LEN);
		memset(full_buffer_out,0,BUFFER_LEN);
		memset(coef,0,MAX_ORDER+1);
		
		coeficientes(filter_order, fc1, fc2, SAMPLE_GEN_FREQ, type,coef);
		arm_fir_init_f32(&S, filter_order+1, (float32_t *)&coef[0], firStateF32, BUFFER_LEN);
		
		cur_index_out = 0;
		
		tx_mutex_put(&mutex);
	}
}

void    thread_menu_entry(ULONG thread_input)
{
	while(1){		
		ADCProcessorTrigger(ADC0_BASE, 0);
		while(!ADCIntStatus(ADC0_BASE, 0, false)){}
		uint32_t ui32Value[2];
		ADCSequenceDataGet(ADC0_BASE, 0, ui32Value);
		uint32_t push_1 = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1);
		uint32_t push_2 = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2);
		userInput(ui32Value[0],push_1,push_2);
		updateMenu();
		tx_thread_sleep(1);
	}
}


void ADC1SS0_Handler(void){
	ADCIntClear(ADC1_BASE, 0);
	
	ADCSequenceDataGet(ADC1_BASE, 0, &sample);
	buffer_in[cur_index] = sample;
	
	if (flag){
		float sample_normalized = full_buffer_out[cur_index_out++]*1.0/4095;
		if (on && (type == 2 || type == 3)) sample_normalized+=0.5;
		if (cur_index_out == BUFFER_LEN){
			cur_index_out = 0;
			if (buffer_out_id == 1){buffer_out_id = 2;buffer_out = buffer_out_2; full_buffer_out = buffer_out_1;}
			else{buffer_out_id = 1;buffer_out = buffer_out_1; full_buffer_out = buffer_out_2;}
		}	
		int ticks;
		if (pwd_ticks*sample_normalized < 1) ticks = 1;
		else ticks = pwd_ticks*sample_normalized;
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5,ticks);
	}

	cur_index++;
	if (cur_index == BUFFER_LEN){
			cur_index = 0;
			tx_event_flags_set(&event_flags, 0x1, TX_OR);
			if (buffer_in_id == 1){buffer_in_id = 2;buffer_in = buffer_2; full_buffer = buffer_1;}
			else{buffer_in_id = 1;buffer_in = buffer_1; full_buffer = buffer_2;}
	}
}

void coeficientes(int ordem, float fc1, float fc2, float fs, int tipo, float* coeficientes){
  
	float meia_ordem = (float) ordem/2;
	float denominador_normalizar = 0;

	if(tipo == 1){
			denominador_normalizar=0;
			for(int i=0;i<=ordem;i++){
					if(i!=meia_ordem){
							coeficientes[i] = (0.54-0.46*cos(2*i*M_PI/ordem))*(sin(2*M_PI*fc1/fs*(i-meia_ordem))/(M_PI*(i-meia_ordem)));
					}else{
							coeficientes[i]=(0.54-0.46*cos(2*i*M_PI/ordem))*2*fc1/(fs);
					}
					denominador_normalizar+= (coeficientes[i]);
			}
			for(int i=0;i<=ordem;i++){
					coeficientes[i] = coeficientes[i]/denominador_normalizar;
			}
	}
	else if(tipo == 2){
        denominador_normalizar=0;
        for(int i=0;i<=ordem;i++){
            if(i!=meia_ordem){
                coeficientes[i] = -1*(0.54-0.46*cos(2*i*M_PI/ordem))*(sin(2*M_PI*fc1/fs*(i-meia_ordem))/(M_PI*(i-meia_ordem)));
            }else{
                coeficientes[i]= -1*(0.54-0.46*cos(2*i*M_PI/ordem))*2*fc1/(fs);
            }
        denominador_normalizar+= (coeficientes[i]);
        }
        for(int i=0;i<=ordem;i++){
            coeficientes[i] = coeficientes[i]/fabs(denominador_normalizar);
        }
        coeficientes[(int)meia_ordem] +=1;
    }
    else if(tipo ==3 ){
        float denominador_normalizar_real =0,denominador_normalizar_imaginario =0, f_media = (fc1+fc2)/(2*fs);
        for(int i=0;i<=ordem;i++){
            if(i!=meia_ordem)
                coeficientes[i] = (0.54-0.46*cos(2*i*M_PI/ordem))*(-1*sin(2*M_PI*fc1/fs*(i-meia_ordem))/(M_PI*(i-meia_ordem))+sin(2*M_PI*fc2/fs*(i-meia_ordem))/(M_PI*(i-meia_ordem)));
            else{
                coeficientes[(int)meia_ordem]=(0.54-0.46*cos(2*i*M_PI/ordem))*(-2*fc1/fs+2*fc2/fs);
            }
            if(coeficientes[i]>0){
                denominador_normalizar_real+=coeficientes[i];
            }
            else if(coeficientes[i]<0){
                denominador_normalizar_imaginario-=coeficientes[i];
            }
        }        
        for(int i=0;i<=ordem;i++){
            if(coeficientes[i]>0){    
                coeficientes[i] = coeficientes[i]/denominador_normalizar_real;
            }else if(coeficientes[i]<0){
                coeficientes[i] = coeficientes[i]/denominador_normalizar_imaginario;
            }
        }
    }
}

void gpio_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
	{
	}
	GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_1);
	GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_2);
}

//G1
void pwm_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG))
	{
	}
	GPIOPinConfigure(GPIO_PG1_M0PWM5); 
	
	GPIOPinTypePWM(GPIO_PORTG_BASE,GPIO_PIN_1);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
	{
	}
	
	PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_1);
	
	PWMGenConfigure(PWM0_BASE, PWM_GEN_2,PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	
	pwd_ticks = sysClock/(PWM_FREQ);
	
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, pwd_ticks);

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5,pwd_ticks*0.5);
	
	PWMGenEnable(PWM0_BASE, PWM_GEN_2);
	PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
}

void adc_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}
	
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH9);
	
	ADCSequenceEnable(ADC0_BASE, 0);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
	{}
	
	//B4
	ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_TIMER, 0);
	ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH10);
	ADCSequenceEnable(ADC1_BASE, 0);
		
	ADCIntClear(ADC1_BASE, 0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	uint32_t timerTicks = sysClock/SAMPLE_GEN_FREQ-1;
	TimerLoadSet(TIMER0_BASE, TIMER_A, timerTicks);
	TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
		
	ADCIntEnable(ADC1_BASE, 0);
	IntEnable(INT_ADC1SS0);
	TimerEnable(TIMER0_BASE, TIMER_A);
	IntMasterEnable();
}

void uart_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, sysClock);
}
