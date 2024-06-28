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
       if(operator >= minusoperator)
          operator--;//minusoperator;
       if(operator <= mini)
          operator=mini;
    }
    return(operator);
}

