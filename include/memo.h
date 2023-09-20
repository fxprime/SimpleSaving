#ifndef MEMO_H
#define MEMO_H
#include "common.h"
#include "coin.h"
#include "billpuller.h"
/* -------------------------------------------------------------------------- */
/*                                   EEPROM                                   */
/* -------------------------------------------------------------------------- */
#include <ESP_EEPROM.h>

class IMem {
  private:
    Coin* m_coin;
    Bill* m_bill;
    state_struct_t m_state;
  public:
    void init(Coin* coin, Bill* bill) {
      m_coin = coin;
      m_bill = bill;  
      EEPROM.begin(sizeof(state_struct_t));
      load();
      PRINTLN("IMem init");
    }
    void save() {
      PRINTLN("IMem save");
      m_coin->save(m_state);  // pull coin data to m_state
      m_bill->save(m_state);  // pull bill data to m_state
      EEPROM.put(0, m_state); // put m_state to EEPROM 
      boolean ok = EEPROM.commit(); // commit to flash
      PRINTLN((ok) ? "Commit OK" : "Commit failed");
    }
    void load() {
      PRINTLN("IMem load");   
      if(EEPROM.percentUsed()>=0) {
        EEPROM.get(0, m_state); 
        PRINTLN("EEPROM has data from a previous run.");
        PRINT(EEPROM.percentUsed());
        PRINTLN("% of ESP flash space currently used");
      } else {
        PRINTLN("EEPROM size changed - EEPROM data zeroed - commit() to make permanent");    
      }
      m_coin->load(m_state);
      m_bill->load(m_state);
    }
    void reset() {
      PRINTLN("IMem reset");
      m_coin->reset();
      m_bill->reset();
      save();
    }
};
#endif // MEMO_H