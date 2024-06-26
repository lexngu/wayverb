#include "core/reverb_time.h"
#include "core/cl/scene_structs.h"
#include "core/geo/box.h"
#include "core/scene_data_loader.h"

#include "gtest/gtest.h"

#ifndef OBJ_PATH
#define OBJ_PATH ""
#endif

#ifndef OBJ_PATH_TUNNEL
#define OBJ_PATH_TUNNEL ""
#endif

#ifndef OBJ_PATH_BEDROOM
#define OBJ_PATH_BEDROOM ""
#endif

#ifndef OBJ_PATH_BAD_BOX
#define OBJ_PATH_BAD_BOX ""
#endif

using namespace wayverb::core;

const util::aligned::vector<const char*> test_scenes{
        OBJ_PATH, OBJ_PATH_TUNNEL, OBJ_PATH_BEDROOM, OBJ_PATH_BAD_BOX};

const util::aligned::vector<geo::box> test_boxes{
        geo::box{glm::vec3{-1}, glm::vec3{0}},
        geo::box{glm::vec3{-1}, glm::vec3{1}},
        geo::box{glm::vec3{0}, glm::vec3{1}},
        geo::box{glm::vec3{-10}, glm::vec3{-9}},
        geo::box{glm::vec3{9}, glm::vec3{10}},
        geo::box{glm::vec3{0, 1, 2}, glm::vec3{3, 4, 5}}};

TEST(reverb_time, triangles_are_oriented) {
    {
        util::aligned::vector<triangle> triangles{{0, 0, 1, 2}};
        ASSERT_TRUE(triangles_are_oriented(triangles.begin(), triangles.end()));
    }

    {
        util::aligned::vector<triangle> triangles{{0, 0, 1, 2}, {0, 0, 2, 3}};
        ASSERT_TRUE(triangles_are_oriented(triangles.begin(), triangles.end()));
    }

    {
        util::aligned::vector<triangle> triangles{{0, 0, 1, 2}, {0, 0, 3, 2}};
        ASSERT_FALSE(
                triangles_are_oriented(triangles.begin(), triangles.end()));
    }

    for (const auto& fname : test_scenes) {
        const auto scene = scene_data_loader{fname}.get_scene_data();
        const auto& triangles = scene->get_triangles();
        ASSERT_TRUE(triangles_are_oriented(triangles.begin(), triangles.end()));
    }

    for (const auto& box : test_boxes) {
        const auto scene = geo::get_scene_data(
                box, make_surface<simulation_bands>(0.5, 0.5));
        const auto& triangles = scene.get_triangles();
        ASSERT_TRUE(triangles_are_oriented(triangles.begin(), triangles.end()));
    }
}

TEST(reverb_time, area) {
    {
        const geo::box box{glm::vec3{-1}, glm::vec3{0}};
        ASSERT_EQ(area(geo::get_scene_data(box, 0)), 6);
    }

    {
        const geo::box box{glm::vec3{-1}, glm::vec3{1}};
        ASSERT_EQ(area(geo::get_scene_data(box, 0)), 24);
    }

    {
        const geo::box box{glm::vec3{0}, glm::vec3{1}};
        ASSERT_EQ(area(geo::get_scene_data(box, 0)), 6);
    }
}

TEST(reverb_time, room_volume) {
    for (const auto& box : test_boxes) {
        const auto scene = geo::get_scene_data(box, 0);
        const auto& triangles = scene.get_triangles();
        ASSERT_TRUE(triangles_are_oriented(triangles.begin(), triangles.end()));
        const auto dim = dimensions(box);
        ASSERT_NEAR(estimate_room_volume(scene), dim.x * dim.y * dim.z, 0.0001);
    }

    for (const auto& fname : test_scenes) {
        const scene_data_loader loader{fname};
        const auto& scene = loader.get_scene_data();
        const auto volume = estimate_room_volume(*scene);
        std::cout << "volume of model: " << fname << " is: " << volume << '\n';
    }
}

TEST(reverb_time, sabine) {
    const geo::box southern_box{glm::vec3{0, 0, 0},
                                glm::vec3{5.56, 3.97, 2.81}};
    auto southern_scene = geo::get_scene_data(
            southern_box, make_surface<simulation_bands>(0, 0));

    {
        const bands_type air_absorption{};
        for (auto i = 0u; i != 10; ++i) {
            const auto reflection_coefficient = i * 0.1;
            southern_scene.set_surfaces(make_surface<simulation_bands>(
                    1 - pow(reflection_coefficient, 2), 0));
            const auto t30 =
                    sabine_reverb_time(southern_scene, air_absorption) / 2;
            std::cout << "reflection: " << reflection_coefficient
                      << ", t30: " << t30.s[4] << '\n';
        }
    }

    for (const auto& box : test_boxes) {
        const auto air_absorption = 0.0;
        const auto t0 = sabine_reverb_time(
                geo::get_scene_data(box, make_surface<1>(0.0001, 0)),
                air_absorption);

        const auto t1 = sabine_reverb_time(
                geo::get_scene_data(box, make_surface<1>(0.001, 0)),
                air_absorption);

        const auto t2 = sabine_reverb_time(
                geo::get_scene_data(box, make_surface<1>(0.01, 0)),
                air_absorption);

        const auto t3 = sabine_reverb_time(
                geo::get_scene_data(box, make_surface<1>(0.1, 0)),
                air_absorption);

        ASSERT_TRUE(all(t1 < t0));
        ASSERT_TRUE(all(t2 < t1));
        ASSERT_TRUE(all(t3 < t2));
    }
}
