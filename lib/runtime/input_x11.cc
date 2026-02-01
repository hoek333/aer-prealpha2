#if defined(__linux__)
#ifdef AER_HAS_LIB_X11
#include "runtime/input_x11.hh"
#include "runtime/_utils.hh"
#include "runtime/key.hh"
#include <raylib.h>
#include <rigtorp/SPSCQueue.h>

namespace x {
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
} // namespace x


void aer::init_for_input_x11_adapter() { x::XInitThreads(); }


struct aer::InputX11Adapter::Impl {
  x::Display *display;
  int xi_opcode = -1;
};


aer::InputX11Adapter::InputX11Adapter()
    : pimpl(std::make_unique<Impl>()) {
  // create display
  pimpl->display = x::XOpenDisplay(nullptr);
  if (!pimpl->display) {
    throw std::runtime_error(
        "[InputX11Adapter] Failed to open input X display");
  }

  // query XI2
  if (!x::XQueryExtension(pimpl->display, "XInputExtension", &pimpl->xi_opcode,
                          nullptr, nullptr)) {
    throw std::runtime_error("[InputX11Adapter] XInput2 not available");
  }

  // select events
  x::XIEventMask mask;
  unsigned char mask_data[XIMaskLen(XI_LASTEVENT)] = {};
  mask.deviceid = XIAllMasterDevices;
  mask.mask_len = sizeof(mask_data);
  mask.mask = mask_data;
  XISetMask(mask.mask, XI_RawKeyPress);
  XISetMask(mask.mask, XI_RawKeyRelease);
  XISetMask(mask.mask, XI_RawButtonPress);
  XISetMask(mask.mask, XI_RawButtonRelease);
  XISelectEvents(pimpl->display, x::XDefaultRootWindow(pimpl->display), &mask,
                 1);
  XFlush(pimpl->display);
}


void aer::InputX11Adapter::poll_input(rigtorp::SPSCQueue<InputEvent> &queue,
                                      const std::atomic<Music> *music) {

  x::XEvent ev;
  x::XGenericEventCookie *cookie = &ev.xcookie;
  while (x::XPending(pimpl->display) > 0) {
    x::XNextEvent(pimpl->display, &ev);

    // timestamp
    auto timestamp = get_timestamp(music);

    if (!is_raylib_window_focused()) {
      continue; // discard event if window is not focused
    }

    if (!x::XGetEventData(pimpl->display, cookie)) continue;
    uint8_t scancode = static_cast<uint8_t>(
        static_cast<x::XIRawEvent *>(cookie->data)->detail);
    switch (cookie->evtype) {
    case XI_RawKeyPress:
      queue.try_push(InputEvent{
          InputControllerKind::KEY,
          InputEventKind::PRESSED,
          scancode,
          timestamp,
      });
      break;

    case XI_RawKeyRelease:
      queue.try_push(InputEvent{
          InputControllerKind::KEY,
          InputEventKind::RELEASED,
          scancode,
          timestamp,
      });
      break;

    case XI_RawButtonPress:
      queue.try_push(InputEvent{
          InputControllerKind::MOUSE,
          InputEventKind::PRESSED,
          scancode,
          timestamp,
      });
      break;

    case XI_RawButtonRelease:
      if (scancode == (uint8_t)Button::WHEEL_UP ||
          scancode == (uint8_t)Button::WHEEL_DOWN) {
        break;
      }
      queue.try_push(InputEvent{
          InputControllerKind::MOUSE,
          InputEventKind::RELEASED,
          scancode,
          timestamp,
      });
      break;
    }

    x::XFreeEventData(pimpl->display, cookie);
  }
}


#endif
#endif
