void system_init();
void oscillator_init();
void portpin_init();
void Timer0_init();
void Timer1_init();
void uart_init();
void adc_init();
void PWM_init();
void Read_ADC();
void Update_PWM(unsigned int Current);
void inttochar(unsigned int parameter);
void intochar_volt(unsigned int parameter);
void Send_Byte(unsigned char transbyte);
void Send_Integer(unsigned int data);
void delay();
void delay1();
void powerontest();
void Read_Model();
inline void chk_data_remote();
void keydetect();
void default_value();
void resetmem();
void readmem();
void writemem();
char test_gas_hf();
void write(unsigned char addr,unsigned char data);
unsigned char Read( unsigned char addr );
void refresh_disp();
void latchdata(unsigned char);
void clear_all();
//tig file
void TIG_Weld_Process();
unsigned int Update_Data(unsigned int operator,unsigned int mini,unsigned int maxi,unsigned char sign);
unsigned int Fast_Update_Data(unsigned int,unsigned int,unsigned int,unsigned char );
void Update_TIG_Parameters(unsigned char enc_result1);
void exitprocess();
void exitwriteprocess();
void preflow();
void display_error();
//encoder
unsigned char encoder_port(void);
unsigned char scanencoder(void);
//mma
void weld_mma_process();
void Update_MMA_Parameters(unsigned char);