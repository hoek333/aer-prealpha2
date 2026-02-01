#include "runtime/input_factory.hh"
#include "core/platform.hh"
#include "runtime/input.hh"
#include "runtime/input_fallback.hh"
#include "runtime/input_x11.hh"


std::unique_ptr<aer::InputManager>
aer::create_input_manager(size_t queue_size) {
  aer::Platform platform = aer::detect_platform();
  auto ret = std::make_unique<InputManager>(queue_size);

  switch (platform) {
  case Platform::LINUX_X11:
#if defined(__linux__)
#ifdef AER_HAS_LIB_X11
    aer::init_for_input_x11_adapter();
    ret->set_adapter(std::make_unique<InputX11Adapter>());
#else
    ret->set_adapter(std::make_unique<InputFallbackAdapter>());
#endif
#else
    ret->set_adapter(std::make_unique<InputFallbackAdapter>());
#endif
    break;

  default:
    ret->set_adapter(std::make_unique<InputFallbackAdapter>());
  }

  return ret;
}
