#pragma once

#include "hrtf/entry.h"

#include "frequency_domain/multiband_filter.h"

namespace hrtf_data {

inline auto hrtf_band_params(double sample_rate) {
    constexpr range<double> audible_range{20, 20000};

    return frequency_domain::band_edges_and_widths<entry::bands>(
            audible_range / sample_rate, 0.5);
}

template <typename It, typename Callback>
void multiband_filter(It begin,
                      It end,
                      double sample_rate,
                      const Callback& callback) {
    frequency_domain::multiband_filter(
            begin, end, hrtf_band_params(sample_rate), callback);
}

template <typename It>
auto per_band_energy(It begin, It end, double sample_rate) {
    return frequency_domain::per_band_energy(
            begin, end, hrtf_band_params(sample_rate));
}

}  // namespace hrtf_data