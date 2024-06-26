#include "master.h"

#include "../../generic_property_component.h"
#include "../../UtilityComponents/modal_dialog.h"

#include "utilities/string_builder.h"

#include <iomanip>

namespace left_bar {
namespace materials {

class bands_component final : public Component, public Slider::Listener {
public:
    bands_component(double min, double max, double inc) {
        for (auto& i : sliders_) {
            i.setSliderStyle(Slider::SliderStyle::LinearVertical);
            i.setTextBoxStyle(
                    Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 20);
            i.setRange(min, max, inc);
            i.addListener(this);
            addAndMakeVisible(i);
        }
    }

    void set(const wayverb::core::bands_type& bands) {
        for (auto i = 0; i != wayverb::core::simulation_bands; ++i) {
            sliders_[i].setValue(bands.s[i], dontSendNotification);
        }
    }

    wayverb::core::bands_type get() const {
        wayverb::core::bands_type ret;
        for (auto i = 0; i != wayverb::core::simulation_bands; ++i) {
            ret.s[i] = sliders_[i].getValue();
        }
        return ret;
    }

    void resized() override {
        const auto bounds = getLocalBounds();

        auto total_width = [this](auto i) {
            return i * getWidth() / sliders_.size();
        };

        const auto width = total_width(1);

        for (auto i = 0u; i != sliders_.size(); ++i) {
            sliders_[i].setBounds(total_width(i), 0, width, getHeight());
        }
    }

    void sliderValueChanged(Slider*) override { on_change_(get()); }

    using on_change = util::event<wayverb::core::bands_type>;
    on_change::connection connect_on_change(on_change::callback_type callback) {
        return on_change_.connect(std::move(callback));
    }

private:
    std::array<Slider, wayverb::core::simulation_bands> sliders_;

    on_change on_change_;
};

////////////////////////////////////////////////////////////////////////////////

class frequency_labels : public Component {
public:
    frequency_labels() {
        const auto centres = hrtf_data::hrtf_band_centres_hz();

        for (auto i = 0; i != wayverb::core::simulation_bands; ++i) {
            const auto freq = centres[i];
            if (1000 <= freq) {
                labels_[i].setText(
                        util::build_string(
                                std::setprecision(3), centres[i] / 1000, "K"),
                        dontSendNotification);
            } else {
                labels_[i].setText(
                        util::build_string(std::setprecision(3), centres[i]),
                        dontSendNotification);
            }
        }

        for (auto& i : labels_) {
            addAndMakeVisible(i);
        }
    }

    void resized() override {
        const auto bounds = getLocalBounds();

        auto total_width = [this](auto i) {
            return i * getWidth() / labels_.size();
        };

        const auto width = total_width(1);

        for (auto i = 0u; i != labels_.size(); ++i) {
            labels_[i].setBounds(total_width(i), 0, width, getHeight());
        }
    }

private:
    std::array<Label, 8> labels_;
};

////////////////////////////////////////////////////////////////////////////////

class material_component final : public PropertyPanel,
                                 public ComboBox::Listener {
public:
    using material_t = wayverb::combined::model::material;
    using presets_t = main_model::material_presets_t;

    material_component(const presets_t& presets, material_t& model)
            : presets_{presets}
            , model_{model} {
        auto frequencies =
                std::make_unique<property_component_adapter<frequency_labels>>(
                        "band centres / Hz", 25);
        auto absorption =
                std::make_unique<property_component_adapter<bands_component>>(
                        "absorption", 100, 0.01, 1.0, 0.01);
        auto scattering =
                std::make_unique<property_component_adapter<bands_component>>(
                        "scattering", 100, 0.0, 1.0, 0.01);

        auto preset_box =
                std::make_unique<property_component_adapter<ComboBox>>(
                        "presets", 25);

        preset_box->content.setTextWhenNothingSelected("material presets...");
        preset_box->content.addListener(this);

        {
            auto count = 1;
            for (const auto& i : presets) {
                preset_box->content.addItem(i.get_name(), count++);
            }
        }

        const auto update_from_material =
                [ this, a = &absorption->content, s = &scattering->content ](
                        auto& material) {
            a->set(material.get_surface().absorption);
            s->set(material.get_surface().scattering);
        };

        update_from_material(model_);

        connection_ = material_t::scoped_connection{
                model_.connect(update_from_material)};

        const auto update_from_controls =
                [ this, a = &absorption->content, s = &scattering->content ](
                        auto) {
            model_.set_surface(
                    wayverb::core::surface<wayverb::core::simulation_bands>{
                            a->get(), s->get()});
        };

        absorption->content.connect_on_change(update_from_controls);
        scattering->content.connect_on_change(update_from_controls);

        addProperties({frequencies.release()});
        addProperties({absorption.release()});
        addProperties({scattering.release()});
        addProperties({preset_box.release()});

        setSize(497, getTotalContentHeight());
    }

    void comboBoxChanged(ComboBox* cb) override {
        const auto selected = cb->getSelectedItemIndex();
        if (selected != -1) {
            model_.set_surface(presets_[selected].get_surface());
        }
        cb->setSelectedItemIndex(-1, dontSendNotification);
    }

private:
    const presets_t& presets_;
    material_t& model_;
    material_t::scoped_connection connection_;
};

////////////////////////////////////////////////////////////////////////////////

config_item::config_item(wayverb::combined::model::scene& scene,
                         const presets_t& presets,
                         std::shared_ptr<material_t> model,
                         size_t index)
        : scene_{scene}
        , scene_connection_{scene_.connect_visible_surface_changed(
                  [this](auto surf) {
                      show_button_.setToggleState(surf && *surf == index_,
                                                  dontSendNotification);
                  })}
        , presets_{presets}
        , model_{std::move(model)}
        , index_{index}
        , label_{"", model_->get_name()} {
    addAndMakeVisible(label_);
    addAndMakeVisible(show_button_);
    addAndMakeVisible(config_button_);

    show_button_.setTooltip("Display the triangles which use this material.");
    config_button_.setTooltip("Configure this material.");

    setSize(300, 30);
}

void config_item::resized() {
    const auto button_width = this->getHeight();
    auto bounds = this->getLocalBounds();
    config_button_.setBounds(
            bounds.removeFromRight(button_width).reduced(2, 2));
    show_button_.setBounds(
            bounds.removeFromRight(button_width * 2).reduced(2, 2));
    label_.setBounds(bounds.reduced(2, 2));
}

void config_item::buttonClicked(Button* b) {
    if (b == &show_button_) {
        scene_.set_visible_surface(
                b->getToggleState() ? std::nullopt
                                    : std::make_optional(index_));
    } else if (b == &config_button_) {
        begin_modal_dialog(
                model_->get_name(),
                make_done_window_ptr(std::make_unique<material_component>(
                        presets_, *model_)),
                [](auto) {});
    }
}

}  // namespace materials
}  // namespace left_bar
