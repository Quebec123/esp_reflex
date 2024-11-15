#include "app_game.hpp"

#include "app_controller.hpp"
#include "config.hpp"
#include "global.hpp"
#include <driver/gpio.h>
#include <esp_attr.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/projdefs.h>
#include <freertos/queue.h>
#include <hal/gpio_types.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <limits>
#include <thread>

namespace app::game {
namespace impl {

using QueueStorage = std::array<uint8_t,
                                config::game::input_queue_item_size *
                                config::game::input_queue_size>;

[[nodiscard]] static QueueHandle_t& get_gpio_queue() noexcept {
  static StaticQueue_t s_static_queue_handle = {};
  static QueueStorage  s_queue_storage       = {};

  static QueueHandle_t s_queue_handle =
  xQueueCreateStatic(config::game::input_queue_size,
                     config::game::input_queue_item_size,
                     s_queue_storage.data(),
                     &s_static_queue_handle);

  return s_queue_handle;
}

static void IRAM_ATTR isr_buttons_gpio(void* gpio_arg) noexcept {
  const auto gpio_num =
  static_cast<uint8_t>(reinterpret_cast<ptrdiff_t>(gpio_arg));

  BaseType_t higher_priority_task_woken = pdFALSE;
  xQueueSendFromISR(get_gpio_queue(), &gpio_num, &higher_priority_task_woken);

  if (higher_priority_task_woken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

[[nodiscard]] static FinalScore& get_final_score() noexcept {
  static FinalScore s_final_score = {config::game::max_score,
                                     config::game::max_score};
  return s_final_score;
}

[[nodiscard]] static uint8_t generate_random_player_pin(
uint8_t current) noexcept {
  uint8_t pin = current;

  while (pin == current) {
    pin = random() % config::gpio::player1_in.size();
  }

  return pin;
}

static void attach_isr_start() noexcept {
  gpio_isr_handler_add(static_cast<gpio_num_t>(config::gpio::start_in),
                       isr_buttons_gpio,
                       reinterpret_cast<void*>(config::gpio::start_in));
}

static void detach_isr_start() noexcept {
  gpio_isr_handler_remove(static_cast<gpio_num_t>(config::gpio::start_in));
}

static void attach_isr_players() noexcept {
  for (const uint8_t& pin : config::gpio::player1_in) {
    gpio_isr_handler_add(static_cast<gpio_num_t>(pin),
                         isr_buttons_gpio,
                         reinterpret_cast<void*>(pin));
  }

  for (const uint8_t& pin : config::gpio::player2_in) {
    gpio_isr_handler_add(static_cast<gpio_num_t>(pin),
                         isr_buttons_gpio,
                         reinterpret_cast<void*>(pin));
  }
}

static void detach_isr_players() noexcept {
  for (const uint8_t& pin : config::gpio::player1_in) {
    gpio_isr_handler_remove(static_cast<gpio_num_t>(pin));
  }

  for (const uint8_t& pin : config::gpio::player2_in) {
    gpio_isr_handler_remove(static_cast<gpio_num_t>(pin));
  }
}

}    // namespace impl

void wait_for_start_press() noexcept {
  impl::attach_isr_start();

  uint8_t gpio_num = std::numeric_limits<uint8_t>::max();
  while (gpio_num != config::gpio::start_in) {
    xQueueReceive(impl::get_gpio_queue(), &gpio_num, portMAX_DELAY);
  }

  impl::detach_isr_start();
}

void play() noexcept {
  ESP_LOGE("TEST", "GAME_BEGIN");

  impl::attach_isr_players();

  uint8_t player1_target_index =
  impl::generate_random_player_pin(std::numeric_limits<uint8_t>::max());
  uint8_t player2_target_index =
  impl::generate_random_player_pin(std::numeric_limits<uint8_t>::max());

  uint8_t player1_score = 0;
  uint8_t player2_score = 0;

  std::atomic_int8_t timer            = config::game::game_time;
  std::atomic_bool   timer_stop_token = false;

  std::thread timer_thread([&timer, &timer_stop_token]() {
    controller::gpio::display_segment_number(static_cast<uint8_t>(timer),
                                             SegmentDisplay::Timer);
    //ESP_LOGE("TIMER","%u", static_cast<uint8_t>(timer));
    while (timer > 0) {
      if (controller::util::wait_stop_token(1000, timer_stop_token)) {
        break;
      }

      --timer;
      controller::gpio::display_segment_number(static_cast<uint8_t>(timer),
                                               SegmentDisplay::Timer);

    }
  });

  controller::gpio::display_segment_number(player1_score,
                                           SegmentDisplay::Player1);
  controller::gpio::display_segment_number(player2_score,
                                           SegmentDisplay::Player2);

  controller::gpio::turn_on(config::mcp::player1_out.at(player1_target_index),
                            Output::Players);
  //ESP_LOGE("TEST","%u pin for player 1",player1_target_index);
  controller::gpio::turn_on(config::mcp::player2_out.at(player2_target_index),
                            Output::Players);
  //ESP_LOGE("TEST","%u pin for player 2",player2_target_index);
  while (player1_score < config::game::max_score &&
         player2_score < config::game::max_score && timer > 0) {
    uint8_t gpio_num = std::numeric_limits<uint8_t>::max();
    xQueueReceive(impl::get_gpio_queue(),
                  &gpio_num,
                  config::game::game_wait_for_input / portTICK_PERIOD_MS);

    if (timer == 0) {
      break;
    }

    if (gpio_num == config::gpio::player1_in.at(player1_target_index)) {
      controller::gpio::turn_off(
      config::mcp::player1_out.at(player1_target_index),
      Output::Players);

      ++player1_score;

      if (timer == 0) {
        break;
      }

      controller::gpio::display_segment_number(player1_score,
                                               SegmentDisplay::Player1);
     ESP_LOGE("TEST", "%u score", player1_score);
      player1_target_index =
      impl::generate_random_player_pin(player1_target_index);

      if (timer == 0) {
        break;
      }

      controller::gpio::turn_on(
      config::mcp::player1_out.at(player1_target_index),
      Output::Players);
    } else if (gpio_num == config::gpio::player2_in.at(player2_target_index)) {
      controller::gpio::turn_off(
      config::mcp::player2_out.at(player2_target_index),
      Output::Players);

      ++player2_score;

      if (timer == 0) {
        break;
      }

      controller::gpio::display_segment_number(player2_score,
                                               SegmentDisplay::Player2);
     // ESP_LOGE("TEST","%u score", player2_score);
      player2_target_index =
      impl::generate_random_player_pin(player2_target_index);

      if (timer == 0) {
        break;
      }

      controller::gpio::turn_on(
      config::mcp::player2_out.at(player2_target_index),
      Output::Players);
    }
  }

  impl::detach_isr_players();

  timer_stop_token = true;
  timer_thread.join();

  impl::get_final_score() = {player1_score, player2_score};
}

[[nodiscard]] FinalScore get_last_final_score() noexcept {
  return impl::get_final_score();
}

}    // namespace app::game
