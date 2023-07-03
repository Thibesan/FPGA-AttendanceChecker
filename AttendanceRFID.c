/* this code demonstrates the efficacy of a RFID attendance device.
Button 0 simulated the scanning of a RFID tag. The state of Switches
0, 1, and 2 represent the ID of the tag being scanned(like a student number).
A timer begins on the press of button 3(representing the beggining of class)
and the time of any RFID scan is compared to the timers current state. The potentiometer
hooked up to ADC port 0 can be used by the teacher to change the amount of time
students have to arrive before being marked as late.
*/



//ARMv7 micro
#define BUTTON_BASE 		0xFF200050
#define PRIVATE_TIMER_BASE 	0xFFFEC600
#define LED_BASE	      	0xFF200000
#define HEX0_HEX3_BASE 		0xFF200020
#define HEX4_HEX5_BASE 		0xFF200030
#define SW_BASE				0xFF200040
#define ADC_BASE            0xFF204000

typedef struct
{
    int load;
    int count;
    int control;
    int status;
} A9Timer; //using A9 private timer

//for 7seg display use
unsigned char hexCode[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x7, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
unsigned char displayValues[6] = {0, 0, 0, 0, 0, 0};


volatile unsigned char *const buttonPointer = (unsigned char *)BUTTON_BASE;
volatile A9Timer *const timerPointer = (A9Timer *)PRIVATE_TIMER_BASE;
volatile unsigned int *const ledPointer = (unsigned int *)LED_BASE;
volatile unsigned int *const hexPointer[2] = {HEX0_HEX3_BASE, HEX4_HEX5_BASE};
volatile unsigned int *const switchPointer = (unsigned int *)SW_BASE;
volatile unsigned int * const ADC_ptr = (unsigned int *)ADC_BASE;

int time = 0;
int timer = 0;
int present = 0;
unsigned int studentNumbers[6] = {0,1,2,4,5,7};
unsigned int studentAttendance[6] = {0,0,0,0,0,0};
unsigned int studentTime[6] = {0,0,0,0,0,0};
late = 300;

//to display a value on the 7seg diplay
void DisplayHex(unsigned int val, unsigned int studentNum)
{
    if(val == 0)
	{
		*hexPointer[0] = 0x3F5E0800 + hexCode[studentNum];//says good for on time
    	*hexPointer[1] = 0x3D3F;
	}else if(val == 1)
	{
		*hexPointer[0] = 0x07790800 + hexCode[studentNum];//says late for late
    	*hexPointer[1] = 0x3877;
	}else if(val == 2)
	{
		*hexPointer[0] = 0;
    	*hexPointer[1] = 0x5477; // says nA for student not found
	}else if(val == 3)//for displaying the time
	{
		for (int i = 0; i < sizeof(displayValues) / sizeof(displayValues[0]); i++)
    	{
        	displayValues[i] = (studentNum % 10);
        	studentNum = studentNum / 10;
    	}
		*hexPointer[0] = (((hexCode[displayValues[3]] << 8 | hexCode[displayValues[2]]) << 8 | hexCode[displayValues[1]]) << 8 | hexCode[displayValues[0]]);
   		*hexPointer[1] = (hexCode[displayValues[5]] << 8 | hexCode[displayValues[4]]);
	}
}



void InitializeTimer()
{
    timerPointer->load = 200000000;//the timer counts seconds
    timerPointer->status = 0b01;
    timerPointer->control = 0b0000;
}


void RunTimer()
{
    timerPointer->load = 200000000;
    timerPointer->status = 0b01;
    timerPointer->control = 0b0001;
}

											
											  
int main()
{
	InitializeTimer();
	while(1)
	{
		if(*switchPointer>10)
		{
			DisplayHex(3,time);
		}
		
		
		
		ADC_ptr[0] = 0x1;//ADC refresh
		
		if(*buttonPointer == 8 && timer == 0)//begin class(start timer)
		{
			RunTimer();
			timer = 1;
		}
		if (timerPointer->status == 1)
        {
            RunTimer();
            time += 1; //every second
		}
		
		volatile int value = ADC_ptr[0];
        value &= 0xFFF; //get the potentiometer value
		
		if(value<1000) //set the "late" time as 2, 5, 10, or 15 minutes
		{
			late = 120;
		}else if(value<2000)
		{
			late = 300;
		}else if(value<3000)
		{
			late = 600;
		}else
		{
			late = 900;
		}
		
		
		if(*buttonPointer==1) //scan RFID tag
		{
			for(int i=0;i<6;i++)
			{
				if(*switchPointer == studentNumbers[i]) //check to see of it is one the the correct students
				{
					present = 1;
					studentAttendance[i] = 1;
					studentTime[i] = time;
					if(time<15)
					{
						DisplayHex(0, studentNumbers[i]); //if on time
					}else
					{
						DisplayHex(1,studentNumbers[i]);//if late
					}
					break;
				}
			}
			if(present == 0)
			{
				*ledPointer = 2;
				DisplayHex(2,0); //if its the wrong student light up led 1
			}else
			{
				*ledPointer = 1;//if its the right student light up led 0
				present = 0;
			}
		}	
	}
}
											  
											  
											  
											  
											  
											  
											  
											  
											  
											  
											  
											  