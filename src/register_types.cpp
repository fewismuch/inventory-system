#include "register_types.h"
#include <gdextension_interface.h>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include "base/craft_station_type.h"
#include "base/inventory_database.h"
#include "base/item_category.h"
#include "base/item_definition.h"
#include "base/item_stack.h"
#include "base/node_inventories.h"
#include "base/recipe.h"
#include "base/loot_item.h"
#include "base/loot.h"
#include "constraints/inventory_constraint.h"
#include "constraints/grid_inventory_constraint.h"
#include "core/quad_tree.h"
#include "core/hotbar.h"
#include "core/inventory.h"
#include "core/grid_inventory.h"
#include "core/loot_generator.h"
#include "craft/craft_station.h"

#ifdef TOOLS_ENABLED
#include "editor/base_inventory_editor.h"
#include "editor/base_resource_editor.h"
#include "editor/icon_selector.h"
#include "editor/resource_id_editor.h"
#include "editor/item_stack_selector.h"
#include "editor/item_definition_property_editor.h"
#include "editor/custom_properties_editor.h"
#include "editor/categories_in_item_editor.h"
#include "editor/inventory_editor_plugin.h"
#include "editor/inventory_settings.h"
#include "editor/inventory_item_list_editor.h"
#include "editor/item_definitions_editor.h"
#include "editor/item_definition_editor.h"
#include "editor/recipes_editor.h"
#include "editor/recipe_editor.h"
#include "editor/recipes_list_editor.h"
#include "editor/recipe_item_list_editor.h"
#include "editor/ingredient_item_in_recipe_item.h"
#include "editor/craft_station_types_editor.h"
#include "editor/craft_station_type_editor.h"
#include "editor/item_categories_editor.h"
#include "editor/item_category_editor.h"
#include "editor/loots_editor.h"
#include "editor/loot_editor.h"
#include "editor/loot_item_editor.h"
#endif

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(CraftStationType);
		GDREGISTER_CLASS(InventoryDatabase);
		GDREGISTER_CLASS(ItemCategory);
		GDREGISTER_CLASS(ItemDefinition);
		GDREGISTER_CLASS(ItemStack);
		GDREGISTER_CLASS(LootItem);
		GDREGISTER_CLASS(Loot);
		GDREGISTER_CLASS(NodeInventories);
		GDREGISTER_CLASS(Recipe);
		GDREGISTER_CLASS(InventoryConstraint);
		GDREGISTER_CLASS(GridInventoryConstraint);
		GDREGISTER_CLASS(QuadTree);
		GDREGISTER_CLASS(QuadTree::QuadNode);
		GDREGISTER_CLASS(QuadTree::QuadRect);
		GDREGISTER_CLASS(Hotbar);
		GDREGISTER_CLASS(Hotbar::Slot);
		GDREGISTER_CLASS(Inventory);
		GDREGISTER_CLASS(GridInventory);
		GDREGISTER_CLASS(LootGenerator);
		GDREGISTER_CLASS(CraftStation);
		GDREGISTER_CLASS(Crafting);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		// Register the configurable list of allowed custom property names used by the
		// Custom Properties editors (Item Definition / Item Category).
		ProjectSettings *project_settings = ProjectSettings::get_singleton();

		// Script source for the allowed custom property names: the plugin looks up
		// the global class (e.g. AttributesConstant) and reads its constant
		// (e.g. BASE_ATTRIBUTES) so the dropdown stays in sync with game code.
		// An explicit script path can also be set directly as an alternative.
		const String script_setting = "addons/inventory_system/editor/property_names_script";
		if (!project_settings->has_setting(script_setting)) {
			project_settings->set_setting(script_setting, String());
		}
		Dictionary script_info;
		script_info["name"] = script_setting;
		script_info["type"] = Variant::STRING;
		project_settings->add_property_info(script_info);

		const String class_setting = "addons/inventory_system/editor/property_names_class";
		if (!project_settings->has_setting(class_setting)) {
			project_settings->set_setting(class_setting, "AttributesConstant");
		}
		Dictionary class_info;
		class_info["name"] = class_setting;
		class_info["type"] = Variant::STRING;
		project_settings->add_property_info(class_info);

		const String constant_setting = "addons/inventory_system/editor/property_names_constant";
		if (!project_settings->has_setting(constant_setting)) {
			project_settings->set_setting(constant_setting, "BASE_ATTRIBUTES");
		}
		Dictionary constant_info;
		constant_info["name"] = constant_setting;
		constant_info["type"] = Variant::STRING;
		project_settings->add_property_info(constant_info);

		// Manual fallback list, only used when the script source above is not set up.
		const String property_options_setting = "addons/inventory_system/editor/item_property_name_options";
		if (!project_settings->has_setting(property_options_setting)) {
			project_settings->set_setting(property_options_setting, PackedStringArray());
		}
		Dictionary property_info;
		property_info["name"] = property_options_setting;
		property_info["type"] = Variant::PACKED_STRING_ARRAY;
		project_settings->add_property_info(property_info);

		GDREGISTER_INTERNAL_CLASS(InventoryEditor);
		GDREGISTER_INTERNAL_CLASS(BaseInventoryEditor);
		GDREGISTER_INTERNAL_CLASS(BaseResourceEditor);
		GDREGISTER_INTERNAL_CLASS(IconSelector);
		GDREGISTER_INTERNAL_CLASS(ResourceIDEditor);
		GDREGISTER_INTERNAL_CLASS(ItemStackSelector);
		GDREGISTER_INTERNAL_CLASS(ItemDefinitionPropertyEditor);
		GDREGISTER_INTERNAL_CLASS(CustomPropertiesEditor);
		GDREGISTER_INTERNAL_CLASS(CategoriesInItemEditor);
		GDREGISTER_INTERNAL_CLASS(InventoryEditorPlugin);
		GDREGISTER_INTERNAL_CLASS(InventorySettings);
		GDREGISTER_INTERNAL_CLASS(InventoryItemListEditor);
		GDREGISTER_INTERNAL_CLASS(ItemDefinitionsEditor);
		GDREGISTER_INTERNAL_CLASS(ItemDefinitionEditor);
		GDREGISTER_INTERNAL_CLASS(RecipesEditor);
		GDREGISTER_INTERNAL_CLASS(RecipeEditor);
		GDREGISTER_INTERNAL_CLASS(RecipesListEditor);
		GDREGISTER_INTERNAL_CLASS(RecipeItemListEditor);
		GDREGISTER_INTERNAL_CLASS(IngredientItemInRecipeItem);
		GDREGISTER_INTERNAL_CLASS(CraftStationTypesEditor);
		GDREGISTER_INTERNAL_CLASS(CraftStationTypeEditor);
		GDREGISTER_INTERNAL_CLASS(ItemCategoriesEditor);
		GDREGISTER_INTERNAL_CLASS(ItemCategoryEditor);
		GDREGISTER_INTERNAL_CLASS(LootsEditor);
		GDREGISTER_INTERNAL_CLASS(LootEditor);
		GDREGISTER_INTERNAL_CLASS(LootItemEditor);
		EditorPlugins::add_by_type<InventoryEditorPlugin>();
	}
#endif
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	// Nothing to cleanup yet
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT inventory_system_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.register_terminator(uninitialize_gdextension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}