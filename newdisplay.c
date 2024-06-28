void refresh_disp1()
{
seg_count=9;

  if(seg_count==9)
    {
                //clear_all();
                LATBbits.LATB4=0;
                DISP2=0;
                DISP3=0;
                DISP1=0;
                decodeA=1;  //msb
                decodeB=1;
                decodeC=0;  //lsb
                LATBbits.LATB4=1;

                latchdata(led_group3.all);

                  seg_count++;
    }
}


void refresh_disp()
{


    if(seg_count==0)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0; 
                decodeA=0;  //msb
                decodeB=0;
                decodeC=0;  //lsb
                latchdata(cseg_data[3]);
                LATBbits.LATB4=1;
                seg_count++;


    }
    else if(seg_count==1)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0;
                decodeA=0;  //msb
                decodeB=0;
                decodeC=1;  //lsb
                latchdata(cseg_data[4]);
                LATBbits.LATB4=1;
                seg_count++;
    }
    else if(seg_count==2)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0;
                decodeA=0;  //msb
                decodeB=1;
                decodeC=0;  //lsb
                if(dp_on_flag==1)
                {
                arr[7]=1;//0;

                }
                latchdata(cseg_data[5]);
                LATBbits.LATB4=1;
                seg_count++;
                arr[7]=0;//0;

    }
    else if(seg_count==3)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0;
                decodeA=0;  //msb
                decodeB=1;
                decodeC=1;  //lsb
                latchdata(cseg_data[6]);
                LATBbits.LATB4=1;
                seg_count++;
                 //PORTBbits.RB4=0;

    }
    else if(seg_count==4)
    {

                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=1;
                DISP1=0;
                DISP4=0;

                //decodeA=0;//msb
                //decodeB=0;
                //decodeC=0; //lsb
                latchdata(cseg_data[0]);


                //LATBbits.LATB4=1;
                  seg_count++;
    }
    else if(seg_count==5)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP3=0;
                DISP1=0;
                DISP4=0; 
                DISP2=1;
                //PORTBbits.RB4=0;
                //decodeA=0;  //msb
                //decodeB=0;
                //decodeC=0;  //lsb
                if(dp_on_flag1==1)
                {
                arr[7]=1;//0;

                }
                else
                {
                arr[7]=0;
                }
                latchdata(cseg_data[1]);
                //LATBbits.LATB4=1;
                  seg_count++;
                  arr[7]=0;//0;



    }
    else if(seg_count==6)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP4=0;
                DISP1=1;
                //PORTBbits.RB4=0;
                //decodeA=0;  //msb
                //decodeB=0;
                //decodeC=0;  //lsb
                latchdata(cseg_data[2]);
                //LATBbits.LATB4=1;
                seg_count++;

    }
    else if(seg_count==7)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP1=0;
                DISP3=0;
                DISP2=0;
                DISP4=1;
                //PORTBbits.RB4=0;
                //decodeA=0;  //msb
                //decodeB=0;
                //decodeC=0;  //lsb
                latchdata(cseg_data[7]);
                //LATBbits.LATB4=1;
                seg_count++;

    }
    else if(seg_count==8)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0; 
                decodeA=1;  //msb
                decodeB=0;
                decodeC=0;  //lsb
                latchdata(led_group1.all);
                LATBbits.LATB4=1;
                  seg_count++;
    }
    else if(seg_count==9)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0;
                decodeA=1;  //msb
                decodeB=0;
                decodeC=1;  //lsb
                latchdata(led_group2.all);
                LATBbits.LATB4=1;
                seg_count++;
    }

    else if(seg_count==10)
    {
                LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0;
                decodeA=1;  //msb
                decodeB=1;
                decodeC=0;  //lsb
                latchdata(led_group3.all);
                LATBbits.LATB4=1;
                  seg_count++;
    }

    else if(seg_count==11)
    {
               LATBbits.LATB4=0;
                clear_all();
                DISP2=0;
                DISP3=0;
                DISP1=0;
                DISP4=0; 
                decodeA=1;  //msb
                decodeB=1;
                decodeC=1;  //lsb
                latchdata(led_group4.all);
                LATBbits.LATB4=1;
                  seg_count++;
    }
     else if(seg_count==12)
     {
                decodeA=0;  //msb
                decodeB=0;
                decodeC=0;  //lsb
                LATBbits.LATB4=0;
                TRISD=0x01;              //pin D0 input pin
                for(j2=0;j2<5;j2++);     //SURE ABOUT NECESSITY, delay for pin to become an input pin
                if(PROG_KEY==1)         //program key status, pressed(1)
                {
                   for(j2=0;j2<40;j2++);
                   if(PROG_KEY==1)
                   {
                   PROG_SW=1;            //flag
                   }
                   else
                   {
                   PROG_SW=0;
                   }
                }
                else
                {
                    PROG_SW=0;
                }

                  seg_count++;
      }
     else if(seg_count == 13)
     {
         TRISD=0x02;
         for(j2=0;j2<5;j2++);
         if(ENCODER_KEY==1)
         {
            for(j2=0;j2<40;j2++);
            if(ENCODER_KEY==1)
               ENCODER_SW=1;
            else
               ENCODER_SW=0;
         }
         else
             ENCODER_SW=0;

           seg_count++;
         //transmit('E');
     }
     else if(seg_count == 14)
    {
          TRISD=0x04;
          for(j2=0;j2<5;j2++);
         if(TIG_MODE==1)
         {
             for(j2=0;j2<40;j2++);
             if(TIG_MODE==1)
                TIG_SW=1;
             else
                TIG_SW=0;
         }
         else
             TIG_SW=0;    //transmit('T');

            seg_count++;
    }
    else if(seg_count == 15)
    {
          TRISD=0x08;
           for(j2=0;j2<5;j2++);
         if(MENU_KEY==1)
         {
            for(j2=0;j2<40;j2++);
            if(MENU_KEY==1)
                MENU_SW=1;
            else
                MENU_SW=0;
         }
         else
            MENU_SW=0;
                //transmit('N');

            seg_count++;
    }
    else if(seg_count == 16)
    {
         TRISD=0x10;
          for(j2=0;j2<5;j2++);
         if(MODE_KEY==1)
         {
             for(j2=0;j2<40;j2++);
             if(MODE_KEY==1)
                MODE_SW=1;
             else
                MODE_SW=0;
         }
         else
            MODE_SW=0;
            //transmit('M');

           seg_count++;
    }
     else if(seg_count == 17)
    {

         if(FOOT_SELECT==1)
         {
             for(j2=0;j2<40;j2++);
             if(FOOT_SELECT==1)
                FOOT_SW=0;   //normal mode 1,foot mode 0
             else
                FOOT_SW=1;  //foot mode 0
         }
         else
            FOOT_SW=1;  //normal mode 1
            //transmit('M');

           seg_count++;
    }
    else if(seg_count == 18)
    {
          for(j2=0;j2<5;j2++);
         if(GAS_WATER_SELECT==1) 
         {
             for(j2=0;j2<40;j2++);
             if(GAS_WATER_SELECT==1)//water cooled mode
                GAS_WATER_SW=1;     //water cooled mode
             else
               GAS_WATER_SW=0;      //gas cooled mode
         }
         else
            GAS_WATER_SW=0;         //gas cooled mode
            //transmit('M');

           seg_count++;
    }
    else if(seg_count == 19)
    {

         if(WATER_PRESSURE==1)
         {
             for(j2=0;j2<40;j2++);
             if(WATER_PRESSURE==1)
                WATER_PRESSURE_SW=1;//pressure error
             else
               WATER_PRESSURE_SW=0;
         }
         else
            WATER_PRESSURE_SW=0;
            TRISD=0x00;
            //transmit('M');

              seg_count++;
                seg_count=0;
    }
    /*else if(seg_count == 16)
    {
          TRISD=0x3F;
         if(REMOTE_KEY==1)
         {
            for(j2=0;j2<40;j2++);
            if(REMOTE_KEY==1)
                REMOTE_SW=1;
            else
                REMOTE_SW=0;
         }
         else
              REMOTE_SW=0;
         //transmit('R');
         TRISD=0x00;
    }*/




    if(seg_count>=20)
    {
      seg_count=0;
    }
}

void refresh_disp2()
{
   //if(seg_count==0)
    {
    DISP4=1;
    //LATBbits.LATB4=0;
    //clear_all();
    latchdata(cseg_data[7]);
    }
    

}



void latchdata(unsigned char seg_data)
{
    unsigned char temp1,j;
    temp1=seg_data;
//    seg_data=1;
    if(seg_count<8)
    {
  //     for(j=7;j>0;j--)
	{
		
           //arr[7-j]=(lookup_table[seg_data]>>j)&0x01;
           arr[6]=(lookup_table[seg_data]&(0x01<<1))>>1;
           arr[5]=(lookup_table[seg_data]&(0x01<<2))>>2;
           arr[4]=(lookup_table[seg_data]&(0x01<<3))>>3;

           arr[3]=(lookup_table[seg_data]&(0x01<<4))>>4;
           arr[2]=(lookup_table[seg_data]&(0x01<<5))>>5;
           arr[1]=(lookup_table[seg_data]&(0x01<<6))>>6;
           arr[0]=(lookup_table[seg_data]&(0x01<<7))>>7;

	}
	
    }
    else
   {
      for(j=0;j<8;j++)
      {
         temp1=seg_data;
         temp1=(temp1 >> j);
         temp1= (temp1 & 0X01);

         if(temp1==0)
         {
            temp1=0;
         }
         else if(temp1==1)
         {
            temp1=1;
         }
         arr[j]=temp1;
      }

   }
    SEGA=arr[0];
    SEGB=arr[1];
    SEGC=arr[2];
    SEGD=arr[3];
    SEGE=arr[4];
    SEGF=arr[5];
    SEGG=arr[6];
    SEGDP=arr[7];

}


