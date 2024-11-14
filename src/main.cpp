#include "app_controller.hpp"

#include <Arduino.h>
#include<app_game.hpp>
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.print("Hello");
  //app::controller::gpio::turn_on_row(app::Player::Player1,app::Row::Bottom);
  app::controller::take_control_no_return();
}
