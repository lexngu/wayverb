#include "ModelRendererComponent.hpp"

#include "Application.hpp"
#include "CommandIDs.hpp"
#include "OtherComponents/MoreConversions.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "combined/model.h"

ModelRendererComponent::ModelRendererComponent(
        const scene_data &model,
        model::ValueWrapper<int> &shown_surface,
        model::ValueWrapper<model::App> &app,
        model::ValueWrapper<model::RenderState> &render_state)
        : model(model)
        , shown_surface(shown_surface)
        , renderer([ model, sos = app.speed_of_sound.get() ] {
            return SceneRendererContextLifetime{model, sos};
        })
        , app(app)
        , render_state(render_state) {
    set_help("model viewport",
             "This area displays the currently loaded 3D model. Click and drag "
             "to rotate the model, or use the mouse wheel to zoom in and out.");
    addAndMakeVisible(renderer);
}

void ModelRendererComponent::resized() { renderer.setBounds(getLocalBounds()); }

void ModelRendererComponent::set_positions(
        const aligned::vector<glm::vec3> &positions) {
    renderer.context_command([=](auto &i) { i.set_positions(positions); });
}

void ModelRendererComponent::set_pressures(
        const aligned::vector<float> &pressures, float current_time) {
    renderer.context_command(
            [=](auto &i) { i.set_pressures(pressures, current_time); });
}

void ModelRendererComponent::set_impulses(
        const aligned::vector<aligned::vector<impulse>> &impulses,
        const glm::vec3 &source,
        const glm::vec3 &receiver) {
    renderer.context_command(
            [=](auto &i) { i.set_impulses(impulses, source, receiver); });
}

void ModelRendererComponent::receive_broadcast(model::Broadcaster *cb) {
    if (cb == &shown_surface) {
        send_highlighted();
    } else if (cb == &app.source) {
        send_sources();
    } else if (cb == &app.receiver_settings) {
        send_receivers();
    } else if (cb == &render_state.is_rendering) {
        send_is_rendering();
    }
}

void ModelRendererComponent::send_highlighted() {
    renderer.context_command([s = shown_surface.get()](auto &i) {
        i.set_highlighted(s);
    });
}

void ModelRendererComponent::send_sources() {
    renderer.context_command([s = app.source.get()](auto &i) {
        i.set_sources(s);
    });
}

void ModelRendererComponent::send_receivers() {
    renderer.context_command([s = app.receiver_settings.get()](auto &i) {
        i.set_receivers(s);
    });
}

void ModelRendererComponent::send_is_rendering() {
    renderer.context_command([s = render_state.is_rendering.get()](auto &i) {
        i.set_rendering(s);
    });
}

void ModelRendererComponent::renderer_open_gl_context_created(
        const Renderer *r) {
    if (r == &renderer) {
        send_highlighted();
        send_sources();
        send_receivers();
        send_is_rendering();
    }
}

void ModelRendererComponent::renderer_open_gl_context_closing(
        const Renderer *r) {
    //  don't care
}

/*
void ModelRendererComponent::source_dragged(
        SceneRenderer *, const aligned::vector<glm::vec3> &v) {
    assert(app.source.get().size() == v.size());
    for (auto i = 0u; i != v.size(); ++i) {
        app.source[i].set(glm::clamp(
                v[i], model.get_aabb().get_c0(), model.get_aabb().get_c1()));
    }
}
void ModelRendererComponent::receiver_dragged(
        SceneRenderer *, const aligned::vector<glm::vec3> &v) {
    assert(app.receiver_settings.get().size() == v.size());
    for (auto i = 0u; i != v.size(); ++i) {
        app.receiver_settings[i].position.set(glm::clamp(
                v[i], model.get_aabb().get_c0(), model.get_aabb().get_c1()));
    }
}
*/

void ModelRendererComponent::left_panel_debug_show_closest_surfaces(
        const LeftPanel *) {
    generator = std::make_unique<MeshGenerator>(
            model,
            get_waveguide_sample_rate(app.get()),
            app.speed_of_sound.get(),
            [this](auto model) {
                renderer.context_command([m = std::move(model)](auto &i) {
                    i.debug_show_closest_surfaces(std::move(m));
                });
            });
}

void ModelRendererComponent::left_panel_debug_show_boundary_types(
        const LeftPanel *) {
    generator = std::make_unique<MeshGenerator>(
            model,
            get_waveguide_sample_rate(app.get()),
            app.speed_of_sound.get(),
            [this](auto model) {
                renderer.context_command([m = std::move(model)](auto &i) {
                    i.debug_show_boundary_types(std::move(m));
                });
            });
}

void ModelRendererComponent::left_panel_debug_hide_debug_mesh(
        const LeftPanel *) {
    generator = nullptr;
    renderer.context_command([](auto &i) { i.debug_hide_model(); });
}

//----------------------------------------------------------------------------//

ModelRendererComponent::MeshGenerator::MeshGenerator(
        const scene_data &scene,
        double sample_rate,
        double speed_of_sound,
        std::function<void(waveguide::mesh)>
                on_finished)
        : on_finished(on_finished) {
    generator.run(scene, sample_rate, speed_of_sound);
}

void ModelRendererComponent::MeshGenerator::async_mesh_generator_finished(
        const AsyncMeshGenerator *, waveguide::mesh model) {
    on_finished(std::move(model));
}