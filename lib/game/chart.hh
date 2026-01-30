#pragma once
#include "core/fraction.hh"
#include <string>
#include <vector>
namespace aer {


  enum struct ChartElementKind {
    NONE,
    NOTE,
    BPM,
    MARKER,
  };


  struct ChartElement {
    Fraction beat;
    float timestamp;
    ChartElementKind kind;

    Fraction note_length;
    float bpm;
    std::string marker_name;
  };


  /**
   * @class Chart
   * @brief Representation of a chart.
   *
   */
  class Chart {
  public:
    int lanes;
    float offset;
    std::vector<ChartElement> elements;

  public:
    /**
     * @brief Filter the chart's elements by element kind.
     * @param kind
     */
    auto view_kind(ChartElementKind kind);

    /**
     * @copydoc view_kind
     * @note This function returns a vector of references
     */
    auto view_kind_vec(ChartElementKind kind);

    /**
     * @brief Get a view of the elements timestamped between from included and
     * to excluded. You can also filter by element kind.
     *
     * @param from start timestamp of the window
     * @param to end timestamp of the window
     * @param kind element kind
     */
    auto view_window(float from, float to,
                     ChartElementKind kind = ChartElementKind::NONE);

    /**
     * @copydoc view_window
     * @note This function returns a vector of references
     */
    auto view_window_vec(float from, float to,
                         ChartElementKind kind = ChartElementKind::NONE);

    /**
     * @brief Sort the elements by timestamp.
     */
    void sort();

    /**
     * @brief Calculate the timestamps of all elements. This assumes the chart
     * is sorted.
     */
    void calculate_timestamps();

    /**
     * @brief Sort and timestamp the elements
     */
    void prepare();
  };


  /**
   * @brief Get the length of a beat in ms with the provided BPM
   * @param bpm
   * @return length of a beat
   */
  constexpr float bpm_to_ms(float bpm);


} // namespace aer
