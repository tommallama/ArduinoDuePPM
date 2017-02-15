/***********************************************************************
*  DUE_PPM
*  -This sketch gerenates a standard or inverted PPM 
*    signal using interrupts
*  Register definitions:
*   Arduino\hardware\arduino\sam\system\CMSIS\Device\ATMEL\sam3xa\include\instance
*  
*  Last edit 4/11/2015
***********************************************************************/

/***********************************************************************
*  Includes Section
***********************************************************************/
// No includes


/***********************************************************************
*  Globals, Constants and Definitions
***********************************************************************/
#define PPM_PIN 2                           // PPM output pin, Pin 2, PB25     **9.3.2 PIO Controller B Multiplexing**
#define MAX_PPM_CHANNELS 4                  // Number of PPM channels
#define PPM_FRAME_LENGTH 20000              // Frame length usually 20-22.5ms in microseconds... not critical
#define PPM_PULSE_LENGTH 250                // Pulse length, not critical. This is the high time
#define TICKS_PER_us  42                    // Timer is clocked at 42MHz, so 42 ticks per us

volatile int ppm[MAX_PPM_CHANNELS];         // Array of channels, this will store the commanded periods
int PPMmin = 900;                           // Minimum pulse width in useconds, only used in initialization
byte PPM_cur_ch = 0;                        // Index of current channel in use. DO NOT USE OUTSIDE OF INTERRUPT
unsigned int PPM_sum = 0;                   // Running measure of current used frame. DO NOT USE OUTSIDE OF INTERRUPT


/***********************************************************************
*  Setup Function
*    Runs once to configure all that which requires configuring
***********************************************************************/
void setup() {
  pinMode(PPM_PIN, OUTPUT);                                // Port B pin 25 configure as output                **9.3.2 PIO Controller B Multiplexing**
  analogWrite(PPM_PIN, 1);                                 // Allows bypassing Arduino configs...Don't change!
  
  // Timer counter descriptions starts on  **Page 856**
  REG_PIOB_PDR  = 1 << 25;                                 // Disable PIO, enable peripheral                   **31.7.2 PIO Controller PIO Disable Register**
  REG_PIOB_ABSR = 1 << 25;                                 // Select peripheral B (Timer counter 0 output)     **31.7.24 PIO Peripheral AB Select Register**       
  REG_TC0_WPMR  = 0x54494D00;                              // Enable write to registers "TIM" in ASCII!        **36.7.20 TC Write Protection Mode Register**
  REG_TC0_CMR0  = 0b00000000000010011100010000000000;      // Set channel mode register (see datasheet)        **36.7.3 TC Channel Mode Register: Waveform Mode**
//  REG_TC0_CMR0= 0b00000000000001101100010000000000;      // CMR for inverted output                          **36.7.3 TC Channel Mode Register: Waveform Mode**
  REG_TC0_RC0   = 100000000;                               // Counter period... just any value to init         **36.7.8 TC Register C**
  REG_TC0_CCR0  = 0b101;                                   // Enable clock, software trigger(kick start)       **36.7.1 TC Channel Control Register**
  REG_TC0_IER0  = 0b00010000;                              // Enable interrupt on counter = rc                 **36.7.10 TC Interrupt Enable Register**
  REG_TC0_IDR0  = 0b11101111;                              // Disable other interrupts                         **36.7.11 TC Interrupt Disable Register**
  REG_TC0_RA0   = PPM_PULSE_LENGTH * TICKS_PER_us;         // Pulse length setting for RA compare   
  
  for (int i = 0; i < MAX_PPM_CHANNELS; i++) {             // Set all channels to minimum to avoid interrupt weirdness
    ppm[i] = PPMmin;                    
  }
  NVIC_EnableIRQ(TC0_IRQn);                                // Enable TC0 interrupts **10.20.10.1 NVIC programming hints**

}


/***********************************************************************
*  Loop Function
*    This function runs repeatedly, for EVER!...
***********************************************************************/
void loop()
{
  // This is where the value for the PPM are set, values between ~900 and ~2000
  // Channel order varies from manufacturer to manufacturer...
  ppm[0] = 1000;  
  ppm[1] = 1500;
  ppm[2] = 1500;
  ppm[3] = 1000;  
  
  while(1){} 
}



/***********************************************************************
*  TC0 Handler 
*    This function is the interrupt handler for Timer Counter 0(TC0)
***********************************************************************/
void TC0_Handler()
{ 
  long dummy = REG_TC0_SR0;                                      // Vital - reading this clears interrupt flag, MUST BE DONE!
  if (PPM_cur_ch < MAX_PPM_CHANNELS)                             // Do this if we haven't output all the channels yet
  {
    REG_TC0_RC0 = ppm[PPM_cur_ch] * TICKS_PER_us;                // Update RC to next channel in line
    PPM_sum += ppm[PPM_cur_ch];                                  // Add channels run time to the running count of time
    PPM_cur_ch++;                                                // Increment channel index for the next interrupt
  }
  else                                                           // Do this if we've run out of channels to update
  {
    REG_TC0_RC0 = (PPM_FRAME_LENGTH - PPM_sum) * TICKS_PER_us;   // Update RC to the remaining time so we just idle
    PPM_cur_ch = 0;                                              // Reset channel index
    PPM_sum = 0;                                                 // Reset running count of time
  }
}








