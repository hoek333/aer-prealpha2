#include "core/platform.hh"
#include <cstdlib>


bool aer::has_linux_x11() {
#ifdef AER_HAS_LIB_X11
  return std::getenv("DISPLAY") != nullptr;
#else
  return false;
#endif
}


bool aer::has_linux_wayland() {
  return std::getenv("WAYLAND_DISPLAY") != nullptr;
}


bool aer::has_windows() {
#if defined(_WIN32)
  return true;
#else
  return false;
#endif
}


aer::Platform aer::detect_platform() {
#if defined(__linux__)
  if (has_linux_x11()) {
    return Platform::LINUX_X11;
  } else if (has_linux_wayland()) {
    return Platform::LINUX_WAYLAND;
  } else {
    return Platform::UNKNOWN;
  }
#endif

#if defined(_WIN32)
  return Platform::WINDOWS;
#endif

  // fallback
  return Platform::UNKNOWN;
}
