#include "game/chart.hh"
#include "core/fraction.hh"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>


auto aer::Chart::view_kind(ChartElementKind kind) {
  return elements | std::ranges::views::filter(
                        [&](const ChartElement &e) { return e.kind == kind; });
}


auto aer::Chart::view_kind_vec(ChartElementKind kind) {
  auto f = view_kind(kind);
  return std::vector<std::reference_wrapper<ChartElement>>(f.begin(), f.end());
}


auto aer::Chart::view_window(float from, float to, ChartElementKind kind) {
  return elements //
         | std::ranges::views::filter([&](ChartElement &e) {
             if (kind != ChartElementKind::NONE) {
               return e.kind == kind;
             } else {
               return true;
             }
           }) //
         | std::ranges::views::filter([&](ChartElement &e) {
             return e.timestamp >= from && e.timestamp < to;
           });
}


void aer::Chart::sort() {
  std::sort(elements.begin(), elements.end(),
            [](const ChartElement &lhs, const ChartElement &rhs) {
              return lhs.beat < rhs.beat;
            });
}


void aer::Chart::calculate_timestamps() {
  // first element must be a BPM
  assert(elements.begin()->kind == ChartElementKind::BPM);
  assert(elements.begin()->beat == 0);

  // timestamp bpms
  auto bpms_view = view_kind(ChartElementKind::BPM);
  std::vector<std::reference_wrapper<ChartElement>> bpms(bpms_view.begin(),
                                                         bpms_view.end());
  double current = offset;
  for (auto it = bpms.begin(); it != bpms.end(); it++) {
    it->get().timestamp = current;
    if (it + 1 != bpms.end()) {
      Fraction beat_length = (it + 1)->get().beat - it->get().beat;
      current += beat_length.flatten() * bpm_to_ms(it->get().bpm);
    }
  }

  // timestamp everything else
  elements //
      | std::ranges::views::filter(
            [](ChartElement &e) { return e.kind != ChartElementKind::BPM; }) //
      | std::ranges::views::transform([&](ChartElement &e) {
          auto last_bpm = std::prev(std::upper_bound(
              bpms.begin(), bpms.end(), e.beat,
              [](const Fraction &beat, const ChartElement &bpm_elm) {
                return beat < bpm_elm.beat;
              }));

          Fraction beat_length = e.beat - last_bpm->get().beat;
          e.timestamp = last_bpm->get().timestamp +
                        beat_length.flatten() * bpm_to_ms(last_bpm->get().bpm);
          return e;
        });
}


void aer::Chart::prepare() {
  sort();
  calculate_timestamps();
}


constexpr float aer::bpm_to_ms(float bpm) { return 60000 / bpm; }
