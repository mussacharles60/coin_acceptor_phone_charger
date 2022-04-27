/** 
* Based microcontroller project for coin acceptor with phone charging vending machine without change in return.
*
* Author: Mussa Charles | mussacharles60@gmail.com
* GitHub: https://github.com/mussacharles60/coin_acceptor_phone_charger
*/

#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

elapsedMillis waiting_timer;

elapsedMillis countdown_timer_1;
elapsedMillis countdown_timer_2;
elapsedMillis countdown_timer_3;

elapsedMillis led_timer_1;
elapsedMillis led_timer_2;
elapsedMillis led_timer_3;

elapsedMillis auto_start_timer_1;
elapsedMillis auto_start_timer_2;
elapsedMillis auto_start_timer_3;

elapsedMillis buzzer_timer;

int buzzer_count = 0;
boolean make_sound = false;

long UNIT_SECONDS = 60;
long UNIT_AMOUNT = 50;

long timer_1_count = 0;
long timer_2_count = 0;
long timer_3_count = 0;

long int hour_1 = 0;
long int hour_2 = 0;
long int hour_3 = 0;

long int minute_1 = 0;
long int minute_2 = 0;
long int minute_3 = 0;

long int second_1 = 0;
long int second_2 = 0;
long int second_3 = 0;

long int total_time_1 = 0;
long int total_time_2 = 0;
long int total_time_3 = 0;


int total_waiting_time = 60; // sec
int waiting_counter = 0; // sec

int auto_waiting_time = 10; // sec
int auto_waiting_counter_1 = 0; // sec
int auto_waiting_counter_2 = 0; // sec
int auto_waiting_counter_3 = 0; // sec

boolean is_switch_1_on = false;
boolean is_switch_2_on = false;
boolean is_switch_3_on = false;

boolean was_charging_1_complete = false;
boolean was_charging_2_complete = false;
boolean was_charging_3_complete = false;

int switch_1 = 7;
int switch_2 = 8;
int switch_3 = 9;
int buzzer = 10;

int ok_button = A0;

int led_1 = A1;
int led_2 = A2;
int led_3 = A3;

const int coinInt = 0;
int pulseCount = 0;
boolean isCoinInserted = false;
boolean is_insertion_process = false;

int pulse_counted = 0;
long current_amount = 0;

boolean is_switch_1_charging = false;
boolean is_switch_2_charging = false;
boolean is_switch_3_charging = false;

void setup() {
  Serial.begin(9600);

  attachInterrupt(coinInt, coinInserted, RISING);

  pinMode(switch_1, OUTPUT);
  pinMode(switch_2, OUTPUT);
  pinMode(switch_3, OUTPUT);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);

  digitalWrite(switch_1, 1);
  digitalWrite(switch_2, 1);
  digitalWrite(switch_3, 1);
  digitalWrite(led_1, 0);
  digitalWrite(led_2, 0);
  digitalWrite(led_3, 0);

  pinMode(ok_button, INPUT_PULLUP);

  lcd.begin(20, 4);
  lcd.print("   PHONE CHARGING   ");
  lcd.setCursor(0, 2);
  lcd.print("  VENDING  MACHINE  ");
  delay(2000);
}

void loop() {
//  tone(buzzer, 1000);
  home();
  buttons();
  insertionProcess();
  if (is_insertion_process) {
    if (waiting_timer > 1000) {
      waiting_counter++;
      waiting_timer = 0;
      if (waiting_counter >= total_waiting_time) {
        waiting_counter = 0;
        current_amount = 0;
        is_insertion_process = false;
        isCoinInserted = false;
      }
    }
  }
  else {
    waiting_counter = 0;
  }

  digitalWrite(switch_1, is_switch_1_charging ? 0 : 1);
  digitalWrite(switch_2, is_switch_2_charging ? 0 : 1);
  digitalWrite(switch_3, is_switch_3_charging ? 0 : 1);

  countdown();
  buzzer_sound();
}

void insertionProcess() {
  if (is_insertion_process) {
    if (getFreeSwitch() == 1 && !is_switch_1_charging) {
      lcd.setCursor(0, 0);
      lcd.print("OK, amount:");
      lcd.print(current_amount);

      if (auto_start_timer_1 > 1000) {
        auto_waiting_counter_1++;
        auto_start_timer_1 = 0;
        if (auto_waiting_counter_1 >= auto_waiting_time) {
          auto_waiting_counter_1 = 0;
          delay(250);
          startProcess(1, current_amount);

          is_insertion_process = false;
          waiting_counter = 0;
          current_amount = 0;
        }
      }
    }
    else if (getFreeSwitch() == 2 && !is_switch_2_charging) {
      lcd.setCursor(0, 1);
      lcd.print("OK, amount:");
      lcd.print(current_amount);
      if (auto_start_timer_2 > 1000) {
        auto_waiting_counter_2++;
        auto_start_timer_2 = 0;
        if (auto_waiting_counter_2 >= auto_waiting_time) {
          auto_waiting_counter_2 = 0;
          delay(250);
          startProcess(2, current_amount);

          is_insertion_process = false;
          waiting_counter = 0;
          current_amount = 0;
        }
      }
    }
    else if (getFreeSwitch() == 3 && !is_switch_3_charging) {
      lcd.setCursor(0, 2);
      lcd.print("OK, amount:");
      lcd.print(current_amount);
      if (auto_start_timer_3 > 1000) {
        auto_waiting_counter_3++;
        auto_start_timer_3 = 0;
        if (auto_waiting_counter_3 >= auto_waiting_time) {
          auto_waiting_counter_3 = 0;
          delay(250);
          startProcess(3, current_amount);

          is_insertion_process = false;
          waiting_counter = 0;
          current_amount = 0;

        }
      }
    }
  }
}

int getFreeSwitch() {
  if (!is_switch_1_charging && !is_switch_2_charging && !is_switch_3_charging) {
    return 1;
  }
  else if (!is_switch_1_charging && !is_switch_2_charging && is_switch_3_charging) {
    return 1;
  }
  else if (!is_switch_1_charging && is_switch_2_charging && !is_switch_3_charging) {
    return 1;
  }
  else if (!is_switch_1_charging && is_switch_2_charging && is_switch_3_charging) {
    return 1;
  }
  else if (is_switch_1_charging && !is_switch_2_charging && !is_switch_3_charging) {
    return 2;
  }
  else if (is_switch_1_charging && !is_switch_2_charging && is_switch_3_charging) {
    return 2;
  }
  else if (is_switch_1_charging && is_switch_2_charging && !is_switch_3_charging) {
    return 3;
  }
  else if (is_switch_1_charging && is_switch_2_charging && is_switch_3_charging) {
    return 0;
  }
  else return 0;
}

void home() {
  if (!is_insertion_process) {
    if (!is_switch_1_charging && !was_charging_1_complete) {
      lcd.setCursor(0, 0);
      lcd.print("Insert coin         ");
      digitalWrite(led_1, 1);
    }
    if (!is_switch_2_charging && !was_charging_2_complete) {
      lcd.setCursor(0, 1);
      lcd.print("Insert coin         ");
      digitalWrite(led_2, 1);
    }
    if (!is_switch_3_charging && !was_charging_3_complete) {
      lcd.setCursor(0, 2);
      lcd.print("Insert coin         ");
      digitalWrite(led_3, 1);
    }
    if (!is_switch_1_charging || !is_switch_2_charging || !is_switch_3_charging) {
      lcd.setCursor(0, 3);
      lcd.print(" ** INSERT  COIN ** ");
    }
    if (is_switch_1_charging && is_switch_2_charging && is_switch_3_charging) {
      lcd.setCursor(0, 3);
      lcd.print(" WAIT FOR FREE PORT ");
    }
  }
  if (is_insertion_process) {
    lcd.setCursor(0, 3);
    lcd.print("  --- PRESS OK ---  ");
  }

}

void coinInserted() {
  Serial.println("coin inserted");
  if (getFreeSwitch() != 0) {
    isCoinInserted = true;
    is_insertion_process = true;
    current_amount = current_amount + 50;
    waiting_counter = 0;

    auto_waiting_counter_1 = 0;
    auto_waiting_counter_2 = 0;
    auto_waiting_counter_3 = 0;

    auto_start_timer_1 = 0;
    auto_start_timer_2 = 0;
    auto_start_timer_3 = 0;
  }
}

void buttons() {
  if (is_insertion_process) {
    if (digitalRead(ok_button) == 0) {
      delay(250);
      if (digitalRead(ok_button) == 1) {
        if (getFreeSwitch() == 1) {
          startProcess(1, current_amount);
        }
        else if (getFreeSwitch() == 2) {
          startProcess(2, current_amount);
        }
        else if (getFreeSwitch() == 3) {
          startProcess(3, current_amount);
        }

        is_insertion_process = false;
        waiting_counter = 0;
        current_amount = 0;
      }
    }
  }
}

void startProcess(int switch_no, long total_amount) {
  if (switch_no == 1) {
    total_time_1 = (total_amount * UNIT_SECONDS) / UNIT_AMOUNT;
    is_switch_1_charging = true;
  }
  else if (switch_no == 2) {
    total_time_2 = (total_amount * UNIT_SECONDS) / UNIT_AMOUNT;
    is_switch_2_charging = true;
  }
  else if (switch_no == 3) {
    total_time_3 = (total_amount * UNIT_SECONDS) / UNIT_AMOUNT;
    is_switch_3_charging = true;
  }
}

void countdown() {
  if (is_switch_1_charging) {
    if (countdown_timer_1 > 1000) {
      countdown_timer_1 = 0;

      second_1--;
      total_time_1--;
      hour_1 = total_time_1 / 3600;
      minute_1 = (total_time_1 - (hour_1 * 3600)) / 60;
      second_1 = total_time_1 - ((hour_1 * 3600) + (minute_1 * 60));

      //digitalWrite(led_1, !digitalRead(led_1));
      if (total_time_1 == 0) {
        is_switch_1_charging = false;
        buzzer_count = 1;
        was_charging_1_complete = true;
      }
    }

    if (led_timer_1 > 500) {
      led_timer_1 = 0;
      digitalWrite(led_1, !digitalRead(led_1));
    }

    lcd.setCursor(0, 0);
    lcd.print("Charging... ");
    if (hour_1 < 10) {
      lcd.print("0");
    }
    lcd.print(hour_1);
    lcd.print(":");
    if (minute_1 < 10) {
      lcd.print("0");
    }
    lcd.print(minute_1);
    lcd.print(":");
    if (second_1 < 10) {
      lcd.print("0");
    }
    lcd.print(second_1);
  }
  if (is_switch_2_charging) {
    if (countdown_timer_2 > 1000) {
      countdown_timer_2 = 0;

      second_2--;
      total_time_2--;
      hour_2 = total_time_2 / 3600;
      minute_2 = (total_time_2 - (hour_2 * 3600)) / 60;
      second_2 = total_time_2 - ((hour_2 * 3600) + (minute_2 * 60));

      if (total_time_2 == 0) {
        is_switch_2_charging = false;
        buzzer_count = 1;
        was_charging_2_complete = true;
      }
    }

    if (led_timer_2 > 500) {
      led_timer_2 = 0;
      digitalWrite(led_2, !digitalRead(led_2));
    }

    lcd.setCursor(0, 1);
    lcd.print("Charging... ");
    if (hour_2 < 10) {
      lcd.print("0");
    }
    lcd.print(hour_2);
    lcd.print(":");
    if (minute_2 < 10) {
      lcd.print("0");
    }
    lcd.print(minute_2);
    lcd.print(":");
    if (second_2 < 10) {
      lcd.print("0");
    }
    lcd.print(second_2);
  }
  if (is_switch_3_charging) {
    if (countdown_timer_3 > 1000) {
      countdown_timer_3 = 0;

      second_3--;
      total_time_3--;
      hour_3 = total_time_3 / 3600;
      minute_3 = (total_time_3 - (hour_3 * 3600)) / 60;
      second_3 = total_time_3 - ((hour_3 * 3600) + (minute_3 * 60));

      if (total_time_3 == 0) {
        is_switch_3_charging = false;
        buzzer_count = 1;
        was_charging_3_complete = true;
      }
    }

    if (led_timer_3 > 500) {
      led_timer_3 = 0;
      digitalWrite(led_3, !digitalRead(led_3));
    }

    lcd.setCursor(0, 2);
    lcd.print("Charging... ");
    if (hour_3 < 10) {
      lcd.print("0");
    }
    lcd.print(hour_3);
    lcd.print(":");
    if (minute_3 < 10) {
      lcd.print("0");
    }
    lcd.print(minute_3);
    lcd.print(":");
    if (second_3 < 10) {
      lcd.print("0");
    }
    lcd.print(second_3);
  }
}

void buzzer_sound() {
  if (buzzer_count > 0) {
    if (buzzer_timer > 500) {
      buzzer_timer = 0;
      buzzer_count++;
      make_sound = !make_sound;
      if (make_sound) {
        tone(buzzer, 1000);
        if (was_charging_1_complete) {
          lcd.setCursor(0, 0);
        }
        if (was_charging_2_complete) {
          lcd.setCursor(0, 1);
        }
        if (was_charging_3_complete) {
          lcd.setCursor(0, 2);
        }
        lcd.print("Charging complete :)");
      }
      else {
        noTone(buzzer);
      }

      if (buzzer_count >= 15) {
        buzzer_count = 0;
        make_sound = false;
        noTone(buzzer);
        if (was_charging_1_complete) {
          was_charging_1_complete = false;
        }
        if (was_charging_2_complete) {
          was_charging_2_complete = false;
        }
        if (was_charging_3_complete) {
          was_charging_3_complete = false;
        }
      }
    }
  }
  else {
    buzzer_timer = 0;
    make_sound = false;
    noTone(buzzer);
  }
}