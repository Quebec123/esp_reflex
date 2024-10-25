#ifndef ESP_REFLEX_APP_CONTROLLER_HPP
#define ESP_REFLEX_APP_CONTROLLER_HPP

#include "global.hpp"

#include <atomic>
#include <cstdint>

namespace app::controller {

[[noreturn]] void take_control_no_return() noexcept;

namespace gpio {

struct PlayerPins {
  uint8_t pin_in;
  uint8_t pin_out;
};

void turn_on(const uint8_t pin, Output output) noexcept;
void turn_off(const uint8_t pin, Output output) noexcept;
void turn_on_row(Player player, Row row) noexcept;
void turn_off_row(Player player, Row row) noexcept;
void all_off() noexcept;
void display_segment_number(uint8_t number, SegmentDisplay display) noexcept;
void turn_off_segment(SegmentDisplay display) noexcept;

}    // namespace gpio

namespace util {

[[nodiscard]] gpio::PlayerPins get_random_player_pins(Player player) noexcept;
[[nodiscard]] bool             wait_stop_token(const uint32_t&         delayMs,
                                               const std::atomic_bool& stop_token) noexcept;

}    // namespace util

}    // namespace app::controller

#endif    //ESP_REFLEX_APP_CONTROLLER_HPP
