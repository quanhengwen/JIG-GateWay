/******************************************************************************

                  ��Ȩ���� (C), 2014-2024, ���������ǿƼ����޹�˾

 ******************************************************************************
  �� �� ��   : DS3231.C
  �� �� ��   : ����
  ��    ��   : ty
  ��������   : 2014��9��26��
  ����޸�   :
  ��������   : ʱ��оƬDS331�������򣬲���ģ��IIC
  �����б�   :
              BCD2HEX
              delay_1us
              delay_us
              HEX2BCD
              I2C1_ReadByte
              I2C1_WriteByte
              I2C_DS3231_Config
              IIC_ack
              IIC_noack
              IIC_readbyte
              IIC_sendbyte
              IIC_start
              IIC_stop
              IIC_waitack
              ModifyTimeBCD
              ModifyTimeHEX
              ReadTimeHEX
              SDA_DIR_SET
  �޸���ʷ   :
  1.��    ��   : 2014��9��26��
    ��    ��   : ty
    �޸�����   : �����ļ�

******************************************************************************/
#include "stm32f10x.h"
#include "DS3231.h"
//#include "SMITServerToMJXConcentrator.h"

unsigned char BCD2HEX(unsigned char ucValue);
unsigned char HEX2BCD(unsigned char ucValue);
unsigned char I2C1_ReadByte(unsigned char  id, unsigned char read_address);


 /*******************************************************************************
* Function Name : BCD2HEX
* Description   : BCD change to HEX.
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char BCD2HEX(unsigned char ucValue)   //BCDת��ΪByte
{
     unsigned char i;
     i= ucValue & 0x0f;
     ucValue >>= 4;
     ucValue &= 0x0f;
     ucValue *= 10;
     i += ucValue;     
     return i;
}
 
/*******************************************************************************
* Function Name : delay_1us
* Description   : ��ʱ1US
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void delay_1us(void)
{
    __nop(); 	 //72Mʱ��Ƶ�ʣ���ôһ�������� 1/72΢��
    __nop();
    __nop();
    __nop();
    __nop();
    __nop(); 
    __nop();
    __nop();

}

 /*******************************************************************************
* Function Name : delay_us
* Description   : ��ʱN US
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void delay_us(unsigned int uiCounter)
{
	while(uiCounter--){
		delay_1us();
	}

}
 
/*******************************************************************************
* Function Name : HEX2BCD
* Description   : HEX change to BCD.
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char HEX2BCD(unsigned char ucValue)
{
   unsigned char i,j,k;
   i = ucValue/10;
   j = ucValue%10;
   k = j+(i<<4);
   return k;
}
/*******************************************************************************
* Function Name : SDA_DIR_SET
* Description   : ����SDA�����������
* Input         : ucSdaDir
                    0--�����1--����
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void SDA_DIR_SET(unsigned char ucSdaDir) //SDA
{
    GPIO_InitTypeDef  GPIO_InitStructure;
//    if(ucSdaDir==0){
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          // SDA 
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_Init(GPIOD, &GPIO_InitStructure); 
//    }
//    else if(ucSdaDir==1){
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          // SDA 
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//        GPIO_Init(GPIOD, &GPIO_InitStructure); 
//    }
    if(ucSdaDir==0){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;          // SDA 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure); 
    }
    else if(ucSdaDir==1){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;          // SDA 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
        GPIO_Init(GPIOB, &GPIO_InitStructure); 
    }	
		
}
/*******************************************************************************
* Function Name : IIC_start
* Description   : IIC��ʼ�ź�
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_start(void)
{
	SDA_DIR_SET(0);   //SDA�����
	IIC_SDA_HIGH;	 //SDAΪ�ߵ�ƽ 	  
	IIC_SCL_HIGH;	 //SCLΪ�ߵ�ƽ
	delay_us(4);
 	IIC_SDA_LOW; //SDA�½���
    IIC_SCL_LOW;
}
/*******************************************************************************
* Function Name : IIC_stop
* Description   : IICֹͣ�ź�
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_stop(void)
{
	SDA_DIR_SET(0);   				 //SDA�����
 	IIC_SDA_LOW; //SDA�½���
    IIC_SCL_LOW;
	delay_us(4);
 	IIC_SCL_HIGH; //SDA�½���
    IIC_SDA_HIGH;
}
/*******************************************************************************
* Function Name : IIC_waitack
* Description   : IIC�ȴ�Ӧ���ź�
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char IIC_waitack(void)
{
	unsigned char times=0;
	SDA_DIR_SET(1);     //SDA����Ϊ����  
	IIC_SDA_HIGH;//sda���͸ߵ�ƽ
	delay_us(1);	   
	IIC_SCL_HIGH;	//ʱ�Ӹߵ�ƽ��������
	delay_us(1);	 
	while(IIC_SDA_Read){	//�����ݣ�ֱ���ӻ�����Ӧ��
		if(++times>250){
			IIC_stop(); //��Ӧ��ֹͣ
			return 1;
		}
	}
	IIC_SCL_LOW;//ʱ������͵�ƽ���½��� 	   
	return 0;  
}
/*******************************************************************************
* Function Name : IIC_ack
* Description   : IICӦ���ź�
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_ack(void)
{
    SDA_DIR_SET(0);
	IIC_SCL_LOW; //ʱ�ӵ͵�ƽ
	IIC_SDA_LOW; //���ݵ͵�ƽ,��ʾ��Ӧ��
	delay_us(2);
	IIC_SCL_HIGH;	  //ʱ��������
	delay_us(2);
	IIC_SCL_LOW; //ʱ���½���
}
/*******************************************************************************
* Function Name : IIC_ack
* Description   : IIC��Ӧ���ź�
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_noack(void)
{
    SDA_DIR_SET(0);
	IIC_SCL_LOW;  //ʱ�ӵ͵�ƽ
	IIC_SDA_HIGH;	   //���ݸߵ�ƽ,��ʾ��Ӧ��
	delay_us(2);
	IIC_SCL_HIGH;		//ʱ��������
	delay_us(2);
	IIC_SCL_LOW;	//ʱ���½���
}
/*******************************************************************************
* Function Name : IIC_sendbyte
* Description   : IICд����
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_sendbyte(unsigned char  ucData)
{                        
    unsigned char  t;      
    SDA_DIR_SET(0);
    IIC_SCL_LOW; //ʱ�ӵ͵�ƽ��ֻ��SCLΪ�Ͳſɸı�����
    for(t=0;t<8;t++)//ѭ������8���ֽ�
    {              
 		if ((ucData&0x80)>>7) 	
            IIC_SDA_HIGH; //������7-tλ
		else 
            IIC_SDA_LOW;
        ucData<<=1; 	  
		delay_us(2); 
        
		IIC_SCL_HIGH; //����SCL��������
		delay_us(2); 
		IIC_SCL_LOW; //SCL�ָ��͵�ƽ��׼���޸�SDA	
		delay_us(2);
    }	 
}
/*******************************************************************************
* Function Name : IIC_sendbyte
* Description   : IIC������
* Input         : ��
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char IIC_readbyte(unsigned char ucDck)
{
	unsigned char i,receive=0;
	SDA_DIR_SET(1);//׼���������ݣ�SDA����Ϊ����
    for(i=0;i<8;i++ )			//ѭ������8λ
	{
        IIC_SCL_LOW; //SCLΪ�͵�ƽ 
        delay_us(2);
		IIC_SCL_HIGH;   //SCL�����أ���ʼ��ȡ����
        receive<<=1;
        if(IIC_SDA_Read) //��ȡ����
			receive|=0x01; //�������������Ϊ1��receive���λӦΪ1  
		delay_us(1); 
    }					 
    if (ucDck) //�����ҪӦ��
        IIC_ack();//����Ӧ��
    else
        IIC_noack(); //������Ӧ��   
    return receive;
}

/*******************************************************************************
* Function Name : I2C1_WriteByte
* Description   : I2C1_WriteByte.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void I2C1_WriteByte(unsigned char id,unsigned char write_address,unsigned char byte)
{

    IIC_start();//������ʼ����
    IIC_sendbyte(id);
    IIC_waitack();
    IIC_sendbyte(write_address);
    IIC_waitack();
    IIC_sendbyte(byte);
    IIC_waitack();
    IIC_stop();
    
}
/*******************************************************************************
* Function Name : I2C1_ReadByte
* Description   : I2C1_ReadByte.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
unsigned char I2C1_ReadByte(unsigned char  id, unsigned char read_address)
 {  
     unsigned char temp;         

    IIC_start();//������ʼ����
    IIC_sendbyte(id);
    IIC_waitack();
    IIC_sendbyte(read_address);
    IIC_waitack();
     
    IIC_start(); 
    IIC_sendbyte(id+1);
    IIC_waitack();
    temp=IIC_readbyte(0);
    IIC_stop();
    return temp;    
 }
/*******************************************************************************
* Function Name : I2C_DS3231_Config
* Description   : I2C_DS3231_Config.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void I2C_DS3231_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//PCB��һ�� ������PD0 PD1

//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;/*PD1-SDA,PD0-SCL*/
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 /*50MHz*/
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//PCB�ڶ��� ������PB6 PB7

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7; /*PB7-SDA,PB6-SCL*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /*50MHz*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	

}

/*******************************************************************************
* Function Name : ModifyTimeHEX
* Description   : ModifyTimeHEX.
* Input         : sTime.  HEX����
* Output        : None.
* Return        : None.
*******************************************************************************/
 void ModifyTimeHEX(ScmDS3231Struct sTime)
 {
		unsigned char ucTemp;
	  /* ������ */
		ucTemp = HEX2BCD(sTime.ucYear);
		I2C1_WriteByte(0xd0,0x06,ucTemp);
	  /* ������ */
	 	ucTemp = HEX2BCD(sTime.ucMonth);
		I2C1_WriteByte(0xd0,0x05,ucTemp);
	  /* �������� */
	  ucTemp = HEX2BCD(sTime.ucDate);
		I2C1_WriteByte(0xd0,0x04,ucTemp);
	  /* ���ڼ����� */
	  ucTemp = HEX2BCD(sTime.ucDay);
		I2C1_WriteByte(0xd0,0x03,ucTemp);
	  /* Сʱ���� */
	  ucTemp = HEX2BCD(sTime.ucHour);
		I2C1_WriteByte(0xd0,0x02,ucTemp);
	  /* �������� */
	  ucTemp = HEX2BCD(sTime.ucMinute);
		I2C1_WriteByte(0xd0,0x01,ucTemp);
	  /* ������ */
	  ucTemp = HEX2BCD(sTime.ucSecond);
		I2C1_WriteByte(0xd0,0x00,0);
 }
 
/*******************************************************************************
* Function Name : ModifyTimeBCD
* Description   : ModifyTimeBCD.
* Input         : sTime. BCD������
* Output        : None.
* Return        : None.
*******************************************************************************/
 void ModifyTimeBCD(ScmDS3231Struct sTime)
 {
	  /* ������ */
		I2C1_WriteByte(0xd0,0x06,sTime.ucYear);
	  /* ������ */
		I2C1_WriteByte(0xd0,0x05,sTime.ucMonth);
	  /* �������� */
		I2C1_WriteByte(0xd0,0x04,sTime.ucDate);
	  /* Сʱ���� */
		I2C1_WriteByte(0xd0,0x02,sTime.ucHour);
	  /* �������� */
		I2C1_WriteByte(0xd0,0x01,sTime.ucMinute);
	  /* ������ */
		I2C1_WriteByte(0xd0,0x00,sTime.ucSecond);
 } 
 /*******************************************************************************
* Function Name : ReadTimeHEX
* Description   : ReadTimeHEX.
* Input         : *psTime. �����Ĳ���ΪHEX
* Output        : None.
* Return        : None.
*******************************************************************************/
 void ReadTimeHEX(ScmDS3231Struct *psTime)
 {
	  /* ������ */
	  psTime->ucYear = BCD2HEX(I2C1_ReadByte(0xd0,0x06));
	  /* ������ */
	  psTime->ucMonth = BCD2HEX(I2C1_ReadByte(0xd0,0x05));
	  /* �������� */
	  psTime->ucDate = BCD2HEX(I2C1_ReadByte(0xd0,0x04));
	  /* �������ڼ� */
	  psTime->ucDay = BCD2HEX(I2C1_ReadByte(0xd0,0x03));
	  /* ����Сʱ */
	  psTime->ucHour = BCD2HEX(I2C1_ReadByte(0xd0,0x02));
	  /* �������� */
	  psTime->ucMinute = BCD2HEX(I2C1_ReadByte(0xd0,0x01));
	  /* ������ */
	  psTime->ucSecond = BCD2HEX(I2C1_ReadByte(0xd0,0x00));
 } 
 
/* END:   Added by ty, 2014/9/26 */

