#ifndef ESP_REFLEX_GLOBAL_HPP
#define ESP_REFLEX_GLOBAL_HPP

#include <cstdint>

namespace app {

enum class SegmentDisplay : uint8_t {
  Player1,
  Player2,
  Timer
};

enum class Output : uint8_t {
  Players,
  SegPlayer1,
  SegPlayer2,
  SegTimer,
  Gpio
};

enum class Player : uint8_t {
  Player1,
  Player2
};

enum class Row : uint8_t {
  Bottom,
  MiddleBottom,
  MiddleTop,
  Top
};

}    // namespace app

#endif    //ESP_REFLEX_GLOBAL_HPP
