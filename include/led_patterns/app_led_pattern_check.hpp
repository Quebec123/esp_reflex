#ifndef ESP_REFLEX_APP_LED_PATTERN_CHECK_HPP
#define ESP_REFLEX_APP_LED_PATTERN_CHECK_HPP

#include "app_controller.hpp"
#include "config.hpp"
#include "global.hpp"
#include "led_patterns/app_led_pattern.hpp"

#include <cstddef>
#include <cstdint>

namespace app::led_pattern {

constexpr inline LedPattern<1> check = {
  {{[]() noexcept {
      for (const uint8_t pin : config::mcp::player1_out) {
        controller::gpio::turn_on(pin, Output::Players);
      }
      for (const uint8_t pin : config::mcp::player2_out) {
        controller::gpio::turn_on(pin, Output::Players);
      }
      for (const uint8_t pin : config::mcp::seg_left_pins) {
        controller::gpio::turn_on(pin, Output::SegPlayer1);
        controller::gpio::turn_on(pin, Output::SegPlayer2);
        controller::gpio::turn_on(pin, Output::SegTimer);
      }
      for (const uint8_t pin : config::mcp::seg_right_pins) {
        controller::gpio::turn_on(pin, Output::SegPlayer1);
        controller::gpio::turn_on(pin, Output::SegPlayer2);
        controller::gpio::turn_on(pin, Output::SegTimer);
      }
      controller::gpio::turn_on(config::gpio::start_out, Output::Gpio);
    },
    3000}}};

}    // namespace app::led_pattern

#endif    //ESP_REFLEX_APP_LED_PATTERN_CHECK_HPP
