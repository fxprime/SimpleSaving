#include <Arduino.h>
#include "common.h"
#include "coin.h"
#include "bill.h"
#include "billswitcher.h"
#include "billpuller.h"
#include "display.h"
#include "memo.h"


/* -------------------------------------------------------------------------- */
/*                              GLOBAL VARIABLES                              */
/* -------------------------------------------------------------------------- */
/* --------------------------------- Pinout --------------------------------- */
pin_t _pin_lcd_sda(4, OUTPUT);         //D2 on Nodemcu v2
pin_t _pin_lcd_scl(5, OUTPUT);         //D1 on Nodemcu v2 
pin_t _pin_coin(14, INPUT_PULLUP);     //D5 on Nodemcu v2
pin_t _pin_bill(A0, INPUT);            //ADC0 on Nodemcu v2
pin_t _pin_puller(15, OUTPUT);         //D8 on Nodemcu v2 
pin_t _pin_bleft(2, INPUT_PULLUP);     //D4 on Nodemcu v2 *** HIGH At boot
pin_t _pin_bright(12, INPUT_PULLUP);   //D6 on Nodemcu v2

/* --------------------------------- Object --------------------------------- */
Coin _coin; // สร้าง object สำหรับเซ็นเซอร์เหรียญ
Bill _bill; // สร้าง object สำหรับเซ็นเซอร์ธนบัตร
Display _disp; // สร้าง object สำหรับจอ LCD
IMem _memo; // สร้าง object สำหรับเก็บข้อมูลเงินที่อ่านได้
BillSwitch _butt; // สร้าง object สำหรับปุ่มกด
BillPuller _puller; // สร้าง object สำหรับมอเตอร์ดึงธนบัตร

/* ---------------------------------- Start --------------------------------- */
void setup() {  
  Serial.begin(115200);
  while (!Serial) { ;  }
  PRINTLN("\n\n\nSerial setup at 115200");

  _butt.init(_pin_bleft, _pin_bright);
  _coin.init(_pin_coin);
  _bill.init(_pin_bill);
  _disp.init(_pin_lcd_sda, _pin_lcd_scl, &_coin, &_bill, &_puller);
  _memo.init(&_coin, &_bill);
  _puller.init(_pin_puller);
  
  PRINTLN("Setup done");
}

void loop() {
  _coin.update();     // ตรวจสอบสัญญาณจากเซ็นเซอร์เหรียญ
  _bill.update();     // ตรวจสอบสัญญาณจากเซ็นเซอร์ธนบัตร
  _butt.update();     // ตรวจสอบสัญญาณจากปุ่มกด
  _puller.update();   // เช็คว่ามอเตอร์ทำงานนานเกินไปหรือยัง

  // ถ้ามีการกดปุ่มซ้าย ให้ปรับสถานะแบงค์มูลค่าน้อยลง
  if(_butt.leftButtonPressed()){
    _bill.prevBill();
  }
  // ถ้ามีการกดปุ่มขวา ให้ปรับสถานะแบงค์มูลค่ามากขึ้น
  if(_butt.rightButtonPressed()){
    _bill.nextBill();
  } 
  // ถ้ามีการกดปุ่มซ้ายและขวาพร้อมกัน ให้รีเซ็ตค่าเงินทั้งหมด
  if(_butt.bothButtonsPressed()){ 
    _memo.reset();
  }

  // หลังจากเซนเซอร์เหรียญทำงานเสร็จสิ้น สถานะของ coin จะเป็น inserted 
  // ให้บันทึกข้อมูลลง EEPROM
  if(_coin.inserted()) {
    _memo.save();
  }

  // หลังจากมอเตอร์ทำงานเสร็จสิ้น สถานะของ bill จะเป็น inserted
  // ให้บันทึกข้อมูลลง EEPROM
  if(_bill.inserted()) {
    _memo.save();
  } 
  
  // ถ้าตรวจจับเจอธนบัตร ให้มอเตอร์หมุน แต่ถ้าธนบัตรพ้นตัวเซ็นเซอร์แล้ว
  // แต่มอเตอร์ยังหมุนอยู่ ให้หน่วงเวลาเล็กน้อยเพื่อให้ธนบัตรพ้นระยะราง
  // แล้วค่อยหยุดทำงาน เมื่อหยุดทำงานแล้วให้เปลี่ยนสถานะเป็น inserted
  if(_bill.detected()) {
    _puller.run();
  }else if(_bill.isInserting()){
    _puller.stoping();
    if(_puller.isStoped()) {
      _bill.setInserted();
    }
  }

  // หน้าจอจะแสดงผลตามสถานะของเงินที่อ่านได้ โดยดึงค่าจาก object
  // coin bill และ puller มาแสดงผลทุก ๆ 100 ms
  _disp.show();
} 