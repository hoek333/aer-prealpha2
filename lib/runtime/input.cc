#include "runtime/input.hh"
#include <raylib.h>


std::optional<double>
aer::InputPlatformAdapter::get_timestamp(const std::atomic<Music> *music) {
  std::optional<double> timestamp;
  if (music != nullptr && IsMusicValid(*music)) {
    timestamp = GetMusicTimePlayed(*music) * 1000;
  } else {
    timestamp = std::nullopt;
  }
  return timestamp;
}


aer::InputManager::InputManager(size_t queue_size, std::atomic<Music> *music)
    : music(music)
    , queue(queue_size)
    , adapter()
    , thread() {
  thread = std::jthread(&InputManager::run, this);
}


void aer::InputManager::run(std::stop_token stop) {
  while (!stop.stop_requested()) {
    if (adapter != nullptr && polling) {
      adapter->poll_input(queue, music);
    }
  }
}


std::vector<aer::InputEvent> aer::InputManager::consume_events() {
  float now = GetMusicTimePlayed(*music) * 1000;
  std::vector<InputEvent> ret;
  if (queue.empty()) return ret;
  ret.reserve(32);

  while (true) {
    auto *ie = queue.front();
    if (ie == nullptr || ie->timestamp > now) break;
    ret.push_back(*ie);
    queue.pop();
  }

  return ret;
}
