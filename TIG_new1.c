void TIG_Weld_Process()
{

      //intochar_volt(Actual_Volt);
      //$dp_on_flag1=1;
    if(Cyclestart==0)
    {
      oper=pre;
    }

////*************************** 2T WITH foot mode and normal tig mode(not pulse)****************************************
    if(led_group1.bitsize.TWOT==1 && led_group4.bitsize.NORMAL==1 && (FOOT_SW==0))
   {
       switch(oper)
       {
           case pre:
            {
               if(Cyclestart==0)
               {
                   
                   preflow();
                }
                if((Time_100ms >=para.preflow.time) && OCVONf==0)
                {

	           Update_PWM(min_current); //15A


                    //GAS_RELAY_OUT=1;
                    Update_PWM(min_current);    //minimum current for arc striking--- 15A
                    OCVONf=0;//1;  ASN
                    Time_100ms=0;
                    msec1=0;

     		    if(led_group4.bitsize.TIGHF==1)
                    HF_RELAY_OUT=1;

                    oper=start1;  //ASN
                }
            
                if(TORCH==0)
               {
                   oper=exit1;
                }
               break;
            }


           case start1:
            {
                if(Actual_Current > 2)//(2*10)))
                {
                   if(Time_ms>=2 && Actual_Current > 2)    //(time 100msec*2=200msec && actual current>2)
                   {
                       HF_RELAY_OUT=0;
                       //GAS_RELAY_OUT=1;
                       Time_100ms=0;
                       msec1=0;
                       Time_ms=0;
                       led_group2.all=0x00;
                       led_group3.all=0x00;
                       led_group2.bitsize.I1_MAIN=1;
//                     Update_PWM(para.weld.peak);   //set current
                       Update_PWM(Actual_Footsw);
                       oper=weld;
                       dispcurrentf=1;
                       dispvtgf=1;
                   }
                }
                else if(Time_100ms >= 20)  //(HF on for 100msec*20= 2 seconds)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    curerrf=1;  //ASN
		    Error_no=4;
                    oper=exit1;
                }

                if(TORCH==0)
                {
                    oper=exit1;
                }
                break;
            }
            case weld:
            {
                weld_flag=1;
                if(ARC_DETEC_FLAG==0)
                {
                  oper=postflow;  //to go to postflow send control to downslope wit downslope flag==0;
                  Error_no=4;
                }


                    if(TORCH==0 || Error_no!=0)
                    {

                    if(TORCH==0)
                    torchreleased=1;

   	            //GAS_RELAY_OUT=1;
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.POSTFLOW=1;
                    Time_100ms=0;
                    msec1=0;
//                    //Update_PWM(0);   //set current
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
	            OCVONLAT=1;

                    oper=postflow;
                    dispcurrentf=0;
                    dispvtgf=0;
                    cseg_data[3] ='P'- 0x30;
                    cseg_data[4] ='S'- 0x30;
                    cseg_data[5] ='T'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                    }
                else
                {
                    //GAS_RELAY_OUT=1;
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.I1_MAIN=1;
                    Update_PWM(Actual_Footsw);
                }
                break;
	    }
            case postflow:
            {
                                weld_flag=0;
				if(TORCH==0)
				torchreleased=1;


                 if( ( (Time_100ms >= para.postflow.time) || (TORCH==1) )&& (torchreleased==1)  )  ////postflow torch detection
                 {
                    Time_100ms=0;
                    msec1=0;
                    oper=exit1;
                 }
                else if( (Time_100ms >= para.postflow.time) )  ////akshay S N 15.05.2017 spot
                {
                 Time_100ms = para.postflow.time;
                 CCPR1L=0;
                 CCP1CONbits.DC1B=0;
                 GAS_RELAY_OUT=0;
                 HF_RELAY_OUT=0;
                 if(Error_no!=0)
                 display_error();
                }
                 break;
                 }
            case exit1:
            {

                  if(curerrf!=0)
                 display_error();

                 exitprocess();
//           /*     if(TORCH==1)            //akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                    break;
//                }
//            */
//
//                //akshay S N 15.05.2017
                if(TORCH==1 && curerrf==0)  //if torch is presses continously and no current error
                {
                oper=pre;//end;
//                //break;  ////akshay S N 15.05.2017 do not break here so that all sequence parameters get loaded with ther default value
                }
                if(TORCH==1 && curerrf!=0) //if torch is presses continously and current error error was there then control will stuck here only still user releases the torch
                {
                oper=exit1;
                display_error();
                break;
                }

                Error_no=0;
                curerrf=0;
                exitwriteprocess();
                updatedisptig=1;
                break;

            }
        }//end switchfoot mode
    }//end if foot mode
    
    
    /***************************************normal*******************2T mode************************************************/
    else if(led_group1.bitsize.TWOT==1 && (led_group4.bitsize.NORMAL==1 || led_group4.bitsize.PULSE==1) && (FOOT_SW==1))
    {
        switch(oper)
        {
            case pre:
            {
                if(Cyclestart==0)
                {
                    if(para.weld.base >= para.weld.I1 && led_group4.bitsize.PULSE==1)  //@ if in pulse mode if base current is greater than peak current
                    {
                        para.weld.base = para.weld.I1;
                    }

                    preflow();
                }

                if((Time_100ms >=para.preflow.time) && OCVONf==0) //akshay S N 15.05.2017
                {
                 if( (para.upslope.time>0))
                  {
                      arc_strike_current=min_current;
                      Update_PWM(arc_strike_current); //15A
                  }
                 else
                 {
                      arc_strike_current=para.weld.I1;
                      Update_PWM(para.weld.I1);  //xx
                 }

                    OCVONf=0;   //1
                    Time_100ms=0;
                    Time_ms=0;
                    msec1=0;
//                    //led_group2.all=0x00;
//                    //led_group3.all=0x00;
//                    //led_group2.bitsize.START_CUR=1;
//
//
                    if(led_group4.bitsize.TIGHF==1)
                    HF_RELAY_OUT=1;

                    oper=start1;
                }

                if(TORCH==0)
                {
                    oper=exit1;

                }
               break;
            }
            case start1:
            {
                //GAS_RELAY_OUT=1;              //commented
                if(Actual_Current > 2)//|| (Time_100ms >= 20))//(2*10)))
                {
                   if(Time_ms>=2 && Actual_Current >2)   //HF time and actual current
                   {
                        HF_RELAY_OUT=0;                  //HF relay off

                        arc_detection_count=0;  //ASN 19.05.2017
                        ARC_DETEC_FLAG=1;       //ASN 19.05.2017

                        Time_100ms=0;
                        msec1=0;
                        Time_ms=0;
//                     //   Update_PWM(minimum.weld.I1);   //minimum current  //akshay S N 15.05.2017  to start with main current if upslope time=0
                        if(para.upslope.time>0)
                        {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.UPSLOPE=1;
////                            Upslope_cntr=para.upslope.time*100.0;
////                            UP_Step=(float)((para.weld.I1 - minimum.weld.I1)/(Upslope_cntr));
//
//
                         if(para.weld.I1<arc_strike_current)
                         {
                         UP_Step=(float)((para.weld.I1 - mincurrent)/(para.upslope.time*10.0));
                         Upslope_cntr=para.upslope.time*10;
                         Upcurrent=mincurrent;
                         }
                         else
                         {
                         UP_Step=(float)((para.weld.I1 - arc_strike_current)/(para.upslope.time*10.0));
                         Upslope_cntr=para.upslope.time*10;
                         Upcurrent=arc_strike_current;
                         }

                           msec10_cntr=0;
                            upslopef=1;
                            oper=upslope;
                            cseg_data[3] ='U'- 0x30;
                            cseg_data[4] ='P'- 0x30;
                            cseg_data[5] ='S'- 0x30;
                            cseg_data[6] ='P'- 0x30;
                            Disp_mesgf=1;
                            Disp_mesg_cntr=0;
                            dp_on_flag=0;
                            dp_on_flag1=0;
                      }
                      else
                      {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.I1_MAIN=1;
                            Update_PWM(para.weld.I1);
                            oper=weld;
                            dispcurrentf=1;
                            dispvtgf=1;
                            if(led_group4.bitsize.PULSE==1)
                            {
                                pulsef=1;
                            }
                       }

                   }
                }
                else if(Time_100ms >= 20)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    curerrf=1;  //0 //akshay S N 15.05.2017
                    Error_no=4;
                    oper=exit1;
                }

                if(TORCH==0)
                {
                    oper=exit1;
                    torchreleased=1;
                }
                break;
            }
           case upslope:
            {
                if(upslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.UPSLOPE=1;
                    if(led_group4.bitsize.PULSE==1)
                    {
                        pulsef=1;
                        if(pwmresult>=para.weld.base)
                        {
                            (dutyon==1)?(Update_PWM(para.weld.base)):(Update_PWM(pwmresult));
                        }
                        else
                        {
                            Update_PWM(pwmresult);
                        }
                    }
                    else
                    {
                        Update_PWM(pwmresult);
                    }
                    oper=upslope;

                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.I1_MAIN=1;
                    Update_PWM(para.weld.I1);

                    arc_detection_count=0;  //asn 22.05.2017
                    ARC_DETEC_FLAG=1;  //asn 22.05.2017

                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;
                }

                if(ARC_DETEC_FLAG==0)  //in upslope if err004 goto exit := asn 22.05.2017
                {
                    oper=exit1;
                    Error_no=4;
                    curerrf=1;

                }

                 if(TORCH==0)
                 {
                    oper=exit1;
                 }




               break;
            }
            case weld:
            {
                //GAS_RELAY_OUT=1;       //commented
                if(TORCH==0)
                {
                    if(TORCH==0)    //akshay S N 15.05.2017 this flag is used in  case : postflow
                    torchreleased =1;

                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
                    Down_Step=(float)((para.weld.I1 - MIN_WELD_I1)/(para.dnslope.time*10.0));
                    Downslope_cntr=para.dnslope.time*10;
////                    Downslope_cntr=para.dnslope.time*100.0;
////                    Down_Step=(float)((para.weld.I1 - minimum.weld.I1)/(Downslope_cntr));
                    pwmresult=para.weld.I1;
                    Downcurrent=para.weld.I1;
                    oper=downslope;
                    msec10_cntr=0;
                    downslopef=1;
                    dispcurrentf=0;
                    dispvtgf=0;
                    cseg_data[3] ='D'- 0x30;
                    cseg_data[4] ='W'- 0x30;
                    cseg_data[5] ='S'- 0x30;
                    cseg_data[6] ='P'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;  //sayali
                }
                else
                {
                    if(led_group4.bitsize.PULSE==1)
                    {
                      if(dutyon==1)
                        {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.BASE_CUR=1;
                            Update_PWM(para.weld.base);
                        }
                        else
                        {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.I1_MAIN=1;
                            Update_PWM(para.weld.I1);
                        }
                    }
                    else
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        Update_PWM(para.weld.I1);
                    }
                }

                if(ARC_DETEC_FLAG==0)
                {
                  oper=downslope;  //to go to postflow send control to downslope wit downslope flag==0;
                  Error_no=4;
                }
                break;
            }
            case downslope:
            {
                  if(TORCH==0)    //akshay S N 15.05.2017 this flag is used in  case : postflow
                  torchreleased=1;
                if((TORCH==1) && (torchreleased==1)  )  ////
                 {                    
//                    oper=exit1;
//--------------------//25122021--------------------------------                   
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.I1_MAIN=1;
                    Update_PWM(para.weld.I1);
                    arc_detection_count=0;  
                    ARC_DETEC_FLAG=1;  
                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;
                    msec1=0;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;                    
//-----------------------------------------------------                    
                 }
                else if(downslopef==1 &&  ARC_DETEC_FLAG==1)  // asn 22.05.2017
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
//                    //Update_PWM(pwmresult);
                    if(led_group4.bitsize.PULSE==1)
                    {
                        pulsef=1;
                        if(pwmresult>=para.weld.base)
                        {
                            (dutyon==1)?(Update_PWM(para.weld.base)):(Update_PWM(pwmresult));
                        }
                        else
                        {
                            Update_PWM(pwmresult);
                        }
                    }
                    else
                    {
                        Update_PWM(pwmresult);
                    }
                    oper=downslope;
                }
                else if (TORCH==0 || ARC_DETEC_FLAG==0 || TORCH==1)   //asn 22.05.2017 if torch released in down slope or arc diminished (No error 4 message)
                {
                    if(TORCH==0)
                    torchreleased=1;

                    downslopef=0;   //asn 22.05.2017
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.POSTFLOW=1;
                    Time_100ms=0;
                    msec1=0;
                    CCPR1L=0;           // PWM =0
                    CCP1CONbits.DC1B=0; //

                    oper=postflow;

                    OCVONLAT=1;  //ocv off
                    HF_RELAY_OUT=0;  //hf relay off


                    dispcurrentf=0;  //ASN 20.05.2017
                    dispvtgf=0;
                    cseg_data[3] ='P'- 0x30;
                    cseg_data[4] ='S'- 0x30;
                    cseg_data[5] ='T'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }
                break;
            }
            case end1:
            {


                break;
            }
            case postflow:
           {

                if(TORCH==0)
                 torchreleased=1;


               if( ( (Time_100ms >= para.postflow.time) || (TORCH==1) )&& (torchreleased==1)  )  ////
                 {
                    Time_100ms=0;
                    msec1=0;
                    oper=exit1;
                 }
                else if( (Time_100ms >= para.postflow.time) )  ////akshay S N 15.05.2017 spot
                {
                Time_100ms = para.postflow.time;
                CCPR1L=0;
                CCP1CONbits.DC1B=0;
                GAS_RELAY_OUT=0;
                HF_RELAY_OUT=0;
                 if(Error_no!=0)
                 display_error();
                }




                 break;
            }
            case exit1:
            {
                 //cseg_data[0] =0;
                 //cseg_data[1] =0;
                 //cseg_data[2] =0;
                
                 dp_on_flag1=1;
                
                 intochar_volt(Actual_Volt);

//
                if(curerrf!=0)
                 display_error();

                 exitprocess();
//           /*     if(TORCH==1)            //akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                    break;
//                }
//            */
//
//                //akshay S N 15.05.2017
                if(TORCH==1 && curerrf==0)  //if torch is presses continously and no current error
                {
                oper=pre;//end;
//                //break;  ////akshay S N 15.05.2017 do not break here so that all sequence parameters get loaded with ther default value
                }
                if(TORCH==1 && curerrf!=0) //if torch is presses continously and current error error was there then control will stuck here only still user releases the torch
                {
                oper=exit1;
                display_error();
                break;
                }

                Error_no=0;
                curerrf=0;
                exitwriteprocess();
                updatedisptig=1;
                break;
            }
//           /* default:
//            {
//
//
//                break;
//            }*/
     }//switch
    }//IF OF 2T WITH NORMAL END HERE
////************************************2T WITH NORMAL or PULSE END HERE****************************//
////***************************4T WITH NORMAL START HERE**********************************
    else if(led_group1.bitsize.FOURT==1 && (led_group4.bitsize.NORMAL==1 || led_group4.bitsize.PULSE==1 ))
    {
    switch(oper)
        {
            case pre:
            {
                if(Cyclestart==0)
                {
                    if(para.weld.base >= para.weld.I1 && led_group4.bitsize.PULSE==1)
                    {
                     para.weld.base = para.weld.I1;
                    }

                    preflow();
                }
//                //if((Time_100ms >=(para.preflow.time*10)) && OCVONf==0)
                if(Time_100ms >=para.preflow.time && OCVONf==0)
                {
                    Update_PWM(para.start.amp);   //minimum current for arc striking
                    OCVONf=0;  //1  akshay S N 15.05.2017
                    Time_100ms=0;
                    msec1=0;
                    Time_ms=0;
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.START_CUR=1;
//                   // oper=pre;     //commented thiss line akshay S N 15.05.2017
                    if(led_group4.bitsize.TIGHF==1)
                      HF_RELAY_OUT=1;
//
//            //added this block start: akshay S N 15.05.2017
                    cseg_data[3] ='S'- 0x30;
                    cseg_data[4] ='T'- 0x30;
                    cseg_data[5] ='R'- 0x30;
                    cseg_data[6] ='T'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;

                    oper=start1;  // uncommented thiss line akshay S N 15.05.2017
             //added this block end: akshay S N 15.05.2017
                }
//
////commented this block akshay S N 15.05.2017
//           /*  if((Time_100ms >=2) && OCVONf==1) //100msec=1 ocv on for 1 second
//                {
//                    Time_100ms=0;
//                    msec1=0;
//                    Time_ms=0;
//                    if(led_group4.bitsize.TIGHF==1)
//                        HF_RELAY_OUT=1;
//                    oper=start1;
//                    seg_data[0] ='S'- 0x30;
//                    seg_data[1] ='T'- 0x30;
//                    seg_data[2] ='R'- 0x30;
//                    seg_data[3] ='T'- 0x30;
//                    Disp_mesgf=1;
//                    Disp_mesg_cntr=0;
//                    dp_on_flag=0;
//                }    */
               break;
            }
            case start1:
            {
                if(Actual_Current >2)//(2*10)))
                {
                   if(Time_ms>=2 && Actual_Current > 2)
                   {
                        HF_RELAY_OUT=0;
                        Time_100ms=0;
                        msec1=0;
                        Update_PWM(para.start.amp);   //minimum current for arc striking
                        oper=start1;
                        ARC_ON_FLAG=1;
                        pwmresult=para.start.amp;  //ASN to avoid glitch in upslope
                        if(TORCH==0)
                        {
                           Time_100ms=0;
                           msec1=0;
                           Time_ms=0;
                           led_group2.all=0x00;
                           led_group3.all=0x00;
                           led_group2.bitsize.UPSLOPE=1;
                           Update_PWM(para.start.amp);   //minimum current
//                        //     UP_Step=(float)((para.weld.I1 - para.start.amp)/(para.upslope.time*10.0));
//                        //    Upslope_cntr=para.upslope.time*10;
//                        //    Upcurrent=para.start.amp;
//
//
                           if(para.weld.I1>=para.start.amp && para.upslope.time>0)
                           {
                            UP_Step=(float)((para.weld.I1 - para.start.amp)/(para.upslope.time*10.0));
                            Upslope_cntr=para.upslope.time*10;
                            Upcurrent=para.start.amp;
                           }
                               else
                           {
                              UP_Step=0;
                              Upcurrent=para.weld.I1;
                               Upslope_cntr=0;
                           }
//
//
//
                           msec10_cntr=0;
                           upslopef=1;
                           oper=upslope;
                           cseg_data[3] ='U'- 0x30;
                           cseg_data[4] ='P'- 0x30;
                           cseg_data[5] ='S'- 0x30;
                           cseg_data[6] ='P'- 0x30;
                           Disp_mesgf=1;
                           Disp_mesg_cntr=0;
                           dp_on_flag=0;
                           dp_on_flag1=0;
                        }
                    }
                }
//
                if(Time_100ms >= 20 && ARC_ON_FLAG==0)    // ASN if arc is not strike the HF will on for 2 sec and goto exit without postflow
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    curerrf=1;
                    oper=exit1;
                    Error_no=4;//akshay S N 15.05.2017
                }

                if(ARC_DETEC_FLAG==0 && ARC_ON_FLAG==1)  //ASN 22.05.2017 //if arc is striked but get vanished in 2 sec goto postflow
                {
                    oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
		    Error_no=4;
                }

                break;
            }
           case upslope:
            {
                if(upslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.UPSLOPE=1;
                    if(led_group4.bitsize.PULSE==1)
                   {
                        pulsef=1;
                        if(pwmresult>=para.weld.base)
                        {
                            (dutyon==1)?(Update_PWM(para.weld.base)):(Update_PWM(pwmresult));
                        }
                        else
                        {
                            Update_PWM(pwmresult);
                        }
                    }
                    else
                    {
                        Update_PWM(pwmresult);
                    }
                    oper=upslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    //led_group2.bitsize.PEAK_CUR=1;
                    //Update_PWM(para.weld.peak);   //set current
                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;
                }
//
                if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                {
                upslopef=0;
                oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
                Error_no=4;
                }
               break;
            }
            case weld:
            {
                 if(TORCH==0)
                 {
                   cWeld4TLatchfg = 0;
                 }
                if((TORCH==1)&&(cWeld4TLatchfg == 0))
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
                    pwmresult=para.weld.I1;
                    if(para.weld.I1>para.end.amp)
                    {
                    Down_Step=(float)((para.weld.I1 - para.end.amp)/(para.dnslope.time*10.0));
                    Downslope_cntr=para.dnslope.time*10;
                    Downcurrent=para.weld.I1;
                    }
                   else
                    {
                    Down_Step=0;
                    Downslope_cntr=0;
                    Downcurrent=para.end.amp;
                    }



                    oper=downslope;
                    msec10_cntr=0;
//                    //Send_Byte('8');
                    downslopef=1;
                    dispcurrentf=0;
                    dispvtgf=0;
                    cseg_data[3] ='D'- 0x30;
                    cseg_data[4] ='W'- 0x30;
                    cseg_data[5] ='S'- 0x30;
                    cseg_data[6] ='P'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }
                else
                {
                    if(led_group4.bitsize.PULSE==1)
                    {
                        if(dutyon==1)
                        {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.BASE_CUR=1;
                            Update_PWM(para.weld.base);
                        }
                        else
                        {
                            led_group2.all=0x00;
                           led_group3.all=0x00;
//                            led_group2.bitsize.PEAK_CUR=1;
//                           Update_PWM(para.weld.peak);
                            led_group2.bitsize.I1_MAIN=1;
                            Update_PWM(para.weld.I1);
                        }
                    }
                    else
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
//                        led_group2.bitsize.PEAK_CUR=1;
//                        Update_PWM(para.weld.peak);
                         led_group2.bitsize.I1_MAIN=1;
                         Update_PWM(para.weld.I1);
                    }


                }
//
//
                if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                {
                   oper= end1;
                    Error_no=4;

                }
//
//
                break;
            }
            case downslope:
            {
                if(TORCH==0)
                torchreleased=1;
                
                if((TORCH==1) && (torchreleased==1)  )  
                 {                    
//                    oper=exit1;
//-------------------------//25122021-------------------------------------                    
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.I1_MAIN=1;
                    Update_PWM(para.weld.I1);
                    arc_detection_count=0;  
                    ARC_DETEC_FLAG=1;  
                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;
                    msec1=0;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;  
                    torchreleased = 0;
                    cWeld4TLatchfg = 1;
//----------------------------------------------------------------
                 }
                else if(downslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
                    if(led_group4.bitsize.PULSE==1)
                    {
                        pulsef=1;
                        if(pwmresult>=para.weld.base)
                        {
                            (dutyon==1)?(Update_PWM(para.weld.base)):(Update_PWM(pwmresult));
                        }
                        else
                        {
                            Update_PWM(pwmresult);
                        }
                    }
                    else
                    {
                        Update_PWM(pwmresult);
                    }
                    oper=downslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
////                    led_group3.bitsize.POSTFLOW=1;
                    led_group3.bitsize.END_CUR=1;
                    Time_100ms=0;
                    msec1=0;
//                    //Update_PWM(0);   //set current
                    Update_PWM(para.end.amp);
                    oper=end1;
                    cseg_data[3] ='E'- 0x30;
                    cseg_data[4] ='N'- 0x30;
                    cseg_data[5] ='D'- 0x30;
                    cseg_data[6] ='A'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }

                 if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                {
                    oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
                    Error_no=4;
                    downslopef=0;
                 }

                break;
            }
            case end1:
            {
                if(TORCH==0 || Error_no!=0)
                {
                    if(TORCH==0)  //asn 22.05.2017
                    torchreleased=1;
//
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.POSTFLOW=1;
                    Time_100ms=0;
                    msec1=0;
//                    //Update_PWM(0);
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    oper=postflow;
//
                    OCVONLAT=1;  //ocv off
                    HF_RELAY_OUT=0;  //hf relay off
                    OCVONf=0;
                    pulsef=0;
                    upslopef=0;
                    downslopef=0;
//
//
                    cseg_data[3] ='P'- 0x30;
                    cseg_data[4] ='S'- 0x30;
                    cseg_data[5] ='T'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    Disp_mesgf=1;
                    dispcurrentf=0;
                    dispvtgf=0;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;  //sayali 17.07.18
                    break;// asn 22.05.2017
                }
//
                if(ARC_DETEC_FLAG==0 )  //ASN 22.05.2017 arc vanished in end current
                {
                 Error_no=4;  //goto to post flow
                }
                break;
            }
            case postflow:
            {
                if(TORCH==0)
                torchreleased=1;
//
                if( ( (Time_100ms >= para.postflow.time) || (TORCH==1) )&& (torchreleased==1)  )  //akshay S N 22.05.2017 4t normal
                {
                Time_100ms=0;
                msec1=0;
                oper=exit1;
                }
                else if( (Time_100ms >= para.postflow.time) )  ////akshay S N 15.05.2017
                {
               Time_100ms = para.postflow.time;
               CCPR1L=0;
                CCP1CONbits.DC1B=0;
                GAS_RELAY_OUT=0;
                HF_RELAY_OUT=0;
                if(Error_no!=0)
                display_error();
                }

            break;
            }
            case exit1:
            {
                cseg_data[3] =0;
                cseg_data[4] =0;
                cseg_data[5] =0;
                cseg_data[6] =0;

               if(curerrf!=0)
                 display_error();
//
                 exitprocess();
//           /*     if(TORCH==1)            //akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                    break;
//                }
//            */
//
//                //akshay S N 15.05.2017
                if(TORCH==1 && curerrf==0)  //if torch is presses continously and no current error
                {
                oper=pre;//end;
//                //break;  ////akshay S N 15.05.2017 do not break here so that all sequence parameters get loaded with ther default value
                }
                if(TORCH==1 && curerrf!=0) //if torch is presses continously and current error error was there then control will stuck here only still user releases the torch
                {
                oper=exit1;
                display_error();
                break;
                }
//
                Error_no=0;
                curerrf=0;
                exitwriteprocess();
                updatedisptig=1;
                break;
            }
//           ///* default:
//           // {
//
//
//            //    break;
//         //  }
     }//switch 4T
    }//if 4T
//    /*********************************spot********************************/
    else if(led_group1.bitsize.SPOT==1 && led_group4.bitsize.NORMAL==1)
    {
        switch(oper)
        {
            case pre:
            {
                if(Cyclestart==0)
               {

                    preflow();
                }
                if((Time_100ms >=para.preflow.time) && OCVONf==0)
                {
                   //Update_PWM(minimum.weld.I1);   //minimum current for arc striking commented by akshay S N 15.05.2017
//
                    if(para.upslope.time>0)       //akshay S N 15.05.2017  In 2t mode if upslope is zero then start with main current directly
                        Update_PWM(min_current); //15A
                        else
                            Update_PWM(para.weld.I1);
//
                    OCVONf=0; //1  by akshay S N 15.05.2017
                    Time_100ms=0;
                    msec1=0;
                    Time_ms=0;
//                   //  oper=pre;                 Commented by akshay S N 15.05.2017
                    if(led_group4.bitsize.TIGHF==1)
                    HF_RELAY_OUT=1;
//
                    oper=start1;
               }
//
//
//         /*      Commented by akshay S N 15.05.2017
//          
//                if((Time_100ms >=2) && OCVONf==1) //100msec=1 ocv on for 1 second
//                {
//                    Time_100ms=0;
//                    msec1=0;
//                    Time_ms=0;
//                    if(led_group4.bitsize.TIGHF==1)
//                        HF_RELAY_OUT=1;
//                    oper=start1;
//                }
//       */
                if(TORCH==0)
                {
                    oper=exit1;
                }
                break;
            }
            case start1:
            {
                if(Actual_Current > 2)
                {
                   if(Time_ms>=2 && Actual_Current > 2)
                   {
                       HF_RELAY_OUT=0;
                       Time_100ms=0;
                       msec1=0;
                       Time_ms=0;
                     //Update_PWM(minimum.weld.I1);
                       //minimum current  by Akshay S N
                       if(para.upslope.time>0)
                       {
                           led_group2.all=0x00;
                           led_group3.all=0x00;
                           led_group2.bitsize.UPSLOPE=1;
                           UP_Step=(float)((para.weld.I1 - MIN_WELD_I1)/(para.upslope.time*10.0));
                           Upslope_cntr=para.upslope.time*10;
                           Upcurrent=MIN_WELD_I1;
                           msec10_cntr=0;
                           upslopef=1;
                           oper=upslope;
                           cseg_data[3] ='U'- 0x30;
                           cseg_data[4] ='P'- 0x30;
                           cseg_data[5] ='S'- 0x30;
                           cseg_data[6] ='P'- 0x30;
                           Disp_mesgf=1;
                           Disp_mesg_cntr=0;
                           dp_on_flag=0;
                           dp_on_flag1=0; //sayali 17.07.18
                      }
                     else
                     {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        Update_PWM(para.weld.I1);   //set current
                        Time_100ms=0;
                        msec1=0;
                        oper=weld;
                       dispcurrentf=1;
                       dispvtgf=1;
                      }
                
                   }
                }
//
//
                if(Time_100ms >= 20)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    curerrf=1;
                    Error_no=4;
                    oper=exit1;
                }
//
                if(TORCH==0)
                {
                   oper=exit1;
                }
                break;
            }
           case upslope:
            {
                if(upslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.UPSLOPE=1;
                    Update_PWM(pwmresult);
                    oper=upslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.I1_MAIN=1;
                    Update_PWM(para.weld.I1);   //set current
                    Time_100ms=0;
                    msec1=0;
                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;

                }
//
//
                if(TORCH==0 || ARC_DETEC_FLAG==0)
               {
                    oper=exit1;  //akshay S N 15.05.2017 if torch relased in upslope goto postflow\

                    if(TORCH==0)
                    torchreleased_spot=1;

                    if(ARC_DETEC_FLAG==0)
                    {
                    curerrf=1;
                    Error_no=4;
                    }
                }
//
//
                break;
            }
            case weld:
            {
                //if(TORCH==0)
                if( Time_100ms >=para.spottime || TORCH==0 )  //prev: if(Time_100ms >=para.spottime) => akshay S N 15.05.2017
                {
                    if(TORCH==0)    //akshay S N 15.05.2017 this flag is used in  case : postflow
                    {
                    torchreleased_spot=1;
                     downslopef=0;
                    }
                    else
                         downslopef=1;
//
                    Time_100ms=0;
                    msec1=0;
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
//
                    Down_Step=(float)((para.weld.I1 - MIN_WELD_I1)/(para.dnslope.time*10.0));
                    Downslope_cntr=para.dnslope.time*10;
                    Downcurrent=para.weld.I1;
//
//
                     oper=downslope;
                    msec10_cntr=0;
//                //    downslopef=1;
                    dispcurrentf=0;
                    dispvtgf=0;
                    cseg_data[3] ='D'- 0x30;
                    cseg_data[4] ='W'- 0x30;
                    cseg_data[5] ='S'- 0x30;
                    cseg_data[6] ='P'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;   //sayali 17.07.18
                }
                else
                {
                    Update_PWM(para.weld.I1);   //set current
                }
//              /*  if(TORCH==0) //commented by akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                }
//               */
                if(ARC_DETEC_FLAG==0 ) // akshay S N 22.05.2017
                {
                    oper=downslope;
                    downslopef=0;  //asn 22.05.2017
                    Error_no=4;
                }
//
                break;
            }
            case downslope:
            {
                if(TORCH==0)    //akshay S N 15.05.2017 this flag is used in  case : postflow
                  torchreleased_spot=1;
//
//
                 if(ARC_DETEC_FLAG==0 ) // akshay S N 22.05.2017
                {
                    oper=downslope;
                    downslopef=0;  //asn 22.05.2017
                    Error_no=4;
                }
//
//
                if(downslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
                    Update_PWM(pwmresult);
                     oper=downslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.POSTFLOW=1;
                    Time_100ms=0;
                    msec1=0;
//                    //Update_PWM(0);   //set current
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    OCVONLAT=1;  //ocv off
                    HF_RELAY_OUT=0;  //hf relay off
//
                    oper=postflow;
                    cseg_data[3] ='P'- 0x30;
                    cseg_data[4] ='S'- 0x30;
                    cseg_data[5] ='T'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    Disp_mesgf=1;
                    dispcurrentf=0;
                    dispvtgf=0;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;  //sayali 17.7.18
                }
                break;
//
//
//
//
//
//
            }
            case end1:
            {
//
//
                break;
            }
            case postflow:
            {
                    if(TORCH==0)
                    torchreleased_spot=1;
//
//
//
                if( ( (Time_100ms >= para.postflow.time) || (TORCH==1) )&& (torchreleased_spot==1)  )  ////akshay S N 15.05.2017 spot
                 {
                    Time_100ms=0;
                    msec1=0;
                    oper=exit1;
                 }
                else if( (Time_100ms >= para.postflow.time) )  ////akshay S N 15.05.2017 spot
                {
                    Time_100ms = para.postflow.time;
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    GAS_RELAY_OUT=0;
                    HF_RELAY_OUT=0;
                    if(Error_no!=0)
                    display_error();
                }
//
                 break;
            }
            case exit1:
            {
                cseg_data[3] =0;
                cseg_data[4] =0;
                cseg_data[5] =0;
                cseg_data[6] =0;

                 if(curerrf!=0)
                 display_error();
//
                 exitprocess();
//           /*     if(TORCH==1)            //akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                    break;
//                }
//            */
//
//                //akshay S N 15.05.2017
                if(TORCH==1 && curerrf==0)  //if torch is presses continously and no current error
                {
                oper=pre;//end;
//                //break;  ////akshay S N 15.05.2017 do not break here so that all sequence parameters get loaded with ther default value
                }
                if(TORCH==1 && curerrf!=0) //if torch is presses continously and current error error was there then control will stuck here only still user releases the torch
                {
                oper=exit1;
                display_error();
                break;
                }
//
                Error_no=0;
                curerrf=0;
                exitwriteprocess();
                updatedisptig=1;
                break;
            }
        } //end switch
    } //end of else if of SPOT

//    /*******************************Cycle Normal ********************************************************/
   else if(led_group1.bitsize.CYCLE==1 && led_group4.bitsize.NORMAL==1)
    {
         switch(oper)
        {
            case pre:
            {
                if(Cyclestart==0)
                {
//                   /* led_group2.all=0x00;
//                    led_group3.all=0x00;
//                    led_group2.bitsize.PREFLOW=1;
//                    Cyclestart=1;
//                    Time_100ms=0;
//                    msec1=0;
//                    GAS_RELAY_OUT=1;
//                    oper=pre;
//                    dispcurrentf=0;
//                    seg_data[0] ='P'- 0x30;
//                    seg_data[1] ='R'- 0x30;
//                    seg_data[2] ='E'- 0x30;
//                    seg_data[3] ='F'- 0x30;
//                    Disp_mesgf=1;
//                    Disp_mesg_cntr=0;
//                    dp_on_flag=0;
//                    */
                    preflow();
                }
               //if((Time_100ms >=(para.preflow.time*10)) && OCVONf==0)
                if(Time_100ms >=para.preflow.time && OCVONf==0)
                {
                    Update_PWM(para.start.amp);   //minimum current for arc striking
                    OCVONf=0;  //1   by akshay S N
                    Time_100ms=0;
                    msec1=0;
                    Time_ms=0;
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.START_CUR=1;
//
//                    //oper=pre; commented by akshay S N
//
                    if(led_group4.bitsize.TIGHF==1)
                        HF_RELAY_OUT=1;
//
//                    /*Added by akshay S N*/
                    cseg_data[3] ='S'- 0x30;
                    cseg_data[4] ='T'- 0x30;
                    cseg_data[5] ='R'- 0x30;
                    cseg_data[6] ='T'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                    oper=start1;

                }

//    /*
//                if((Time_100ms >=2) && OCVONf==1) //100msec=1 ocv on for 1 second
//                {
//                    Time_100ms=0;
//                    msec1=0;
//                    Time_ms=0;
//                    if(led_group4.bitsize.TIGHF==1)
//                        HF_RELAY_OUT=1;
//                    oper=start1;
//                    seg_data[0] ='S'- 0x30;
//                    seg_data[1] ='T'- 0x30;
//                    seg_data[2] ='R'- 0x30;
//                    seg_data[3] ='T'- 0x30;
//                    Disp_mesgf=1;
//                    Disp_mesg_cntr=0;
//                    dp_on_flag=0;
//                }
//         */
               break;
            }
            case start1:
            {
                if(Actual_Current > 2)//(2*10)))
                {
                    if(Time_ms>=2 && Actual_Current > 2)
                    {
                        HF_RELAY_OUT=0;
                        Time_100ms=0;
                        msec1=0;
                        Update_PWM(para.start.amp);   //minimum current for arc striking
                        oper=start1;
                        ARC_ON_FLAG=1;
                        pwmresult=para.start.amp;  //ASN to avoid glitch in upslope
                        if(TORCH==0)
                          {
                           HF_RELAY_OUT=0;
                           Time_100ms=0;
                           msec1=0;
                           Time_ms=0;
                           led_group2.all=0x00;
                           led_group3.all=0x00;
                           led_group2.bitsize.UPSLOPE=1;
                           Update_PWM(para.start.amp);
//
//
//                        //   UP_Step=(float)((para.weld.I1 - para.start.amp)/(para.upslope.time*10.0));
//                      //     Upslope_cntr=para.upslope.time*10;
//                      //     Upcurrent=para.start.amp;
//
                            if(para.weld.I1>=para.start.amp && para.upslope.time>0)
                           {
                            UP_Step=(float)((para.weld.I1 - para.start.amp)/(para.upslope.time*10.0));
                            Upslope_cntr=para.upslope.time*10;
                            Upcurrent=para.start.amp;
                           }
                               else
                           {
                               UP_Step=0;
                               Upcurrent=para.weld.I1;
                               Upslope_cntr=0;
                           }
//
//
//
//
                           msec10_cntr=0;
                           upslopef=1;
                           oper=upslope;
                           cyclemodef=1;
                           cseg_data[3] ='U'- 0x30;
                           cseg_data[4] ='P'- 0x30;
                           cseg_data[5] ='S'- 0x30;
                           cseg_data[6] ='P'- 0x30;
                           Disp_mesgf=1;
                           Disp_mesg_cntr=0;
                           dp_on_flag=0;
                           dp_on_flag1=0;
                        }
                     }
                }
//
                if(Time_100ms >= 20 && ARC_ON_FLAG==0)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    Error_no=4;
                    oper=exit1;
                    curerrf=1;
                }
//
                if(ARC_DETEC_FLAG==0 && ARC_ON_FLAG==1)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    Error_no=4;
                    oper=end1;
                    curerrf=1;
               }
//
//
                break;
            }
            case upslope:
            {
                if(upslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.UPSLOPE=1;
                    Update_PWM(pwmresult);
                    oper=upslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.I1_MAIN=1;
                    Update_PWM(para.weld.I1);   //set current
                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;
                }
//
                if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                {
                oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
                Error_no=4;
                }
//
               break;
            }
            case weld:
            {
                if((TORCH==1)&&(torchpresd==0))
                {
                        torchpresd=1;
                        Time_100ms=0;
                        msec1=0;
//                        //cntsecond=0;
                }
                if(torchpresd==1)
                {
                        if(Time_100ms<=20)
                        {
                                if(TORCH==0)
                                {
                                       cyclecurf= !cyclecurf;
                                        torchpresd=0;
                                }
                        }
                        else
                        {
                                torchpresd=0;
                                Time_100ms=0;
                                msec1=0;
                                cyclecurf=0;
                                led_group2.all=0x00;
                                led_group3.all=0x00;
                                led_group3.bitsize.DOWNLSLOPE=1;
//
//                           //     Down_Step=(float)((cyle_mode_current - para.end.amp)/(para.dnslope.time*10.0));
//                          //      Downslope_cntr=para.dnslope.time*10;
//                          //      Downcurrent=cyle_mode_current;
                                pwmresult=cyle_mode_current;
                                if(cyle_mode_current>=para.end.amp)
                                {
                                Down_Step=(float)((cyle_mode_current - para.end.amp)/(para.dnslope.time*10.0));
                                Downslope_cntr=para.dnslope.time*10;
                                Downcurrent=cyle_mode_current;
                                }
                                else
                                {
                                Down_Step=0;
                                Downslope_cntr=0;
                                Downcurrent=para.end.amp;
                                }
//
//
//
//
                                oper=downslope;
                                msec10_cntr=0;
                                cyclemodef=0;
                                downslopef=1;
                                dispcurrentf=0;
                                dispvtgf=0;
                                cseg_data[3] ='D'- 0x30;  //downslope start here
                                cseg_data[4] ='W'- 0x30;
                                cseg_data[5] ='S'- 0x30;
                                cseg_data[6] ='P'- 0x30;
                                Disp_mesgf=1;
                                Disp_mesg_cntr=0;
                                dp_on_flag=0;
                                dp_on_flag1=0;
                       }
                }
                if(cyclemodef==1)
                {
                    if(cyclecurf==0)
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I1_MAIN=1;
                        cyle_mode_current=para.weld.I1;
                    }
                    else
                    {
                        led_group2.all=0x00;
                        led_group3.all=0x00;
                        led_group2.bitsize.I2_CYCLE=1;
                        cyle_mode_current=para.weld.I2;
                    }
                    Update_PWM(cyle_mode_current);
                }

                    if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                    {
                        oper= end1;
                        Error_no=4;
                    }

            break;
            }
            case downslope:
            {
                if(downslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
                    Update_PWM(pwmresult);
                    oper=downslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.END_CUR=1;
                    Time_100ms=0;
                    msec1=0;
                    Update_PWM(para.end.amp);
                    oper=end1;
                    cseg_data[3] ='E'- 0x30;
                    cseg_data[4] ='N'- 0x30;
                    cseg_data[5] ='D'- 0x30;
                    cseg_data[6] ='A'- 0x30;
                    Disp_mesgf=1;
                    dispcurrentf=0;  //asn 22.05.17
                    dispvtgf=0;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }


                    if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                    {
                    oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
                    Error_no=4;
                    }

                 break;

            }
            case end1:
            {

                    if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                    {
                    oper= end1;  //ASN 22.05.2017 for going to postflow cycle
                    Error_no=4;
                    }
//
                if(TORCH==0  || Error_no!=0)
                {
                    if(TORCH==0)
                       torchreleased=1;

                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.POSTFLOW=1;
                    Time_100ms=0;
                    msec1=0;
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    OCVONLAT=1;  //ocv off
                    HF_RELAY_OUT=0;  //hf relay off

                    oper=postflow;
                    cseg_data[3] ='P'- 0x30;
                    cseg_data[4] ='S'- 0x30;
                    cseg_data[5] ='T'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    Disp_mesgf=1;
	            dispcurrentf=0;  //asn 22.05.17
                    dispvtgf=0;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }




                break;
            }
            case postflow:
            {
               if(TORCH==0)
                    torchreleased=1;



                if( ( (Time_100ms >= para.postflow.time) || (TORCH==1) )&& (torchreleased ==1)  )  ////akshay S N 15.05.2017 spot
                 {
                    Time_100ms=0;
                    msec1=0;
                    oper=exit1;
                 }
                else if( (Time_100ms >= para.postflow.time) )  ////akshay S N 15.05.2017 spot
                {
                    Time_100ms = para.postflow.time;
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    GAS_RELAY_OUT=0;
                    HF_RELAY_OUT=0;

                    if(Error_no!=0)
                    display_error();
                }

                 break;
            }
            case exit1:
            {
                cseg_data[3] =0;
                cseg_data[4] =0;
                cseg_data[5] =0;
                cseg_data[6] =0;
//                /*CCPR1L=0;
//                CCP1CONbits.DC1B=0;
//                OCVONLAT=1;
//                GAS_RELAY_OUT=0;
//                HF_RELAY_OUT=0;
//                OCVONf=0;
//                pulsef=0;
//                Time_100ms=0;
//                msec1=0;
//                upslopef=0;
//                downslopef=0;
//                tempcnt=0;
//                Update_dispf=1;
//                dispcurrentf=0;
//                if(TORCH==1)
//                {
//                    oper=exit1;
//                    break;
//                }
//                led_group2.all=0x00;
//                led_group3.all=0x00;
//                led_group2.bitsize.I1_MAIN=1;
//                writemem();
//                Cyclestart=0;
//                 */
                  if(curerrf!=0)
                 display_error();

                 exitprocess();
//           /*     if(TORCH==1)            //akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                    break;
//                }
//            */
//
//                //akshay S N 15.05.2017
                if(TORCH==1 && curerrf==0)  //if torch is presses continously and no current error
                {
                oper=pre;//end;
//                //break;  ////akshay S N 15.05.2017 do not break here so that all sequence parameters get loaded with ther default value
                }
                if(TORCH==1 && curerrf!=0) //if torch is presses continously and current error error was there then control will stuck here only still user releases the torch
                {
                oper=exit1;
                display_error();
                break;
                }

                Error_no=0;
                curerrf=0;
                exitwriteprocess();
                updatedisptig=1;
                break;
            }
//
        }//switch
    }// end of CYCLE NORMAL
//    /***********************************Cycle Pulse Mode***************************************************************/
     else if(led_group1.bitsize.CYCLE==1 && led_group4.bitsize.PULSE==1 )
    {
        switch(oper)
        {
            case pre:
            {
                if(Cyclestart==0)
                {
                    if(para.weld.base >= para.weld.I1 && led_group4.bitsize.PULSE==1)
                    {
                        para.weld.base = para.weld.I1;
                    }
//                    /*led_group2.all=0x00;
//                    led_group3.all=0x00;
//                    led_group2.bitsize.PREFLOW=1;
//                    Cyclestart=1;
//                    Time_100ms=0;
//                    msec1=0;
//                    GAS_RELAY_OUT=1;
//                    oper=pre;
//                    dispcurrentf=0;
//                    seg_data[0] ='P'- 0x30;
//                    seg_data[1] ='R'- 0x30;
//                    seg_data[2] ='E'- 0x30;
//                    seg_data[3] ='F'- 0x30;
//                    Disp_mesgf=1;
//                    Disp_mesg_cntr=0;
//                    dp_on_flag=0;
//                     */
                    preflow();
                }
                if(Time_100ms >=para.preflow.time && OCVONf==0)
                {
                    Update_PWM(para.start.amp);   //minimum current for arc striking
                    OCVONf=1;
                    Time_100ms=0;
                    msec1=0;
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.START_CUR=1;
                    oper=pre;
                    if(led_group4.bitsize.TIGHF==1)
                        HF_RELAY_OUT=1;
                }
                if((Time_100ms >=5) && OCVONf==1) //100msec=1 ocv on for 1 second
                {
                    Time_100ms=0;
                    msec1=0;
                    Time_ms=0;
                    if(led_group4.bitsize.TIGHF==1)
                        HF_RELAY_OUT=1;
                    oper=start1;
                    cseg_data[3] ='S'- 0x30;
                    cseg_data[4] ='T'- 0x30;
                    cseg_data[5] ='R'- 0x30;
                    cseg_data[6] ='T'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
               }
               break;
            }
            case start1:
            {
                if(Actual_Current > 2)
                {
                    if(Time_ms>=2 && Actual_Current > 2)
                    {
                        HF_RELAY_OUT=0;
                        Update_PWM(para.start.amp);   //minimum current for arc striking
                        Time_100ms=0;
                        msec1=0;
                        oper=start1;
						ARC_ON_FLAG=1;

                            pwmresult=para.start.amp;  //ASN to avoid glitch in upslope
                        if(TORCH==0)
                        {
                           HF_RELAY_OUT=0;
                           Time_100ms=0;
                           msec1=0;
                           Time_ms=0;
                           led_group2.all=0x00;
                           led_group3.all=0x00;
                           led_group2.bitsize.UPSLOPE=1;
                           Update_PWM(para.start.amp);   //minimum current
//
//
//                          //UP_Step=(float)((para.weld.I1 - para.start.amp)/(para.upslope.time*10.0));
//                         //  Upslope_cntr=para.upslope.time*10;
//                        //   Upcurrent=para.start.amp;
//
                            if(para.weld.I1>=para.start.amp && para.upslope.time>0)
                           {
                            UP_Step=(float)((para.weld.I1 - para.start.amp)/(para.upslope.time*10.0));
                            Upslope_cntr=para.upslope.time*10;
                            Upcurrent=para.start.amp;
                           }
                            else
                           {
                               UP_Step=0;
                               Upcurrent=para.weld.I1;
                               Upslope_cntr=0;
                           }

                           msec10_cntr=0;
                           upslopef=1;
                           oper=upslope;
                           cyclemodef=1;
                           cseg_data[3] ='U'- 0x30;
                           cseg_data[4] ='P'- 0x30;
                           cseg_data[5] ='S'- 0x30;
                           cseg_data[6] ='P'- 0x30;
                           Disp_mesgf=1;
                           Disp_mesg_cntr=0;
                           dp_on_flag=0;
                           dp_on_flag1=0;
                        }
                    }

                }

                if(Time_100ms >= 20 && ARC_ON_FLAG==0)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    Error_no=4;
                    oper=exit1;
                    curerrf=1;
                }

                if(ARC_DETEC_FLAG==0 && ARC_ON_FLAG==1)
                {
                    HF_RELAY_OUT=0;
                    Time_100ms=0;
                    msec1=0;
                    Error_no=4;
                    oper=end1;
                    curerrf=1;
               }
                break;
            }
           case upslope:
            {
                if(upslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group2.bitsize.UPSLOPE=1;
                    pulsef=1;
                    if(pwmresult>=para.weld.base)
                    {
                        (dutyon==1)?(Update_PWM(para.weld.base)):(Update_PWM(pwmresult));
                    }
                    else
                    {
                        Update_PWM(pwmresult);
                    }
                    oper=upslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    oper=weld;
                    dispcurrentf=1;
                    dispvtgf=1;
                }

				if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                {
                oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
                Error_no=4;
                }

               break;
            }
            case weld:
            {
                if((TORCH==1)&&(torchpresd==0))
                {
                        torchpresd=1;
                        Time_100ms=0;
                        msec1=0;
                }
                if(torchpresd==1)
                {
                        if(Time_100ms<=20)
                        {
                                if(TORCH==0)
                                {
                                        cyclecurf= !cyclecurf;
                                        torchpresd=0;
                                }
                        }
                        else
                        {
                                torchpresd=0;
                                Time_100ms=0;
                                msec1=0;
                                cyclecurf=0;
                                led_group2.all=0x00;
                                led_group3.all=0x00;
                                led_group3.bitsize.DOWNLSLOPE=1;
//
//                            //    Down_Step=(float)((cyle_mode_current - para.end.amp)/(para.dnslope.time*10.0));
//                           //     Downslope_cntr=para.dnslope.time*10;
//                           //     Downcurrent=cyle_mode_current;
//
                                if(cyle_mode_current>=para.end.amp)
                                {
                                Down_Step=(float)((cyle_mode_current - para.end.amp)/(para.dnslope.time*10.0));
                                Downslope_cntr=para.dnslope.time*10;
                                Downcurrent=cyle_mode_current;
                                }
                                else
                                {
                                Down_Step=0;
                                Downslope_cntr=0;
                                Downcurrent=para.end.amp;
                                }

                                oper=downslope;
                                msec10_cntr=0;
                                cyclemodef=0;
                                downslopef=1;
                                dispcurrentf=0;
                                dispvtgf=0;
                                cseg_data[3] ='D'- 0x30;
                                cseg_data[4] ='W'- 0x30;
                                cseg_data[5] ='S'- 0x30;
                                cseg_data[6] ='P'- 0x30;
                                Disp_mesgf=1;
                                Disp_mesg_cntr=0;
                                dp_on_flag=0;
                                dp_on_flag1=0;
                       }
                }
                if(cyclemodef==1)
                {
                    if(cyclecurf==0)
                    {
                            if(dutyon==1)
                            {
                                led_group2.all=0x00;
                                led_group3.all=0x00;
                                led_group2.bitsize.BASE_CUR=1;
                                cyle_mode_current=para.weld.base;
                            }
                            else
                           {
                                led_group2.all=0x00;
                                led_group3.all=0x00;
                                led_group2.bitsize.I1_MAIN=1;
                                cyle_mode_current=para.weld.I1;
                            }
                    }
                    else
                    {
                            led_group2.all=0x00;
                            led_group3.all=0x00;
                            led_group2.bitsize.I2_CYCLE=1;
                            cyle_mode_current=para.weld.I2;
                    }
                    Update_PWM(cyle_mode_current);
                }

                    if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                    {
                        oper= end1;
                        Error_no=4;
                    }
                break;
            }
            
            case downslope:
            {
                if(downslopef==1)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.DOWNLSLOPE=1;
                    pulsef=1;
                    if(pwmresult>=para.weld.base)
                    {
                        (dutyon==1)?(Update_PWM(para.weld.base)):(Update_PWM(pwmresult));
                    }
                    else
                    {
                        Update_PWM(pwmresult);
                    }
                    oper=downslope;
                }
                else
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.END_CUR=1;
                    Time_100ms=0;
                    msec1=0;
                    Update_PWM(para.end.amp);
                    oper=end1;
                    cseg_data[3] ='E'- 0x30;
                    cseg_data[4] ='N'- 0x30;
                    cseg_data[5] ='D'- 0x30;
                    cseg_data[6] ='A'- 0x30;
                    Disp_mesgf=1;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }

				if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
				{
				oper= end1;//exit1; //ASN 22.05.2017 for going to postflow cycle
				Error_no=4;
				}



                break;
            }
            case end1:
            {

                if(ARC_DETEC_FLAG==0)  //ASN 22.05.2017
                {
                oper= end1;  //ASN 22.05.2017 for going to postflow cycle
                Error_no=4;
                }

                 if(TORCH==0  || Error_no!=0)
                {
                    led_group2.all=0x00;
                    led_group3.all=0x00;
                    led_group3.bitsize.POSTFLOW=1;
                    Time_100ms=0;
                    msec1=0;
//                    //Update_PWM(0);
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    OCVONLAT=1;  //ocv off
                    HF_RELAY_OUT=0;  //hf relay off

                    oper=postflow;
                    cseg_data[3] ='P'- 0x30;
                    cseg_data[4] ='S'- 0x30;
                    cseg_data[5] ='T'- 0x30;
                    cseg_data[6] ='F'- 0x30;
                    Disp_mesgf=1;
                    dispcurrentf=0;
                    dispvtgf=0;
                    Disp_mesg_cntr=0;
                    dp_on_flag=0;
                    dp_on_flag1=0;
                }
                break;
            }
            case postflow:
            {
			if(TORCH==0)
				torchreleased=1;

            if( ( (Time_100ms >= para.postflow.time) || (TORCH==1) )&& (torchreleased ==1)  )  ////akshay S N 15.05.2017 spot
				{
				Time_100ms=0;
				msec1=0;
				oper=exit1;
				}
                else if( (Time_100ms >= para.postflow.time) )  ////akshay S N 15.05.2017 spot
                {
                    Time_100ms = para.postflow.time;
                    CCPR1L=0;
                    CCP1CONbits.DC1B=0;
                    GAS_RELAY_OUT=0;
                    HF_RELAY_OUT=0;

                    if(Error_no!=0)
                    display_error();
                }

                 break;
            }
            case exit1:
            {
//                /*CCPR1L=0;
//                CCP1CONbits.DC1B=0;
//                OCVONLAT=1;
//                GAS_RELAY_OUT=0;
//                HF_RELAY_OUT=0;
//                OCVONf=0;
//                pulsef=0;
//                Time_100ms=0;
//                msec1=0;
//                upslopef=0;
//                downslopef=0;
//                tempcnt=0;
//                dispcurrentf=0;
//                Update_dispf=1;
//                if(TORCH==1)
//                {
//                    oper=exit1;
//                    break;
//                }
//                led_group2.all=0x00;
//                led_group3.all=0x00;
//                led_group2.bitsize.I1_MAIN=1;
//                writemem();
//                Cyclestart=0;
//                 */
                  if(curerrf!=0)
                 display_error();
//
                 exitprocess();
//           /*     if(TORCH==1)            //akshay S N 15.05.2017
//                {
//                    oper=exit1;
//                    break;
//                }
//            */
//
//                //akshay S N 15.05.2017
                if(TORCH==1 && curerrf==0)  //if torch is presses continously and no current error
                {
                oper=pre;//end;
//                //break;  ////akshay S N 15.05.2017 do not break here so that all sequence parameters get loaded with ther default value
                }
                if(TORCH==1 && curerrf!=0) //if torch is presses continously and current error error was there then control will stuck here only still user releases the torch
                {
                oper=exit1;
                display_error();
                break;
                }

                Error_no=0;
                curerrf=0;
                exitwriteprocess();
                updatedisptig=1;
                break;
            }
//           ///* default:
//           // {
//
//
//            //    break;
//         //  }
     }//switch
    }//end of else if CYCLE PULSE
}//tig process

void exitprocess()
{

    CCPR1L=0;
    CCP1CONbits.DC1B=0;
    OCVONLAT=1;
    GAS_RELAY_OUT=0;
    delay();
    /*if(GAS_RELAY_OUT==0)
    {
        
    cseg_data[3] ='R'- 0x30;
    delay();
    }*/
    HF_RELAY_OUT=0;
    OCVONf=0;
    pulsef=0;
    Time_100ms=0;
    msec1=0;
    upslopef=0;
    downslopef=0;
    tempcnt=0;
    dispcurrentf=0;
    dispvtgf=0;
    Update_dispf=1;

    /*if(TORCH==1)
    {
        oper=exit1;
        break;
    }*/

}
void exitwriteprocess()
{
    led_group2.all=0x00;
    led_group3.all=0x00;
    led_group2.bitsize.I1_MAIN=1;
    writemem();
    Cyclestart=0;
}
void preflow()
{
    led_group2.all=0x00;
    led_group3.all=0x00;
    led_group2.bitsize.PREFLOW=1;
    Cyclestart=1;
    Time_100ms=0;
    msec1=0;
    curerrf=0;

    pwmresult=0;
    torchpresd_spot=0;
    torchreleased_spot=0;
    torchpresd =0;
    torchreleased =0;
    Error_no=0;
    ARC_ON_FLAG=0;

    GAS_RELAY_OUT=1;
    oper=pre;
    dispcurrentf=0;
    dispvtgf=0;
    dp_on_flag=0;
    dp_on_flag1=0;
    cseg_data[3] ='P'- 0x30;
    cseg_data[4] ='R'- 0x30;
    cseg_data[5] ='E'- 0x30;
    cseg_data[6] ='F'- 0x30;
    Disp_mesgf=1;
    Disp_mesg_cntr=0;
    
}
void Update_TIG_Parameters(unsigned char enc_result1)
{
    if(calibrationmode==1)
    {
        para.pwmslope=Update_Data(para.pwmslope,MIN_PWM_SLOPE,MAX_PWM_SLOPE,enc_result1);
        slope=(para.pwmslope/1000.0);
        if(Disp_mesgf==0)
        {
           inttochar(para.pwmslope);
           dp_on_flag=0;
        }
    }
    else if(tigwelding==1)
    {
        para.weld.I1=Update_Data(para.weld.I1,mincurrent,maxcurrent,enc_result1);
    }
    else
    {
        if(led_group2.bitsize.PREFLOW==1)
        {
            para.preflow.time=Update_Data(para.preflow.time,MIN_PREFLOW,MAX_PREFLOW,enc_result1);
            if(Disp_mesgf==0)
            {
               inttochar(para.preflow.time);
               dp_on_flag=1;
               dp_on_flag1=0;
            }
        }
        else if(led_group2.bitsize.START_CUR==1)
        {
            //para.start.amp=Update_Data(para.start.amp,minimum.start.amp,maximum.start.amp,enc_result1);
            para.start.amp=Fast_Update_Data(para.start.amp,mincurrent,maxcurrent,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.start.amp);
                dp_on_flag=0;
            }
        }
        else if( led_group2.bitsize.UPSLOPE==1)
        {
            para.upslope.time=Update_Data(para.upslope.time,MIN_UPSLOPE_T,MAX_UPSLOPE_T,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.upslope.time);
                dp_on_flag=1;
                dp_on_flag1=0;
            }
        }
        else if(( led_group2.bitsize.I1_MAIN==1) && (FOOT_SW==1))
        {
            //para.weld.I1=Update_Data(para.weld.I1,minimum.weld.I1,maximum.weld.I1,enc_result1);
            para.weld.I1=Fast_Update_Data(para.weld.I1,mincurrent,maxcurrent,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.weld.I1);
                dp_on_flag=0;
            }
           // Ilsb_msb_byte=para.weld.I1;
           // transmit_to_datalogger=1;
        }
        else if(led_group2.bitsize.I2_CYCLE==1)
        {
            //para.weld.I2=Update_Data(para.weld.I2,minimum.weld.I2,maximum.weld.I2,enc_result1);
            para.weld.I2=Fast_Update_Data(para.weld.I2,mincurrent,maxcurrent,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.weld.I2);
                dp_on_flag=0;
            }
        }
        else if(led_group2.bitsize.BASE_CUR==1)
        {
            //para.weld.base=Update_Data(para.weld.base,minimum.weld.base,maximum.weld.base,enc_result1);
            if(para.weld.base >= para.weld.I1)
            {
                para.weld.base = para.weld.I1;
            }
            para.weld.base=Fast_Update_Data(para.weld.base,mincurrent,para.weld.I1,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.weld.base);
                dp_on_flag=0;
            }
        }
        else if( led_group2.bitsize.PEAK_CUR==1)
        {
            //para.weld.peak=Update_Data(para.weld.peak,minimum.weld.peak,maximum.weld.peak,enc_result1);
            para.weld.peak=Fast_Update_Data(para.weld.peak,mincurrent,maxcurrent,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.weld.peak);
                dp_on_flag=0;
            }
            //data1=para.weld.peak;
            //writemem();
        }
        else if( led_group2.bitsize.PULSE_FREQ==1)
        {
            para.weld.freq=Fast_Update_Data(para.weld.freq,MIN_WELD_FREQ,MAX_WELD_FREQ,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.weld.freq);
                dp_on_flag=0;
            }
        }
        else if( led_group3.bitsize.DUTY ==1)
        {
            if(led_group1.bitsize.SPOT==1)
            {
                para.spottime=Update_Data(para.spottime,MIN_SPOT_TIME,MAX_SPOT_TIME,enc_result1);
                if(Disp_mesgf==0)
                {
                    inttochar(para.spottime);
                    dp_on_flag=1;
                    dp_on_flag1=0;
                }
            }
            else
            {
                para.weld.duty=Update_Data(para.weld.duty,MIN_WELD_DUTY,MAX_WELD_DUTY,enc_result1);
                if(Disp_mesgf==0)
                {
                    inttochar(para.weld.duty);
                    dp_on_flag=0;
                }
            }
        }
        else if( led_group3.bitsize.DOWNLSLOPE==1)
        {
            para.dnslope.time=Update_Data(para.dnslope.time,MIN_DOWN_SLOPE_T,MAX_DOWN_SLOPE_T,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.dnslope.time);
                dp_on_flag=1;
                dp_on_flag1=0;
            }
        }
        else if( led_group3.bitsize.END_CUR==1)
        {
            //para.end.amp=Update_Data(para.end.amp,minimum.end.amp,maximum.end.amp,enc_result1);
            para.end.amp=Fast_Update_Data(para.end.amp,mincurrent,maxcurrent,enc_result1);
            if(Disp_mesgf==0)
            {
               inttochar(para.end.amp);
               dp_on_flag=0;
            }
        }
        else if( led_group3.bitsize.POSTFLOW==1)
        {
            para.postflow.time=Update_Data(para.postflow.time,MIN_POSTFLOW_T,MAX_POSTFLOW_T,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.postflow.time);
                dp_on_flag=1;
                dp_on_flag1=0;
            }
        }
    }
    //writemem();
}
unsigned int Update_Data(unsigned int operator,unsigned int mini,unsigned int maxi,unsigned char sign)
{
    if(sign=='+')
    {
       operator++;//plusoperator;
       if(operator>=maxi)
       {
          operator=maxi;
       }
    }
    if(sign=='-')
    {
       if(operator >0)//(operator >= minusoperator)  necessary for negative side lock
          operator--;//minusoperator;
       if(operator <= mini)
          operator=mini;
    }
    return(operator);
}
unsigned int Fast_Update_Data(unsigned int operator,unsigned int mini,unsigned int maxi,unsigned char sign)
{
    if(sign=='+')
    {
       //operator++;//plusoperator;
        operator=operator+plusoperator;
       if(operator>=maxi)
       {
          operator=maxi;
       }
    }
    if(sign=='-')
    {
       if(operator >= minusoperator)
          //operator--;//minusoperator;
           operator=operator-minusoperator;
       if(operator <= mini)
          operator=mini;
    }
    return(operator);

}
void display_error()
 {

     for(j2=0;j2<100;j2++);
   switch(Error_no)
        {
            case 4:
            {
            cseg_data[3] ='E'- 0x30;
            cseg_data[4] ='R'- 0x30;
            cseg_data[5] ='0'- 0x30;
            cseg_data[6] ='4'- 0x30;
            break;
            }

       }
    Disp_mesgf=1;
    Disp_mesg_cntr=0;
    dp_on_flag=0;

 }