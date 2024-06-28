/* 
 * File:   newmain.c
 * Author: sm06
 *for Champ tig 400PD as well as Champ tig 400P
 *Created on 4 May, 2018, 10:50 AM
 *Implemented for PA79/423/H. In champ tig 400PD and Champ tig 300/400P(wef 1 sept 2019)
 * PIC18f46k22 (earlier it was pic18f4520)
 * //with prescale of 1:2 and pll enabled ie 32mhz.
 * //rev 1 was ok. For making default model selection to 400P and reducing model selection time to 10sec(earlier it was 30 sec)
 * -rev 2 is made. No other functionality change.
 * rev 3 to add VRD(in-built) in MMA // 20-02-24   RR&KK
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include"D:\Ketan\CHAMP TIG 400 P\16022024 NP\p18f46k22.h"
#include"p18f46k22.h"
#include "xc.h"

#include  "newdefine.c"
#include  "variables.c"
#include  "function.c"
#include  "newdisplay.c"
#include "TIG_new1.c"
#include "MMA_new.c"
#include "encoder.c"
#include "string.h"



#pragma config CONFIG1H=0X08        //internal oscillator selected
#pragma config CONFIG2L=0X01        //
#pragma config CONFIG2H=0X00        //WDT DISABLED
#pragma config CONFIG3H=0X00        //MCLR FUNCTION ENABLED
#pragma config CONFIG4L=0X80        //
#pragma config CONFIG5L=0X00        //
#pragma config CONFIG5H=0X80        //
#pragma config CONFIG6L=0X00        //
#pragma config CONFIG6H=0X80        //
#pragma config CONFIG7L=0X00        //
#pragma config CONFIG7H=0X00        //

void interrupt isr()
{
    if(INTCONbits.TMR0IE & INTCONbits.TMR0IF)//INTERRRUPT @ 100 uSEC
    {
        /*msone++;
        if(msone>100)
        {
            flag_fg=1;
        }*/
        INTCONbits.TMR0IF=0;
        TMR0=0XFCDF;   //0XFE6F;         //100 USEC @32MHZ
        time_10msec++;
      //---------------------------------------VRD(in-built)---------------------------------------------------------------------  
        
        if((led_group4.bitsize.MMA==1) && (VRDTimecnt_2s >= 150))
        { 
            VRDTimecnt_2s = 0;                     
            cVRD_fg          = 2;
            cRecordVRDONvtg  = 1;
            iSampleVRDvtgcnt = 0; 
            //CCP1CONbits.DC1B=0;
            //CCPR1L=0;
            //GAS_RELAY_OUT=~GAS_RELAY_OUT;   
        }   
        if(time_10msec>=100)
        {
             //LATAbits.LATA6^=1;
             time_10msec=0;
             updatecurcnt++;
             writememcnt++;
             
            if(cVRD_fg==1)
            {
                VRDTimecnt_2s++;
            }
             
            if(resetcntf==1)   //for manual reset
            {
                resetcntf=0;
                resettime++;
            }
             
            if(modelselectflag==1) //for model selection through software
            {
                 modelselectflag=0;
                 modelselecttime++;

            }
            if(test_time_f==1) //for testing gas hf (encoder sw)
            {
                test_time++;
            }

            if(upslopef==1)
            {
                msec10_cntr++;
                if(Upslope_cntr >= msec10_cntr)             //upsolpe
                {
                    Upcurrent=Upcurrent+UP_Step;
                    pwmresult=(unsigned int)Upcurrent;
                    //Update_PWM(pwmresult);   //set current
                }
                else
                {
                    upslopef=0;
                    //Update_PWM(para.weld.peak);   //set current
                }
             }

              if(downslopef==1)
            {
                msec10_cntr++;
                if(Downslope_cntr >= msec10_cntr)           //downslope
                {
                    if(Downcurrent>=Down_Step)
                    {
                        Downcurrent=Downcurrent-Down_Step;
                        pwmresult=(unsigned int)Downcurrent;
                    }
                    else
                    {
                        pwmresult=0;
                    }
                    //Update_PWM(pwmresult);   //set current
                }
                else
                {
                    downslopef=0;
                    //Update_PWM(0);   //set current
                  //  CCPR1L=0;
                 //   CCP1CONbits.DC1B=0;
                }
                //Send_Byte('~');
            }     
        }

        if((pulsef==1))//||(torchsw==1))       // 2 T  & 4 T & CYCLE mode Pulse
        {
           countp++;
           if(dutyon ==0)
           {
              if(countp >= pulse_duty_on)
              {
                 countp  = 0;
                 dutyon = 1;
              }
           }
           else
           {
              if(countp >= pulse_duty_off)
              {
                 countp  = 0;
                 dutyon = 0;
              }
           }
        }
        return;
    }

    if(PIE1bits.TMR1IE & PIR1bits.TMR1IF)     //DISPLAY
    {
        PIR1bits.TMR1IF=0;
        TMR1 =0XC17F;  //0XF05F;     //DD70;//E0BF//C17F; for 1msec interrupt   @16MHZ            /* clear timer1 register */
        msec1++;
        msec10++;
        msec100++;
        display_sec++;
        Disp_mesg_cntr++;
        torchdelms++;
        enc_result=scanencoder();

        hotstarttime++;
        count_10ms++;
       
/*=============================Read torch sw start========================*/
         if(torchdelms>=5)
        {
        torchdelms=0;
		TORCHSWIO =1;
		if(kpressread==0)
		{
			torchstatus1=TORCH_SW_IN;//TORCHIP;
		//	inchstatus1=INCHIP;
			kpressread=1;
		}
		else if(kpressread==1)
		{
			torchstatus2=TORCH_SW_IN;//TORCHIP;

			if(torchstatus1==torchstatus2)
			{
                            if(torchstatus2==0)            //TORCH LOGIC
                            {

                            TORCH_SW=1;
                            TORCH=1;
                            }
                            else
                                {

                                TORCH_SW=0;
                                TORCH=0;
                                }
			}
			kpressread=0;
		}
	}

  /*=============================Read torch sw end========================*/
         if( Actual_Current < ARC_DETEC_I_LMT )
            {
            if(++arc_detection_count>=ARC_DETEC_T_LMT)
            {
            arc_detection_count=ARC_DETEC_T_LMT;
            ARC_DETEC_FLAG=0;
            }
            }
        else
            {
            arc_detection_count=0;
            ARC_DETEC_FLAG=1;
            }

        if(msec1>=100)
        {
            msec1=0;
            Time_100ms++;
            Time_ms++;
        }
        if(msec100 < 200)
        {
            if(enc_result=='+')
                pluscount++;
            else if(enc_result=='-')
                minuscount++;
        }
        else if(msec100 >= 200)
        {
            msec100=0;
            plusoperator=pluscount;
            minusoperator=minuscount;
            if(plusoperator <= 1)
              plusoperator=1;
            if(minusoperator <= 1)
              minusoperator=1;
            pluscount=0;
            minuscount=0;
           // Send_Byte(plusoperator);
           // Send_Byte(minusoperator);
        }
        if(display_sec>=1)
        {
           display_sec=0;
           refresh_disp();
        }
         //refresh_disp();
         return;
     }

    if(PIE1bits.TMR2IE & PIR1bits.TMR2IF)      //PWM
    {
         PIR1bits.TMR2IF=0;
         return;
    }
   
    //adc flag
    if(PIE1bits.ADIE & PIR1bits.ADIF)           //ADC INTRRUPT
    {
        PIR1bits.ADIF=0;
        ADCON0bits.GODONE=0;
        //ADCON1bits.PCFG=13;//AN0 & AN1
        //ANSELA=0X03;
        Read_adc_flag=1;
        //Read_ADC();
        return;
    }
    
    if(PIE1bits.TX1IE & PIR1bits.TX1IF)
    {
        PIR1bits.TX1IF=0; //uart1 sayali 19.07.18
        return;
    }

    if(PIE1bits.RC1IE & PIR1bits.RC1IF)
    {
        //PIR1bits.RC1IF=0;
        //Send_Byte('A');
        //uartdata=getchar();
        
        uartdata= RCREG1;  //usart1 sayali 19.7.18
        //TXREG=uartdata;
        //Send_Byte(recrev[reccnt]);
        if(RCSTA1bits.FERR) //no
        {
          RCSTA1bits.SPEN=0;
          RCSTA1bits.SPEN=1;
          
        }
        if(RCSTA1bits.OERR)//no sayali
        {
          RCSTA1bits.CREN=0;
          RCSTA1bits.CREN=1;
          RCSTA1bits.SPEN=0;
          RCSTA1bits.SPEN=1;
         
        }
        /*if(uartdata == '@'&& revstring==0)
        {
            revstring=1;
            reccnt=0;
            string_recvd = 0;
        }
        else if(revstring==1)
        {
            recrev[reccnt] = uartdata;
            //Send_Byte(recrev[reccnt]);
            reccnt++;
            if(reccnt == 7)
            {
                reccnt=0;
                revstring=0;
                string_recvd = 1;
                recvedrevstrg=1;
            }
        }*/
        if(uartdata == '$' && startptr == 0)
        {
           reccnt     = 0;
           startptr   = 1;
           checksum  = 0;
           donottrans = 1;
           string_recvd = 0;
           recvedrevstrg=0;
            //dflag=1;
        }
        else if(startptr == 1)
        {
           recdata[reccnt] = uartdata;
           if(recdata[0]=='@')
           {
               recrev[reccnt]=recdata[reccnt];
           }
           if(reccnt < 5)
                checksum = checksum+uartdata;
           reccnt++;
           if(reccnt == 6)
          {
             reccnt       = 0;
             startptr     = 0;
             if(checksum == recdata[5])
             {
                 string_recvd = 1;
                 if(recdata[0]=='#')
                 {
                      lsb          = recdata[1];
                      msb          = recdata[2];
                      Rpeakcurrent  = (lsb|(msb<<8));

                      lsb       = recdata[3];
                      msb       = recdata[4];
                      Rbasecurrent  = (lsb|(msb<<8));
                 }
                 else
                     recvedrevstrg=1;
             }
             else
                string_recvd = 0;

             checksum  = 0;
          }
        }
    }
}
void main()
{
    system_init();
    for(iii=0;iii<4000;iii++);
    powerontest();
    delay();
    //delay();
    OCVONLAT=1;
    readmem();
    Read_Model();
    if(MODEL==1)                    //300P
    {
        cseg_data[7]='A'-0X30;
        cseg_data[0]='D'-0x30;
        cseg_data[1]='O'-0X30;
        cseg_data[2]='R'-0X30;
        
        cseg_data[3] ='3'- 0x30;
        cseg_data[4] ='P'- 0x30;
        cseg_data[5] ='R'- 0x30;     // Model name & soft REV
        cseg_data[6] ='3'- 0x30;
        mincurrent=5;
        maxcurrent=300;
        maxmmacurrent=250;
        delay();
        delay();
        //delay();
    }

    else if(MODEL==2)                    //300P
    {
        cseg_data[7]='A'-0X30;
        cseg_data[0]='D'-0x30;
        cseg_data[1]='O'-0X30;
        cseg_data[2]='R'-0X30;
        
        cseg_data[3] ='4'- 0x30;
        cseg_data[4] ='P'- 0x30;
        cseg_data[5] ='R'- 0x30;     // Model name & soft REV
        cseg_data[6] ='3'- 0x30;
        mincurrent=10;
        maxcurrent=400;
        maxmmacurrent=400;
        delay();
        //delay();
         
    }
  
    readmem();
   
    slope=(para.pwmslope/1000.0);

      if(led_group4.bitsize.LIFTARC==1)
    {
        for(i=0; i<14; i++)
        {
            parau.arrayu[i]=lifttigmemarr[i];
        }
    }

      if(led_group4.bitsize.TIGHF==1)
    {
        for(i=0; i<14; i++)
        {
            parau.arrayu[i]=hftigmemarr[i];
        }
    }
   
    oper=pre;
    Update_dispf=1;
    if(led_group4.bitsize.MMA==1 && led_group4.bitsize.TIGHF==0 && led_group4.bitsize.LIFTARC ==0)
    {
        cVRD_fg=1;VRDTimecnt_2s=0;
    }
    while(1)
    {
        {
         if(MENU_SW==1)                  //Reset function if garbage data is written into memory
         {
           resetcntf=1;                   //flag is set, increases resettime variable in interrupt
           if(resettime>=3000)              //Reset will be done after 30 sec //resettime>=3000
            {
                cseg_data[3] ='R' - 0x30;
                cseg_data[4] ='S' - 0x30;
                cseg_data[5] ='E' - 0x30;
                cseg_data[6] ='T' - 0x30;
                donotwrite=1;                 //do not write anything in memory
                delay();
                delay();
                resetmem();                  //default values assigned to memory
                readmem();                  //read memory loaded with default values
                Update_dispf=1;
           }
            
         }
         else
         {
           resetcntf=0;
           resettime=0;
           donotwrite=0;

         }
        }//reset loop ends here
    
   
   
         if(ENCODER_SW==1)        //for testing gas and hf relay
         {
            test_time_f=1;
            if(test_time>=2000) //Reset will be done after 30 sec //resettime>=3000
            {
                cseg_data[3] ='T' - 0x30;
                cseg_data[4] ='E' - 0x30;
                cseg_data[5] ='S' - 0x30;
                cseg_data[6] ='T' - 0x30;
                donotwrite=1;
                delay();
                delay();
                test_gas_hf();
                Update_dispf=1;
            }
            else
            {
            resetcntf=0;
            resettime=0;
            donotwrite=0;
            }
         }//enc gashf loop ends here

        {if(PROG_SW==1)
        {
            modelselectflag=1;      //once flag is set, model sel counter increases
            if(modelselecttime>=1000) //10 sec @changed as per Quality's suggestion
            {
                
                modelflag=1;
                
                cseg_data[7]='S'-0X30;
                cseg_data[0]='L'-0x30;
                cseg_data[1]='C'-0X30;
                cseg_data[2]='T'-0X30;

                cseg_data[3] ='M'- 0x30;
                cseg_data[4] ='O'- 0x30;
                cseg_data[5] ='D'- 0x30;     // Model name
                cseg_data[6] ='L'- 0x30;
                delay();

                
                if(modelcount==0x01)
                {
                    cseg_data[3] ='3'- 0x30;
                    cseg_data[4] ='0'- 0x30;
                    cseg_data[5] ='0'- 0x30;     // Model name 300p
                    cseg_data[6] ='P'- 0x30;
                    //delay();
                }
                else if(modelcount==0x02)
                {
                    cseg_data[3] ='4'- 0x30;
                    cseg_data[4] ='0'- 0x30;
                    cseg_data[5] ='0'- 0x30;     // Model name 400p
                    cseg_data[6] ='P'- 0x30;
                    //delay();
                }
                else
                {
                    cseg_data[3] ='4'- 0x30;
                    cseg_data[4] ='0'- 0x30;
                    cseg_data[5] ='0'- 0x30;     // Model name default 400p @sayali 27/08
                    cseg_data[6] ='P'- 0x30;

                }
                
                }
                //modelflag=1;

        
        }
        else
            {
                 modelselectflag=0;
                 modelselecttime=0;
                 //modelflag=0;
            }
    }//model selection loop ends here

         



         if(Disp_mesgf==1 && Disp_mesg_cntr > 500)
        {
            Disp_mesgf=0;
            Disp_mesg_cntr=0;               //for display the led position & data
            Update_dispf=1;
        }

         if(string_recvd==1)             //Serial string received
        {
            //if(recvedrevstrg==1 && TORCH==0 && Cyclestart==0)
            if((recdata[0]=='@') && (recvedrevstrg==1))
            {
                recvedrevstrg=0;
                string_recvd=0;
                cseg_data[3] ='R' - 0x30;   //7
                cseg_data[4] ='E' - 0x30;   //0
                cseg_data[5] ='M' - 0x30;   //1
                cseg_data[6] ='T' - 0x30;   //2
                delay();
                cseg_data[3] =recrev[1]; //-0x30;
                cseg_data[4] =recrev[2]; //- 0x30;
                cseg_data[5] =recrev[3]; // - 0x30;
                cseg_data[6] =recrev[4]; //0x0A;//- 0x30;
                dp_on_flag=1;
                dp_on_flag1=0;
                delay();
                delay();
                Update_dispf=1;
                //Delay();
            }
            else if(recdata[0]=='#')
            {
               chk_data_remote();
                if(led_group4.bitsize.TIGHF==1 || led_group4.bitsize.LIFTARC ==1)
                {
                    if(prevpeakcurrent!=peakcurrent)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        if(led_group4.bitsize.PULSE==1)
                            led_group2.bitsize.PEAK_CUR=1;

                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        para.weld.I1=peakcurrent;
                        if(para.weld.I1>=maxcurrent)
                            para.weld.I1=maxcurrent;
                        if(para.weld.I1<=mincurrent)
                            para.weld.I1=mincurrent;
                        //inttochar(para.weld.I1);
                        //dp_on_flag=0;
                        prevpeakcurrent=peakcurrent;
                        Update_dispf=1;
                    }
                    if(led_group4.bitsize.PULSE==1)
                    {
                        if(prevbasecurrent!=basecurrent)
                        {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.BASE_CUR=1;
                            led_group1.bitsize.AMP=1;
                            led_group1.bitsize.SEC=0;
                            led_group1.bitsize.FREQ=0;
                            para.weld.base=basecurrent;
                            if(para.weld.base>=para.weld.I1)
                                   para.weld.base=para.weld.I1;
                            if(para.weld.base<=mincurrent)
                                   para.weld.base=mincurrent;
                            //inttochar(para.weld.base);
                            //dp_on_flag=0;
                            prevbasecurrent=basecurrent;
                            Update_dispf=1;
                        }
                    }
                }
                else if(led_group4.bitsize.MMA==1)
                {
                    if(prevpeakcurrent!=peakcurrent)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        para.mmacurrent=peakcurrent;
                        if(para.mmacurrent>maxmmacurrent)
                            para.mmacurrent=maxmmacurrent;
                        if(para.mmacurrent<50)
                        {
                            para.mmacurrent=50;
                        }
                        prevpeakcurrent=peakcurrent;
                        Update_dispf=1;
                    }
                }
            }
        }//end of if string_recvd
   /*******************************************pressure error*********************************/
          {   //Pressure Error
             
            if((WATER_PRESSURE_SW == 1)&&(GAS_WATER_SW == 1)&&(led_group4.bitsize.TIGHF==1 || led_group4.bitsize.LIFTARC ==1) && modelflag==0)//NOT IN MMA
            {
                if(!blink_timer)
                {
                      blink_timer  = 1;
                      count_10ms   = 0;
                }

                {
                   if(count_10ms >= 500 && !blink)
                    {
                          blink_timer = 0;
                          blink        = 1;
                          cseg_data[3] ='P'- 0x30;
                          cseg_data[4] ='E'- 0x30;
                          cseg_data[5] ='R'- 0x30;
                          cseg_data[6] ='R'- 0x30;
                     }
                     else if(count_10ms >= 500 && blink)
                     {
                          blink_timer  = 0;
                          blink        = 0;
                          cseg_data[3]  = 0x0A;
                          cseg_data[4]  = 0x0A;
                          cseg_data[5]  = 0x0A;
                          cseg_data[6]  = 0x0A;
                     }
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        pressurerr=1;
                        //protection_byte=0x07;
                        //tigwelding=1;
                        CCPR1L=0;
                        CCP1CONbits.DC1B=0;
                        GAS_RELAY_OUT=0;
                        HF_RELAY_OUT=0;
                        oper=exit1;
                }
            }
            else
            {
                pressurerr=0;
                tigwelding=0;
                //protection_byte=0x00;
            }
        }
 /*********************MMA process******************************************/
       if(led_group4.bitsize.MMA==1 && led_group4.bitsize.TIGHF==0 && led_group4.bitsize.LIFTARC ==0)
       {
            
             //OCVONLAT=0;
            weld_mma_process();      //MMA
            if(cVRD_fg==2) 
            {           
                //if(((iActualVRDvtgON) > Actual_Volt) && (cRecordVRDONvtg == 0)) // 190224
                if((Actual_Volt <= (iActualVRDvtgON-40)) && (cRecordVRDONvtg == 0))
                {  
                    cVRD_fg = 0;
                    Update_PWM(mmacurrent1);
                }
                else
                {
                    CCP1CONbits.DC1B=0;
                    CCPR1L=0;
                }
            }
       }

/**********************************TIG OR LIFT ARC********************************************/

       if(led_group4.bitsize.MMA==0 && (led_group4.bitsize.TIGHF==1 || led_group4.bitsize.LIFTARC ==1))
       {
           if(TORCH==0 && Cyclestart==0)
           {
               if(pressurerr==0)
               {
                    OCVONLAT=1;
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                   // weldon_byte='F';

                    if(footsetupf==1)
                    {
                        if(dispmesgf==0 && saveoffsetf==0 && setmaxcntf==0)     //Footswitch setting
                        {
                            cseg_data[3] ='F'- 0x30;
                            cseg_data[4] ='S'- 0x30;
                            cseg_data[5] ='E'- 0x30;
                            cseg_data[6] ='T'- 0x30;
                            Disp_mesgf=1;
                            Disp_mesg_cntr=0;
                            dp_on_flag=0;
                            delay();
                            dispmesgf=1;
                            fminoff=0;
                        }
                        else if(dispmesgf==1 && saveoffsetf==0 && setmaxcntf==0)
                        {
                            if(fminoff==0)
                            {
                                cseg_data[3] ='F'- 0x30;
                                cseg_data[4] ='M'- 0x30;
                                cseg_data[5] ='I'- 0x30;
                                cseg_data[6] ='N'- 0x30;
                                Disp_mesgf=1;
                                Disp_mesg_cntr=0;
                                dp_on_flag=0;
                                delay();
                                fminoff=1;
                            }
                            fminoffset=footrefadc;
                            inttochar(footrefadc);
                            dp_on_flag=0;
                        }
                        else if(saveoffsetf==0 && setmaxcntf==1)
                        {
                            if(Fsetmax==0)
                            {
                                cseg_data[3] =0X16;//'F'- 0x30;
                                cseg_data[4] =0X1D; //'M'- 0x30;
                                cseg_data[5] =0X11;  //'A'- 0x30;
                                cseg_data[6] =0X2D; //'X'- 0x30;
                                Disp_mesgf=1;
                                Disp_mesg_cntr=0;
                                dp_on_flag=0;
                                delay();
                                //Delay();
                                Fsetmax=1;
                            }
                            fmaxoffset=footrefadc;
                            inttochar(footrefadc);
                        }
                        else if(saveoffsetf==1)
                        {
                            fmaxoffset=fmaxoffset-fminoffset;
                            fmaxoffset1=(unsigned int)fmaxoffset/10.0;                          lowbyte=fminoffset;                  //write minimum offset in memory
                            hibyte=fminoffset>>8;
                            write(0X64,lowbyte);
                            write(0X65,hibyte);

                            lowbyte=fmaxoffset;                  //write maximum offset in memory
                            hibyte=fmaxoffset>>8;
                            write(0X66,lowbyte);
                            write(0X67,hibyte);

                            cseg_data[3] ='S'- 0x30;
                            cseg_data[4] ='A'- 0x30;
                            cseg_data[5] ='V'- 0x30;
                            cseg_data[6] ='D'- 0x30;
                            Disp_mesgf=1;
                            Disp_mesg_cntr=0;
                            dp_on_flag=0;
                            delay();
                            dispmesgf=0;
                            saveoffsetf=0;
                            Fsetmax=0;
                            setmaxcntf=0;
                            footsetupf=0;
                        }
                    }
                    else
                    {
                        //enc_result=scanencoder();
                        if(((enc_result=='+') || (enc_result=='-') || (Update_dispf==1) ||(updatedisptig==1))&& (pressurerr==0))
                        {
                            Update_TIG_Parameters(enc_result);
                            writememcnt=0;
                            writememf=1;
                            enc_result=0;
                            Update_dispf=0;
                            updatedisptig=0;
                        }
                        if((writememf==1) && (writememcnt>=100)&& (donotwrite==0))//writememcnt>=100
                        {
                            writememf=0;
                            writememcnt=0;
                            writemem();
                        }
                    }
               }
                keydetect();
           }
           else
           {
               if(Error_no==0)
               OCVONLAT=0;

               if(led_group4.bitsize.PULSE==1 && tempcnt==0)
               {
                    fpulse_period   = (10000.0 / para.weld.freq); //10000
                    fpulse_duty_on  = ((fpulse_period /100) * para.weld.duty);
                    pulse_duty_on   = (unsigned int) fpulse_duty_on;
                    fpulse_duty_off = (fpulse_period - pulse_duty_on);
                    pulse_duty_off  =(unsigned int) fpulse_duty_off;
                    tempcnt=1;
               }
               if(tigwelding==0)
               {
                   TIG_Weld_Process();
                   tigwelding=1;            //clear in pressure error else
               }
              // Ilsb_msb_byte=Actual_Current;
              // weldon_byte='O';
               //transmit_to_datalogger=1;
               if((MODE_SW==1)&&(TIG_SW==1)&& (pressurerr==0)&& FOOT_SW==1)//normal mode
                {
                    calibrationmode=1;
                    calibration=1;
                    cseg_data[3] ='C'- 0x30;
                    cseg_data[4] ='L'- 0x30;
                    cseg_data[5] ='O'- 0x30;
                    cseg_data[6] ='N'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                }
               //if(calibrationmode==1 && (pressurerr==0))
               if(pressurerr==0)
               {
                   //enc_result=scanencoder();
                    if((enc_result=='+' || enc_result=='-' || Update_dispf==1)&& (pressurerr==0))
                    {
                        Update_TIG_Parameters(enc_result);
                        enc_result=0;
                        Update_dispf=0;
                    }
               }
                if((MENU_SW==1) && (calibrationmode==1)&& (pressurerr==0))
                {
                    //write(0X63,para.pwmslope);
                    lowbyte=para.pwmslope;
                    hibyte=para.pwmslope>>8;
                    write(0X71,lowbyte);
                    write(0X72,hibyte);
                    calibrationmode=0;
                    cseg_data[3] ='C'- 0x30;
                    cseg_data[4] ='L'- 0x30;
                    cseg_data[5] ='O'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    delay();
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                }
               if(updatecurcnt>=100 && pressurerr==0 && dispcurrentf==1 && calibrationmode==0)// && curerrf==0)//updatecurcnt>=100
               {
                    updatecurcnt=0;
                    inttochar(Actual_Current);
                    //intochar_volt(123);
                    dp_on_flag=0;
               }
          }
        }

         if(Read_adc_flag==1)            //Read ADC
        {
             if(dispvtgf==1)
             {
                intochar_volt(Actual_Volt);
                if(led_group2.bitsize.I1_MAIN==1) //added by sayali
                {
                    dp_on_flag1=1;
                }
                intochar_volt(Actual_Volt);
               
             }

            Read_ADC();
            for(iii=0;iii<100;iii++);
            Read_adc_flag=0;
        //    intochar_volt(Read_adc_flag);
            ADCON0bits.GODONE=1;
        }

        if(TORCH==0 && Cyclestart==0 && led_group2.bitsize.I1_MAIN==1 && led_group4.bitsize.MMA==0)  //voltage display when torch off
        {
          
          intochar_volt(Actual_Volt); //added by sayali
          dp_on_flag1=1;              //added by sayali
        }

      /* if(Read_adc_flag==1)            //Read ADC
        {
            Read_adc_flag=0;
            Read_ADC();
            for(iii=0;iii<100;iii++);
            ADCON0bits.GODONE=1;
        } */
         
    }//while ends here
}//main ends here

void system_init()
{
     oscillator_init();
     portpin_init();
     Timer0_init();
     Timer1_init();
     uart_init();
     adc_init();
     PWM_init();

}
void oscillator_init()
{
    OSCCON=0x74;          //stable HF internal oscillator @16MHz
    OSCTUNEbits.TUN=0;    //OSCILLATOR MODULE IS RUNNING AT FACTORY CALIBRATED FREQUENCY
    OSCTUNEbits.PLLEN=1;  //pLL enabled
    OSCTUNEbits.INTSRC=1; //internal osci high frequency source
    OSCCONbits.IRCF=7;
}
void portpin_init()
{
    TRISA=0X33;  //RA0,RA1 AS INPUT 0011 0011
    PORTA=0X00;
    ANSELA=0X03; //RA0 AND RA1 AS ANALOG

    TRISB=0X03;  //RB0 & RB1= I/P
    PORTB=0X00;
    ANSELB=0X02;//RB1 AS ANALOG(FOOT REFERENCE)

    TRISC=0X19;  //0001 1001
    PORTC=0X00;
    ANSELC=0X00;
    

    TRISD=0X00;
    ANSELD=0X00;
    PORTD=0X00;

    TRISE=0X02;//RE0 IS USED FOR DISPLAY. SO, OUTPUT. 0X03; //RE0,RE1=I/P
    PORTE=0X00;
    ANSELE=0X00;
}
void Timer0_init()
{
   TMR0 =0;               /* clear timer0 register     */
   //T0CON=0xE0;
  
   T0CONbits.T0CS=0;      /*internal instruction cycle clock an and timer mode(not counter mode)*/
   T0CONbits.T0SE=0;      /*increment on low to high transition on T0CKI pin*/
   T0CONbits.PSA=0;       /*prescaler assigned to timer zero*/
   T0CONbits.T08BIT=0;     /*16 bit timer*/
   T0CONbits.T0PS=0;       /*1:2 prescale value*/
   T0CONbits.TMR0ON=1;     /* start the timer*/
   INTCONbits.TMR0IF=0;   /*timer 0 flag cleared*/
   INTCONbits.TMR0IE=1;   /*timer 0 interrupt enabled*/
   INTCONbits.GIE=1;
   INTCONbits.PEIE=1;
}
void Timer1_init()
{
   TMR1 =0;               /* clear timer1 register     */
   T1CONbits.T1CKPS=0;   /*prescale 1:1*/
   T1CONbits.TMR1CS0=0;  /*clock select*/
   T1CONbits.TMR1CS1=0;  /*clock select*/
   T1CONbits.T1RD16=1;     /*enables 16 bit operation of timer*/
   T1CONbits.TMR1ON=1;     /* start the timer*/
   T1GCONbits.TMR1GE=0;
   PIR1bits.TMR1IF=0;
   PIE1bits.TMR1IE=1;
   INTCONbits.GIE=1;
   INTCONbits.PEIE=1;

}

void uart_init()
{
   /*TXSTA=0;
    BAUDCON=0;
    RCSTA=0;
    RCSTAbits.SPEN=1;
    TRISC=0xC0;
    TXSTAbits.BRGH=0;
    BAUDCONbits.BRG16=0;
    TXSTAbits.SYNC=0;
    TXSTAbits.TX9=0;
    SPBRG=204;//51;//103;
    SPBRGH=0;
    TXSTAbits.TXEN=1;*/
    //OpenUSART(USART_TX_INT_OFF  & USART_RX_INT_OFF  & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_BRGH_LOW,103); //25 for 4mhz internal clock for 2400 baud rate
    RCSTA1bits.SPEN=1;
    TRISCbits.RC6=1;//0xC0;
    TRISCbits.RC7=1;//0xC0;
    TXSTA1bits.BRGH=0;
    BAUDCON1bits.BRG16=0;
    TXSTA1bits.SYNC=0;
    //SPBRGH1=0;
    SPBRG1=104;//51;Baud rate 4800bps sayali 20.07.18
    //TXSTA1=0x20;
    TXSTA1bits.TXEN=1;
    RCSTA1bits.CREN=1;
    PIE1bits.RC1IE=1;
    //OpenUSART(USART_TX_INT_OFF,100);
    //OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_BRGH_LOW,103);//      USART_TX_INT_OFF,103);
}
void adc_init()
{
    //ADCON1=0x0D;//FC; 0x04 for analog foot switch input
    //TRISA=0x03;
    ADCON1bits.PVCFG=0X00; //internal VDD
    ADCON1bits.NVCFG=0X00;//AN0 & AN1
    ADCON2bits.ADFM=1;
    ADCON0bits.CHS=0; //channel 0 selected;
    ADCON2bits.ADCS=5;// fosc/16
    ADCON2bits.ACQT=5;//16 TAD
    ADCON0bits.ADON=1;//turn on adc
    PIR1bits.ADIF=0;  //clear interupt flag
    PIE1bits.ADIE=1;  //Enable interrupt.
    channel=0;
    for(iii=0;iii<10;iii++); //wait;
    ADCON0bits.GODONE=1;
}
void PWM_init()
{
    CCP1CONbits.P1M=0;
    CCP1CONbits.CCP1M=13;
    TRISCbits.RC2=0;
    PR2=0X4F;            //pwm period
    CCP1CONbits.DC1B=0;
    CCPR1L=0;
    T2CONbits.T2CKPS=0;     //PRESCALE IS 1
    T2CONbits.T2OUTPS=0;     //POST SCALE 1
    T2CONbits.TMR2ON=1;     //START TIMER 2
}

void Update_PWM(unsigned int Current)
{
    unsigned int Temp=0x00;//,Address=0x0001; //maximum 100%=320 count
    //Dutycnt=(unsigned int)(Current*slope)+offset;
    Dutycnt=(unsigned int)(Current*slope)+35;

    CCP1CONbits.DC1B=Dutycnt;
    Temp=Dutycnt >> 2;
    CCPR1L=Temp;
}

void Read_ADC()
{
  unsigned char high_byte=0,low_byte=0;
  unsigned int temp=0;
  low_byte=ADRESL;
  high_byte=ADRESH;
  temp=ADRESH;
  temp=temp<<8;
  temp=(temp & 0xFF00);
  temp=temp + ADRESL;
  
  if(channel==0)
  {
        ADC_Volt=temp;
        Total_ADC_Volt=Total_ADC_Volt + ADC_Volt;
        avgcnt0++;
        if(avgcnt0 >= 128)
        {
            Actual_Volt=(Total_ADC_Volt >> 7)*2;  //*2 added by sayali 18.07.18
            Total_ADC_Volt=0;
            avgcnt0=0;
            
            if(cRecordVRDONvtg == 1)
            {
                iSampleVRDvtgcnt++;
                if(iSampleVRDvtgcnt >= 3)
                {
                    cRecordVRDONvtg=0;iSampleVRDvtgcnt=0;
                    iActualVRDvtgON = Actual_Volt;
                }
            }
        }

        channel=1;
        ADCON0bits.CHS=1; //channel 1 selected;
  }
  
  
  
  
 
  else if(channel==1)
  {
        ADC_Current=temp;
        Total_ADC_Current=Total_ADC_Current+ADC_Current;
        avgcnt1++;
        if(avgcnt1 >=64)
        {
             if(offsetf==0)
                   offcnt++;
            Avg_Current=Total_ADC_Current >>6;
            if(MODEL==1)
            {
                //Actual_Current=(unsigned int)((Avg_Current*30)/63.4);//65.5);
                //Actual_Current=Actual_Current<<;
                Actual_Current=(Avg_Current>>1);///2.0);
            }
            else if(MODEL==2)
            {
                //Actual_Current=(unsigned int)((Avg_Current*40)/63.4);//65.5);
                Actual_Current=(unsigned int)((Avg_Current*5)>>3);
                //Actual_Current=Actual_Current>>3;
            }
            if(Actual_Current>=fbcuroff)
            {
                Actual_Current=Actual_Current-fbcuroff;
            }
            else
                Actual_Current=0;

            if(offsetf==0 && offcnt==4)
            {
                fbcuroff=Actual_Current;
                offsetf=1;
             }
            Total_ADC_Current=0;
            avgcnt1=0;
        }
        if(FOOT_SW==0)
        {
            channel=10;
             ADCON1bits.PVCFG=0X00; //internal VDD
             ADCON1bits.NVCFG=0X00;//AN0 & AN1
            ADCON0bits.CHS=10; //channel 10 selected;
        }
        else
        {
            ADCON1bits.PVCFG=0X00; //internal VDD
            ADCON1bits.NVCFG=0X00;//AN0 & AN1
            channel=0;
            ADCON0bits.CHS=0; //channel 0 selected;
        }
  }
  else if(channel==10)
  {
        ADC_Footsw=temp;
        ADRES=0;
        Total_ADC_Footsw=Total_ADC_Footsw+ADC_Footsw;
        avgcnt2++;
        if(avgcnt2 >= 64)
        {
            Avg_Footsw=Total_ADC_Footsw >> 6;
            footrefadc=Avg_Footsw;
            //fmaxoffset=fmaxoffset-fminoffset;
            if(Avg_Footsw>=fminoffset)
            {
                Avg_Footsw=Avg_Footsw-fminoffset;
            }
            //if(Avg_Footsw >= fmaxoffset)
              //  Avg_Footsw = fmaxoffset;
            //fmaxoffset1=(unsigned int)fmaxoffset/10.0;
            if(MODEL==1)
            {
                Actual_Footsw=(unsigned int)5+((Avg_Footsw*29.5)/fmaxoffset1);

                if(Actual_Footsw>= 300)
                {
                    Actual_Footsw=300;
                }
            }
            else if(MODEL==2)
            {
                Actual_Footsw=(unsigned int)10+((Avg_Footsw*39.0)/fmaxoffset1);

                if(Actual_Footsw>= 400)
                {
                    Actual_Footsw=400;
                }
            }
            Total_ADC_Footsw=0;
            avgcnt2=0;
            //Send_Byte('#');
            //Send_Integer(Actual_Footsw);
            //Send_Byte(0x0d);
            //Send_Byte(0x0a);
        }
        ADCON1bits.PVCFG=0X00; //internal VDD
        ADCON1bits.NVCFG=0X00;//AN0 & AN1
        channel=0;
        ADCON0bits.CHS=0; //channel 0 selected;
  }
}

void inttochar(unsigned int parameter)
{
   cseg_data[3] =0;
   cseg_data[6] = parameter % 10;
   parameter /= 10;
   cseg_data[5] = parameter % 10;
   parameter /= 10;
   cseg_data[4] = parameter;
}
void intochar_volt(unsigned int parameter)
{
    //cseg_data[0] =0;
   //dp_on_flag1=1;
   cseg_data[2] = parameter % 10;
   parameter /= 10;
   cseg_data[1] = parameter % 10;
   parameter /= 10;
   cseg_data[0] = parameter%10;
   parameter /= 10;
   cseg_data[7] = parameter;

}
void Send_Byte(unsigned char transbyte)
{
    while(TXSTAbits.TRMT != 1);
     TXREG=transbyte;
}
void Send_Integer(unsigned int data)
{
    unsigned char higher=0,lower=0;
    unsigned int temp=0;
    temp=data;
    lower=temp;
    //(data&0x00ff);
    temp=data>>8;
    higher=temp;
    Send_Byte(higher);
    Send_Byte(lower);
}
void delay()        //delay for 100msec
{

    int ii,ii1;
    for(ii=0;ii<200;ii++)  //300
    {//100msec delay
    for(ii1=0;ii1<2000;ii1++)  //800
    {
    }
    }
    
}
void delay1()        //delay for 100msec
{

    int ii,ii1;
    for(ii=0;ii<30;ii++)//30
    {//100msec delay
    for(ii1=0;ii1<80;ii1++)//80
    {
    }
    }

}
void powerontest()
{
    cseg_data[7] =0x2B;
    cseg_data[0] =0x2B;
    cseg_data[1] =0x2B;
    cseg_data[2] =0x2B;
    cseg_data[3] =0x2B;
    cseg_data[4] =0x2B;
    cseg_data[5] =0x2B;
    cseg_data[6] =0x2B;
    led_group1.all=0XFF;
    led_group2.all=0XFF;
    led_group3.all=0XFF;
    led_group4.all=0XFF;
 
}
void Read_Model()
{
    if(modelcount==0x01)//(MODEL_SW1==1)        // && MODEL_SW2==1) Only pin RC4 is used for model selection in rev4
    {
        MODEL=1;        //300P
    }
    else if(modelcount==0x02)//(MODEL_SW1==0)   // && MODEL_SW2==0)
    {
        MODEL=2;        //400P
    }
    else
    {
         MODEL=2;        //400P default
    }
}
inline void chk_data_remote()
{


  if(MODEL==1)
  {
      peakcurrent = Rpeakcurrent/2.68;
      //peakcurrent = 5+(Rpeakcurrent*29.5/80.4);
      basecurrent= Rbasecurrent/2.68;
     // basecurrent= 5+(Rbasecurrent*29.5/80.4);
  }
  else if(MODEL==2)
  {
      peakcurrent = Rpeakcurrent/2.01;
      basecurrent= Rbasecurrent/2.01;
      //peakcurrent = 5+(Rpeakcurrent*39.5/80.4);
     // basecurrent= 5+(Rbasecurrent*39.5/80.4);
  }
}

void keydetect()
{
   if((MODE_SW==1)&&(TIG_SW==1)&& FOOT_SW==1 && pressurerr==0 && modelflag==0)  //normal mode not in model selection mode
    {
        calibrationmode=1;
        calibration=1;
        cseg_data[3] ='C'- 0x30;
        cseg_data[4] ='L'- 0x30;
        cseg_data[5] ='O'- 0x30;
        cseg_data[6] ='N'- 0x30;
        Disp_mesgf=1;
        Disp_mesg_cntr=0;
        dp_on_flag=0;
    }
   if((MENU_SW==1) && (calibrationmode==1)&& pressurerr==0 && modelflag==0) //not in model sel mode
    {
        //write(0X63,para.pwmslope);
        lowbyte=para.pwmslope;
        hibyte=para.pwmslope>>8;
        write(0X71,lowbyte);
        write(0X72,hibyte);
        calibrationmode=0;
        cseg_data[3] ='C'- 0x30;
        cseg_data[4] ='L'- 0x30;
        cseg_data[5] ='O'- 0x30;
        cseg_data[6] ='F'- 0x30;
        delay();
        Disp_mesgf=1;
        Disp_mesg_cntr=0;
    }
   if((MODE_SW==1)&&(TIG_SW==1)&& FOOT_SW==0 && pressurerr==0 && modelflag==0)//foot mode sayali
    {
        footsetupf=1;
    }
   if((MENU_SW==1) && (footsetupf==1))
    {
        if(setmaxcntf==0 && saveoffsetf==0 )
            setmaxcntf=1;
        else
            saveoffsetf=1;
    }
   if(PREV_PROG_SW!=PROG_SW)
    {
        if(PROG_SW==1 && modelflag==0)  //not in model sel mode
        {
            if(led_group4.bitsize.MMA==1)
            {
                led_group1.all=0;
                led_group2.all=0;
                led_group3.all=0;
                led_group4.all=0;
                led_group4.bitsize.TIGHF =1; //ENTRY IN TIG MODE
                {
                    led_group4.bitsize.NORMAL=1;
                    led_group1.bitsize.TWOT=1;
                    led_group2.bitsize.I1_MAIN=1;
                    led_group1.bitsize.AMP=1;
                }
                mmacurrent=para.mmacurrent;
                memcpy(parau.arrayu,hftigmemarr,13* sizeof(int));
            }
            else if(led_group4.bitsize.TIGHF==1)
            {
                led_group1.all=0;
                led_group2.all=0;
                led_group3.all=0;
                led_group4.all=0;
                led_group4.bitsize.LIFTARC =1;
                {
                    led_group4.bitsize.NORMAL=1;
                    led_group1.bitsize.TWOT=1;
                    led_group2.bitsize.I1_MAIN=1;
                    led_group1.bitsize.AMP=1;
                }
                memcpy(hftigmemarr,parau.arrayu,13* sizeof(int));
                memcpy(parau.arrayu,lifttigmemarr,13* sizeof(int));
            }
            else if(led_group4.bitsize.LIFTARC==1)
            {
                led_group1.all=0;
                led_group2.all=0;
                led_group3.all=0;
                led_group4.all=0;
                led_group4.bitsize.MMA=1;
                led_group2.bitsize.I1_MAIN=1;
                cVRD_fg=1;VRDTimecnt_2s=0; 
                led_group1.bitsize.AMP=1;
                memcpy(lifttigmemarr,parau.arrayu,13* sizeof(int));
                //para.mmacurrent=mmacurrent; 
            }
            Update_dispf=1;
         }
        else if(PROG_SW==1 && modelflag==1 && savemodelflag==1)
        {
           
            savemodelflag=0;
            if(modelcount==0x01)
            {
                MODEL=1;
                cseg_data[7]='3'-0X30;
                cseg_data[0]='0'-0x30;
                cseg_data[1]='0'-0X30;
                cseg_data[2]='P'-0X30;
            }
            else if(modelcount==0x02)
            {
                MODEL=2;
                cseg_data[7]='4'-0X30;
                cseg_data[0]='0'-0x30;
                cseg_data[1]='0'-0X30;
                cseg_data[2]='P'-0X30;
            }
            
            cseg_data[3] ='S'- 0x30;
            cseg_data[4] ='A'- 0x30;
            cseg_data[5] ='V'- 0x30;     // Model name & soft REV
            cseg_data[6] ='D'- 0x30;
            delay();

            modelflag=0;
            Update_dispf=1;

        }
         PREV_PROG_SW=PROG_SW;
    }
   if(led_group4.bitsize.MMA==0 && (led_group4.bitsize.TIGHF==1 || led_group4.bitsize.LIFTARC ==1)&& pressurerr==0) //in normal weld mode
    {
        //cseg_data[3] ='R'- 0x30;
        if(PREV_MODE_SW!=MODE_SW && modelflag==0)   //not in model sel mode
        {
           
            if((MODE_SW==1) && (FOOT_SW==1)) //normal mode sayali
            {
               if(led_group1.bitsize.TWOT==1)
               {
                  led_group1.bitsize.TWOT=0;
                  led_group1.bitsize.FOURT=1;
                  led_group1.bitsize.SPOT=0;
                  led_group1.bitsize.CYCLE=0;
               }
               else if(led_group1.bitsize.FOURT==1)
               {
                  led_group1.bitsize.TWOT=0;
                  led_group1.bitsize.FOURT=0;
                  led_group1.bitsize.SPOT=1;
                  led_group1.bitsize.CYCLE=0;
               }
               else if(led_group1.bitsize.SPOT==1)
               {
                  led_group1.bitsize.TWOT=0;
                  led_group1.bitsize.FOURT=0;
                  led_group1.bitsize.SPOT=0;
                  led_group1.bitsize.CYCLE=1;
               }
               else if(led_group1.bitsize.CYCLE==1)
               {
                  led_group1.bitsize.TWOT=1;
                  led_group1.bitsize.FOURT=0;
                  led_group1.bitsize.SPOT=0;
                  led_group1.bitsize.CYCLE=0;
               }
               led_group2.all=0x00;
               led_group3.all=0x00;
               led_group1.bitsize.SEC=0;
               led_group1.bitsize.FREQ=0;
               led_group1.bitsize.AMP=1;
               if(led_group4.bitsize.NORMAL==1)
               {
                   led_group2.bitsize.I1_MAIN=1;
               }
               else if(led_group4.bitsize.PULSE==1)
               {
                   led_group2.bitsize.I1_MAIN=1;
                   led_group2.bitsize.PEAK_CUR=1;
               }
               Update_dispf=1;
            }
            PREV_MODE_SW=MODE_SW;
        }
        if(PREV_TIG_SW!=TIG_SW && modelflag==0)  //not in model sel mode sayali
        {
             
            if((TIG_SW==1) && (FOOT_SW==1))  //normal mode
            {
                led_group2.all=0x00;
                led_group3.all=0x00;
                if(led_group4.bitsize.PULSE==1)
                {
                    led_group4.bitsize.PULSE=0;
                    led_group4.bitsize.NORMAL=1;
                    led_group2.bitsize.I1_MAIN=1;
                    inttochar(para.weld.I1);  //added by sayali
                    dp_on_flag=0;
                }
                else if(led_group4.bitsize.NORMAL==1)
                {
                    led_group4.bitsize.PULSE=1;
                    led_group4.bitsize.NORMAL=0;
                    led_group2.bitsize.I1_MAIN=1;
                    led_group2.bitsize.PEAK_CUR=1;
                    inttochar(para.weld.I1);  //added by sayali
                    dp_on_flag=0;
                }
                led_group1.bitsize.SEC=0;
                led_group1.bitsize.FREQ=0;
                led_group1.bitsize.AMP=1;
            }
            PREV_TIG_SW=TIG_SW;
        }
        if(FOOT_SW==0 && Cyclestart==0 && pressurerr==0 && modelflag==0) //FOOT mode
        {
            if(footswitchf==0)
            {
                led_group2.all=0x00;
                led_group3.all=0x00;
                led_group2.bitsize.I1_MAIN=1;
                led_group4.bitsize.NORMAL=1;
                led_group4.bitsize.PULSE=0;
                led_group1.bitsize.TWOT=1;
                led_group1.bitsize.FOURT=0;
                led_group1.bitsize.SPOT=0;
                led_group1.bitsize.CYCLE=0;
                led_group2.bitsize.PEAK_CUR=0;
                normaldispupdatf=1;
                footswitchf=1;
            }
            if(led_group2.bitsize.I1_MAIN==1 && footsetupf==0)
            {
                cseg_data[3] ='F'- 0x30;
                cseg_data[4] ='O'-0X30;
                cseg_data[5] ='O'-0X30;         //Foot
                cseg_data[6] ='T'- 0x30;
                dp_on_flag=0;
              //inttochar(Actual_Footsw);
              //inttochar(Avg_Footsw);
            }
           // inttochar(footrefadc);

        }
        else
        {
            if(normaldispupdatf==1)
            {
                normaldispupdatf=0;
                footswitchf=0;
                Update_dispf=1;
            }
        }

        if(PREV_MENU_SW!=MENU_SW)
        {
            if(modelflag==0)
            {
            if((MENU_SW==1)&&(calibrationmode==0)&& footsetupf==0 && pressurerr==0 )
            {
                //*************************************************************************
                //Added pref, pstf on lower display for champ tig 300/400P
                if(led_group1.bitsize.TWOT==1 && led_group4.bitsize.NORMAL==1 && FOOT_SW==0)//foot mode 
                {
                    if(led_group2.bitsize.PREFLOW==1)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;

                        cseg_data[3] ='F'- 0x30;
                        cseg_data[4] =0X2C;
                        cseg_data[5] =0X2C;          //Foot
                        cseg_data[6] ='T'- 0x30;
                        Disp_mesgf=1;
                        dp_on_flag=0;
                        Disp_mesg_cntr=0;
                        //inttochar(Actual_Footsw);
                        //inttochar(Avg_Footsw);
                        //inttochar(footrefadc);
                   }
                   else if(led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0 ;  //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                       delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }

                }
else if(led_group1.bitsize.TWOT==1 && led_group4.bitsize.NORMAL==1 && FOOT_SW==1)//normal mode
                {
                    if(led_group2.bitsize.PREFLOW==1)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0 ;  //added by sayali
                   }
                   else if( led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='M'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='I'- 0x30;
                        cseg_data[2] ='N'- 0x30;

                        cseg_data[3] ='M'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='I'- 0x30;
                        cseg_data[6] ='N'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                   }
                   else if(led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;  //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                       delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }
               } //if of 2T with normal
                //*****************************************
               else if(led_group1.bitsize.TWOT==1 && led_group4.bitsize.PULSE==1)
               {
                    if(led_group2.bitsize.PREFLOW==1)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group2.bitsize.PEAK_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='E'- 0x30;
                        cseg_data[1] ='A'- 0x30;
                        cseg_data[2] ='K'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='E'- 0x30;
                        cseg_data[5] ='A'- 0x30;
                        cseg_data[6] ='K'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.BASE_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='B'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='E'- 0x30;

                        cseg_data[3] ='B'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='E'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.BASE_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DUTY=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='U'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='Y'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='U'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='Y'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DUTY==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.PULSE_FREQ=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=1;
                        cseg_data[7] ='F'- 0x30;
                        cseg_data[0] ='R'- 0x30;
                        cseg_data[1] ='E'- 0x30;
                        cseg_data[2] ='Q'- 0x30;

                        cseg_data[3] ='F'- 0x30;
                        cseg_data[4] ='R'- 0x30;
                        cseg_data[5] ='E'- 0x30;
                        cseg_data[6] ='Q'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.PULSE_FREQ==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                        delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }
               }//else if of 2T with pulse
               //***********for 4T with normal **************************************
               else if(led_group1.bitsize.FOURT==1 && led_group4.bitsize.NORMAL==1)
               {
                   if(led_group2.bitsize.PREFLOW==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.START_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='S'- 0x30;
                        cseg_data[0] ='T'- 0x30;
                        cseg_data[1] ='R'- 0x30;
                        cseg_data[2] ='T'- 0x30;

                        cseg_data[3] ='S'- 0x30;
                        cseg_data[4] ='T'- 0x30;
                        cseg_data[5] ='R'- 0x30;
                        cseg_data[6] ='T'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                    }
                   else if(led_group2.bitsize.START_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='M'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='I'- 0x30;
                        cseg_data[2] ='N'- 0x30;

                        cseg_data[3] ='M'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='I'- 0x30;
                        cseg_data[6] ='N'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                   }
                   else if(led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.END_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='E'- 0x30;
                        cseg_data[0] ='N'- 0x30;
                        cseg_data[1] ='D'- 0x30;
                        cseg_data[2] ='A'- 0x30;

                        cseg_data[3] ='E'- 0x30;
                        cseg_data[4] ='N'- 0x30;
                        cseg_data[5] ='D'- 0x30;
                        cseg_data[6] ='A'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.END_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                        delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }
               } //IF end on 4T & NORMAL
               else if(led_group1.bitsize.FOURT==1 && led_group4.bitsize.PULSE==1)
               {
                   if(led_group2.bitsize.PREFLOW==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.START_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='S'- 0x30;
                        cseg_data[0] ='T'- 0x30;
                        cseg_data[1] ='R'- 0x30;
                        cseg_data[2] ='T'- 0x30;

                        cseg_data[3] ='S'- 0x30;
                        cseg_data[4] ='T'- 0x30;
                        cseg_data[5] ='R'- 0x30;
                        cseg_data[6] ='T'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                    }
                   else if(led_group2.bitsize.START_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group2.bitsize.PEAK_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='E'- 0x30;
                        cseg_data[1] ='A'- 0x30;
                        cseg_data[2] ='K'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='E'- 0x30;
                        cseg_data[5] ='A'- 0x30;
                        cseg_data[6] ='K'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.BASE_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='B'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='E'- 0x30;

                        cseg_data[3] ='B'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='E'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.BASE_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DUTY=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='U'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='Y'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='U'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='Y'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        dp_on_flag=0;
                        Disp_mesg_cntr=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DUTY==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.PULSE_FREQ=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=1;
                        cseg_data[7] ='F'- 0x30;
                        cseg_data[0] ='R'- 0x30;
                        cseg_data[1] ='E'- 0x30;
                        cseg_data[2] ='Q'- 0x30;

                        cseg_data[3] ='F'- 0x30;
                        cseg_data[4] ='R'- 0x30;
                        cseg_data[5] ='E'- 0x30;
                        cseg_data[6] ='Q'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.PULSE_FREQ==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.END_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='E'- 0x30;
                        cseg_data[0] ='N'- 0x30;
                        cseg_data[1] ='D'- 0x30;
                        cseg_data[2] ='A'- 0x30;

                        cseg_data[3] ='E'- 0x30;
                        cseg_data[4] ='N'- 0x30;
                        cseg_data[5] ='D'- 0x30;
                        cseg_data[6] ='A'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.END_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                        delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }
               } //end else if of 4T PULSE
               else if(led_group1.bitsize.SPOT==1 && led_group4.bitsize.NORMAL==1)
               {
                   if(led_group2.bitsize.PREFLOW==1)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='M'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='I'- 0x30;
                        cseg_data[2] ='N'- 0x30;

                        cseg_data[3] ='M'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='I'- 0x30;
                        cseg_data[6] ='N'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DUTY=1;              //SPOT TIME LED
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='S'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='O'- 0x30;
                        cseg_data[2] ='T'- 0x30;

                        cseg_data[3] ='S'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='O'- 0x30;
                        cseg_data[6] ='T'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        //dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DUTY==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                        delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }
               }//end of spot
               else if(led_group1.bitsize.CYCLE==1 && led_group4.bitsize.NORMAL==1)
               {
                   if(led_group2.bitsize.PREFLOW==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.START_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='S'- 0x30;
                        cseg_data[0] ='T'- 0x30;
                        cseg_data[1] ='R'- 0x30;
                        cseg_data[2] ='T'- 0x30;

                        cseg_data[3] ='S'- 0x30;
                        cseg_data[4] ='T'- 0x30;
                        cseg_data[5] ='R'- 0x30;
                        cseg_data[6] ='T'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                  }
                  else if(led_group2.bitsize.START_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='M'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='I'- 0x30;
                        cseg_data[2] ='N'- 0x30;

                        cseg_data[3] ='M'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='I'- 0x30;
                        cseg_data[6] ='N'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                   }
                   else if( led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I2_CYCLE=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='C'- 0x30;
                        cseg_data[0] ='Y'- 0x30;
                        cseg_data[1] ='L'- 0x30;
                        cseg_data[2] ='E'- 0x30;

                        cseg_data[3] ='C'- 0x30;
                        cseg_data[4] ='Y'- 0x30;
                        cseg_data[5] ='L'- 0x30;
                        cseg_data[6] ='E'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.I2_CYCLE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.END_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='E'- 0x30;
                        cseg_data[0] ='N'- 0x30;
                        cseg_data[1] ='D'- 0x30;
                        cseg_data[2] ='A'- 0x30;

                        cseg_data[3] ='E'- 0x30;
                        cseg_data[4] ='N'- 0x30;
                        cseg_data[5] ='D'- 0x30;
                        cseg_data[6] ='A'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.END_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                       delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }

               }//end of cycle normal
               else if(led_group1.bitsize.CYCLE==1 && led_group4.bitsize.PULSE==1)
               {
                   if(led_group2.bitsize.PREFLOW==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.START_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='S'- 0x30;
                        cseg_data[0] ='T'- 0x30;
                        cseg_data[1] ='R'- 0x30;
                        cseg_data[2] ='T'- 0x30;

                        cseg_data[3] ='S'- 0x30;
                        cseg_data[4] ='T'- 0x30;
                        cseg_data[5] ='R'- 0x30;
                        cseg_data[6] ='T'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                  }
                  else if(led_group2.bitsize.START_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.UPSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='U'- 0x30;
                        cseg_data[0] ='P'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='U'- 0x30;
                        cseg_data[4] ='P'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.UPSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        led_group2.bitsize.PEAK_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='E'- 0x30;
                        cseg_data[1] ='A'- 0x30;
                        cseg_data[2] ='K'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='E'- 0x30;
                        cseg_data[5] ='A'- 0x30;
                        cseg_data[6] ='K'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if( led_group2.bitsize.I1_MAIN==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I2_CYCLE=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='C'- 0x30;
                        cseg_data[0] ='Y'- 0x30;
                        cseg_data[1] ='L'- 0x30;
                        cseg_data[2] ='E'- 0x30;

                        cseg_data[3] ='C'- 0x30;
                        cseg_data[4] ='Y'- 0x30;
                        cseg_data[5] ='L'- 0x30;
                        cseg_data[6] ='E'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.I2_CYCLE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.BASE_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='B'- 0x30;
                        cseg_data[0] ='A'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='E'- 0x30;

                        cseg_data[3] ='B'- 0x30;
                        cseg_data[4] ='A'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='E'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.BASE_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DUTY=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='U'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='Y'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='U'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='Y'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DUTY==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.PULSE_FREQ=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=1;
                        cseg_data[7] ='F'- 0x30;
                        cseg_data[0] ='R'- 0x30;
                        cseg_data[1] ='E'- 0x30;
                        cseg_data[2] ='Q'- 0x30;

                        cseg_data[3] ='F'- 0x30;
                        cseg_data[4] ='R'- 0x30;
                        cseg_data[5] ='E'- 0x30;
                        cseg_data[6] ='Q'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group2.bitsize.PULSE_FREQ==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.DOWNLSLOPE=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='D'- 0x30;
                        cseg_data[0] ='W'- 0x30;
                        cseg_data[1] ='S'- 0x30;
                        cseg_data[2] ='P'- 0x30;

                        cseg_data[3] ='D'- 0x30;
                        cseg_data[4] ='W'- 0x30;
                        cseg_data[5] ='S'- 0x30;
                        cseg_data[6] ='P'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.DOWNLSLOPE==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.END_CUR=1;
                        led_group1.bitsize.AMP=1;
                        led_group1.bitsize.SEC=0;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='E'- 0x30;
                        cseg_data[0] ='N'- 0x30;
                        cseg_data[1] ='D'- 0x30;
                        cseg_data[2] ='A'- 0x30;

                        cseg_data[3] ='E'- 0x30;
                        cseg_data[4] ='N'- 0x30;
                        cseg_data[5] ='D'- 0x30;
                        cseg_data[6] ='A'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.END_CUR==1)
                   {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group3.bitsize.POSTFLOW=1;
                        led_group1.bitsize.AMP=0;
                        led_group1.bitsize.SEC=1;
                        led_group1.bitsize.FREQ=0;
                        cseg_data[7] ='P'- 0x30;
                        cseg_data[0] ='S'- 0x30;
                        cseg_data[1] ='T'- 0x30;
                        cseg_data[2] ='F'- 0x30;

                        cseg_data[3] ='P'- 0x30;
                        cseg_data[4] ='S'- 0x30;
                        cseg_data[5] ='T'- 0x30;
                        cseg_data[6] ='F'- 0x30;
                        delay();
                        Disp_mesgf=1;
                        Disp_mesg_cntr=0;
                        dp_on_flag=0;
                        dp_on_flag1=0;   //added by sayali
                   }
                   else if(led_group3.bitsize.POSTFLOW==1)
                   {
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.PREFLOW=1;
                       led_group1.bitsize.AMP=0;
                       led_group1.bitsize.SEC=1;
                       led_group1.bitsize.FREQ=0;
                       cseg_data[7] ='P'- 0x30;
                       cseg_data[0] ='R'- 0x30;
                       cseg_data[1] ='E'- 0x30;
                       cseg_data[2] ='F'- 0x30;

                       cseg_data[3] ='P'- 0x30;
                       cseg_data[4] ='R'- 0x30;
                       cseg_data[5] ='E'- 0x30;
                       cseg_data[6] ='F'- 0x30;
                       delay();
                       Disp_mesgf=1;
                       Disp_mesg_cntr=0;
                       dp_on_flag=0;
                       dp_on_flag1=0;   //added by sayali
                   }
               } //end of cycle pulse
                enc_result='~';
            }
            }
            else if(modelflag==1)
            {
                modelcount++;
                if(modelcount==0x01)
                {
                    //modelcount=0x02;
                    //MODEL=2;
                    cseg_data[3] ='3'- 0x30;
                    cseg_data[4] ='0'- 0x30;
                    cseg_data[5] ='0'- 0x30;
                    cseg_data[6] ='P'- 0x30;
                    delay();

                }
                else if(modelcount==0x02)
                {
                    //modelcount=0x01;
                    //MODEL=1;
                    cseg_data[3] ='4'- 0x30;
                    cseg_data[4] ='0'- 0x30;
                    cseg_data[5] ='0'- 0x30;
                    cseg_data[6] ='P'- 0x30;
                    delay();

                }
                else if(modelcount<0x01 || modelcount>0x02)
                {
                    modelcount=0x01;
                    //MODEL=1;
                    cseg_data[3] ='3'- 0x30;
                    cseg_data[4] ='0'- 0x30;
                    cseg_data[5] ='0'- 0x30;
                    cseg_data[6] ='P'- 0x30;
                    delay();

                }
                savemodelflag=1;

            }
            PREV_MENU_SW=MENU_SW;
        }
        if(PREV_GASWATER_SW!=GAS_WATER_SW)
        {
            if(GAS_WATER_SW==1)
                gaswatersw=GAS_COOLED;
            else
                gaswatersw=WATER_COOLED;

            PREV_GASWATER_SW=GAS_WATER_SW;
        }
        if(PREV_PRESSURE_SW!=WATER_PRESSURE_SW)
        {
            if(WATER_PRESSURE_SW==1)
                pressure=WATER_PRESENT;
            else
                pressure=WATER_ABSENT;

            PREV_PRESSURE_SW=WATER_PRESSURE_SW;
        }
        if(PREV_TORCH_SW!=TORCH_SW)
        {
            if(TORCH_SW==1)
            KEYCODE='J';

            PREV_TORCH_SW=TORCH_SW;
        }
        if(led_group1.bitsize.SPOT==1)
        {
            led_group4.bitsize.PULSE=0;
            led_group4.bitsize.NORMAL=1;
            led_group2.bitsize.PEAK_CUR=0;
        }
    }
    else if(led_group4.bitsize.MMA==1 && led_group4.bitsize.TIGHF==0 && led_group4.bitsize.LIFTARC ==0)
    {
        led_group2.bitsize.I1_MAIN=1;
        led_group1.bitsize.AMP=1;
    }
}
void default_value()
{
    //para.mmacurrent=50;
    if(para.preflow.time ==0XFFFF || para.preflow.time> 50 )
    {
        para.preflow.time = 0;
    }
    if(para.start.amp == 0XFFFF || para.start.amp >maxcurrent )
    {
        para.start.amp = mincurrent;
    }
    if(para.upslope.time == 0XFFFF || para.upslope.time >100 )
    {
        para.upslope.time = 0;
    }
    if(para.weld.I1 == 0XFFFF || para.weld.I1 >maxcurrent )
    {
        para.weld.I1 = mincurrent;
    }
    if(para.weld.I2 == 0XFFFF || para.weld.I2 >maxcurrent)
    {
        para.weld.I2 = mincurrent;
    }
    if(para.weld.base == 0XFFFF || para.weld.base >maxcurrent )
    {
        para.weld.base = mincurrent;
    }
    if(para.weld.peak == 0XFFFF || para.weld.peak >maxcurrent )
    {
        para.weld.peak = mincurrent;
    }
    if(para.weld.freq == 0XFFFF || para.weld.freq >300 )
    {
        para.weld.freq = 1;
    }
    if(para.spottime == 0XFFFF || para.spottime >100 )
    {
        para.spottime = 10;
    }
    if(para.weld.duty == 0XFFFF || para.weld.duty >90 )
    {
        para.weld.duty = 10;
    }
    if(para.dnslope.time == 0XFFFF || para.dnslope.time >100 )
    {
        para.dnslope.time = 0;
    }
    if(para.end.amp == 0XFFFF || para.end.amp >maxcurrent )
    {
        para.end.amp = mincurrent;
    }
    if(para.postflow.time == 0XFFFF || para.postflow.time >200 )
    {
        para.postflow.time = 1;
    }
}
void resetmem()
{
    for(i=0;i<64;i++)
    {
        write(i,0XFF);
    }
    para.ledgp1= 0XFF;
    para.ledgp2= 0XFF;
    para.ledgp3= 0XFF;
    para.ledgp4= 0XFF;
    write(0X58, para.ledgp1);
    write(0X59, para.ledgp2);
    write(0X60, para.ledgp3);
    write(0X61, para.ledgp4);
    write(0X69,0XFF);
    write(0X70,0XFF);
}

void writemem()
{
    unsigned char Addr1=0, Addr2=0,x=0;
     para.ledgp1=led_group1.all;
     para.ledgp2=led_group2.all;
     para.ledgp3=led_group3.all;
     para.ledgp4=led_group4.all;
    if(led_group4.bitsize.TIGHF==1)
    {
        for(i=0;i<14;i++)
        {
               lowbyte=parau.arrayu[i];                  //write in memory hour of arc on timer
               hibyte=parau.arrayu[i]>>8;
               Addr1=(i*2);
               Addr2=(Addr1+1);
               EE_Data=parau.arrayu[i];
               if(EE_Data != hftigprevmemarr[i])
               {
                       write(Addr1,lowbyte);
                       write(Addr2,hibyte);
                       hftigprevmemarr[i]= parau.arrayu[i];
               }
        }
   }
   if(led_group4.bitsize.LIFTARC==1)
   {
        x=0;
        for(i=15;i<29;i++)
        {
               lowbyte=parau.arrayu[x];                  //write in memory hour of arc on timer
               hibyte=parau.arrayu[x]>>8;
               Addr1=(i*2);
               Addr2=(Addr1+1);
               EE_Data=parau.arrayu[x];
               if(EE_Data != lifttigprevmemarr[x])
               {
                       write(Addr1,lowbyte);
                       write(Addr2,hibyte);
                       lifttigprevmemarr[x]= parau.arrayu[x];
               }
               x++;
        }
   }
   if(led_group4.bitsize.MMA==1)
   {
       if(prevmacurrent!=para.mmacurrent)
       {
            lowbyte=para.mmacurrent;
            hibyte=para.mmacurrent>>8;
            write(0X69,lowbyte);
            write(0X70,hibyte);
            prevmacurrent=para.mmacurrent;
       }
   }
   if(prvledgp1!=para.ledgp1)
   {
        write(0X58, para.ledgp1);
        prvledgp1= para.ledgp1;
   }
   if(prvledgp2!=para.ledgp2)
   {
        write(0X59, para.ledgp2);
        prvledgp2= para.ledgp2;
   }
   if(prvledgp3!=para.ledgp3)
   {
        write(0X60, para.ledgp3);
        prvledgp3= para.ledgp3;
   }
   if(prvledgp4!=para.ledgp4)
   {
        write(0X61, para.ledgp4);
        prvledgp4= para.ledgp4;
   }

    write(0x74, modelcount);
}

void readmem()
{
    unsigned char addr1=0,addr2=0,x=0;

    for(i=0; i<14; i++)
    {
        addr1=(i*2);
        addr2=(addr1+1);
        lowbyte=Read(addr1);
        hibyte=Read(addr2);
        tempvar=hibyte<<8;
        tempvar=(tempvar | lowbyte);
        parau.arrayu[i] =tempvar;
        hftigprevmemarr[i]=parau.arrayu[i];
    }
    default_value();
    for(i=0; i<14; i++)
    {
        hftigmemarr[i]=parau.arrayu[i];
    }
    for(i=15; i<29; i++)
    {
        addr1=(i*2);
        addr2=(addr1+1);
        lowbyte=Read(addr1);
        hibyte=Read(addr2);
        tempvar=hibyte<<8;
        tempvar=(tempvar | lowbyte);
        parau.arrayu[x] =tempvar;
        lifttigprevmemarr[x]=parau.arrayu[x];
        x++;
    }
    default_value();
    for(i=0; i<14; i++)
    {
        lifttigmemarr[i]=parau.arrayu[i];
    }
    {
        lowbyte=Read(0X69);            //for MMA CURRENT
        hibyte=Read(0X70);
        tempvar=hibyte<<8;
        tempvar=(tempvar | lowbyte);
        para.mmacurrent =tempvar;
    }
    if(para.mmacurrent==0 || para.mmacurrent==0XFFFF || para.mmacurrent > maxmmacurrent || para.mmacurrent < 50)
    {
        para.mmacurrent=50;
    }

    para.ledgp1=Read(0X58);
    para.ledgp2=Read(0X59);
    para.ledgp3=Read(0X60);
    para.ledgp4=Read(0X61);
    //para.pwmslope=Read(0X63);

    {
        lowbyte=Read(0X71);            //for MMA CURRENT
        hibyte=Read(0X72);
        tempvar=hibyte<<8;
        tempvar=(tempvar | lowbyte);
        para.pwmslope =tempvar;
    }
    if(para.ledgp1==0XFF)
    {
        led_group1.all=0x00;
        led_group1.bitsize.TWOT=1;
        led_group1.bitsize.AMP=1;
    }
    else
        led_group1.all=para.ledgp1;

    if(para.ledgp2==0XFF)
    {
        led_group2.all=0x00;
        led_group2.bitsize.I1_MAIN=1;
    }
    else
        led_group2.all=para.ledgp2;

    if(para.ledgp3==0XFF)
    {
        led_group3.all=0x00;
    }
    else
       led_group3.all=para.ledgp3;

    if(para.ledgp4==0XFF)
    {
        led_group4.all=0x00;
        led_group4.bitsize.TIGHF=1;
        led_group4.bitsize.NORMAL=1;
    }
    else
        led_group4.all=para.ledgp4;

    if(led_group4.bitsize.PULSE==1)
    {
        led_group2.all=0;
        led_group3.all=0;
        led_group2.bitsize.I1_MAIN=1;
        led_group2.bitsize.PEAK_CUR=1;
    }
    else
    {
        led_group2.all=0;
        led_group3.all=0;
        led_group2.bitsize.I1_MAIN=1;
    }

    if(para.pwmslope==0XFFFF ||para.pwmslope>700 || para.pwmslope<500)
    {
        para.pwmslope=600;
    }
    {
        lowbyte=Read(0X64);                 //FOOT SWITCH MIN OFFSET
        hibyte=Read(0x65);
        tempvar=hibyte<<8;
        tempvar=(tempvar | lowbyte);
        fminoffset =tempvar;

        lowbyte=Read(0X66);                 //FOOT SWITCH MAX OFFSET
        hibyte=Read(0x67);
        tempvar=hibyte<<8;
        tempvar=(tempvar | lowbyte);
        fmaxoffset =tempvar;
        fmaxoffset1=(unsigned int)fmaxoffset/10.0;
    }
    if(fminoffset==0XFFFF)
    {
        fminoffset=0;
    }
    if(fmaxoffset==0xFFFF)
    {
        fmaxoffset=700.0;
    }

    if((led_group4.bitsize.MMA==1)&&(led_group4.bitsize.TIGHF==1)&&(led_group4.bitsize.LIFTARC==1))
    {
        led_group4.all=0;
        led_group4.bitsize.TIGHF=1;
        led_group4.bitsize.NORMAL=1;
    }
    else if(((led_group4.bitsize.MMA==1)&&(led_group4.bitsize.TIGHF==1)) ||((led_group4.bitsize.MMA==1)&&(led_group4.bitsize.LIFTARC==1))||
            ((led_group4.bitsize.TIGHF==1)&&(led_group4.bitsize.LIFTARC==1)))
    {
        led_group4.all=0;
        led_group4.bitsize.TIGHF=1;
        led_group4.bitsize.NORMAL=1;
    }

    lowbyte=Read(0x74);           //modelcount is stired at 0x74
    modelcount=lowbyte;

}
void write(unsigned char addr,unsigned char data)
{
 	EEADR = addr;
  	EEDATA = data;
        INTCONbits.GIE = 0;
  	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;
	EECON1bits.WREN = 1;
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;
	while(EECON1bits.WR==1);				//Wait till the write completion
        INTCONbits.GIE = 1;
	EECON1bits.WREN = 0;

}

unsigned char Read(unsigned char addr)
{
	EEADR = addr;
  	EECON1bits.CFGS = 0;
	EECON1bits.EEPGD = 0;
	EECON1bits.RD = 1;
	Nop();							//Nop may be required for latency at high frequencies
	Nop();							//Nop may be required for latency at high frequencies
	return (EEDATA);
                                      // return with read byte
}

char test_gas_hf()
{
    char gas=0,hf=0,count_test=0;

    GAS_RELAY_OUT=0;
    HF_RELAY_OUT=0;

    while(1)
    {

    count_test=Update_Data(count_test,0,1,enc_result);

    if(count_test==1)
    {
      if(MENU_SW==1)
      {
          HF_RELAY_OUT=1;
          GAS_RELAY_OUT=0;
      }
      else
      {
          HF_RELAY_OUT=0;
          GAS_RELAY_OUT=0;
      }



    dispcurrentf=0;
    cseg_data[7] ='H'- 0x30;
    cseg_data[0] ='F'- 0x30;
    cseg_data[1] ='R'- 0x30;
    cseg_data[2] ='L'- 0x30;
    Disp_mesgf=1;
    Disp_mesg_cntr=0;
    dp_on_flag=0;
    hf=1;
    gas=0;
    }
    else if(count_test==0)
     {
    dispcurrentf=0;
    cseg_data[7] ='G'- 0x30;
    cseg_data[0] ='A'- 0x30;
    cseg_data[1] ='S'- 0x30;
    cseg_data[2] ='R'- 0x30;

     if(MENU_SW==1)
     {
          GAS_RELAY_OUT=1;
          HF_RELAY_OUT=0;
     }
      else
      {
          GAS_RELAY_OUT=0;
          HF_RELAY_OUT=0;
      }

    Disp_mesgf=1;
    Disp_mesg_cntr=0;
    dp_on_flag=0;
    hf=0;
    gas=1;
    }




    if(PROG_SW==1)
       {
        return 0;
        test_time=0;
       }

    }



}

 void clear_all()
{
    SEGA=0;
    SEGB=0;
    SEGC=0;
    SEGD=0;
    SEGE=0;
    SEGF=0;
    SEGG=0;
    SEGDP=0;



}





