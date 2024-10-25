#ifndef ESP_REFLEX_APP_LED_PATTERN_GENERAL_HPP
#define ESP_REFLEX_APP_LED_PATTERN_GENERAL_HPP

#include "app_controller.hpp"
#include "config.hpp"
#include "global.hpp"
#include "led_patterns/app_led_pattern.hpp"

#include <cstdint>

namespace app::led_pattern {

constexpr inline LedPattern<1> general = {
  {{[]() noexcept {
      const uint8_t p1_pin =
      controller::util::get_random_player_pins(Player::Player1).pin_out;
      const uint8_t p2_pin =
      controller::util::get_random_player_pins(Player::Player2).pin_out;

      controller::gpio::turn_on(p1_pin, Output::Players);
      controller::gpio::turn_on(p2_pin, Output::Players);

      controller::gpio::turn_on(config::gpio::start_out, Output::Gpio);
    },
    500}}};

}    // namespace app::led_pattern

#endif    //ESP_REFLEX_APP_LED_PATTERN_GENERAL_HPP
