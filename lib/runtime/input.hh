#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <raylib.h>
#include <rigtorp/SPSCQueue.h>
#include <stop_token>
#include <thread>
#include <vector>
namespace aer {


  enum struct InputControllerKind {
    KEY,
    MOUSE,
  };


  enum struct InputEventKind {
    PRESSED,
    RELEASED,
  };


  struct InputEvent {
    InputControllerKind controller;
    InputEventKind kind;
    uint8_t code;
    std::optional<double> timestamp;
  };


  /**
   * @class InputPlatformAdapter
   * @brief Abstract class for the OS-specific parts of @ref InputManager.
   *
   */
  class InputPlatformAdapter {
  protected:
    /**
     * @brief Attempts to get a timestamp
     */
    std::optional<double> get_timestamp(const std::atomic<Music> *music);

  public:
    virtual void poll_input(rigtorp::SPSCQueue<InputEvent> &queue,
                            const std::atomic<Music> *music) = 0;
  };


  /**
   * @class InputManager
   * @brief Manages input through a queue. When any input is pressed, an event
   * is sent to the queue as soon as possible. It is then the user's job to free
   * up the queue by popping events and processing them with @ref
   * consume_events.
   *
   * The events are timestamped relative to the music's current position. If the
   * song is not playing then the timestamp will be equal to 0.
   */
  class InputManager {
    std::atomic<Music> *music;
    rigtorp::SPSCQueue<InputEvent> queue;
    std::unique_ptr<InputPlatformAdapter> adapter;
    std::jthread thread;
    bool polling = false;

  private:
    /**
     * @brief Job executed by the input thread
     * @param stop thread stop token
     */
    void run(std::stop_token stop);

  public:
    InputManager(size_t queue_size, std::atomic<Music> *music = nullptr);

    /**
     * @brief Set the manager's adapter. You must do this before you begin
     * polling.
     * @param adapter Adapter that the manager should use
     *
     * @thread-safe This is not thread-safe. Do not change the adapter while
     * polling.
     */
    void set_adapter(std::unique_ptr<InputPlatformAdapter> adapter) {
      this->adapter = std::move(adapter);
    }

    /**
     * @brief Set or replace the manager's Music pointer.
     * @param music atomic Music pointer
     */
    void set_music(std::atomic<Music> *music) { this->music = music; }

    /**
     * @brief Return the polling status of the manager
     * @return true if currently polling input, false if not
     */
    bool is_polling() const { return polling; }

    /**
     * @brief It is best to start polling after the raylib window has been
     * created. For the time being you can start polling before the window
     * creation, but we have no guarantee that this behavior will stick in
     * future Raylib versions.
     */
    void start_polling() { polling = true; }
    void pause_polling() { polling = false; }

    /**
     * @brief Consume all events older than the current time. You will typically
     * call this function at the beginning of every frame.
     * @return A vector containing all the consumed events. They are sorted from
     * oldest to newest.
     */
    std::vector<InputEvent> consume_events();

    /**
     * @brief Stop the input thread. This instance will become unusable.
     */
    void stop_thread() { thread.request_stop(); }
  };


} // namespace aer
