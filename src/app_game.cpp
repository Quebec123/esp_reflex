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

/**
 * @brief ISR handler for button GPIO interrupts.
 *
 * This function is called when a button GPIO interrupt occurs. It sends the GPIO number
 * to the queue and yields from the ISR if a higher priority task was woken.
 *
 * @param gpio_arg Pointer to the GPIO number that triggered the interrupt.
 */
static void IRAM_ATTR isr_buttons_gpio(void* gpio_arg) noexcept {
  // Convert the GPIO argument to a uint8_t GPIO number
  const auto gpio_num =
  static_cast<uint8_t>(reinterpret_cast<ptrdiff_t>(gpio_arg));

  BaseType_t higher_priority_task_woken = pdFALSE;
  // Send the GPIO number to the queue from the ISR
  xQueueSendFromISR(get_gpio_queue(), &gpio_num, &higher_priority_task_woken);

  // Yield from the ISR if a higher priority task was woken
  if (higher_priority_task_woken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}
/**
 * @brief Retrieves the final score of the game.
 *
 * This function returns a reference to a static `FinalScore` object that holds
 * the final scores for both players. The initial scores are set to the maximum
 * score defined in the configuration.
 *
 * @return A reference to the `FinalScore` object containing the final scores.
 */
[[nodiscard]] static FinalScore& get_final_score() noexcept {
  // Static variable to hold the final scores, initialized to max scores
  static FinalScore s_final_score = {config::game::max_score,
                                     config::game::max_score};
  return s_final_score;
}

/**
 * @brief Generates a random player pin different from the current one.
 *
 * This function generates a random pin index for a player that is different
 * from the provided current pin index. It ensures that the new pin index
 * is not the same as the current one.
 *
 * @param current The current pin index that should not be selected.
 * @return A new random pin index different from the current one.
 */
[[nodiscard]] static uint8_t generate_random_player_pin(
uint8_t current) noexcept {
  uint8_t pin = current;

  // Generate a new random pin index until it is different from the current one
  while (pin == current) {
    pin = random() % config::gpio::player1_in.size();
  }

  return pin;
}

/**
 * @brief Attaches the ISR handler for the start button GPIO interrupt.
 *
 * This function adds the ISR handler for the start button GPIO interrupt.
 * It associates the `isr_buttons_gpio` function with the GPIO number
 * specified in the configuration for the start button.
 */
static void attach_isr_start() noexcept {
  gpio_isr_handler_add(static_cast<gpio_num_t>(config::gpio::start_in),
                       isr_buttons_gpio,
                       reinterpret_cast<void*>(config::gpio::start_in));
}

/**
 * @brief Detaches the ISR handler for the start button GPIO interrupt.
 *
 * This function removes the ISR handler associated with the start button GPIO
 * interrupt, effectively disabling the interrupt for the start button.
 */
static void detach_isr_start() noexcept {
  gpio_isr_handler_remove(static_cast<gpio_num_t>(config::gpio::start_in));
}

/**
 * @brief Attaches the ISR handlers for the player buttons GPIO interrupts.
 *
 * This function adds the ISR handlers for the player buttons GPIO interrupts.
 * It associates the `isr_buttons_gpio` function with the GPIO numbers
 * specified in the configuration for both player 1 and player 2 buttons.
 */
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

/**
 * @brief Detaches the ISR handlers for the player buttons GPIO interrupts.
 *
 * This function removes the ISR handlers associated with the player buttons GPIO
 * interrupts, effectively disabling the interrupts for both player 1 and player 2 buttons.
 */
static void detach_isr_players() noexcept {
  for (const uint8_t& pin : config::gpio::player1_in) {
    gpio_isr_handler_remove(static_cast<gpio_num_t>(pin));
  }

  for (const uint8_t& pin : config::gpio::player2_in) {
    gpio_isr_handler_remove(static_cast<gpio_num_t>(pin));
  }
}

}    // namespace impl

/**
 * @brief Waits for the start button press.
 *
 * This function waits for the start button to be pressed by attaching the ISR handler
 * for the start button GPIO interrupt. It continuously checks the GPIO queue until
 * the start button GPIO number is received, indicating the button press. Once the
 * button is pressed, it detaches the ISR handler.
 */
void wait_for_start_press() noexcept {
  impl::attach_isr_start();

  uint8_t gpio_num = std::numeric_limits<uint8_t>::max();
  while (gpio_num != config::gpio::start_in) {
    xQueueReceive(impl::get_gpio_queue(), &gpio_num, portMAX_DELAY);
  }

  impl::detach_isr_start();
}

/**
 * @brief Executes the main game loop.
 *
 * This function runs the main game loop, handling the game logic for both players.
 * It initializes the ISR handlers for the player buttons, sets up the game timer,
 * and manages the game state until one of the players reaches the maximum score
 * or the timer runs out. The function also updates the display with the current
 * scores and the remaining time.
 */
void play() noexcept {
  ESP_LOGE("TEST", "GAME_BEGIN");

  // Attach ISR handlers for player buttons
  impl::attach_isr_players();

  // Generate initial random target pins for both players
  uint8_t player1_target_index =
  impl::generate_random_player_pin(std::numeric_limits<uint8_t>::max());
  uint8_t player2_target_index =
  impl::generate_random_player_pin(std::numeric_limits<uint8_t>::max());

  // Initialize player scores
  uint8_t player1_score = 0;
  uint8_t player2_score = 0;

  // Initialize game timer and stop token
  std::atomic_int8_t timer            = config::game::game_time;
  std::atomic_bool   timer_stop_token = false;

  // Start the game timer thread
  std::thread timer_thread([&timer, &timer_stop_token]() {
    controller::gpio::display_segment_number(static_cast<uint8_t>(timer),
                                             SegmentDisplay::Timer);
    while (timer > 0) {
      if (controller::util::wait_stop_token(1000, timer_stop_token)) {
        break;
      }

      --timer;
      controller::gpio::display_segment_number(static_cast<uint8_t>(timer),
                                               SegmentDisplay::Timer);
    }
  });

  // Display initial scores
  controller::gpio::display_segment_number(player1_score,
                                           SegmentDisplay::Player1);
  controller::gpio::display_segment_number(player2_score,
                                           SegmentDisplay::Player2);

  // Turn on initial target pins for both players
  controller::gpio::turn_on(config::mcp::player1_out.at(player1_target_index),
                            Output::Players);
  controller::gpio::turn_on(config::mcp::player2_out.at(player2_target_index),
                            Output::Players);

  // Main game loop
  while (player1_score < config::game::max_score &&
         player2_score < config::game::max_score && timer > 0) {
    uint8_t gpio_num = std::numeric_limits<uint8_t>::max();
    xQueueReceive(impl::get_gpio_queue(),
                  &gpio_num,
                  config::game::game_wait_for_input / portTICK_PERIOD_MS);

    if (timer == 0) {
      break;
    }

    // Check if player 1 pressed the correct button
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
      player1_target_index =
      impl::generate_random_player_pin(player1_target_index);

      if (timer == 0) {
        break;
      }

      controller::gpio::turn_on(
      config::mcp::player1_out.at(player1_target_index),
      Output::Players);
    }
    // Check if player 2 pressed the correct button
    else if (gpio_num == config::gpio::player2_in.at(player2_target_index)) {
      controller::gpio::turn_off(
      config::mcp::player2_out.at(player2_target_index),
      Output::Players);

      ++player2_score;

      if (timer == 0) {
        break;
      }

      controller::gpio::display_segment_number(player2_score,
                                               SegmentDisplay::Player2);
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

  // Detach ISR handlers for player buttons
  impl::detach_isr_players();

  // Stop the timer thread
  timer_stop_token = true;
  timer_thread.join();

  // Store the final scores
  impl::get_final_score() = {player1_score, player2_score};
}

[[nodiscard]] FinalScore get_last_final_score() noexcept {
  return impl::get_final_score();
}

}    // namespace app::game
