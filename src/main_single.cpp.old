#include <Arduino.h>
// #include "config.h"

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

/* -------------------------------------------------------------------------- */
/*                                BILL SWITCHER                               */
/* -------------------------------------------------------------------------- */
#include <EncButton.h>

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
    void keepRunning() {
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
    bool stoped() {
      return m_state == ST_IDLE;
    }
    bool isStuck() {
      return m_state == ST_STUCK;
    }
};

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
    Display() : m_lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE) {
      
    }
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
      }

      if(m_bill->isInserting()){
        if(millis() - m_lastUpdate > 100) {
          m_lastUpdate = millis();
          m_lcd.setCursor(0, 0);
          m_lcd.print("   PLEASE WAIT  ");
          m_lcd.setCursor(0, 1);
          m_lcd.print(" Inserting bill ");
        }
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
        // sprintf(buf, "C:%4d B:%4d", coinTotal, billTotal);
        // m_lcd.setCursor(0, 0);
        // m_lcd.print(buf);
        // sprintf(buf, "T:%4d NOW:%4d", total, m_bill->currentBill());
        // m_lcd.setCursor(0, 1);
        // m_lcd.print(buf);
      } 
    }
};

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
Coin _coin;
Bill _bill;
Display _disp;
IMem _memo;
BillSwitch _butt;
BillPuller _puller;
 
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
  _coin.update();
  _bill.update(); 
  _butt.update();
  _puller.update();

  if(_butt.leftButtonPressed()){
    _bill.prevBill();
  }
  if(_butt.rightButtonPressed()){
    _bill.nextBill();
  } 
  if(_butt.bothButtonsPressed()){ 
    _memo.reset();
  }

  if(_coin.inserted()) {
    _memo.save();
  }

  if(_bill.inserted()) {
    _memo.save();
  } 
  
  if(_bill.detected()) {
    _puller.keepRunning();
  }else if(_bill.isInserting()){
    _puller.stoping();
    if(_puller.stoped()) {
      _bill.setInserted();
    }
  }

  _disp.show();
} 