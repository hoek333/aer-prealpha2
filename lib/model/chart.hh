#pragma once
#include "core/fraction.hh"
#include <string>
#include <vector>
namespace aer {


  namespace chart {


    enum struct ElementKind {
      NONE,
      NOTE,
      BPM,
      MARKER,
    };


    struct Element {
      Fraction beat;
      float timestamp;
      ElementKind kind;

      Fraction note_length;
      float bpm;
      std::string marker_name;
    };


  } // namespace chart


  /**
   * @brief Get the length of a beat in ms with the provided BPM
   * @param bpm
   * @return length of a beat
   */
  constexpr float bpm_to_ms(float bpm);


  class Chart {
  public:
    int lanes;
    float offset;
    std::vector<chart::Element> elements;

  public:
    /**
     * @brief Filter the chart's elements by element kind.
     * @param kind
     */
    auto view_kind(chart::ElementKind kind);

    /**
     * @copydoc view_kind
     * @note This function returns a vector of references
     */
    auto view_kind_vec(chart::ElementKind kind);

    /**
     * @brief Get a view of the elements timestamped between from included and
     * to excluded. You can also filter by element kind.
     *
     * @param from start timestamp of the window
     * @param to end timestamp of the window
     * @param kind element kind
     */
    auto view_window(float from, float to,
                     chart::ElementKind kind = chart::ElementKind::NONE);
    /**
     * @copydoc view_window
     * @note This function returns a vector of references
     */
    auto view_window_vec(float from, float to,
                         chart::ElementKind kind = chart::ElementKind::NONE);

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


} // namespace aer
