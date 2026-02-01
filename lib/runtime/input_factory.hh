#pragma once
#include "runtime/input.hh"
#include <memory>
namespace aer {


  /**
   * @brief Automatically detect platform and create an input manager with
   * the corresponding adapter.
   * @warning This function should be called before the raylib window is
   * initialized
   *
   * @param queue_size Size of the input queue
   */
  std::unique_ptr<InputManager> create_input_manager(size_t queue_size);


} // namespace aer
