#ifndef ESP_REFLEX_APP_LED_PATTERN_START_HPP
#define ESP_REFLEX_APP_LED_PATTERN_START_HPP

#include "app_controller.hpp"
#include "config.hpp"
#include "global.hpp"
#include "led_patterns/app_led_pattern.hpp"

namespace app::led_pattern {

constexpr inline LedPattern<5> start = {
  {{[]() noexcept {
      controller::gpio::turn_on(config::mcp::seg_left_pin_g, Output::SegTimer);
      controller::gpio::turn_on(config::mcp::seg_right_pin_g, Output::SegTimer);

      controller::gpio::display_segment_number(5, SegmentDisplay::Player1);
      controller::gpio::display_segment_number(5, SegmentDisplay::Player2);
    },
    500},
   {[]() noexcept {
      controller::gpio::turn_on_row(Player::Player1, Row::Bottom);
      controller::gpio::turn_on_row(Player::Player2, Row::Bottom);

      controller::gpio::display_segment_number(4, SegmentDisplay::Player1);
      controller::gpio::display_segment_number(4, SegmentDisplay::Player2);
    },
    500},
   {[]() noexcept {
      controller::gpio::turn_on_row(Player::Player1, Row::MiddleBottom);
      controller::gpio::turn_on_row(Player::Player2, Row::MiddleBottom);

      controller::gpio::display_segment_number(3, SegmentDisplay::Player1);
      controller::gpio::display_segment_number(3, SegmentDisplay::Player2);
    },
    500},
   {[]() noexcept {
      controller::gpio::turn_on_row(Player::Player1, Row::MiddleTop);
      controller::gpio::turn_on_row(Player::Player2, Row::MiddleTop);

      controller::gpio::display_segment_number(2, SegmentDisplay::Player1);
      controller::gpio::display_segment_number(2, SegmentDisplay::Player2);
    },
    500},
   {[]() noexcept {
      controller::gpio::turn_on_row(Player::Player1, Row::Top);
      controller::gpio::turn_on_row(Player::Player2, Row::Top);

      controller::gpio::display_segment_number(1, SegmentDisplay::Player1);
      controller::gpio::display_segment_number(1, SegmentDisplay::Player2);
    },
    500}}
};

}    // namespace app::led_pattern

#endif    //ESP_REFLEX_APP_LED_PATTERN_START_HPP
