#pragma once
namespace aer {


  enum struct Platform {
    UNKNOWN,
    LINUX_X11,
    LINUX_WAYLAND,
    WINDOWS,
  };


  bool has_linux_x11();
  bool has_linux_wayland();
  bool has_windows();

  Platform detect_platform();

} // namespace aer
