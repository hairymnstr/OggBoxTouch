//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F429IGT6,����ԭ��Apollo STM32F4/F7������,��Ƶ180MHZ������12MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/08/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VCC          ��        DC5V/3.3V      //��Դ
//      GND          ��          GND          //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������ΪSPI����
//     LCDģ��                STM32��Ƭ��    
//    SDI(MOSI)      ��          PF9          //Һ����SPI��������д�ź�
//    SDO(MISO)      ��          PF8          //Һ����SPI�������ݶ��źţ��������Ҫ�������Բ�����
//=======================================Һ���������߽���==========================================//
//     LCDģ�� 					      STM32��Ƭ�� 
//       LED         ��          PD6          //Һ������������źţ��������Ҫ���ƣ���5V��3.3V
//       SCK         ��          PF7          //Һ����SPI����ʱ���ź�
//      DC/RS        ��          PD5          //Һ��������/��������ź�
//       RST         ��          PD12         //Һ������λ�����ź�
//       CS          ��          PD11         //Һ����Ƭѡ�����ź�
//=========================================������������=========================================//
//���ģ�鲻���������ܻ��ߴ��д������ܣ����ǲ���Ҫ�������ܣ�����Ҫ���д���������
//	   LCDģ��                STM32��Ƭ�� 
//      T_IRQ        ��          PH11         //�����������ж��ź�
//      T_DO         ��          PG3          //������SPI���߶��ź�
//      T_DIN        ��          PI3          //������SPI����д�ź�
//      T_CS         ��          PI8          //������Ƭѡ�����ź�
//      T_CLK        ��          PH6          //������SPI����ʱ���ź�
**************************************************************************************************/		
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "main.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi5;  //SPI���

/*****************************************************************************
 * @name       :uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte)
 * @date       :2018-08-09 
 * @function   :Write a byte of data using STM32's hardware SPI
 * @parameters :TxData:Data to be written
 * @retvalue   :Data received by the bus
******************************************************************************/
uint8_t SPI_WriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&hspi5,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;           	     //�����յ�������			
} 

/*****************************************************************************
 * @name       :void SPI_SetSpeed(SPI_TypeDef* SPIx,uint8_t SpeedSet)
 * @date       :2018-08-09 
 * @function   :Set hardware SPI Speed
 * @parameters :SPI_BaudRatePrescaler:SPI speed
 * @retvalue   :None
******************************************************************************/
void SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    // assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    // __HAL_SPI_DISABLE(&SPI5_Handler);            //�ر�SPI
    // SPI5_Handler.Instance->CR1&=0XFFC7;          //λ3-5���㣬�������ò�����
    // SPI5_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
    // __HAL_SPI_ENABLE(&SPI5_Handler);   
} 


/*****************************************************************************
 * @name       :void SPI5_Init(void)	
 * @date       :2018-08-09 
 * @function   :Initialize the STM32 hardware SPI5
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void SPI5_Init(void)
{
//     SPI5_Handler.Instance=SPI5;                         //SP5
//     SPI5_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
//     SPI5_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
//     SPI5_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//     SPI5_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
//     SPI5_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
//     SPI5_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ�����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
// //    SPI5_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
//     SPI5_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//     SPI5_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
//     SPI5_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
//     SPI5_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
//     HAL_SPI_Init(&SPI5_Handler);//��ʼ��
    
//     __HAL_SPI_ENABLE(&SPI5_Handler);                    //ʹ��SPI5
// 	                       //��������
}

/*****************************************************************************
 * @name       :void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)	
 * @date       :2018-08-09 
 * @function   :Configuring SPI pins and clocks,
                it will be called by HAL_SPI_Init
 * @parameters :hspi:SPI handle
 * @retvalue   :None
******************************************************************************/
// void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
// {
//     GPIO_InitTypeDef GPIO_Initure;
    
//     __HAL_RCC_GPIOF_CLK_ENABLE();       //ʹ��GPIOFʱ��
//     __HAL_RCC_SPI5_CLK_ENABLE();        //ʹ��SPI5ʱ��
    
//     //PF7,8,9
//     GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
//     GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
//     GPIO_Initure.Pull=GPIO_PULLUP;                  //����
//     GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            
//     GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //����ΪSPI5
//     HAL_GPIO_Init(GPIOF,&GPIO_Initure);
// }
