//RELATED TO DISPLAY
#define DISP1  LATEbits.LATE0
#define DISP2  LATAbits.LATA6
#define DISP3  LATAbits.LATA7
#define DISP4  LATCbits.LATC5

#define decodeA LATBbits.LATB5              //msb
#define decodeB LATBbits.LATB6
#define decodeC LATBbits.LATB7              //lsb

#define SEGA  LATDbits.LATD0                //For seven segment display segment latching
#define SEGB  LATDbits.LATD1
#define SEGC  LATDbits.LATD2
#define SEGD  LATDbits.LATD3
#define SEGE  LATDbits.LATD4
#define SEGF  LATDbits.LATD5
#define SEGG  LATBbits.LATB2
#define SEGDP  LATBbits.LATB3



//MODEL READING SWITCH
#define MODEL_SW1 PORTCbits.RC4     //keep it for model selection
//#define MODEL_SW2 PORTCbits.RC5

//KEY ASSIGNMENTS
#define PROG_KEY        PORTDbits.RD0
#define ENCODER_KEY     PORTDbits.RD1
#define TIG_MODE        PORTDbits.RD2
#define MENU_KEY        PORTDbits.RD3
#define MODE_KEY        PORTDbits.RD4
#define REMOTE_KEY      PORTDbits.RD5

#define FOOT_SELECT       PORTBbits.RB0
#define GAS_WATER_SELECT  PORTCbits.RC3
#define WATER_PRESSURE    PORTCbits.RC0
//TORCH
#define TORCH_SW_IN         PORTEbits.RE1
#define TORCHSWIO	    TRISEbits.RE1

#define HF_RELAY_OUT    PORTAbits.RA2
#define GAS_RELAY_OUT   PORTAbits.RA3

#define GAS_RELAY_LATCH LATAbits.LATA3  // Toggle

#define OCVONLAT        LATCbits.LATC1  //0 means welding start
#define ENCODER         PORTA
#define HOTSTART        15

//#define NORMAL1         0
//#define FOOT_SWITCH     1
#define GAS_COOLED      0
#define WATER_COOLED    1
#define WATER_PRESENT   0
#define WATER_ABSENT    1
/************************************************MAX MIN LIMIT MACROS STRT HERE****************************************/
#define MIN_PREFLOW 0  //
#define MAX_PREFLOW 50  //

#define MIN_POSTFLOW_T 0.1
#define MAX_POSTFLOW_T 200   //

#define MIN_DOWN_SLOPE_T 0
#define MAX_DOWN_SLOPE_T 100  //

#define MIN_SPOT_TIME 10
#define MAX_SPOT_TIME 100  //


#define MIN_START_AMP 5
#define MAX_START_AMP 300  //

#define MIN_UPSLOPE_T 0
#define MAX_UPSLOPE_T 100  //

#define MIN_WELD_I1 5
#define MAX_WELD_I1 300  //

#define MIN_WELD_I2 5
#define MAX_WELD_I2 300  //

#define MIN_WELD_BASE  5
#define MAX_WELD_BASE  300  //

#define MIN_WELD_PEAK  5
#define MAX_WELD_PEAK  300  //

#define MIN_WELD_FREQ 1
#define MAX_WELD_FREQ 300  //

#define MIN_WELD_DUTY 10
#define MAX_WELD_DUTY 90

#define MIN_END_AMP 5
#define MAX_END_AMP 300


#define MIN_ARCFORCE 0
#define MAX_ARCFORCE 100

#define MIN_MMA_CURRENT 50
#define MAX_MMA_CURRENT 250

#define MIN_PWM_SLOPE 400
#define MAX_PWM_SLOPE 650

#define ARC_DETEC_T_LMT 2000   //2000*1msec=2sec
#define ARC_DETEC_I_LMT 5  //10Amp
