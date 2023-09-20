#ifndef COIN_H
#define COIN_H

#include "common.h"

class Coin: public Money {
  public:
    typedef enum coin_t{
      COIN_1,
      COIN_2,
      COIN_5,
      COIN_10,
      COIN_N
    }coin_t;     
  private: 
    typedef enum state_t{
      ST_IDLE,
      ST_INSERTING,
      ST_INSERTED,
    }state_t;
    typedef enum edge_t{
      EDGE_RISING,
      EDGE_FALLING
    }edge_t;
 
    int m_state = ST_IDLE;
    int m_predictedCoin = COIN_1;
    int m_lastState = EDGE_RISING;
    unsigned long m_lastState_ts = 0;
  public:
    void init( pin_t& pin) {
      PRINTLN("Coin init"); 
      m_pin = pin;
      m_pin.configDigital();

      m_value.resize(COIN_N);
      m_count.resize(COIN_N);
      m_value[COIN_1] = 1;
      m_value[COIN_2] = 2;
      m_value[COIN_5] = 5;
      m_value[COIN_10] = 10;
      reset();
    } 
    void update() { 
      
      if(m_state==ST_IDLE && m_pin.digitalRead() == LOW) {
        PRINTLN("Coin reading");
        m_state = ST_INSERTING;
        m_predictedCoin = COIN_1;
        m_lastState_ts = millis();
        m_lastState = EDGE_FALLING;
      } 

      if(m_state==ST_INSERTING) {  
        bool isRising = m_pin.digitalRead() == HIGH;
        if(m_lastState == EDGE_FALLING && isRising) {
          m_lastState = EDGE_RISING;
          m_lastState_ts = millis();
        }else if(m_lastState == EDGE_RISING && !isRising) {
          m_lastState = EDGE_FALLING;
          m_predictedCoin = (m_predictedCoin + 1) % COIN_N;
        }  

        bool timeout = millis() - m_lastState_ts > 150;
        if(timeout) {
          m_lastState_ts = 0;
          m_state = ST_INSERTED; 
        }
      }

    }  
    bool inserted() {  
      if(m_state == ST_INSERTED) {
        m_count[m_predictedCoin] ++;
        m_state = ST_IDLE;

        PRINT("Coin ");
        PRINT(m_value[m_predictedCoin]);
        PRINT(" inserted total = ");
        PRINTLN(total());
        return true;
      } 
      return false;
    }
    bool isInserting() {
      return m_state==ST_INSERTING;
    }
    void save(state_struct_t& state) {
      PRINTLN("Coin save");
      for(int i = 0; i < COIN_N; i++) {
        state.coin_cnt[i] = m_count[i];
      }
    }
    void load(state_struct_t& state) {
      PRINTLN("Coin load");
      for(int i = 0; i < COIN_N; i++) {
        m_count[i] = state.coin_cnt[i];
      }
    }
};

#endif // COIN_H