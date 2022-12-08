//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <string.h>

//System states
typedef enum
{
	D_State,
	S_State,
	B_State,
	C_State,
	W_State,
	R_State,
	M_State,
} sys_state;

//System Events
typedef enum 
{
	Data_Req_Event,
	Poll_ADC_Event,
	Whistle_Event,
	PB_Short_Event,
	PB_Long_Event,
	Count_Event,
	Reset_Event,
	Wait_Event,
	No_Event,
} sys_event;

#define STORAGE_SIZE	5

//Globals for state and events
sys_state next_state;
sys_event new_event;
//Whistle count var
unsigned char whistle_count = FALSE;
unsigned long time_count = 0;
unsigned char cnt = 0;
//Settings flags
unsigned char precision = 1; //0 = 1s, 1 = 0.5s, 2 = 0.1s precision
unsigned char sensitivity = 0xB0; //Trigger level for microphone
unsigned char sense_cnt = 0;
unsigned long sense_total;
unsigned char y = 3;
unsigned char cflag = 0;
//Data stored in memory
unsigned long time_memory[STORAGE_SIZE];
unsigned char mem_index = 0;
//Strings
char semicolon[2] = ":";
char w_string[8] = "Whistle";
char b_string[7] = "Button";
char r_string[12] = "Resolution";
char s_string[12] = "Sensitivity";
char precision_array[3][5] = { "1.0s", "0.5s", "0.1s"};
//Function prototypes
void display_data(unsigned char setting);
void display_count(void);
void convert_to_string(unsigned long time);
unsigned long mem_avg(void);
unsigned long mem_longest(void);
unsigned long mem_shortest(void);

void main(void)
{
	unsigned char index = 0, minmax = 0;
	Timer_100ms_EnableInt();
	while (index < STORAGE_SIZE)
	{
		time_memory[index++] = 0;
	}
	LCD_Start();
	PRT0IC0 = 0;
	PRT0IC1 = 1;
	PRT0IE = 1;
	INT_MSK0 |= 0X20;
	M8C_EnableGInt;
	ADC_Start(ADC_HIGHPOWER);
	Buffer_Start(Buffer_HIGHPOWER);
	//Set default reset state
	next_state = D_State;
	new_event = Reset_Event;
	while (1)
	{
		//Wait for a change in the event
		while (new_event == No_Event);
		//Event seen, switch on state
		switch (next_state)
		{
			//Data display state, shown on reset
			case D_State:
			{
				//On reset or entering state from mode change, show
				//data and change event and do nothing
				if (new_event == Reset_Event || new_event == Data_Req_Event)
				{
					//Find average, longest, and shortest times
					display_data(0);
					new_event = No_Event;
				}
				// PB is pressed long, move to Sound Meas. mode
				else if (new_event == PB_Long_Event)
				{
					LCD_Init();
					LCD_Position(0,0);
					LCD_PrString(w_string);
					next_state =  S_State;
					new_event = Poll_ADC_Event;
				}
				// PB is pressed short
				else if (new_event == PB_Short_Event)
				{				
					display_data(!minmax);
					minmax = !minmax;
					new_event = No_Event;
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
			//Sound Measurement state
			case S_State:
			{
				if (new_event == Poll_ADC_Event)
				{
					ADC_GetSamples(1);
					while (!ADC_fIsDataAvailable());
					if (ADC_bClearFlagGetData() > sensitivity) {
						new_event = Whistle_Event;
					}
				}			
				//Whistle heard, move to count mode
				else if (new_event == Whistle_Event)
				{
					next_state = C_State;
					new_event = Count_Event;
					whistle_count = TRUE;
				}
				// PB is pressed long, move to PB Meas. mode
				else if (new_event == PB_Long_Event)
				{
					LCD_Init();
					LCD_Position(0,0);
					LCD_PrString(b_string);
					next_state =  B_State;
					new_event = No_Event;
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
			//PB Measurement mode
			case B_State:
			{
				//PB pressed short, begin counting
				if (new_event == PB_Short_Event)
				{
					next_state = C_State;
					new_event = Count_Event;
					whistle_count = FALSE;
				}
				//PB pressed long, move to Res. Select state
				else if (new_event == PB_Long_Event)
				{
					LCD_Init();
					LCD_Position(0,0);
					LCD_PrString(r_string);
					LCD_Position(1,0);
					LCD_PrString(precision_array[precision]);
					next_state = R_State;
					new_event = No_Event;
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
			//Counting mode state
			case C_State:
			{
				//Entering Count mode
				if (new_event == Count_Event)
				{
					time_count = 0;
					Timer_100ms_Start();
					cflag = 1;
					new_event = No_Event;
					if (whistle_count == 1)
					{
						new_event = Poll_ADC_Event;
						while (time_count < 5);
					}
				}					
				//React to PB event always, whistle event if coming from sound mode
				else if ( (new_event == PB_Short_Event) || (new_event == Whistle_Event && whistle_count == TRUE) )
				{
					//Stop timer and change state
					Timer_100ms_Stop();
					cflag = 0;
					next_state = W_State;
					new_event = Wait_Event;
				}
				else if (new_event == Poll_ADC_Event)
				{
					ADC_GetSamples(1);
					while (!ADC_fIsDataAvailable());				
					if (ADC_bClearFlagGetData() > sensitivity) {
						new_event = Whistle_Event;
					}
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
			//Wait state, delay time and display final time
			case W_State:
			{
				//If direct from count state
				if (new_event == Wait_Event)
				{
					time_memory[mem_index++ % STORAGE_SIZE] = time_count;
					display_count();
					PRT0IE = 0;
					time_count = 0;
					Timer_100ms_Start();
					while (time_count / 10 < y);
					Timer_100ms_Stop();
					time_count = 0;
					PRT0IE = 1;
					LCD_Init();
					LCD_Position(0,0);
					if (whistle_count == TRUE) {
						LCD_PrString(w_string);
						next_state = S_State;
						new_event = Poll_ADC_Event;
					}
					else {
						LCD_PrString(b_string);
						next_state = B_State;
						new_event = No_Event;
					}
					
				}
				//PB pressed long, go to res. select state
				else if (new_event == PB_Long_Event)
				{
					LCD_Init();
					LCD_Position(0,0);
					LCD_PrString(r_string);
					next_state = R_State;
					new_event = No_Event;
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
			//Resolution Select state
			case R_State:
			{
				//PB pressed short, change resolution
				if (new_event == PB_Short_Event)
				{
					//Set new resolution
					precision = (precision + 1) %3;
					LCD_Position(1,0);
					LCD_PrString(precision_array[precision]);
					new_event = No_Event;
				}
				//PB pressed long, go to mic sense state
				else if (new_event == PB_Long_Event)
				{
					LCD_Init();
					LCD_Position(0,0);
					LCD_PrString(s_string);
					LCD_Position(1,0);
					LCD_PrHexByte(sensitivity);
					next_state = M_State;
					new_event = No_Event;
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
			//Microphone Sensitivity state
			case M_State:
			{
				//PB pressed short, change mic sense
				if (new_event == PB_Short_Event)
				{
					sense_total = 0;
					for (sense_cnt = 0; sense_cnt < 10; sense_cnt++)
					{
						ADC_GetSamples(1);
						while (ADC_fIsDataAvailable());
						sense_total += (long) ADC_bClearFlagGetData();
					}
					sensitivity = (unsigned char ) ( (sense_total / 10) + 0x20);
					LCD_Position(1,0);
					LCD_PrHexByte(sensitivity);
					PRT0IE = 0;
					time_count = 0;
					Timer_100ms_Start();
					while (time_count / 10 < y);
					Timer_100ms_Stop();
					time_count = 0;
					PRT0IE = 1;
					next_state = D_State;
					new_event = Data_Req_Event;
					
				}
				//PB pressed long, go to data state
				else if (new_event == PB_Long_Event)
				{
					next_state = D_State;
					new_event = Data_Req_Event;
				}
				//Event not used for this state, reset var and keep state
				else 
					new_event = No_Event;
				break;
			}
		}
	}
}

#pragma interrupt_handler Timer_100ms_ISR
void Timer_100ms_ISR(void) {
	time_count++;
	if (cflag)
		display_count();
}

#pragma interrupt_handler PB_ISR
void PB_ISR(void) {
	PRT0IE = 0;
	cnt = 0;
	Timer_PB_Start();
	while (cnt < 4)
	{
		while ( (INT_CLR1 &  0x04) == 0);
		cnt++;
		INT_CLR1 &= ~0x04;
	}
	Timer_PB_Stop();
	if ( (PRT0DR & 0x01) == 1){
		new_event = PB_Long_Event;
	}
	else {
		new_event = PB_Short_Event;
	}
	while ( (PRT0DR & 0x01) != 0);
	PRT0IE = 1;
}
//Display data memory
void display_data(unsigned char setting)
{
	char longest[12] = "";
	char shortest[12] = "";
	char average[12] = "";
	char avg_str[5] = "Avg ";
	char shortest_str[5] = "Min ";
	char longest_str[5] = "Max ";
	LCD_Position(0,0);
	LCD_PrString(avg_str);
	convert_to_string(mem_avg());
	if (setting) {
		LCD_Position(1,0);
		LCD_PrString(shortest_str);
		convert_to_string(mem_shortest());
	}
	else {
		LCD_Position(1,0);
		LCD_PrString(longest_str);
		convert_to_string(mem_longest());
	}	
}
//Displays the current count on the LCD
void display_count(void)
{
	LCD_Position(1,0);
	convert_to_string(time_count);
}
//Converts a read time to hh:mm:ss(:ms) format
void convert_to_string(unsigned long time)
{
	//Calculate times
	unsigned char ms, sec, min, hr;
	char zero[3] = "0";
	char temp_buf[3] = "";
	ms = time % 10;
	time = time / 10;
	sec =  time % 60;
	time = time / 60;
	min = time % 60;
	time = time /60;
	hr = (unsigned char)time;
	//Create string
	itoa(temp_buf, hr, 10);
	if (hr < 10)
	{
		strcat(zero, temp_buf);
		LCD_PrString(zero);
		zero[1] = '\0';
	}
	else
		LCD_PrString(temp_buf);
	LCD_PrString(semicolon);
	itoa(temp_buf, min, 10);
	if (min < 10)
	{
		strcat(zero, temp_buf);
		LCD_PrString(zero);
		zero[1] = '\0';
	}
	else
		LCD_PrString(temp_buf);
	LCD_PrString(semicolon);
	itoa(temp_buf, sec, 10);
	if (sec < 10)
	{
		strcat(zero,temp_buf);	
		LCD_PrString(zero);
	}
	else
		LCD_PrString(temp_buf);
	if (precision > 0)
	{
		LCD_PrString(semicolon);
		if (precision == 1)//Nearest half second
		{
			if (ms > 5)
				ms = 5;
			else 
				ms = 0;
		}
		itoa(temp_buf, ms, 10);
		LCD_PrString(temp_buf);
	}
}

//Calculate and return the average time
unsigned long mem_avg(void)
{
	unsigned long avg = 0;
	unsigned char i = 0, j = 0;
	while (i < STORAGE_SIZE)
	{
		avg += time_memory[i];	
		if (time_memory[i++] != 0)
			j++;
	}
	if (j == 0)
		return 0;
	return avg / j;
}
//Find and return the longest time
unsigned long mem_longest(void)
{
#if STORAGE_SIZE < 255
	unsigned char i;
	unsigned char max_index = 0;
#else 
	unsigned int i;
	unsigned int max_index = 0;
#endif
	for (i = 1; i < STORAGE_SIZE; i++)
	{
		if (time_memory[i] > time_memory[max_index])
			max_index = i;
	}
	return time_memory[max_index];
}
//Find and return the shortest time
unsigned long mem_shortest(void)
{
#if STORAGE_SIZE < 255
	unsigned char i;
	unsigned char min_index = 0;
#else 
	unsigned int i;
	unsigned int min_index = 0;
#endif
	for (i = 1; i < STORAGE_SIZE; i++)
	{
		if (time_memory[i] < time_memory[min_index] && time_memory[i] != 0)
			min_index = i;
	}
	return time_memory[min_index];
}






























