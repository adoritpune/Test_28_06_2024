unsigned char encoder_port(void)
{
    unsigned char portvalue=0x00,status;
    //TRISA=0x33;
    ADCON1bits.PVCFG=0X00;  //FOR 46K22
    ADCON1bits.NVCFG=0X00;    //FOR 46K22 ANALOG PINS SELECTED AT ADC_INIT
    portvalue=ENCODER;
    portvalue=(portvalue & 0x30);
    portvalue=(portvalue >> 4);
    new=portvalue;
    temp=new;
    if((temp ^ old) !=0)
        status=1;
    else
        status=0;
    old=new;
    return(status);
}

unsigned char scanencoder(void)
{
    unsigned char result=0;
    if(encoder_port()== 1 && modelflag==0)  //not in model sel mode
    {
        if(fpulse==0 && spulse==0)
        {
            if(new==0x02)    //10
            {
                fpulse=1;
                spulse=0;
                plus=1;
                result='$';
            }
            if(new==0x01)     //01
            {
                fpulse=1;
                spulse=0;
                minus=1;
                result='$';
            }
        }
        else if(fpulse==1 && spulse==0)
        {
            if(new==0x03) //&& firstpulse1==0)//03
           {
              fpulse=1;
              spulse=1;
              result='$';
           }
        }
        else if(fpulse==1 && spulse==1)
        {
            if(new==0x01 && plus==1)// && firstpulse1==0)//03
            {
                fpulse=0;
                plus=0;
                result='+';
            }
            if(new==0x02 && minus==1)// && firstpulse1==0)//03
            {
                fpulse=0;
                minus=0;
                result='-';
            }
            fpulse=0;
            spulse=0;
        }
        else
        {
            plus=0;
            minus=0;
            fpulse=0;
            spulse=0;
            result='$';
        }
    }
    else
   {
      result='$';
   }
 return(result);
}




