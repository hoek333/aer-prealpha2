#include "runtime/_utils.hh"
#include <raylib.h>


bool aer::is_raylib_window_focused() { return IsWindowFocused(); }
