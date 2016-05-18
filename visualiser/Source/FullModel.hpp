#pragma once

#include "RenderState.hpp"

namespace model {

class FullModel {
public:
    config::Combined combined;
    std::vector<SceneData::Material> materials;
    std::vector<SceneData::Material> presets;
    FullReceiverConfig receiver;
    RenderState render_state;
};

template <>
class ValueWrapper<FullModel> : public NestedValueWrapper<FullModel> {
public:
    using NestedValueWrapper<FullModel>::NestedValueWrapper;

protected:
    void set_value(const FullModel& u, bool do_notify = true) override {
        combined.set(u.combined, do_notify);
        materials.set(u.materials, do_notify);
        presets.set(u.presets, do_notify);
        receiver.set(u.receiver, do_notify);
        render_state.set(u.render_state, do_notify);
    }

    void reseat_value(FullModel& u) override {
        combined.reseat(u.combined);
        materials.reseat(u.materials);
        presets.reseat(u.presets);
        receiver.reseat(u.receiver);
        render_state.reseat(u.render_state);
    }

public:
    ValueWrapper<config::Combined> combined{this, t->combined};
    ValueWrapper<std::vector<SceneData::Material>> materials{this,
                                                             t->materials};
    ValueWrapper<std::vector<SceneData::Material>> presets{this, t->presets};
    ValueWrapper<FullReceiverConfig> receiver{this, t->receiver};
    ValueWrapper<RenderState> render_state{this, t->render_state};
};

}  // namespace model