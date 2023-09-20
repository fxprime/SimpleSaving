#ifndef BILLSWITCHER_H
#define BILLSWITCHER_H
/* -------------------------------------------------------------------------- */
/*                                BILL SWITCHER                               */
/* -------------------------------------------------------------------------- */
#include <EncButton.h>
#include "common.h"

class BillSwitch {
  private: 
    pin_t m_left_pin;
    pin_t m_right_pin;
    Button *m_left_btn;
    Button *m_right_btn; 
    VirtButton *m_both_btn;
  public:
    void init(pin_t& left_pin, pin_t& right_pin) {
      PRINTLN("Button init");
      m_left_pin = left_pin;
      m_right_pin = right_pin; 
      m_left_pin.configDigital();
      m_right_pin.configDigital();
      m_left_btn = new Button(m_left_pin.pin);
      m_right_btn = new Button(m_right_pin.pin);
      m_both_btn = new VirtButton(); 
    }
    void update() {
      m_left_btn->tick();
      m_right_btn->tick();
      m_both_btn->tick(*m_left_btn, *m_right_btn);
    }
    bool leftButtonPressed() {
      if(m_left_btn->click()) {
        PRINTLN("Button leftButtonPressed");
        return true;
      }
      return false;
    }
    bool rightButtonPressed() {
      if(m_right_btn->click()) {
        PRINTLN("Button rightButtonPressed");
        return true;
      }
      return false;
    }
    bool bothButtonsPressed() {
      if(m_both_btn->click()) {
        PRINTLN("Button bothButtonsPressed");
        return true;
      }
      return false;
    }
};

#endif // BILLSWITCHER_H