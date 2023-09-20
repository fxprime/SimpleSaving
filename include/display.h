#ifndef DISPLAY_H
#define DISPLAY_H
#include "common.h"
#include "coin.h"
#include "bill.h"
#include "billpuller.h"

/* -------------------------------------------------------------------------- */
/*                                   DISPLAY                                  */
/* -------------------------------------------------------------------------- */

#include <Wire.h>               
#include <LiquidCrystal_I2C.h>  //Enjoyneeering version
  
class Display {
  private:
    LiquidCrystal_I2C m_lcd;
    Coin* m_coin;
    Bill* m_bill;
    BillPuller* m_puller;
    pin_t m_sda;
    pin_t m_scl;
    unsigned long m_lastUpdate = 0;
  public:
    Display() : m_lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE) { }

    void init(pin_t& sda, pin_t& scl, Coin* coin, Bill* bill, BillPuller* puller) {
      PRINTLN("Display init");
      m_coin = coin;
      m_bill = bill;
      m_puller = puller;
      m_sda = sda;
      m_scl = scl;

      while (m_lcd.begin(16, 2, LCD_5x8DOTS, 4, 5, 400000, 250) != 1)  
      {
        PRINTLN(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
        WAIT(5000);   
      } 
      PRINTLN(F("LCD is connected"));
      m_lcd.begin();
      m_lcd.backlight();
      m_lcd.clear();
    }

    void show() {  
      if(m_puller->isStuck()) {
        if(millis() - m_lastUpdate > 100) {
          m_lastUpdate = millis();
          m_lcd.setCursor(0, 0);
          m_lcd.print("Err: Bill stuck ");
          m_lcd.setCursor(0, 1);
          m_lcd.print("REBOOT TO RESET ");
        } 
        return;
      }

      if(m_coin->isInserting()){
        if(millis() - m_lastUpdate > 100) {
          m_lastUpdate = millis();
          m_lcd.setCursor(0, 0);
          m_lcd.print("   PLEASE WAIT  ");
          m_lcd.setCursor(0, 1);
          m_lcd.print(" Inserting coin ");
        }
        return;
      }

      if(m_bill->isInserting()){
        if(millis() - m_lastUpdate > 100) {
          m_lastUpdate = millis();
          m_lcd.setCursor(0, 0);
          m_lcd.print("   PLEASE WAIT  ");
          m_lcd.setCursor(0, 1);
          m_lcd.print(" Inserting bill ");
        }
        return;
      }

      if(millis() - m_lastUpdate > 100) {
        m_lastUpdate = millis();

        int coinTotal = m_coin->total();
        int billTotal = m_bill->total();
        int total = coinTotal + billTotal; 
        char buf[17];
        sprintf(buf, "Saving   %4d B.", total);
        m_lcd.setCursor(0, 0);
        m_lcd.print(buf);
        sprintf(buf, "<< Bill  %4d >>", m_bill->currentBill());
        m_lcd.setCursor(0, 1);
        m_lcd.print(buf);

        // Uncomment this to show total coin and bill
        // We can show each type of coin and bill too
        // But it's too much for this small LCD

        // sprintf(buf, "C:%4d B:%4d", coinTotal, billTotal);
        // m_lcd.setCursor(0, 0);
        // m_lcd.print(buf);
        // sprintf(buf, "T:%4d NOW:%4d", total, m_bill->currentBill());
        // m_lcd.setCursor(0, 1);
        // m_lcd.print(buf);
      } 
    }
};

#endif // DISPLAY_H