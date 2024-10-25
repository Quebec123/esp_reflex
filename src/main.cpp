#include "app_controller.hpp"

#include <Arduino.h>

void setup() {
}

void loop() {
  app::controller::take_control_no_return();
}
