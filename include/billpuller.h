#ifndef BILLPULLER_H
#define BILLPULLER_H

#include "common.h"
/* -------------------------------------------------------------------------- */
/*                                 BILL PULLER                                */
/* -------------------------------------------------------------------------- */
/**
 * @brief Class ควบคุมการทำงานของมอเตอร์ดึงธนบัตร
 */
class BillPuller { 
  public: 
    typedef enum state_t{
      ST_IDLE,
      ST_RUNNING,
      ST_STOPING, // หมุนสักแปปก่อนหยุดทำงานให้ชัวร์ว่าธนบัตรออกมาแล้ว
      ST_STUCK
    }state_t;
  private:
    pin_t m_pin; 
    state_t m_state = ST_IDLE;
    unsigned long m_startTime = 0;
    unsigned long m_startStopingTime = 0;
  public:
    void init(pin_t& pin) {
      PRINTLN("BillPuller init");
      m_pin = pin;
      m_pin.configDigital();
    }
    void update() {
      if(m_state == ST_RUNNING || m_state == ST_STOPING) {
        m_pin.pwmWrite(150);
      } else {
        m_pin.pwmWrite(0);
      }
      if(m_state == ST_RUNNING && millis() - m_startTime > 2000) {
        m_state = ST_STUCK; 
        m_pin.pwmWrite(0);  
      }
      if(m_state == ST_STOPING && millis() - m_startStopingTime > 500) {
        PRINTLN("BillPuller stoped"); 
        m_state = ST_IDLE;
      }
    }
    void run() {
      if(m_state == ST_IDLE) {
        PRINTLN("BillPuller keepRunning"); 
        m_state = ST_RUNNING;
        m_startTime = millis();
      }
      if(m_state == ST_STOPING) {
        PRINTLN("BillPuller still keepRunning"); 
        m_state = ST_RUNNING; 
        m_startStopingTime = 0;
      }
    }  
    void stoping() {
      if(m_state == ST_RUNNING) {
        PRINTLN("BillPuller stoping"); 
        m_state = ST_STOPING;
        m_startStopingTime = millis();
      }
    }
    bool isStoped() {
      return m_state == ST_IDLE;
    }
    bool isStuck() {
      return m_state == ST_STUCK;
    }
};
#endif // BILLPULLER_H