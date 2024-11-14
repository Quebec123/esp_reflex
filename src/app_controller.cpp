#include "app_controller.hpp"
#define MCP23017_GPIOA 0x12
#include "app_game.hpp"
#include "config.hpp"
#include "global.hpp"
#include "led_patterns/app_led_pattern.hpp"
#include "led_patterns/app_led_pattern_check.hpp"
#include "led_patterns/app_led_pattern_end.hpp"
#include "led_patterns/app_led_pattern_general.hpp"
#include "led_patterns/app_led_pattern_start.hpp"
#include <Adafruit_MCP23X17.h>
#include <driver/gpio.h>
#include <esp32-hal-gpio.h>
#include <esp_log.h>
#include <esp_random.h>
#include <hal/gpio_types.h>

#include <Arduino.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <thread>
namespace app::controller {

namespace impl {

[[nodiscard]] static Adafruit_MCP23X17& get_mcp_players() noexcept {
  static Adafruit_MCP23X17 s_mcp_players;
  return s_mcp_players;
}

[[nodiscard]] static Adafruit_MCP23X17& get_mcp_seg_player1() noexcept {
  static Adafruit_MCP23X17 s_mcp_seg_player1;
  return s_mcp_seg_player1;
}

[[nodiscard]] static Adafruit_MCP23X17& get_mcp_seg_player2() noexcept {
  static Adafruit_MCP23X17 s_mcp_seg_player2;
  return s_mcp_seg_player2;
}

[[nodiscard]] static Adafruit_MCP23X17& get_mcp_seg_timer() noexcept {
  static Adafruit_MCP23X17 s_mcp_seg_timer;
  return s_mcp_seg_timer;
}

[[nodiscard]] constexpr static std::array<bool, 7> get_segment_for_digit(
uint8_t digit) noexcept {
  switch (digit) {
    case 0:
      return {true, true, true, true, true, true, false};
    case 1:
      return {false, true, true, false, false, false, false};
    case 2:
      return {true, true, false, true, true, false, true};
    case 3:
      return {true, true, true, true, false, false, true};
    case 4:
      return {false, true, true, false, false, true, true};
    case 5:
      return {true, false, true, true, false, true, true};
    case 6:
      return {true, false, true, true, true, true, true};
    case 7:
      return {true, true, true, false, false, false, false};
    case 8:
      return {true, true, true, true, true, true, true};
    case 9:
      return {true, true, true, true, false, true, true};
    default:
      return {false, false, false, false, false, false, false};
  }
}

static void init_gpio() {
  gpio_set_direction(static_cast<gpio_num_t>(config::gpio::start_out),
                     GPIO_MODE_OUTPUT);
  gpio_set_level(static_cast<gpio_num_t>(config::gpio::start_out), LOW);

  for (const uint8_t pin : config::gpio::player1_in) {
    gpio_set_direction(static_cast<gpio_num_t>(pin), GPIO_MODE_INPUT);
    gpio_set_intr_type(static_cast<gpio_num_t>(pin), GPIO_INTR_NEGEDGE);
  }

  for (const uint8_t pin : config::gpio::player2_in) {
    gpio_set_direction(static_cast<gpio_num_t>(pin), GPIO_MODE_INPUT);
    gpio_set_intr_type(static_cast<gpio_num_t>(pin), GPIO_INTR_NEGEDGE);
  }

  gpio_set_direction(static_cast<gpio_num_t>(config::gpio::start_in),
                     GPIO_MODE_INPUT);
  gpio_set_intr_type(static_cast<gpio_num_t>(config::gpio::start_in),
                     GPIO_INTR_NEGEDGE);

  gpio_install_isr_service(0);
}

static void init_random() {
  const uint32_t seed = esp_random();    //true random from entropy
  randomSeed(seed);
}

static void init_i2c_devices() noexcept {
  bool success = true;

  if (!get_mcp_players().begin_I2C(config::i2c::address_game_leds)) {

    ESP_LOGE("MCP",
             "Failed to initialize MCP for game leds, i2c address: 0x%x",
             config::i2c::address_game_leds);

    success = false;
  }
  else{
    for(int i=0;i<16;i++){
      get_mcp_players().pinMode(i,OUTPUT);
    }
  }

  if (!get_mcp_seg_player1().begin_I2C(config::i2c::address_player1_seg)) {
    ESP_LOGE("MCP",
             "Failed to initialize MCP for player1 seg, i2c address: 0x%x",
             config::i2c::address_player1_seg);

    success = false;
  }
  else{
  for(int i=0;i<16;i++){
    get_mcp_seg_player1().pinMode(i,OUTPUT);
  }}
  if (!get_mcp_seg_player2().begin_I2C(config::i2c::address_player2_seg)) {
    ESP_LOGE("MCP",
             "Failed to initialize MCP for player2 seg, i2c address: 0x%x",
             config::i2c::address_player2_seg);

    success = false;
  }
  else{
    for(int i=0;i<16;i++){
      get_mcp_seg_player2().pinMode(i,OUTPUT);
    }
  }
  if (!get_mcp_seg_timer().begin_I2C(config::i2c::address_time_seg)) {
    ESP_LOGE("MCP",
             "Failed to initialize MCP for timer seg, i2c address: 0x%x",
             config::i2c::address_time_seg);

    success = false;
  }
  else{
    for(int i=0;i<16;i++){
      get_mcp_seg_timer().pinMode(i,OUTPUT);
    }}

  if (!success) {
    ESP_LOGE("MCP", "Failed to initialize all MCP devices, rebooting in 3s");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    std::terminate();
  }

}

template<size_t StageCount>
static void execute_led_pattern(const LedPattern<StageCount>& pattern,
                                std::atomic_bool& stop_token) noexcept {
  if (pattern.empty()) {
    ESP_LOGE("LedPattern", "Pattern is empty");
    return;
  }

  if (stop_token.load()) {
    ESP_LOGW("LedPattern", "Stop token is set, not executing pattern");
    return;
  }

  controller::gpio::all_off();

  for (const auto& [stage, delayMs] : pattern) {
    stage();

    if (util::wait_stop_token(delayMs, stop_token)) {
      controller::gpio::all_off();
      return;
    }
  }

  controller::gpio::all_off();
}

}    // namespace impl

[[noreturn]] void take_control_no_return() noexcept {
  impl::init_gpio();
  impl::init_random();
  impl::init_i2c_devices();

  std::atomic_bool stop_token = false;

  ESP_LOGE("TEST", "EXECUTING CHECK PATTERN");
  impl::execute_led_pattern(led_pattern::check, stop_token);

  while (true) {
    std::thread general_pattern_thread {[&stop_token]() noexcept {
      while (true) {
        ESP_LOGE("TEST", "EXECUTING GENERAL PATTERN");
        impl::execute_led_pattern(led_pattern::general, stop_token);

        if (stop_token.load()) {
          return;
        }
      }
    }};

    app::game::wait_for_start_press();

    stop_token.store(true);
    general_pattern_thread.join();
    stop_token.store(false);

    ESP_LOGE("TEST", "EXECUTING START PATTERN");
    impl::execute_led_pattern(led_pattern::start, stop_token);

    app::game::play();

    ESP_LOGE("TEST", "EXECUTING END PATTERN");
    impl::execute_led_pattern(led_pattern::end, stop_token);
  }
}

namespace gpio {

void turn_on(const uint8_t pin, Output output) noexcept {
  switch (output) {
    case Output::Players:
      ESP_LOGE("TEST", "TURNING ON PIN %u", static_cast<unsigned int>(pin));
      impl::get_mcp_players().digitalWrite(pin, HIGH);
      break;
    case Output::SegPlayer1:
      impl::get_mcp_seg_player1().digitalWrite(pin, HIGH);
      break;
    case Output::SegPlayer2:
      impl::get_mcp_seg_player2().digitalWrite(pin, HIGH);
      break;
    case Output::SegTimer:
      impl::get_mcp_seg_timer().digitalWrite(pin, HIGH);
      break;
    case Output::Gpio:
      gpio_set_level(static_cast<gpio_num_t>(pin), HIGH);
      break;
  }
}

void turn_off(const uint8_t pin, Output output) noexcept {
  switch (output) {
    case Output::Players:
      impl::get_mcp_players().digitalWrite(pin, LOW);
      break;
    case Output::SegPlayer1:
      impl::get_mcp_seg_player1().digitalWrite(pin, LOW);
      break;
    case Output::SegPlayer2:
      impl::get_mcp_seg_player2().digitalWrite(pin, LOW);
      break;
    case Output::SegTimer:
      impl::get_mcp_seg_timer().digitalWrite(pin, LOW);
      break;
    case Output::Gpio:
      gpio_set_level(static_cast<gpio_num_t>(pin), LOW);
      break;
  }
}

void turn_on_row(Player player, Row row) noexcept {
  switch (player) {
    case Player::Player1:
      switch (row) {
        case Row::Bottom:
          turn_on(config::mcp::player1_out_left_bottom, Output::Players);
          turn_on(config::mcp::player1_out_right_bottom, Output::Players);
          break;
        case Row::MiddleBottom:
          turn_on(config::mcp::player1_out_left_middle_bottom, Output::Players);
          turn_on(config::mcp::player1_out_right_middle_bottom,
                  Output::Players);
          break;
        case Row::MiddleTop:
          turn_on(config::mcp::player1_out_left_middle_top, Output::Players);
          turn_on(config::mcp::player1_out_right_middle_top, Output::Players);
          break;
        case Row::Top:
          turn_on(config::mcp::player1_out_left_top, Output::Players);
          turn_on(config::mcp::player1_out_right_top, Output::Players);
          break;
      }
      break;
    case Player::Player2:
      switch (row) {
        case Row::Bottom:
          turn_on(config::mcp::player2_out_left_bottom, Output::Players);
          turn_on(config::mcp::player2_out_right_bottom, Output::Players);
          break;
        case Row::MiddleBottom:
          turn_on(config::mcp::player2_out_left_middle_bottom, Output::Players);
          turn_on(config::mcp::player2_out_right_middle_bottom,
                  Output::Players);
          break;
        case Row::MiddleTop:
          turn_on(config::mcp::player2_out_left_middle_top, Output::Players);
          turn_on(config::mcp::player2_out_right_middle_top, Output::Players);
          break;
        case Row::Top:
          turn_on(config::mcp::player2_out_left_top, Output::Players);
          turn_on(config::mcp::player2_out_right_top, Output::Players);
          break;
      }
      break;
  }
}

void turn_off_row(Player player, Row row) noexcept {
  switch (player) {
    case Player::Player1:
      switch (row) {
        case Row::Bottom:
          turn_off(config::mcp::player1_out_left_bottom, Output::Players);
          turn_off(config::mcp::player1_out_right_bottom, Output::Players);
          break;
        case Row::MiddleBottom:
          turn_off(config::mcp::player1_out_left_middle_bottom,
                   Output::Players);
          turn_off(config::mcp::player1_out_right_middle_bottom,
                   Output::Players);
          break;
        case Row::MiddleTop:
          turn_off(config::mcp::player1_out_left_middle_top, Output::Players);
          turn_off(config::mcp::player1_out_right_middle_top, Output::Players);
          break;
        case Row::Top:
          turn_off(config::mcp::player1_out_left_top, Output::Players);
          turn_off(config::mcp::player1_out_right_top, Output::Players);
          break;
      }
      break;
    case Player::Player2:
      switch (row) {
        case Row::Bottom:
          turn_off(config::mcp::player2_out_left_bottom, Output::Players);
          turn_off(config::mcp::player2_out_right_bottom, Output::Players);
          break;
        case Row::MiddleBottom:
          turn_off(config::mcp::player2_out_left_middle_bottom,
                   Output::Players);
          turn_off(config::mcp::player2_out_right_middle_bottom,
                   Output::Players);
          break;
        case Row::MiddleTop:
          turn_off(config::mcp::player2_out_left_middle_top, Output::Players);
          turn_off(config::mcp::player2_out_right_middle_top, Output::Players);
          break;
        case Row::Top:
          turn_off(config::mcp::player2_out_left_top, Output::Players);
          turn_off(config::mcp::player2_out_right_top, Output::Players);
          break;
      }
      break;
  }
}

void all_off() noexcept {
  for (const uint8_t& pin : config::mcp::player1_out) {
    turn_off(pin, Output::Players);
  }
  for (const uint8_t& pin : config::mcp::player2_out) {
    turn_off(pin, Output::Players);
  }
  for (const uint8_t& pin : config::mcp::seg_left_pins) {
    turn_off(pin, Output::SegPlayer1);
    turn_off(pin, Output::SegPlayer2);
    turn_off(pin, Output::SegTimer);
  }
  for (const uint8_t& pin : config::mcp::seg_right_pins) {
    turn_off(pin, Output::SegPlayer1);
    turn_off(pin, Output::SegPlayer2);
    turn_off(pin, Output::SegTimer);
  }
  turn_off(config::gpio::start_out, Output::Gpio);
}

void display_segment_number(uint8_t number, SegmentDisplay display) noexcept {
  if (number > 99) {
    ESP_LOGE("LedPattern", "Number out of range, displaying max number");
    number = 99;
  }

  const uint8_t tens = number / 10;
  const uint8_t ones = number % 10;

  for (size_t i = 0; i < 7; ++i) {
    switch (display) {
      case SegmentDisplay::Player1:
        impl::get_segment_for_digit(tens).at(i)
        ? turn_on(config::mcp::seg_left_pins.at(i), Output::SegPlayer1)
        : turn_off(config::mcp::seg_left_pins.at(i), Output::SegPlayer1);

        impl::get_segment_for_digit(ones).at(i)
        ? turn_on(config::mcp::seg_right_pins.at(i), Output::SegPlayer1)
        : turn_off(config::mcp::seg_right_pins.at(i), Output::SegPlayer1);

        break;
      case SegmentDisplay::Player2:
        impl::get_segment_for_digit(tens).at(i)
        ? turn_on(config::mcp::seg_left_pins.at(i), Output::SegPlayer2)
        : turn_off(config::mcp::seg_left_pins.at(i), Output::SegPlayer2);

        impl::get_segment_for_digit(ones).at(i)
        ? turn_on(config::mcp::seg_right_pins.at(i), Output::SegPlayer2)
        : turn_off(config::mcp::seg_right_pins.at(i), Output::SegPlayer2);

        break;
      case SegmentDisplay::Timer:
        impl::get_segment_for_digit(tens).at(i)
        ? turn_on(config::mcp::seg_left_pins.at(i), Output::SegTimer)
        : turn_off(config::mcp::seg_left_pins.at(i), Output::SegTimer);

        impl::get_segment_for_digit(ones).at(i)
        ? turn_on(config::mcp::seg_right_pins.at(i), Output::SegTimer)
        : turn_off(config::mcp::seg_right_pins.at(i), Output::SegTimer);

        break;
    }
  }
}

void turn_off_segment(SegmentDisplay display) noexcept {
  for (const uint8_t& pin : config::mcp::seg_left_pins) {
    switch (display) {
      case SegmentDisplay::Player1:
        turn_off(pin, Output::SegPlayer1);
        break;
      case SegmentDisplay::Player2:
        turn_off(pin, Output::SegPlayer2);
        break;
      case SegmentDisplay::Timer:
        turn_off(pin, Output::SegTimer);
        break;
    }
  }
  for (const uint8_t& pin : config::mcp::seg_right_pins) {
    switch (display) {
      case SegmentDisplay::Player1:
        turn_off(pin, Output::SegPlayer1);
        break;
      case SegmentDisplay::Player2:
        turn_off(pin, Output::SegPlayer2);
        break;
      case SegmentDisplay::Timer:
        turn_off(pin, Output::SegTimer);
        break;
    }
  }
}

}    // namespace gpio

namespace util {

[[nodiscard]] gpio::PlayerPins get_random_player_pins(Player player) noexcept {
  const uint8_t random_index =
  static_cast<const uint8_t>(random(config::gpio::player1_in.size()));

  if (player == Player::Player1) {
    return {config::gpio::player1_in.at(random_index),
            config::mcp::player1_out.at(random_index)};
  } else {
    return {config::gpio::player2_in.at(random_index),
            config::mcp::player2_out.at(random_index)};
  }
}

[[nodiscard]] bool wait_stop_token(
const uint32_t&         delayMs,
const std::atomic_bool& stop_token) noexcept {
  if (stop_token.load()) {
    return true;
  }

  if (delayMs == 0) {
    return false;
  }

  for (uint32_t wait_time  = 0; wait_time < delayMs;
       wait_time          += config::min_delay) {
    if (stop_token.load()) {
      return true;
    }

    vTaskDelay(config::min_delay / portTICK_PERIOD_MS);
  }

  return false;
}

}    // namespace util

}    // namespace app::controller
