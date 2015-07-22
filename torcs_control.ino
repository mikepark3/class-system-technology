// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13

#include <string.h>
#include <SPI.h>
#include "mcp_can.h"

#define PI 3.14159265359

short CC_trigger;
short LKAS_trigger;
float current_speed;
float target_speed;
float accel;
float angle;
float lookahead; // it will be use when advacned algorithm is made.
float toLeft;
float toRight;
float brake = 0;
float steer_value;
float steer_lock = 0.366519;
float left_steer;
float right_steer;
unsigned long id;
unsigned char len;

MCP_CAN CAN(9);                                            // Set CS to pin 9 for SeeedStudio CAN BUS Shield.

void setup()
{
    Serial.begin(115200);

START_INIT:

    if(CAN.begin(CAN_1000KBPS)==CAN_OK)                  // init can bus : baudrate = 1000k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }  
}


void loop()
{
  unsigned char buf[8];
  while(1)  // read the buffer until buffer is clear.
  {
    CAN.readMsgBufID(&id, &len, buf);
      if(len !=0)
      {
        switch(id)
        {
          case 0x0A0:
            memcpy(&CC_trigger, buf, sizeof(short));
            memcpy(&LKAS_trigger, buf+2, sizeof(short));
            memcpy(&current_speed, buf+4, sizeof(float));
            break;
          
          case 0x0A1:
            memcpy(&target_speed, buf, sizeof(float));
            memcpy(&accel, buf+4, sizeof(float));
            break;
            
          case 0x0B0:
            memcpy(&angle, buf, sizeof(float));
            memcpy(&lookahead, buf+4, sizeof(float));
            break;
          
          case 0x0B1:
            memcpy(&toLeft, buf, sizeof(float));
            memcpy(&toRight, buf+4, sizeof(float));
            break;
          
          default:
            break;
        }
      }
      else
      {
        break;
      }
  }
  
  control();
  
}

void control(void)
{
  TASK_CC();
  TASK_LKAS();
}

void TASK_CC(void)
{
  unsigned char buf[8];
 
  if(CC_trigger > 0)
  {
    
    if(abs(target_speed - current_speed) >= target_speed * 0.001)
    {
      accel = 0.25 * (target_speed - current_speed) * 0.3 + 0.25;
    }
    else
    {
      accel = 0.001 * (target_speed - current_speed) * 0.3;
    }
  }
  else
  {
    accel = 0;
  }
  
  memcpy(buf, &accel, sizeof(float));
  memcpy(buf+4, &brake, sizeof(float));
  
  CAN.sendMsgBuf(0x0A5, 0, 8, buf);
}

void TASK_LKAS(void)
{
  unsigned char buf[8];
  
  if(LKAS_trigger > 0)
  { 
    
    if(toLeft > toRight)
    {
      angle -= ((-toLeft + 3*toRight) / 2) / (toRight + toLeft) ;
    }
    else
    {
      angle -= ((-3*toLeft + toRight) / 2) /(toRight + toLeft);  
    }
    
    steer_value = angle / steer_lock;
//    Serial.println(steer_value); 
    if(steer_value > 0.0)
    {
      left_steer = steer_value;
      right_steer = 0.0;
    }
    
    else if(steer_value < 0.0)
    {
      right_steer = -(steer_value);
      left_steer = 0.0;
    }
  }
  else
  {
    left_steer = right_steer = 0.0;
  }
  
  memcpy(buf, &left_steer, sizeof(float));
  memcpy(buf+4, &right_steer, sizeof(float));
  
  CAN.sendMsgBuf(0x0B5, 0, 8, buf);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
