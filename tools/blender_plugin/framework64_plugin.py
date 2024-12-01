import bpy

import json
import os
import requests
import sys
import tempfile
import threading

bl_info = {
    "name": "framework64",
    "description": "Plugin for creating games with framework64",
    "author": "Matthew LaRocca",
    "version": (0, 1),
    "blender": (2, 80, 0),
    "support": "COMMUNITY",
    "category": "Development",
}

class framework64_static_mesh_exporter(bpy.types.Operator):
    bl_idname = "wm.framework64_static_mesh_exporter"
    bl_label = "Preview static mesh in engine"

    def execute(self, context):
        self.send_static_mesh_to_console()
        return {"FINISHED"}

    def send_static_mesh_to_console(self):
        temp_dir = tempfile.TemporaryDirectory()

        asset_path = os.path.join(temp_dir.name, "static_mesh.gltf")

        bpy.ops.export_scene.gltf(filepath=asset_path, export_format="GLTF_SEPARATE", export_extras=True)

        transfer_thread = threading.Thread(target=framework64_static_mesh_exporter.webservice_data_transfer, args=(asset_path, temp_dir))
        transfer_thread.start()

    @staticmethod
    def webservice_data_transfer(asset_path, temp_dir):
        endpoint_uri = "http://127.0.0.1:62187"
        print("Transfer static mesh: {} --> {}".format(asset_path, endpoint_uri))

        try:
            r = requests.post(endpoint_uri, data=json.dumps({"type":"mesh", "src": asset_path}))
            r.raise_for_status()
        except requests.exceptions.RequestException as e:
            print("Transfer failed: {}".format(e))
        else:
            print("Transfer complete")
        finally:
            print("cleaning up temp dir: {}".format(temp_dir.name))
            temp_dir.cleanup()
     

class TOPBAR_MT_framework64_menu(bpy.types.Menu):
    bl_label = "framework64"

    def draw(self, context):
        layout = self.layout
        layout.operator("wm.framework64_static_mesh_exporter", text="Preview Static Mesh")

    def menu_draw(self, context):
        layout = self.layout
        layout.menu("TOPBAR_MT_framework64_menu")
        
classes = (
    framework64_static_mesh_exporter,
    TOPBAR_MT_framework64_menu,
)

    
def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_framework64_menu.menu_draw)


def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_framework64_menu.menu_draw)
    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()