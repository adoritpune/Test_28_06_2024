union uled_group1
{
     unsigned char all;
   struct
   {
        unsigned char AMP         :1;
        unsigned char SEC         :1;
        unsigned char FREQ        :1;
        unsigned char ARCFORCEON  :1;
        unsigned char TWOT        :1;
        unsigned char FOURT       :1;
        unsigned char SPOT        :1;
        unsigned char CYCLE       :1;
   }bitsize;
};
union uled_group1 led_group1;

union uled_group2
{
  unsigned char all;
   struct
   {
        unsigned char PREFLOW     :1;
        unsigned char START_CUR   :1;
        unsigned char UPSLOPE     :1;
        unsigned char I1_MAIN     :1;
        unsigned char I2_CYCLE    :1;
        unsigned char BASE_CUR    :1;
        unsigned char PEAK_CUR    :1;
        unsigned char PULSE_FREQ  :1;
   }bitsize;
};
union uled_group2 led_group2;

union uled_group3
{
    unsigned char all;
   struct
   {
        unsigned char DUTY        :1;
        unsigned char DOWNLSLOPE  :1;
        unsigned char END_CUR     :1;
        unsigned char POSTFLOW    :1;
        unsigned char n1          :1;
        unsigned char n2          :1;
        unsigned char n3          :1;
        unsigned char n4          :1;
   }bitsize;
};
union uled_group3 led_group3;

union uled_group4
{
     unsigned char all;
   struct
   {
        unsigned char MMA      :1;
        unsigned char TIGHF    :1;
        unsigned char LIFTARC  :1;
       unsigned char PULSE    :1;
        unsigned char NORMAL   :1;
       unsigned char n1       :1;
        unsigned char n2       :1;
        unsigned char n3       :1;
   }bitsize;
};
union uled_group4 led_group4;

  unsigned int maxcurrent=0;

struct para_s
{
	 unsigned int dummy;                 //0000
	struct            // prog = 0
	{
		  unsigned int time;          //0001
	}
	preflow;
        struct           // prog = 2
	{
		 unsigned int amp;           //0002
        }
	start;
	struct           // prog = 3
	{
		  unsigned int time;          //0003
	}
	upslope;
	struct		// prog = 4
	{
		  unsigned int I1;           //0004
		  unsigned int I2;           //0005
		  unsigned int base;	   //0006
                  unsigned int peak;	   //0007
		  unsigned int freq;	   //0008
		  unsigned int duty;	   //0009
	}
	weld;
	struct		// prog = 5
	{
		  unsigned int time;	   //0010
	}
	dnslope;
	struct		// prog = 6
	{
		  unsigned int amp;           //0011
        }
	end;
	struct		// prog = 8
	{
		 unsigned int time;	   //0012
	}
	postflow;      // prog = 9
	  unsigned int  spottime;		  //0013
	  unsigned char ledgp1;		  //0015 	(lower byte)
	  unsigned char ledgp2;		  //0015 	(higher byte)
	  unsigned char ledgp3;		  //0016 	(lower byte)
	  unsigned char ledgp4;		  //0016 	(higher byte)
          unsigned int  pwmslope;           //0017
         unsigned int  arcforce;           //0018
         unsigned int  mmacurrent;         //0019
         unsigned char  dummyvar;           //0020
};
union para_u
{
	struct para_s paras;
	 unsigned int arrayu[20];
};
union para_u parau;
#define para 	parau.paras

//from previous code
volatile volatile unsigned char prvledgp1=0,prvledgp2=0,prvledgp3=0,prvledgp4=0,normaldispupdatf=0,updatedisptig=0,mincurrent=0,footswitchf=0;
volatile volatile unsigned char Disp_mesgf=0,Update_dispf=0,Time_100ms=0,Read_Torchf=0,msec10=0,Time_ms=0,delay_flag=0,delay_over_flag=0;
volatile volatile unsigned char new,old,temp,Read_adc_flag=0,RLon=0,dp_on_flag=0,dp_on_flag1=0,weld_flag=0,dflag=0,modelselectflag=0,modelflag=0,
        savemodelflag=0;
volatile volatile unsigned char fpulse=0,spulse=0,plus=0,minus=0,enc_result=0,scan_enc_flag=0,seg_count=0,porta=0,modelcount=0x00;

//volatile unsigned char  GASWATER_SW,PRESSURE_Sw;
volatile volatile unsigned char TORCH_SW,msone=0,flag_fg;
volatile volatile unsigned char PREV_PROG_SW,PREV_ENC_SW,PREV_TIG_SW,PREV_MENU_SW,PREV_MODE_SW,PREV_REMOTE_SW,PREV_NOR_FOOT_SW,
              PREV_GASWATER_SW,PREV_PRESSURE_SW,PREV_TORCH_SW,ARC_FORCE_SW=0,PREV_ARC_FORCE_SW;

volatile volatile unsigned char KEYCODE=0,PREV_KEYCODE=0,channel=0,Cyclestart=0,TORCH=0,upslopef=0,downslopef=0,cyclemodef=0;
volatile volatile unsigned int ADC_Volt,ADC_Volt1,ADC_Current,ADC_Footsw,Disp_mesg_cntr=0;
volatile volatile unsigned int prev_current=0,current=0,msec1=0,msec100=0,msec200=0,prev_enc_result=0,encoder=0,pluscount=0,minuscount=0,plusoperator=1,
             minusoperator=1,display_sec=0;
volatile volatile unsigned char kpressread=0,torchstatus1=0,torchstatus2=0,tans=0,torchdelms=0;



volatile  unsigned int iii=0,Duty=0,ddd=0,eee=0,pwmresult=0,footrefadc=0,fminoffset1=0,fmaxoffset1=0;
volatile  unsigned int Total_ADC_Volt=0,Total_ADC_Volt1=0,Total_ADC_Current=0,Total_ADC_Footsw=0;
volatile  unsigned char avgcnt0=0,avgcnt1=0,avgcnt2=0,avgVRDcnt=0,OCVONf=0,dutyon=0,pulsef=0,torchpresd=0,cyclecurf=0,tigwelding=0;
volatile  unsigned int Actual_Volt=4,Avg_Footsw=0,Upslope_cntr=0,Downslope_cntr=0,msec10_cntr=0,Avg_Current=0,fmaxoffset;
volatile  unsigned int Actual_Current=0,Actual_Footsw=0;
volatile float UP_Step=0.0,Down_Step=0.0,Upcurrent=0,Downcurrent=0;
volatile float fpulse_period=0.0,fpulse_duty_on=0.0,fpulse_duty_off=0.0;
volatile volatile unsigned int pulse_duty_on=0,pulse_duty_off=0,countp=0,cyle_mode_current=0,data1=0,EE_Data=0,fminoffset=0;
volatile volatile unsigned char lowbyte=0,hibyte=0,EE_Addr=0,tempcnt=0,uartdata=0,footsetupf=0,dispmesgf=0,saveoffsetf=0,
                setmaxcntf=0,Fsetmax=0,mmaweldon=0,fminoff=0;

volatile float slope=0.6;
 volatile volatile unsigned char torchpresd_spot=0,torchreleased_spot=0,torchreleased=0;//,torchrelease,torchpresd_2t;
volatile volatile unsigned int i=0,porthigh=0,tempvar=0,arcf_current=0,Dutycnt=0,count_10ms=0,fbcuroff=0,mmacurrent=0;
volatile volatile unsigned int peakcurrent=0,basecurrent=0,lsb=0,msb=0,prevpeakcurrent=0,prevbasecurrent=0,min_current=15;
enum {pre,start1,upslope,weld,downslope,end1,postflow,exit1}oper;
volatile unsigned char calibrationmode=0,calibration=0,gaswatersw=0,pressure=0,pressurerr=0,tempflag=0,
              blink_timer=0,blink=0,blink_count=0,MODEL=0,startptr=0,reccnt=0,checksum=0,donottrans=0,
              string_recvd=0,recdata[7],offsetf=0,offcnt=0,memsavecnt=0,curerrf=0,dispcurrentf=0,dispvtgf=0;
volatile unsigned int hftigmemarr[20],lifttigmemarr[20],hftigprevmemarr[20],lifttigprevmemarr[20],Rpeakcurrent=0,Rbasecurrent=0;
volatile unsigned char tighfledgp1,tighfledgp2,tighfledgp3,tighfledgp4,tigarcledgp1,tigarcledgp2,tigarcledgp3,tigarcledgp4;
volatile unsigned int maxmmacurrent=0,prevmacurrent=0,mmacurrent1=0,hotstarttime=0,resetcntf=0,resettime=0,modelselecttime=0;
volatile unsigned char revstring=0,recvedrevstrg=0,recrev[7],writememf=0,hotstartonf=0,donotwrite=0;
volatile unsigned char updatecurcnt=0,writememcnt=0,time_10msec=0;
volatile unsigned int arc_detection_count=0,test_time=0;
volatile unsigned char ARC_DETEC_FLAG=1,ARC_ON_FLAG=0;
volatile unsigned char Error_no=0,test_time_f=0,arc_strike_current=0;
unsigned char cWeld4TLatchfg;

//newly defined
volatile unsigned char cseg_data[8];
volatile unsigned char seg_data,PROG_SW=0,ENCODER_SW=0,TIG_SW=0,MENU_SW=0, MODE_SW=0,FOOT_SW,GAS_WATER_SW=0,WATER_PRESSURE_SW=0,
              REMOTE_SW=0, led_display;
volatile unsigned int time_100ms=0,j1=0,j2=0;
volatile unsigned int one_ms=0;
volatile unsigned char one_sec_ovr=0;
unsigned char arr[8];
// used for VRD 
unsigned int iActualVRDvtgON=0,iSampleVRDvtgcnt=0,VRDTimecnt_2s=0;
unsigned int iActualvolt1;
unsigned char cVRD_fg,cRecordVRDONvtg;
unsigned char weldonfg=0;

 unsigned char lookup_table[60]=
 {
                                                                //0babcdefgd,
								//0b00000000,		//null character forword slash '/'
								0b11111100,		//0
								0b01100000,		//1
								0b11011010,		//2
								0b11110010,		//3
								0b01100110,		//4
								0b10110110,		//5
								0b10111110,		//6
								0b11100000,		//7
								0b11111110,		//8
								0b11110110,		//9
								0b11111100,		//3A
								0b01100000,		//3B
								0b11011010,		//3C
								0b11110010,		//3D
								0b01100110,		//3E
								0b10110110,		//3F
								0b10111110,		//41
								0b11101110,		//A
								0b00111110,		//B
								0b10011100,		//C
								0b01111010,		//D
								0b10011110,		//E
								0b10001110,		//F
								0b10111100,		//G
								0b01101110,		//H
								0b01100000,		//I
								0b01111000,		//J
								0b00001110,		//K
								0b00011100,		//L
								0b10101010,		//M
								0b00101010,		//N
								0b00111010,		//O
								0b11001110,		//P
								0b11100110,		//Q
								0b00001010,		//R
								0b10110110,		//S
								0b00011110,		//T
								0b01111100,		//U
								0b00111000,		//V
								0b01010110,		//W
								0b00100000,		//X
								0b01110110,		//Y
								0b10010010,		//Z
                                                        };

