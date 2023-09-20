#ifndef COMMON_H
#define COMMON_H
#include <Arduino.h>

/* -------------------------------------------------------------------------- */
/*                                   DEFINE                                   */
/* -------------------------------------------------------------------------- */
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#define WAIT(x) delay(x)

/* -------------------------------------------------------------------------- */
/*                                COMMON STRUCT                               */
/* -------------------------------------------------------------------------- */
typedef struct pin_t{
  int pin;
  int mode;
  
  pin_t(int pin, int mode) {
    this->pin = pin;
    this->mode = mode;
  }
  pin_t() { 
    this->pin = 0;
    this->mode = 0;
  }
  void configDigital() {
    ::pinMode(pin, mode);
  }
  void configAnalog(){

  }
  int analogRead() {
    return ::analogRead(pin);
  }
  bool digitalRead() {
    return ::digitalRead(pin);
  }
  void pwmWrite(int value) {
    ::analogWrite(pin, value);
  }
}pin_t;

/**
 * @brief struct สำหรับเก็บสถานะของเงินที่อ่านได้
 * ใส่ไว้ใน EEPROM โดยเก็บเป็น byte array
 * มี 4 ช่องสำหรับเก็บเหรียญ 1, 2, 5, 10 บาท
 * มี 5 ช่องสำหรับเก็บธนบัตร 20, 50, 100, 500, 1000 บาท
 * ในแต่ละช่องเก็บจำนวนเหรียญหรือธนบัตรที่อ่านได้เป็น int
 */
typedef struct state_struct_t{
  int coin_cnt[4] = {0, 0, 0, 0};
  int bill_cnt[5] = {0, 0, 0, 0, 0};
}state_struct_t;


/* -------------------------------------------------------------------------- */
/*                                    MONEY                                   */
/* -------------------------------------------------------------------------- */

class Money {
  protected:
    std::vector<int> m_value;
    std::vector<int> m_count;
    pin_t m_pin; 
  public: 
    virtual void init(pin_t& pin) = 0;
    virtual void update() = 0; 
    virtual bool inserted() = 0;
    virtual void save(state_struct_t& state) = 0;
    virtual void load(state_struct_t& state) = 0; 
    void reset() {
      for(int i = 0; i < m_count.size(); i++) {
        m_count[i] = 0;
      }
    }
    int total() {
      int total = 0;
      for(int i = 0; i < m_count.size(); i++) {
        total += m_count[i] * m_value[i];
      }
      return total;
    } 
};

#endif // COMMON_H