#ifndef BILL_H
#define BILL_H
#include "common.h"

class Bill: public Money {
  public:
    typedef enum state_t{
      ST_IDLE,
      ST_INSERTING,
      ST_INSERTED
    }state_t;
    typedef enum bill_t{
      BILL_20 = 0,
      BILL_50,
      BILL_100,
      BILL_500,
      BILL_1000,
      BILL_N
    }bill_t; 
  private:
    int m_currentBill = BILL_20;
    state_t m_state = ST_IDLE;

  public: 
    
    void init( pin_t& pin) {
      PRINTLN("Bill init");
      m_pin = pin;
      m_pin.configAnalog(); 
      m_value.resize(BILL_N);
      m_count.resize(BILL_N);
      m_value[BILL_20] = 20;
      m_value[BILL_50] = 50;
      m_value[BILL_100] = 100;
      m_value[BILL_500] = 500;
      m_value[BILL_1000] = 1000;
      reset();
    }
    bool detected() {
      return m_pin.analogRead() < 120; //Normally 400 when no detected
    }
    void update() {
      if(m_state == ST_IDLE && detected()) {
        PRINTLN("Bill reading");
        m_state = ST_INSERTING;
      }
    } 
    void setInserted() {
      m_state = ST_INSERTED;
    }
    bool isInserting() {
      return m_state==ST_INSERTING;
    }
    bool inserted() { 
      if(m_state == ST_INSERTED) {
        m_count[m_currentBill] ++; 
        PRINT("Bill ");
        PRINT(m_value[m_currentBill]);
        PRINT(" inserted total = ");
        PRINTLN(total());
        m_state = ST_IDLE;
        return true;
      }
      return false;
    }
    int currentBill() { 
      return m_value[m_currentBill];
    }
    void nextBill() { 
      m_currentBill = (m_currentBill + 1) % BILL_N;
      PRINT("Bill currentBill = ");
      PRINTLN(currentBill()); 
    }
    void prevBill() {
      m_currentBill = (m_currentBill - 1 + BILL_N) % BILL_N;
      PRINT("Bill currentBill = ");
      PRINTLN(currentBill()); 
    }
    void save(state_struct_t& state) {
      PRINTLN("Bill save");
      for(int i = 0; i < BILL_N; i++) {
        state.bill_cnt[i] = m_count[i];
      }
    }
    void load(state_struct_t& state) {
      PRINTLN("Bill load");
      for(int i = 0; i < BILL_N; i++) {
        m_count[i] = state.bill_cnt[i];
      }
    } 
};
#endif // BILL_H