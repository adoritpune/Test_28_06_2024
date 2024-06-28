void weld_mma_process()
{
    intochar_volt(Actual_Volt);
    dp_on_flag1=1;
    if((Actual_Current>10)&&(cVRD_fg==0))
    {
       // weldonfg =1;  // 16-02-24
        mmaweldon=1;                                    //Flag to save the MMA CURRENT in memory after welding & for updatting parameter
        //weldon_byte='O';
        //Ilsb_msb_byte=Actual_Current;
        
        if((MODE_SW==1)&&(TIG_SW==1))
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
        if(calibrationmode==1)
        {
            enc_result=scanencoder();
            //if(enc_result=='+' || enc_result=='-' || Update_dispf==1)
            {
                Update_MMA_Parameters(enc_result);
                enc_result=0;
                Update_dispf=0;
            }
        }
        if((MENU_SW==1) && (calibrationmode==1))
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

        /*if((led_group1.bitsize.ARCFORCEON==1) && Actual_Volt<300.0 && para.mmacurrent<=200)
        {
            arcf_current=(unsigned int)(para.mmacurrent+(((300.0-Actual_Volt)/300.0)*(para.arcforce/100.0)*para.mmacurrent));
            Update_PWM(arcf_current);
        }
        else*/
        if(enc_result=='+' || enc_result=='-' || Update_dispf==1)
        {
            Update_MMA_Parameters(enc_result);
            enc_result=0;
            Update_dispf=0;
        }
        if((hotstarttime<=2000) && (hotstartonf==0)&&(para.mmacurrent<=200))
        {
            Update_PWM(mmacurrent1);
        }
        else
        {
            hotstartonf=1;
            Update_PWM(para.mmacurrent);
        }

        if(updatecurcnt>=100 && calibrationmode==0)
        {
            updatecurcnt=0;
            inttochar(Actual_Current);
            dp_on_flag=0;
        }
       Update_dispf=1;

    }
    else  //WELD OFF
    {
        //weldonfg =0; // 16-02-24
        //weldon_byte='F';
        if((mmaweldon==1)|| ((writememf==1) && (writememcnt>=100) && (donotwrite==0)))
        {
            mmaweldon=0;
            writememf=0;
            writememcnt=0;
            writemem();
            cVRD_fg=1;VRDTimecnt_2s=0; //OCVONLAT=0;// 16-02-24
        }
        keydetect();
        hotstarttime=0;
        hotstartonf=0;
        //enc_result=scanencoder();
        if(enc_result=='+' || enc_result=='-' || Update_dispf==1)
        {
            Update_MMA_Parameters(enc_result);
            writememf=1;
            writememcnt=0;
            enc_result=0;
            Update_dispf=0;
        }
        updatedisptig=1;
        
        if(cVRD_fg<=1)
        {
            if(para.mmacurrent<=200)            //21-01-14
            {
                mmacurrent1=para.mmacurrent+((para.mmacurrent*HOTSTART)/100.0);
                Update_PWM(mmacurrent1);
            }
            else
            {
               Update_PWM(para.mmacurrent);
            }
        }
    }
}

void Update_MMA_Parameters(unsigned char enc_result1)
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
    else
    {
        if(mmaweldon==1)
        {
            if( led_group2.bitsize.I1_MAIN==1)
            {
                para.mmacurrent=Update_Data(para.mmacurrent,MIN_MMA_CURRENT,maxmmacurrent,enc_result1);
                /*if(Disp_mesgf==0)
                {
                    inttochar(para.mmacurrent);
                    dp_on_flag=0;
                }*/
            }
        }
        else
        {
            if( led_group2.bitsize.I1_MAIN==1)
            {
                para.mmacurrent=Fast_Update_Data(para.mmacurrent,MIN_MMA_CURRENT,maxmmacurrent,enc_result1);
                if(Disp_mesgf==0)
                {
                    inttochar(para.mmacurrent);
                    dp_on_flag=0;
                }
                //Ilsb_msb_byte=para.mmacurrent;
                //transmit_to_datalogger=1;
            }
        }
       /* if(arcforceflag==1 && led_group1.bitsize.ARCFORCEON==1)
        {
            para.arcforce=Update_Data(para.arcforce,minimum.arcforce,maximum.arcforce,enc_result1);
            if(Disp_mesgf==0)
            {
                inttochar(para.arcforce);
                dp_on_flag=0;
            }
        }*/
    }
}
