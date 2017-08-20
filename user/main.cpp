/**
  ******************************************************************************
  * @file   : *.cpp
  * @author : shentq
  * @version: V1.2
  * @date   : 2016/08/14

  * @brief   ebox application example .
  *
  * Copyright 2016 shentq. All Rights Reserved.         
  ******************************************************************************
 */


#include "ebox.h"
#include "ads7816.h"
Timer timer2(TIM2);

Ads7816 adc(&PB9,&PB7,&PB8);//PB4是jtag引脚。不要用20pin的jlink去调试程序
uint16_t val;
float voltage;
uint8_t adc_ch,_485_ch;
uint64_t last_updata_time;

uint16_t   val_ch1,val_ch2,val_ch3,val_ch4;
float   voltage_ch1,voltage_ch2,voltage_ch3,voltage_ch4 ;
float   detector_urrent_ch1,detector_temperature_ch2,flue_temperature_ch3,ambient_temperature_ch4 ;

//函数申明
void _485_tx_mode();
void _485_rx_mode();

void _485_send_ch(uint8_t ch);
void adc_read_ch(uint8_t ch);

Pwm pwm1(&PB0);

void t2it()
{
    PA0.toggle();
}


void setup()
{
    ebox_init();
    uart1.begin(115200);
    adc.begin();   
    
//    timer2.begin(100); //设置ADC采样率
//    timer2.attach(t2it);
//    timer2.interrupt(ENABLE);
//    timer2.start();
    //485
    PA11.mode(OUTPUT_PP);
    
    PA4.mode(OUTPUT_PP);
    
    //模拟开关打开
    PB4.mode(OUTPUT_PP);
    PB5.mode(OUTPUT_PP);
    PB6.mode(OUTPUT_PP);
    //测试关管脚
    PA0.mode(OUTPUT_PP);
    PA1.mode(OUTPUT_PP);
    PB0.mode(OUTPUT_PP);
    pwm1.begin(1000, 500);
    pwm1.set_oc_polarity(1);//set output polarity after compare

    uart1.printf("core:%d\r\n",cpu.clock.core);
    uart1.printf("core:%d\r\n",cpu.clock.core);
    uart1.printf("hclk:%d\r\n",cpu.clock.hclk);
    uart1.printf("pclk1:%d\r\n",cpu.clock.pclk1);
    uart1.printf("pclk2:%d\r\n",cpu.clock.pclk2);

}

int main(void)
{

    setup();
    last_updata_time = millis();
    while(1)
    {
        detector_urrent_ch1 = voltage_ch1;///(0.825*1.98);//825欧姆采样电阻，1.98为运放放大倍数，单位uA，酸露漏电流
        detector_temperature_ch2 = voltage_ch2;///1020;//运放放大倍数1020，根据实际情况调整，酸露传感器温度S热电偶
        flue_temperature_ch3 = voltage_ch3;///101;//运放放大倍数101,烟气温度K热电偶
        ambient_temperature_ch4 = voltage_ch4;///3.56;//运放放大倍数3.56,环境温度检测LM35，10mV/℃      

        for(int i = 0; i < 4; i++)
            adc_read_ch(i);
//        detector_urrent_ch1 = voltage_ch1/1.98/0.825;//825欧姆采样电阻，1.98为运放放大倍数，单位uA，酸露漏电流
//        detector_temperature_ch2 = voltage_ch2/1020;//运放放大倍数1020，根据实际情况调整，酸露传感器温度S热电偶,单位//mv，需要制表查询计算
//        flue_temperature_ch3 = voltage_ch3/101;//运放放大倍数101,烟气温度K热电偶,单位//mv，需要制表查询计算
//        ambient_temperature_ch4 = voltage_ch4/3.56/10;//运放放大倍数3.56,环境温度检测LM35，10mV/℃，单位℃，需要零点补偿校准
        if(millis() - last_updata_time >= 1000) //发送485更新速率时间为500ms每次
        {
           last_updata_time = millis();
//            _485_tx_mode();
//            uart1.printf("val_ch1 = 0x%04x\r\nval_ch2 = 0x%04x\r\nval_ch3 = 0x%04x\r\n val_ch4 = 0x%04x\r\n",val_ch1,val_ch2,val_ch3,val_ch4);
//            _485_rx_mode();

            _485_tx_mode();uart1.printf("%d\r\n",millis());
            uart1.printf("detector_urrent_ch1 = %4.3f\r\n detector_temperature_ch2 = %4.3f\r\n flue_temperature_ch3 = %4.3f\r\n ambient_temperature_ch4 = %4.1f\r\n",detector_urrent_ch1,detector_temperature_ch2,flue_temperature_ch3,ambient_temperature_ch4);
            _485_rx_mode();
            PA1.toggle();
            
        }
//          delay_ms(1000);
        
      
    }

}

//485输出
void _485_tx_mode()//如果不输出，将引脚设置翻转
{
    PA11.set();
}
void _485_rx_mode()
{
    PA11.reset();
}
//通道模拟开关选择
void adc_read_ch(uint8_t ch)
{
    PA0.toggle();
    switch(ch)
    {
        case 0: 
            PB4.set();
            PB5.reset(); 
            PB6.reset();
            delay_ms(50);
            val_ch1 = adc.read();
            voltage_ch1 = adc.read_voltage(); //mv
            break;
        case 1:
            PB4.set();
            PB5.set(); 
            PB6.reset();
            delay_ms(50);
            val_ch2 = adc.read();
            voltage_ch2 = adc.read_voltage();//mv
            break;
         case 2:
            PB4.set();
            PB5.reset(); 
            PB6.set();
            delay_ms(50);
            val_ch3 = adc.read();
            voltage_ch3 = adc.read_voltage();//mv
            break;
         case 3:
            PB4.set();
            PB5.set(); 
            PB6.set();
            delay_ms(50);
            val_ch4 = adc.read();
            voltage_ch4 = adc.read_voltage();//mv
            break;
        default:
            break;
    }    
}
