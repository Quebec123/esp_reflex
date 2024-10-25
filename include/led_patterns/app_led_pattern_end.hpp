#ifndef ESP_REFLEX_APP_LED_PATTERN_END_HPP
#define ESP_REFLEX_APP_LED_PATTERN_END_HPP

#include "app_controller.hpp"
#include "app_game.hpp"
#include "config.hpp"
#include "global.hpp"
#include "led_patterns/app_led_pattern.hpp"

namespace app::led_pattern {

constexpr inline LedPattern<12> end = {
  {{[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      controller::gpio::display_segment_number(score_p1,
                                               SegmentDisplay::Player1);
      controller::gpio::display_segment_number(score_p2,
                                               SegmentDisplay::Player2);

      controller::gpio::display_segment_number(0, SegmentDisplay::Timer);
    },
    300},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_on_row(Player::Player1, Row::Bottom);
        controller::gpio::turn_off_segment(SegmentDisplay::Player1);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_on_row(Player::Player2, Row::Bottom);
        controller::gpio::turn_off_segment(SegmentDisplay::Player2);
      } else {
        controller::gpio::turn_on_row(Player::Player1, Row::Bottom);
        controller::gpio::turn_on_row(Player::Player2, Row::Bottom);
        controller::gpio::turn_off_segment(SegmentDisplay::Player1);
        controller::gpio::turn_off_segment(SegmentDisplay::Player2);
      }

      controller::gpio::turn_off_segment(SegmentDisplay::Timer);
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_on_row(Player::Player1, Row::MiddleBottom);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_on_row(Player::Player2, Row::MiddleBottom);
      } else {
        controller::gpio::turn_on_row(Player::Player1, Row::MiddleBottom);
        controller::gpio::turn_on_row(Player::Player2, Row::MiddleBottom);
      }
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_on_row(Player::Player1, Row::MiddleTop);
        controller::gpio::display_segment_number(score_p1,
                                                 SegmentDisplay::Player1);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_on_row(Player::Player2, Row::MiddleTop);
        controller::gpio::display_segment_number(score_p2,
                                                 SegmentDisplay::Player2);
      } else {
        controller::gpio::turn_on_row(Player::Player1, Row::MiddleTop);
        controller::gpio::turn_on_row(Player::Player2, Row::MiddleTop);
        controller::gpio::display_segment_number(score_p1,
                                                 SegmentDisplay::Player1);
        controller::gpio::display_segment_number(score_p2,
                                                 SegmentDisplay::Player2);
      }

      controller::gpio::display_segment_number(0, SegmentDisplay::Timer);
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_on_row(Player::Player1, Row::Top);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_on_row(Player::Player2, Row::Top);
      } else {
        controller::gpio::turn_on_row(Player::Player1, Row::Top);
        controller::gpio::turn_on_row(Player::Player2, Row::Top);
      }
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_off_row(Player::Player1, Row::Bottom);
        controller::gpio::turn_off_segment(SegmentDisplay::Player1);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_off_row(Player::Player2, Row::Bottom);
        controller::gpio::turn_off_segment(SegmentDisplay::Player2);
      } else {
        controller::gpio::turn_off_row(Player::Player1, Row::Bottom);
        controller::gpio::turn_off_row(Player::Player2, Row::Bottom);
        controller::gpio::turn_off_segment(SegmentDisplay::Player1);
        controller::gpio::turn_off_segment(SegmentDisplay::Player2);
      }

      controller::gpio::turn_off_segment(SegmentDisplay::Timer);
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_off_row(Player::Player1, Row::MiddleBottom);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_off_row(Player::Player2, Row::MiddleBottom);
      } else {
        controller::gpio::turn_off_row(Player::Player1, Row::MiddleBottom);
        controller::gpio::turn_off_row(Player::Player2, Row::MiddleBottom);
      }
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_off_row(Player::Player1, Row::MiddleTop);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_off_row(Player::Player2, Row::MiddleTop);
      } else {
        controller::gpio::turn_off_row(Player::Player1, Row::MiddleTop);
        controller::gpio::turn_off_row(Player::Player2, Row::MiddleTop);
      }

      controller::gpio::display_segment_number(score_p1,
                                               SegmentDisplay::Player1);
      controller::gpio::display_segment_number(score_p2,
                                               SegmentDisplay::Player2);

      controller::gpio::display_segment_number(0, SegmentDisplay::Timer);
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      if (score_p1 > score_p2) {
        controller::gpio::turn_off_row(Player::Player1, Row::Top);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_off_row(Player::Player2, Row::Top);
      } else {
        controller::gpio::turn_off_row(Player::Player1, Row::Top);
        controller::gpio::turn_off_row(Player::Player2, Row::Top);
      }
    },
    150},
   {[]() noexcept {
      const auto [score_p1, score_p2] = app::game::get_last_final_score();

      controller::gpio::turn_off_segment(SegmentDisplay::Timer);

      if (score_p1 > score_p2) {
        controller::gpio::turn_on(config::mcp::seg_left_pin_f,
                                  Output::SegTimer);
        controller::gpio::turn_on(config::mcp::seg_left_pin_e,
                                  Output::SegTimer);
      } else if (score_p2 > score_p1) {
        controller::gpio::turn_on(config::mcp::seg_right_pin_b,
                                  Output::SegTimer);
        controller::gpio::turn_on(config::mcp::seg_right_pin_c,
                                  Output::SegTimer);
      } else {
        controller::gpio::turn_on(config::mcp::seg_left_pin_f,
                                  Output::SegTimer);
        controller::gpio::turn_on(config::mcp::seg_left_pin_e,
                                  Output::SegTimer);
        controller::gpio::turn_on(config::mcp::seg_right_pin_b,
                                  Output::SegTimer);
        controller::gpio::turn_on(config::mcp::seg_right_pin_c,
                                  Output::SegTimer);
      }

      controller::gpio::turn_on_row(Player::Player1, Row::MiddleBottom);
      controller::gpio::turn_on_row(Player::Player2, Row::MiddleBottom);
      controller::gpio::turn_on_row(Player::Player1, Row::MiddleTop);
      controller::gpio::turn_on_row(Player::Player2, Row::MiddleTop);
    },
    900},
   {[]() noexcept {
      controller::gpio::turn_off_row(Player::Player1, Row::MiddleBottom);
      controller::gpio::turn_off_row(Player::Player2, Row::MiddleBottom);
      controller::gpio::turn_off_row(Player::Player1, Row::MiddleTop);
      controller::gpio::turn_off_row(Player::Player2, Row::MiddleTop);
      controller::gpio::turn_on_row(Player::Player1, Row::Top);
      controller::gpio::turn_on_row(Player::Player2, Row::Top);
      controller::gpio::turn_on_row(Player::Player1, Row::Bottom);
      controller::gpio::turn_on_row(Player::Player2, Row::Bottom);
    },
    900},
   {[]() noexcept {
      controller::gpio::turn_off_row(Player::Player1, Row::Top);
      controller::gpio::turn_off_row(Player::Player2, Row::Top);
      controller::gpio::turn_off_row(Player::Player1, Row::Bottom);
      controller::gpio::turn_off_row(Player::Player2, Row::Bottom);
    },
    900}}
};

}    // namespace app::led_pattern

#endif    //ESP_REFLEX_APP_LED_PATTERN_END_HPP
