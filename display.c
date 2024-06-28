/*unsigned int lookup_table[44][8]=
							{ //{a,b,c,d,e,f,g,d}
								{0,0,0,0,0,0,0,0},		//null character forword slash '/'
								{1,1,1,1,1,1,0,0},		//0
								{0,1,1,0,0,0,0,0},		//1
								{1,1,0,1,1,0,1,0},		//2
								{1,1,1,1,0,0,1,0},		//3
								{0,1,1,0,0,1,1,0},		//4
								{1,0,1,1,0,1,1,0},		//5
								{1,0,1,1,1,1,1,0},		//6
								{1,1,1,0,0,0,0,0},		//7
								{1,1,1,1,1,1,1,0},		//0
								{1,1,1,1,0,1,1,0},		//9
								{1,1,1,1,1,1,0,0},		//3A
								{0,1,1,0,0,0,0,0},		//3B
								{1,1,0,1,1,0,1,0},		//3C
								{1,1,1,1,0,0,1,0},		//3D
								{0,1,1,0,0,1,1,0},		//3E
								{1,0,1,1,0,1,1,0},		//3F
								{1,0,1,1,1,1,1,0},		//41
								{1,1,1,0,1,1,1,0},		//A
								{0,0,1,1,1,1,1,0},		//B
								{1,0,0,1,1,1,0,0},		//C
								{0,1,1,1,1,0,1,0},		//D
								{1,0,0,1,1,1,1,0},		//E
								{1,0,0,0,1,1,1,0},		//F
								{1,0,1,1,1,1,0,0},		//G
								{0,1,1,0,1,1,1,0},		//H
								{0,1,1,0,0,0,0,0},		//I
								{0,1,1,1,1,0,0,0},		//J
								{0,0,0,0,1,1,1,0},		//K
								{0,0,0,1,1,1,0,0},		//L
								{1,0,1,0,1,0,1,0},		//M
								{0,0,1,0,1,0,1,0},		//N
								{0,0,1,1,1,0,1,0},		//O
								{1,1,0,0,1,1,1,0},		//P
								{1,1,1,0,0,1,1,0},		//Q
								{0,0,0,0,1,0,1,0},		//R
								{1,0,1,1,0,1,1,0},		//S
								{0,0,0,1,1,1,1,0},		//T
								{0,1,1,1,1,1,0,0},		//U
								{0,0,1,1,1,0,0,0},		//V
								{0,1,0,1,0,1,1,0},		//W
								{0,0,1,0,0,0,0,0},		//X
								{0,1,1,1,0,1,1,0},		//Y
								{1,0,0,1,0,0,1,0},		//Z
							};*/



/*void refresh_disp()
{
}*/

/*void refresh_disp(void)
 {
    switch(cseg_count)
    {
        case 0:
                latchdata(0x00);
                DISP3=0;
                LATBbits.LATB4=1;
                decodeA=0;//msb
                decodeB=0;
                decodeC=0; //lsb
                latchdata(cseg_data[0]);
                if(dp_on_flag==1)
                {
                   SEGDP=1;
                }
                break;

        case 1:
                latchdata(0x00);
                //PORTBbits.RB4=1;
                decodeA=0;  //msb
                decodeB=0;
                decodeC=1;  //lsb
                latchdata(cseg_data[1]);
                break;

        case 2:
                latchdata(0x00);
                //PORTBbits.RB4=1;
                decodeA=0;  //msb
                decodeB=1;
                decodeC=0;  //lsb
                latchdata(cseg_data[2]);
                if(dp_on_flag==1)
                {
                   SEGDP=1;
                }
                break;

        case 3:
                latchdata(0x00);
                //PORTBbits.RB4=1;
                decodeA=0;  //msb
                decodeB=1;
                decodeC=1;  //lsb
                latchdata(cseg_data[3]);
                if(dp_on_flag==1)
                {
                   SEGDP=1;
                }
                break;
        case 4:
                LATBbits.LATB4=0;
                DISP1=1;
                latchdata(cseg_data[4]);
                    break;

        case 5:
                 DISP1=0;
                 DISP2=1;
                 latchdata(cseg_data[5]);
                 break;
        case 6:
                 DISP2=0;
                 DISP3=1;
                 latchdata(cseg_data[6]);
                 break;
        default:
                break;
    }
    cseg_count++;
    if(cseg_count >10)//17
    cseg_count=0;
 }*/

/*void latchdata(unsigned char seg_data)
{
    unsigned char arr[8],temp1,j;
    temp1=seg_data;
       for(j=0;j<8;j++)
	{
		arr[j]=lookup_table[seg_data][j];
	}
	if(dp_on_flag==1)
	{
		arr[7]=1;//0;
	}
    SEGA=arr[0];
    SEGB=arr[1];
    SEGC=arr[2];
    SEGD=arr[3];
    SEGE=arr[4];
    SEGF=arr[5];
    SEGG=arr[6];
    SEGDP=arr[7];

}*/
