#ifdef TOOLS_ENABLED

#ifndef ICON_SELECTOR_H
#define ICON_SELECTOR_H

#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/editor_resource_picker.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>

using namespace godot;

class IconSelector : public VBoxContainer {
    GDCLASS(IconSelector, VBoxContainer)

private:
    HBoxContainer* picker_row;
    Label* label;
    EditorResourcePicker* texture_picker;

protected:
    static void _bind_methods();

public:
    IconSelector();
    ~IconSelector();

    void _ready() override;
    void load_icon(const Ref<Texture2D>& icon);
    void set_editor_plugin(EditorPlugin* p_editor_plugin);

private:
    void _on_texture_picker_resource_changed(const Ref<Resource>& resource);
    void _on_texture_picker_resource_selected(const Ref<Resource>& resource, bool inspect);
};

#endif // ICON_SELECTOR_H

#endif // TOOLS_ENABLED
