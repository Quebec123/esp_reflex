#ifndef ESP_REFLEX_CONFIG_HPP
#define ESP_REFLEX_CONFIG_HPP

#include <array>
#include <cstdint>

namespace config {

constexpr inline uint32_t min_delay = 10;

}    // namespace config

namespace config::game {

constexpr inline unsigned int input_queue_size      = 10;
constexpr inline unsigned int input_queue_item_size = sizeof(uint8_t);
constexpr inline uint8_t      max_score             = 99;
constexpr inline uint8_t      game_time             = 30;
constexpr inline uint32_t     game_wait_for_input   = 50;

}    // namespace config::game

namespace config::i2c {

constexpr inline uint8_t i2c_sda = 21;
constexpr inline uint8_t i2c_scl = 22;

constexpr inline uint8_t address_game_leds   = 0x20;
constexpr inline uint8_t address_player1_seg = 0x21;
constexpr inline uint8_t address_time_seg    = 0x22;
constexpr inline uint8_t address_player2_seg = 0x24;

}    // namespace config::i2c

namespace config::gpio {

constexpr inline uint8_t player1_in_left_bottom         = 4;
constexpr inline uint8_t player1_in_left_middle_bottom  = 13;
constexpr inline uint8_t player1_in_left_middle_top     = 16;
constexpr inline uint8_t player1_in_left_top            = 17;
constexpr inline uint8_t player1_in_right_bottom        = 18;
constexpr inline uint8_t player1_in_right_middle_bottom = 19;
constexpr inline uint8_t player1_in_right_middle_top    = 23;
constexpr inline uint8_t player1_in_right_top           = 25;

constexpr inline uint8_t player2_in_left_bottom         = 26;
constexpr inline uint8_t player2_in_left_middle_bottom  = 27;
constexpr inline uint8_t player2_in_left_middle_top     = 32;
constexpr inline uint8_t player2_in_left_top            = 33;
constexpr inline uint8_t player2_in_right_bottom        = 34;
constexpr inline uint8_t player2_in_right_middle_bottom = 35;
constexpr inline uint8_t player2_in_right_middle_top    = 36;
constexpr inline uint8_t player2_in_right_top           = 39;

constexpr inline uint8_t start_in  = 5;
constexpr inline uint8_t start_out = 12;

// the left column is 0 to 3 from bottom to top, the right column is 4 to 7 from bottom to top
constexpr inline std::array<uint8_t, 8> player1_in = {
  player1_in_left_bottom,
  player1_in_left_middle_bottom,
  player1_in_left_middle_top,
  player1_in_left_top,
  player1_in_right_bottom,
  player1_in_right_middle_bottom,
  player1_in_right_middle_top,
  player1_in_right_top};

// the left column is 0 to 3 from bottom to top, the right column is 4 to 7 from bottom to top
constexpr inline std::array<uint8_t, 8> player2_in = {
  player2_in_left_bottom,
  player2_in_left_middle_bottom,
  player2_in_left_middle_top,
  player2_in_left_top,
  player2_in_right_bottom,
  player2_in_right_middle_bottom,
  player2_in_right_middle_top,
  player2_in_right_top};

}    // namespace config::gpio

namespace config::mcp {

constexpr inline uint8_t player1_out_left_bottom         = 0;
constexpr inline uint8_t player1_out_left_middle_bottom  = 1;
constexpr inline uint8_t player1_out_left_middle_top     = 2;
constexpr inline uint8_t player1_out_left_top            = 3;
constexpr inline uint8_t player1_out_right_bottom        = 4;
constexpr inline uint8_t player1_out_right_middle_bottom = 5;
constexpr inline uint8_t player1_out_right_middle_top    = 6;
constexpr inline uint8_t player1_out_right_top           = 7;

constexpr inline uint8_t player2_out_left_bottom         = 8;
constexpr inline uint8_t player2_out_left_middle_bottom  = 9;
constexpr inline uint8_t player2_out_left_middle_top     = 10;
constexpr inline uint8_t player2_out_left_top            = 11;
constexpr inline uint8_t player2_out_right_bottom        = 12;
constexpr inline uint8_t player2_out_right_middle_bottom = 13;
constexpr inline uint8_t player2_out_right_middle_top    = 14;
constexpr inline uint8_t player2_out_right_top           = 15;

constexpr inline uint8_t seg_left_pin_a = 0;
constexpr inline uint8_t seg_left_pin_b = 1;
constexpr inline uint8_t seg_left_pin_c = 2;
constexpr inline uint8_t seg_left_pin_d = 3;
constexpr inline uint8_t seg_left_pin_e = 4;
constexpr inline uint8_t seg_left_pin_f = 5;
constexpr inline uint8_t seg_left_pin_g = 6;

constexpr inline uint8_t seg_right_pin_a = 8;
constexpr inline uint8_t seg_right_pin_b = 9;
constexpr inline uint8_t seg_right_pin_c = 10;
constexpr inline uint8_t seg_right_pin_d = 11;
constexpr inline uint8_t seg_right_pin_e = 12;
constexpr inline uint8_t seg_right_pin_f = 13;
constexpr inline uint8_t seg_right_pin_g = 14;

// the left column is 0 to 3 from bottom to top, the right column is 4 to 7 from bottom to top
constexpr inline std::array<uint8_t, 8> player1_out = {
  player1_out_left_bottom,
  player1_out_left_middle_bottom,
  player1_out_left_middle_top,
  player1_out_left_top,
  player1_out_right_bottom,
  player1_out_right_middle_bottom,
  player1_out_right_middle_top,
  player1_out_right_top};

// the left column is 0 to 3 from bottom to top, the right column is 4 to 7 from bottom to top
constexpr inline std::array<uint8_t, 8> player2_out = {
  player2_out_left_bottom,
  player2_out_left_middle_bottom,
  player2_out_left_middle_top,
  player2_out_left_top,
  player2_out_right_bottom,
  player2_out_right_middle_bottom,
  player2_out_right_middle_top,
  player2_out_right_top};

// a to g from 0 to 6
constexpr inline std::array<uint8_t, 7> seg_left_pins = {seg_left_pin_a,
                                                         seg_left_pin_b,
                                                         seg_left_pin_c,
                                                         seg_left_pin_d,
                                                         seg_left_pin_e,
                                                         seg_left_pin_f,
                                                         seg_left_pin_g};

// a to g from 0 to 6
constexpr inline std::array<uint8_t, 7> seg_right_pins = {seg_right_pin_a,
                                                          seg_right_pin_b,
                                                          seg_right_pin_c,
                                                          seg_right_pin_d,
                                                          seg_right_pin_e,
                                                          seg_right_pin_f,
                                                          seg_right_pin_g};

}    // namespace config::mcp

#endif    //ESP_REFLEX_CONFIG_HPP
