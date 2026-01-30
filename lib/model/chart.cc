#include "model/chart.hh"
#include "core/fraction.hh"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>


constexpr float aer::bpm_to_ms(float bpm) { return 60000 / bpm; }


auto aer::Chart::view_kind(chart::ElementKind kind) {
  return elements | std::ranges::views::filter([&](const chart::Element &e) {
           return e.kind == kind;
         });
}


auto aer::Chart::view_kind_vec(chart::ElementKind kind) {
  auto f = view_kind(kind);
  return std::vector<std::reference_wrapper<chart::Element>>(f.begin(),
                                                             f.end());
}


auto aer::Chart::view_window(float from, float to, chart::ElementKind kind) {
  return elements //
         | std::ranges::views::filter([&](chart::Element &e) {
             if (kind != chart::ElementKind::NONE) {
               return e.kind == kind;
             } else {
               return true;
             }
           }) //
         | std::ranges::views::filter([&](chart::Element &e) {
             return e.timestamp >= from && e.timestamp < to;
           });
}


void aer::Chart::sort() {
  std::sort(elements.begin(), elements.end(),
            [](const chart::Element &lhs, const chart::Element &rhs) {
              return lhs.beat < rhs.beat;
            });
}


void aer::Chart::calculate_timestamps() {
  // first element must be a BPM
  assert(elements.begin()->kind == chart::ElementKind::BPM);
  assert(elements.begin()->beat == 0);

  // timestamp bpms
  auto bpms_view = view_kind(chart::ElementKind::BPM);
  std::vector<std::reference_wrapper<chart::Element>> bpms(bpms_view.begin(),
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
      | std::ranges::views::filter([](chart::Element &e) {
          return e.kind != chart::ElementKind::BPM;
        }) //
      | std::ranges::views::transform([&](chart::Element &e) {
          auto last_bpm = std::prev(std::upper_bound(
              bpms.begin(), bpms.end(), e.beat,
              [](const Fraction &beat, const chart::Element &bpm_elm) {
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
