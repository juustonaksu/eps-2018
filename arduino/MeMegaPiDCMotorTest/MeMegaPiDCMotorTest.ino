/**
 * \par Copyright (C), 2012-2016, MakeBlock
 * @file    MeMegaPiDCMotorTest.ino
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2016/05/17
 * @brief   Description: this file is sample code for MegaPi DC motor device.
 *
 * Function List:
 *    1. void MeMegaPiDCMotorTest::run(int16_t speed)
 *    2. void MeMegaPiDCMotorTest::stop(void)
 *
 * \par History:
 * <pre>
 * <Author>     <Time>        <Version>      <Descr>
 * Mark Yan     2016/05/17    1.0.0          build the new
 * </pre>
 */
#include "MeMegaPi.h"

MeMegaPiDCMotor motor1(PORT4A);

MeMegaPiDCMotor motor2(PORT4B);


uint8_t motorSpeed = 200;

void setup()
{
}

void loop()
{
  motor1.run(motorSpeed); /* value: between -255 and 255. */
  motor2.run(motorSpeed); /* value: between -255 and 255. */
  delay(2000);
  motor1.stop();
  motor2.stop();
  delay(100);
  motor1.run(-motorSpeed);
  motor2.run(-motorSpeed);
  delay(2000);
  motor1.stop();
  motor2.stop();
  delay(2000);
}

