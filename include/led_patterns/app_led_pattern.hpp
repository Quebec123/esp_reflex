#ifndef ESP_REFLEX_APP_LED_PATTERN_HPP
#define ESP_REFLEX_APP_LED_PATTERN_HPP

#include <array>
#include <cstddef>

namespace app::led_pattern {

using StageFn         = void (*)() noexcept;
using DelayAfterStage = uint32_t;

}    // namespace app::led_pattern

namespace app {

template<size_t StageCount>
using LedPattern =
std::array<std::pair<led_pattern::StageFn, led_pattern::DelayAfterStage>,
           StageCount>;

}    // namespace app

#endif    //ESP_REFLEX_APP_LED_PATTERN_HPP
