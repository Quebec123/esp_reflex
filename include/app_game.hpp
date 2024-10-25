#ifndef ESP_REFLEX_APP_GAME_HPP
#define ESP_REFLEX_APP_GAME_HPP

#include <cstdint>

// todo: isrs for buttons, receiving queue from isr, game logic, led output
// todo: ensure that when player reaches 99 points the game ends

namespace app::game {

struct FinalScore {
  uint8_t player1;
  uint8_t player2;
};

void wait_for_start_press() noexcept;
void play() noexcept;

[[nodiscard]] FinalScore get_last_final_score() noexcept;

//  static void attach_isr_gpio() noexcept;
//  static void detach_isr_gpio() noexcept;
//
//  static void IRAM_ATTR isr_gpio(void* arg) noexcept;
//
//  QueueHandle_t m_queue_gpio_input;

}    // namespace app::game

#endif    //ESP_REFLEX_APP_GAME_HPP
