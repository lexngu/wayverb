#include "waveguide/make_transparent.h"
#include "waveguide/mesh.h"
#include "waveguide/postprocessor/microphone.h"
#include "waveguide/preprocessor/single_soft_source.h"
#include "waveguide/waveguide.h"

#include "common/progress_bar.h"
#include "common/spatial_division/voxelised_scene_data.h"

#include "gtest/gtest.h"

#include <algorithm>

TEST(waveguide_init, waveguide_init) {
    const compute_context cc{};

    auto scene_data{geo::get_scene_data(geo::box{glm::vec3{-1}, glm::vec3{1}})};
    scene_data.set_surfaces(make_surface(0.999, 0.999));

    const voxelised_scene_data voxelised{
            scene_data, 5, util::padded(scene_data.get_aabb(), glm::vec3{0.1})};

    constexpr glm::vec3 centre{0, 0, 0};

    const aligned::vector<float> input(20, 1);
    auto transparent{waveguide::make_transparent(input)};

    constexpr auto steps{100};
    transparent.resize(steps, 0);

    constexpr auto speed_of_sound{340.0};
    constexpr auto acoustic_impedance{400.0};

    const auto model{
            waveguide::compute_mesh(cc, voxelised, 0.04, speed_of_sound)};
    const auto receiver_index{compute_index(model.get_descriptor(), centre)};

    progress_bar pb(std::cout, steps);
    const auto output{waveguide::run(cc,
                                     model,
                                     receiver_index,
                                     transparent,
                                     receiver_index,
                                     speed_of_sound,
                                     acoustic_impedance,
                                     [&](auto) { pb += 1; })};

    ASSERT_EQ(transparent.size(), output.size());

    for (auto i{0u}; i != input.size(); ++i) {
        ASSERT_NEAR(output[i].pressure, input[i], 0.00001);
    }
}