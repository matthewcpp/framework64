import bpy
import json
from bpy_extras.io_utils import ImportHelper

bl_info = {
    "name": "Framework64 Level Creator",
    "author": "You",
    "version": (1, 5),
    "blender": (3, 0, 0),
    "location": "View3D > Header, Object Properties, Right-Click Menu",
    "description": "Creates hierarchy, manages chunks, and handles multi-layer properties for framework64.",
    "category": "Object",
}

# ------------------------------------------------------------------------
#   Helper Functions & Dynamic Properties
# ------------------------------------------------------------------------

def get_valid_levels(context):
    """Finds all root objects that have a child starting with 'Scene'"""
    levels = []
    for obj in context.scene.objects:
        if not obj.parent:
            for child in obj.children:
                if child.name.startswith("Scene"):
                    levels.append((obj, child))
                    break
    return levels

def get_layer_items(self, context):
    """Dynamically generates the layer list from the Scene's cached JSON string"""
    if context is None or not hasattr(context, "scene"):
        # Removed the 'LAYER_ACTIVE' icon parameter here
        return [('default', "Default", "Default layer", 1)]
    
    layer_string = context.scene.f64_layer_list
    items = []
    
    # Split the cached string into individual layer names
    for i, name in enumerate(layer_string.split(',')):
        clean_name = name.strip()
        if clean_name:
            # Removed the 'LAYER_ACTIVE' icon parameter here as well.
            # Passing 4 elements tells Blender they are (identifier, name, description, value)
            items.append((clean_name, clean_name, f"Layer: {clean_name}", 1 << i))
            
    if not items:
        return [('default', "Default", "Default layer", 1)]
        
    return items
        

def update_node_type(self, context):
    self["nodeType"] = self.f64_node_type

def update_layers(self, context):
    # ENUM_FLAG returns a set of strings (e.g., {'ground', 'player1'})
    # We join them with a space for the engine pipeline.
    self["layers"] = " ".join(self.f64_layers)

# ------------------------------------------------------------------------
#   Level Creation & Layer JSON Operators
# ------------------------------------------------------------------------

class F64_OT_load_layers(bpy.types.Operator, ImportHelper):
    """Load a Framework64 layers.json file to populate layer choices"""
    bl_idname = "scene.f64_load_layers"
    bl_label = "Load Layers JSON..."
    bl_options = {'REGISTER', 'UNDO'}

    # ImportHelper requires these to filter the file browser
    filename_ext = ".json"
    filter_glob: bpy.props.StringProperty(
        default="*.json",
        options={'HIDDEN'},
        maxlen=255,
    )

    def execute(self, context):
        try:
            with open(self.filepath, 'r') as f:
                data = json.load(f)
            
            # Extract just the keys (names) from the JSON file
            layer_names = list(data.keys())
            
            # Store them as a comma-separated string in the .blend file's scene
            context.scene.f64_layer_list = ",".join(layer_names)
            
            self.report({'INFO'}, f"Loaded {len(layer_names)} F64 Layers")
        except Exception as e:
            self.report({'ERROR'}, f"Failed to load layers: {str(e)}")
            
        return {'FINISHED'}

class F64_OT_create_level(bpy.types.Operator):
    """Create a new Framework64 Level Hierarchy"""
    bl_idname = "object.f64_create_level"
    bl_label = "Create New Level..."
    bl_options = {'REGISTER', 'UNDO'}

    level_name: bpy.props.StringProperty(
        name="Level Name",
        description="The name of the root level node",
        default="Level"
    )

    def invoke(self, context, event):
        return context.window_manager.invoke_props_dialog(self)

    def execute(self, context):
        level_obj = bpy.data.objects.new(self.level_name, None)
        level_obj.empty_display_type = 'PLAIN_AXES'
        level_obj.empty_display_size = 1.0

        scene_obj = bpy.data.objects.new("Scene", None)
        scene_obj.empty_display_type = 'PLAIN_AXES'
        scene_obj.empty_display_size = 0.5
        
        scene_obj.parent = level_obj

        context.collection.objects.link(level_obj)
        context.collection.objects.link(scene_obj)

        bpy.ops.object.select_all(action='DESELECT')
        level_obj.select_set(True)
        context.view_layer.objects.active = level_obj

        self.report({'INFO'}, f"Created Framework64 Hierarchy: {self.level_name}")
        return {'FINISHED'}

class F64_MT_menu(bpy.types.Menu):
    """The framework64 Custom Menu"""
    bl_label = "framework64"
    bl_idname = "F64_MT_menu"

    def draw(self, context):
        layout = self.layout
        layout.operator(F64_OT_create_level.bl_idname)
        layout.separator()
        layout.operator(F64_OT_load_layers.bl_idname, icon='FILE_FOLDER')

def draw_menu(self, context):
    self.layout.menu(F64_MT_menu.bl_idname)

# ------------------------------------------------------------------------
#   Context Menu (Right-Click) Operators
# ------------------------------------------------------------------------

class F64_OT_send_to_level(bpy.types.Operator):
    """Send selected objects to a specific Framework64 Level"""
    bl_idname = "object.f64_send_to_level"
    bl_label = "Send to Level"
    bl_options = {'REGISTER', 'UNDO'}

    target_scene_name: bpy.props.StringProperty()

    def execute(self, context):
        target_scene = context.scene.objects.get(self.target_scene_name)
        
        if not target_scene:
            self.report({'ERROR'}, "Target Scene node not found!")
            return {'CANCELLED'}

        count = 0
        for obj in context.selected_objects:
            if obj == target_scene or obj == target_scene.parent:
                continue

            obj.parent = target_scene
            obj.matrix_parent_inverse.identity()
            count += 1

        self.report({'INFO'}, f"Sent {count} object(s) to {target_scene.parent.name}")
        return {'FINISHED'}

class F64_MT_context_menu(bpy.types.Menu):
    """Dynamically creates the list of levels for the right-click menu"""
    bl_label = "Send to F64 Level..."
    bl_idname = "F64_MT_context_menu"

    def draw(self, context):
        layout = self.layout
        levels = get_valid_levels(context)

        if not levels:
            layout.label(text="No F64 Levels found in scene", icon='ERROR')
            return

        for level_obj, scene_obj in levels:
            op = layout.operator(F64_OT_send_to_level.bl_idname, text=level_obj.name, icon='PRESET')
            op.target_scene_name = scene_obj.name

def draw_context_menu(self, context):
    if context.selected_objects:
        self.layout.separator()
        self.layout.menu(F64_MT_context_menu.bl_idname)

# ------------------------------------------------------------------------
#   Custom Properties Operators & Panels
# ------------------------------------------------------------------------

class F64_OT_add_node_properties(bpy.types.Operator):
    """Add Framework64 Custom Properties to this node"""
    bl_idname = "object.f64_add_node_properties"
    bl_label = "Add F64 Node Properties"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        obj = context.active_object
        if obj:
            # Initialize raw properties
            obj["nodeType"] = "default"
            obj["layers"] = "default"
            
            # Sync UI state
            obj.f64_node_type = 'default'
            obj.f64_layers = {'default'}
            
            self.report({'INFO'}, f"Added F64 properties to {obj.name}")
            
        return {'FINISHED'}

class F64_PT_node_panel(bpy.types.Panel):
    """Creates a Panel in the Object properties window"""
    bl_label = "Framework64 Node Settings"
    bl_idname = "F64_PT_node_panel"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "object"

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def draw(self, context):
        layout = self.layout
        obj = context.active_object

        if "nodeType" not in obj:
            layout.operator(F64_OT_add_node_properties.bl_idname, icon='ADD')
        else:
            box = layout.box()
            box.prop(obj, "f64_node_type")
            box.separator()
            
            # --- NEW TOGGLE UI ---
            
            # Create a row for the toggle button
            row = box.row()
            row.alignment = 'LEFT'
            
            # Swap the icon based on whether it is open or closed
            icon = 'TRIA_DOWN' if obj.f64_show_layers else 'TRIA_RIGHT'
            
            # Draw the boolean property as a textless toggle button
            row.prop(obj, "f64_show_layers", icon=icon, emboss=False, text="Layers (Multi-Select)")
            
            # Only draw the giant grid if the toggle is True
            if obj.f64_show_layers:
                layer_box = box.box() # Optional: puts a nice border around the layers
                layer_box.prop(obj, "f64_layers")

# ------------------------------------------------------------------------
#   Registration
# ------------------------------------------------------------------------

classes = (
    F64_OT_load_layers,
    F64_OT_create_level,
    F64_MT_menu,
    F64_OT_send_to_level,
    F64_MT_context_menu,
    F64_OT_add_node_properties,
    F64_PT_node_panel
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    
    # The string where we cache the parsed JSON for the current .blend file
    bpy.types.Scene.f64_layer_list = bpy.props.StringProperty(default="default")
        
    bpy.types.Object.f64_node_type = bpy.props.EnumProperty(
        name="Node Type",
        description="Framework64 logical node type",
        items=[
            ('default', "Default", "Standard node", 1),
            ('ladder', "Ladder", "Climbable ladder node", 2)
        ],
        default='default',
        update=update_node_type
    )

    bpy.types.Object.f64_show_layers = bpy.props.BoolProperty(
        name="Show Layers",
        default=False,
        description="Toggle the visibility of the layers list"
    )
    
    # By setting options={'ENUM_FLAG'}, Blender treats this as a multi-select set
    bpy.types.Object.f64_layers = bpy.props.EnumProperty(
        name="Layers",
        description="Framework64 logical layers",
        items=get_layer_items,
        options={'ENUM_FLAG'},
        update=update_layers
    )
    
    bpy.types.VIEW3D_MT_editor_menus.append(draw_menu)
    bpy.types.VIEW3D_MT_object_context_menu.append(draw_context_menu)

def unregister():
    bpy.types.VIEW3D_MT_editor_menus.remove(draw_menu)
    bpy.types.VIEW3D_MT_object_context_menu.remove(draw_context_menu)
    
    del bpy.types.Object.f64_node_type
    del bpy.types.Object.f64_layers
    del bpy.types.Scene.f64_layer_list
    del bpy.types.Object.f64_show_layers
    
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()