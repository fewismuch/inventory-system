#ifdef TOOLS_ENABLED

#include "icon_selector.h"

#include <godot_cpp/classes/editor_resource_picker.hpp>
#include <godot_cpp/classes/editor_inspector.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void IconSelector::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_icon", "icon"), &IconSelector::load_icon);
    ClassDB::bind_method(D_METHOD("set_editor_plugin", "editor_plugin"), &IconSelector::set_editor_plugin);
    ClassDB::bind_method(D_METHOD("_on_texture_picker_resource_changed", "resource"), &IconSelector::_on_texture_picker_resource_changed);
    ClassDB::bind_method(D_METHOD("_on_texture_picker_resource_selected", "resource", "inspect"), &IconSelector::_on_texture_picker_resource_selected);

    ADD_SIGNAL(MethodInfo("icon_changed", PropertyInfo(Variant::OBJECT, "icon", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D")));
}

IconSelector::IconSelector() {
    label = nullptr;
    texture_picker = nullptr;
    inspector_scroll = nullptr;
    inspector = nullptr;
    expanded = false;
}

IconSelector::~IconSelector() {
}

void IconSelector::_ready() {
    // First row: label + picker
    picker_row = memnew(HBoxContainer);
    add_child(picker_row);

    label = memnew(Label);
    label->set_custom_minimum_size(Vector2(160, 0));
    label->set_text("Icon");
    picker_row->add_child(label);

    // Create texture picker
    texture_picker = memnew(EditorResourcePicker);
    texture_picker->set_base_type("Texture2D");
    texture_picker->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    // Let the picker button reflect the expanded state, like the editor inspector does.
    texture_picker->set_toggle_mode(true);
    texture_picker->connect("resource_changed", Callable(this, "_on_texture_picker_resource_changed"));
    texture_picker->connect("resource_selected", Callable(this, "_on_texture_picker_resource_selected"));
    picker_row->add_child(texture_picker);

    // Second row: inline inspector used to edit the picked resource (e.g. AtlasTexture).
    // A standalone EditorResourcePicker does not provide any editor for the resource,
    // so we embed one here to allow expanding/editing the resource properties.
    // EditorInspector does not propagate its content's minimum size, so it must be
    // wrapped in a ScrollContainer with an explicit minimum height.
    inspector_scroll = memnew(ScrollContainer);
    inspector_scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
    inspector_scroll->set_custom_minimum_size(Vector2(0, 0));
    inspector_scroll->set_visible(false);
    add_child(inspector_scroll);

    inspector = memnew(EditorInspector);
    inspector->set_v_size_flags(Control::SIZE_EXPAND_FILL);
    inspector->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    inspector_scroll->add_child(inspector);
}

void IconSelector::load_icon(const Ref<Texture2D>& icon) {
    if (icon.is_valid() && texture_picker) {
        texture_picker->set_edited_resource(icon);
        if (expanded && inspector) {
            inspector->edit(icon.ptr());
        }
    }
}

void IconSelector::set_editor_plugin(EditorPlugin* p_editor_plugin) {
    // EditorResourcePicker will automatically use the current editor plugin context
    // No explicit set_editor_plugin method needed for EditorResourcePicker
}

void IconSelector::_on_texture_picker_resource_changed(const Ref<Resource>& resource) {
    Ref<Texture2D> texture = resource;
    emit_signal("icon_changed", texture);

    if (expanded && inspector) {
        if (resource.is_valid()) {
            inspector->edit(resource.ptr());
        } else {
            _set_expanded(false);
        }
    }
}

void IconSelector::_on_texture_picker_resource_selected(const Ref<Resource>& resource, bool inspect) {
    _set_expanded(!expanded);
}

void IconSelector::_set_expanded(bool p_expanded) {
    expanded = p_expanded;

    if (texture_picker) {
        texture_picker->set_toggle_pressed(expanded);
    }

    if (inspector_scroll) {
        if (expanded && texture_picker) {
            Ref<Resource> resource = texture_picker->get_edited_resource();
            if (resource.is_valid()) {
                inspector->edit(resource.ptr());
                inspector_scroll->set_custom_minimum_size(Vector2(0, 500));
                inspector_scroll->set_visible(true);
                return;
            }
        }
        inspector_scroll->set_custom_minimum_size(Vector2(0, 0));
        inspector_scroll->set_visible(false);
    }
}

#endif // TOOLS_ENABLED
