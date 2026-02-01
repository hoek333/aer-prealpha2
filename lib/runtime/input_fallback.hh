#pragma once
#include "runtime/input.hh"
#include <rigtorp/SPSCQueue.h>
namespace aer {


  /**
   * @class InputFallbackAdapter
   * @brief Fallback input polling device. This adapter does not detect any
   * input. Use this with @ref InputManager
   *
   */
  class InputFallbackAdapter : public InputPlatformAdapter {
  public:
    InputFallbackAdapter();

    InputFallbackAdapter(const InputFallbackAdapter &) = delete;
    InputFallbackAdapter &operator=(const InputFallbackAdapter &other) = delete;
    InputFallbackAdapter(InputFallbackAdapter &&) noexcept = default;
    InputFallbackAdapter &operator=(InputFallbackAdapter &&) noexcept = default;

    void poll_input(rigtorp::SPSCQueue<InputEvent> &queue,
                    const std::atomic<Music> *music) override;
  };


} // namespace aer
