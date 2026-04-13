"""
Starlight Studio — Main Editor Application
GPU-accelerated editor built with Dear PyGui.

Usage:
    python pysrc/starlight/editor/studio.py
"""
from __future__ import annotations
import sys
import os
import time

# Add project root to path
_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
if _root not in sys.path:
    sys.path.insert(0, _root)

import dearpygui.dearpygui as dpg

from pysrc.starlight.editor.hierarchy import SceneHierarchy
from pysrc.starlight.editor.inspector import Inspector
from pysrc.starlight.editor.console import Console
from pysrc.starlight.editor.asset_browser import AssetBrowser
from pysrc.starlight.editor.profiler_ui import ProfilerUI
from pysrc.starlight.editor.play_mode import PlayMode
from pysrc.starlight.editor.undo_redo import UndoRedoManager, Command
from pysrc.starlight.editor.timeline import TimelineEditor
from pysrc.starlight.editor.prefab_editor import PrefabEditor
from pysrc.starlight.editor.gizmos import Gizmos
from pysrc.starlight.editor.nodes.shader_graph import ShaderGraphEditor
from pysrc.starlight.editor.nodes.logic_graph import LogicGraphEditor

class AddEntityCommand(Command):
    def __init__(self, studio, entity: dict):
        self.studio = studio
        self.entity = entity
        self.description = f"Add Entity {entity['name']}"

    def do(self):
        if self.entity not in self.studio.entities:
            self.studio.entities.append(self.entity)
            self.studio._rebuild_hierarchy()

    def undo(self):
        if self.entity in self.studio.entities:
            self.studio.entities.remove(self.entity)
            if self.studio.hierarchy.selected_id == self.entity["id"]:
                self.studio.hierarchy.selected_id = None
            self.studio._rebuild_hierarchy()


class PropertyChangeCommand(Command):
    def __init__(self, studio, ent_id: int, prop_name: str, old_val: list, new_val: list):
        self.studio = studio
        self.ent_id = ent_id
        self.prop_name = prop_name
        self.old_val = old_val.copy() if hasattr(old_val, "copy") else old_val
        self.new_val = new_val.copy() if hasattr(new_val, "copy") else new_val
        self.description = f"Change {prop_name} of Entity {ent_id}"

    def do(self):
        self.studio._apply_property_change(self.ent_id, self.prop_name, self.new_val)

    def undo(self):
        self.studio._apply_property_change(self.ent_id, self.prop_name, self.old_val)


class StarlightStudio:
    """Main editor window with docking layout."""

    def __init__(self) -> None:
        self.hierarchy = SceneHierarchy()
        self.inspector = Inspector()
        self.console = Console()
        self.asset_browser = AssetBrowser()
        self.profiler = ProfilerUI()
        self.play_mode = PlayMode()
        self.undo_manager = UndoRedoManager()
        self.timeline = TimelineEditor(self)
        self.prefab_editor = PrefabEditor(self)
        self.gizmos = Gizmos(self)
        self.shader_graph = ShaderGraphEditor(bg_compile_callback=self._on_shader_compiled)
        self.logic_graph = LogicGraphEditor(studio=self)
        self.backend = None
        self.entities: list[dict] = []
        self.frame_count = 0
        self.last_time = time.time()
        self.fps = 0.0
        
        # Inspector state tracking
        self._drag_start_values: dict[str, list] = {}
        
        # Console History
        self._command_history: list[str] = []
        self._history_idx: int = 0

    def _try_connect_backend(self) -> None:
        """Try to import the Rust backend."""
        try:
            sys.path.insert(0, os.path.join(_root, "target", "release"))
            import backend
            self.backend = backend
            self.console.log("[Studio] Backend connected!", "INFO")
        except ImportError:
            self.console.log("[Studio] Backend not available — standalone mode", "WARN")

    def _on_shader_compiled(self, generated_code: str) -> None:
        """Called by Shader Graph when compiling visual nodes to WGSL."""
        if not self.backend:
            self.console.log("Cannot recompile shader without Rust backend!", "ERROR")
            return
            
        try:
            # 1. Load the original shader.wgsl template
            shader_path = os.path.join(_root, "assets", "shaders", "shader.wgsl")
            with open(shader_path, "r", encoding="utf-8") as f:
                source = f.read()
                
            # 2. Inject generated wgsl block overriding the standard PBR inputs
            # In shader.wgsl we find the block calculating albedo, metallic, etc. and append the generated code.
            injection_marker = "let albedo = tex_color.rgb * in.color * in.instance_color.rgb;"
            if injection_marker in source:
                # We will replace the material properties right after the marker
                injected_string = f"{injection_marker}\n\n    // --- VISUAL SHADER GRAPH ---\n{generated_code}\n    // --------------------------\n"
                
                # Replace local variable assignments to use the ones generated by the graph
                # Because wgsl variables albedo, normal, etc. are already let inside fs_main
                # we just use shadowing (let) or modify the source string.
                # To make it simple, we wrap generated into the source and rewrite the following references.
                
                parts = source.split(injection_marker)
                
                # Instead of a simple split, let's just do a string replacement for the standard vars
                modified_source = parts[0] + injected_string + parts[1]
                
                # shadowing old variables
                modified_source = modified_source.replace("let metallic = material.metallic;", "let metallic = generated_metallic;")
                modified_source = modified_source.replace("let roughness = material.roughness;", "let roughness = generated_roughness;")
                modified_source = modified_source.replace("var F0 = vec3<f32>(0.04);\n    F0 = mix(F0, albedo, metallic);", "var F0 = vec3<f32>(0.04);\n    F0 = mix(F0, generated_albedo, metallic);")
                modified_source = modified_source.replace("return (kD * albedo / PI + specular) * radiance * NdotL;", "return (kD * generated_albedo / PI + specular) * radiance * NdotL;")
                modified_source = modified_source.replace("return ambient + color", "return ambient + color + generated_emission")
                
                # Send to backend
                self.backend.recompile_shader(modified_source)
                self.console.log("Material updated via Visual Shader Graph!", "INFO")
            else:
                self.console.log("Could not find WGSL injection marker in shader.wgsl", "ERROR")
        except Exception as e:
            self.console.log(f"Shader Recompilation Failed: {e}", "ERROR")

    def _refresh_entities(self) -> None:
        """Refresh entity list from backend."""
        if not self.backend:
            return
        try:
            ids = self.backend.get_all_entities()
            self.entities = []
            for eid in ids:
                try:
                    t = self.backend.get_transform(eid)
                    self.entities.append({
                        "id": eid,
                        "name": f"Entity_{eid}",
                        "position": [t.x, t.y, t.z] if hasattr(t, 'x') else [0, 0, 0],
                    })
                except Exception:
                    self.entities.append({"id": eid, "name": f"Entity_{eid}", "position": [0, 0, 0]})
        except Exception as e:
            self.console.log(f"[Studio] Failed to refresh: {e}", "ERROR")

    def _create_menu_bar(self) -> None:
        """Create the main menu bar."""
        with dpg.viewport_menu_bar():
            with dpg.menu(label="File"):
                dpg.add_menu_item(label="New Scene", callback=self._on_new_scene)
                dpg.add_menu_item(label="Open Scene...", callback=self._on_open_scene)
                dpg.add_menu_item(label="Save Scene", callback=self._on_save_scene)
                dpg.add_separator()
                dpg.add_menu_item(label="Build Game...", callback=self._on_build_project)
                dpg.add_separator()
                with dpg.menu(label="Prefab"):
                    dpg.add_menu_item(label="Open Prefab...", callback=self._on_open_prefab)
                    dpg.add_menu_item(label="Save & Close Prefab", callback=self._on_close_prefab)
                    dpg.add_menu_item(label="Discard Prefab", callback=self._on_discard_prefab)
                dpg.add_separator()
                dpg.add_menu_item(label="Exit", callback=lambda: dpg.stop_dearpygui())
            with dpg.menu(label="Edit"):
                dpg.add_menu_item(label="Undo (Ctrl+Z)", callback=self._on_undo_req)
                dpg.add_menu_item(label="Redo (Ctrl+Y)", callback=self._on_redo_req)
            with dpg.menu(label="View"):
                dpg.add_menu_item(label="Reset Layout", callback=self._reset_layout)
                dpg.add_separator()
                dpg.add_menu_item(label="Shader Graph...", callback=lambda: self.shader_graph.show())
            with dpg.menu(label="Tools"):
                dpg.add_menu_item(label="Visual Logic (Blueprint)...", callback=lambda: self.logic_graph.show())
                dpg.add_menu_item(label="About Starlight Studio", callback=self._show_about)

    def _on_new_scene(self) -> None:
        if self.prefab_editor.is_active: return
        self.entities.clear()
        self.hierarchy.selected_id = None
        self.console.log("[Studio] New scene created", "INFO")

    def _on_open_scene(self) -> None:
        if self.prefab_editor.is_active: return
        self.console.log("[Studio] Open scene dialog (not yet implemented)", "WARN")

    def _on_save_scene(self) -> None:
        if self.prefab_editor.is_active: return
        self.console.log("[Studio] Save scene dialog (not yet implemented)", "WARN")

    def _on_build_project(self) -> None:
        self.console.log("Starting full project build...", "INFO")
        try:
            from .builder import build_project
            scene_data = {
                "version": 1,
                "sun": {
                    "direction": [-0.5, -1.0, -0.5],
                    "color": [1.0, 1.0, 1.0],
                    "intensity": 1.0
                },
                "entities": self.entities
            }
            build_project(scene_data)
            self.console.log("Build successfully exported to build/ folder!", "INFO")
        except Exception as e:
            import traceback
            traceback.print_exc()
            self.console.log(f"Build failed: {e}", "ERROR")

    def _on_open_prefab(self) -> None:
        path = os.path.join(_root, "assets", "new_prefab.json")
        self.prefab_editor.open_prefab(path)

    def _on_close_prefab(self) -> None:
        self.prefab_editor.save_and_close()

    def _on_discard_prefab(self) -> None:
        self.prefab_editor.discard_and_close()

    def _reset_layout(self) -> None:
        self.console.log("[Studio] Layout reset", "INFO")

    def _show_about(self) -> None:
        if dpg.does_item_exist("about_window"):
            dpg.show_item("about_window")
            return
        with dpg.window(label="About Starlight Studio", tag="about_window",
                        width=400, height=200, modal=True):
            dpg.add_text("Starlight Studio v0.1")
            dpg.add_text("A GPU-accelerated game editor")
            dpg.add_text("")
            dpg.add_text("Built with Dear PyGui + Rust + WGPU")
            dpg.add_separator()
            dpg.add_button(label="Close", callback=lambda: dpg.hide_item("about_window"))

    def _draw_gizmos(self) -> None:
        if self.hierarchy.selected_id is None: return
        ent = next((e for e in self.entities if e["id"] == self.hierarchy.selected_id), None)
        if not ent: return

        pos = ent.get("position", [0, 0, 0])
        # Simple orthographic axes for Gizmos
        # XYZ -> RGB
        length = 2.0
        self.gizmos.draw_line(pos, [pos[0] + length, pos[1], pos[2]], [1.0, 0.0, 0.0])
        self.gizmos.draw_line(pos, [pos[0], pos[1] + length, pos[2]], [0.0, 1.0, 0.0])
        self.gizmos.draw_line(pos, [pos[0], pos[1], pos[2] + length], [0.0, 0.0, 1.0])

        scale = ent.get("scale", [1, 1, 1])
        # Draw bounding box approx
        self.gizmos.draw_wire_cube(pos, [s * 1.05 for s in scale], [1.0, 1.0, 0.0])

    def _update_fps(self) -> None:
        self.frame_count += 1
        now = time.time()
        elapsed = now - self.last_time
        if elapsed >= 1.0:
            self.fps = self.frame_count / elapsed
            self.frame_count = 0
            self.last_time = now

    def run(self) -> None:
        """Start the editor."""
        dpg.create_context()

        # Theme
        with dpg.theme() as global_theme:
            with dpg.theme_component(dpg.mvAll):
                dpg.add_theme_style(dpg.mvStyleVar_WindowRounding, 4)
                dpg.add_theme_style(dpg.mvStyleVar_FrameRounding, 3)
                dpg.add_theme_style(dpg.mvStyleVar_WindowPadding, 8, 6)
                dpg.add_theme_style(dpg.mvStyleVar_ItemSpacing, 8, 4)
                dpg.add_theme_color(dpg.mvThemeCol_WindowBg, (30, 30, 35, 240))
                dpg.add_theme_color(dpg.mvThemeCol_TitleBg, (20, 20, 25, 255))
                dpg.add_theme_color(dpg.mvThemeCol_TitleBgActive, (45, 50, 80, 255))
                dpg.add_theme_color(dpg.mvThemeCol_MenuBarBg, (25, 25, 30, 255))
                dpg.add_theme_color(dpg.mvThemeCol_Header, (50, 55, 85, 200))
                dpg.add_theme_color(dpg.mvThemeCol_HeaderHovered, (60, 65, 100, 200))
                dpg.add_theme_color(dpg.mvThemeCol_Button, (55, 60, 90, 200))
                dpg.add_theme_color(dpg.mvThemeCol_ButtonHovered, (70, 75, 110, 200))
                dpg.add_theme_color(dpg.mvThemeCol_FrameBg, (40, 42, 50, 200))
                dpg.add_theme_color(dpg.mvThemeCol_Tab, (40, 45, 70, 200))
                dpg.add_theme_color(dpg.mvThemeCol_TabActive, (55, 60, 95, 255))
        dpg.bind_theme(global_theme)

        # Custom Themes for Play/Edit modes
        with dpg.theme() as self.play_theme:
            with dpg.theme_component(dpg.mvAll):
                dpg.add_theme_color(dpg.mvThemeCol_TitleBgActive, (160, 40, 40, 255))
                dpg.add_theme_color(dpg.mvThemeCol_TitleBg, (100, 20, 20, 255))
                dpg.add_theme_color(dpg.mvThemeCol_WindowBg, (45, 20, 20, 240))
                
        with dpg.theme() as self.edit_theme:
            with dpg.theme_component(dpg.mvAll):
                dpg.add_theme_color(dpg.mvThemeCol_TitleBgActive, (45, 50, 80, 255))
                dpg.add_theme_color(dpg.mvThemeCol_TitleBg, (20, 20, 25, 255))
                dpg.add_theme_color(dpg.mvThemeCol_WindowBg, (30, 30, 35, 240))

        # Font
        with dpg.font_registry():
            default_font = dpg.add_font(
                os.path.join(_root, "assets", "fonts", "Roboto-Regular.ttf"), 16
            ) if os.path.exists(os.path.join(_root, "assets", "fonts", "Roboto-Regular.ttf")) else None
        if default_font:
            dpg.bind_font(default_font)

        # Primary window
        dpg.create_viewport(title="Starlight Studio", width=1600, height=900)
        dpg.setup_dearpygui()

        with dpg.window(tag="primary_window"):
            self._create_menu_bar()

        dpg.set_primary_window("primary_window", True)

        # Global input handlers
        with dpg.handler_registry():
            dpg.add_key_press_handler(dpg.mvKey_Z, callback=self._on_undo_shortcut)
            dpg.add_key_press_handler(dpg.mvKey_Y, callback=self._on_redo_shortcut)
            dpg.add_key_press_handler(dpg.mvKey_Up, callback=self._on_history_up)
            dpg.add_key_press_handler(dpg.mvKey_Down, callback=self._on_history_down)

        # Texture Registry for Viewport Target
        # Size matches WGPU backend standard or we can dynamically manage it. Currently we assume 800x600 inside the engine context unless resized.
        # WGPU RenderState size: we need to enforce the Viewport size. Let's start with 1024x768 to avoid mismatch if engine initializes differently, actually the engine defaults to 800x600 in Python unless configured?
        # Let's set the dynamic texture size to 800x600 for now.
        with dpg.texture_registry(show=False):
            empty_data = [0.0] * (800 * 600 * 4)
            dpg.add_dynamic_texture(width=800, height=600, default_value=empty_data, tag="viewport_texture")

        # Enable docking
        dpg.configure_app(docking=True, docking_space=True)

        # Create panels
        self._create_hierarchy_panel()
        self._create_inspector_panel()
        self._create_console_panel()
        self._create_asset_browser_panel()
        self._create_profiler_panel()
        self._create_viewport_panel()
        self.timeline.create_panel()
        
        # Try backend
        self._try_connect_backend()

        self.console.log("═══════════════════════════════════", "INFO")
        self.console.log("  Starlight Studio v0.1 Ready!", "INFO")
        self.console.log("═══════════════════════════════════", "INFO")
        self.console.log("Type Python commands in the console below.", "INFO")

        dpg.show_viewport()

        # Main loop
        while dpg.is_dearpygui_running():
            self._update_fps()
            self._update_profiler()
            
            # Use real delta time (or simple 1/60 approximation for editor ticking)
            dt = 1.0 / max(self.fps, 30.0) if self.fps > 0 else 0.016
            self.timeline.update(dt)
            self._draw_gizmos()
            
            # Fetch WGPU frame
            if self.backend:
                try:
                    f32_array = self.backend.render_viewport_f32()
                    if f32_array and len(f32_array) == 800 * 600 * 4:
                        dpg.set_value("viewport_texture", f32_array)
                except Exception as e:
                    pass # Keep going if not supported

            # Tick Custom Scripts in Play Mode
            if self.play_mode.is_playing and hasattr(self, "_runtime_behaviours"):
                for b in self._runtime_behaviours:
                    try:
                        b.update(dt)
                    except Exception as e:
                        self.console.log(f"[{b.__class__.__name__} Update Error] {e}", "ERROR")

            dpg.render_dearpygui_frame()

        dpg.destroy_context()

    # ─── Panel Creation ───────────────────────────────────────────

    def _create_hierarchy_panel(self) -> None:
        with dpg.window(label="Scene Hierarchy", tag="hierarchy_panel",
                        width=280, height=400, pos=(0, 20)):
            dpg.add_button(label="+ Add Entity", callback=self._on_add_entity, width=-1)
            dpg.add_separator()
            dpg.add_text("Entities:", tag="hierarchy_label")
            with dpg.child_window(tag="hierarchy_list", height=-1, border=False):
                dpg.add_text("(empty scene)", tag="hierarchy_empty")

    def _create_inspector_panel(self) -> None:
        with dpg.window(label="Inspector", tag="inspector_panel",
                        width=320, height=400, pos=(1280, 20)):
            dpg.add_text("No entity selected", tag="inspector_title")
            dpg.add_separator()

            with dpg.collapsing_header(label="Transform", default_open=True, tag="transform_header"):
                dpg.add_drag_floatx(label="Position", size=3, tag="pos_drag",
                                     default_value=[0, 0, 0], speed=0.1,
                                     callback=self._on_transform_changed)
                dpg.add_drag_floatx(label="Rotation", size=3, tag="rot_drag",
                                     default_value=[0, 0, 0], speed=1.0,
                                     callback=self._on_transform_changed)
                dpg.add_drag_floatx(label="Scale", size=3, tag="scale_drag",
                                     default_value=[1, 1, 1], speed=0.05,
                                     callback=self._on_transform_changed)

            with dpg.collapsing_header(label="Material", default_open=True, tag="material_header"):
                dpg.add_color_edit(label="Color", tag="color_edit",
                                    default_value=[255, 255, 255, 255],
                                    callback=self._on_color_changed)
                dpg.add_slider_float(label="Metallic", tag="metallic_drag",
                                    default_value=0.0, min_value=0.0, max_value=1.0, callback=self._on_material_changed)
                dpg.add_slider_float(label="Roughness", tag="roughness_drag",
                                    default_value=0.5, min_value=0.0, max_value=1.0, callback=self._on_material_changed)

            # Item handlers to track before/after edits for Undo
            with dpg.item_handler_registry(tag="inspector_item_handler"):
                dpg.add_item_activated_handler(callback=self._on_item_activated)
                dpg.add_item_deactivated_after_edit_handler(callback=self._on_item_deactivated)
            
            dpg.bind_item_handler_registry("pos_drag", "inspector_item_handler")
            dpg.bind_item_handler_registry("rot_drag", "inspector_item_handler")
            dpg.bind_item_handler_registry("scale_drag", "inspector_item_handler")
            dpg.bind_item_handler_registry("color_edit", "inspector_item_handler")

            with dpg.collapsing_header(label="Behaviours", default_open=True, tag="behaviours_header"):
                with dpg.group(horizontal=True):
                    dpg.add_input_text(hint="e.g. scripts/player.py", tag="new_behaviour_input", width=-50)
                    dpg.add_button(label="Add", callback=self._on_add_behaviour)
                dpg.add_separator()
                with dpg.child_window(tag="behaviours_list", height=100, border=False):
                    dpg.add_text("No scripts attached", tag="behaviours_empty")

            with dpg.collapsing_header(label="Native Components", default_open=True, tag="native_components_header"):
                with dpg.group(horizontal=True):
                    dpg.add_combo(items=["Health", "Mana", "PointLight"], tag="new_native_comp_combo", width=-50)
                    dpg.add_button(label="Add", callback=self._on_add_native_component)
                dpg.add_separator()
                with dpg.child_window(tag="native_components_list", height=120, border=False):
                    dpg.add_text("No native components", tag="native_components_empty")

            with dpg.collapsing_header(label="Info", default_open=False):
                dpg.add_text("Entity ID: —", tag="info_id")
                dpg.add_text("Mesh: —", tag="info_mesh")

    def _create_console_panel(self) -> None:
        with dpg.window(label="Console", tag="console_panel",
                        width=1000, height=250, pos=(280, 650)):
            with dpg.group(horizontal=True):
                dpg.add_button(label="Clear", callback=self._on_console_clear, width=60)
                dpg.add_combo(label="", tag="console_filter",
                              items=["ALL", "INFO", "WARN", "ERROR"],
                              default_value="ALL", width=80)

            with dpg.child_window(tag="console_log", height=-35, border=True):
                pass  # Logs added dynamically

            with dpg.group(horizontal=True):
                dpg.add_input_text(label="", tag="console_input",
                                    hint="Type Python command...",
                                    on_enter=True, callback=self._on_console_exec,
                                    width=-80)
                dpg.add_button(label="Run", callback=self._on_console_exec, width=70)

    def _create_asset_browser_panel(self) -> None:
        with dpg.window(label="Asset Browser", tag="asset_panel",
                        width=280, height=250, pos=(0, 420)):
            with dpg.group(horizontal=True):
                dpg.add_input_text(label="", tag="asset_search",
                                    hint="Filter...", width=-80,
                                    callback=self._on_asset_filter)
                dpg.add_button(label="Refresh", callback=self._on_asset_refresh, width=70)
            dpg.add_separator()
            with dpg.child_window(tag="asset_list", height=-1, border=False):
                dpg.add_text("Click 'Refresh' to list assets", tag="asset_empty")

    def _create_profiler_panel(self) -> None:
        with dpg.window(label="Profiler", tag="profiler_panel",
                        width=320, height=250, pos=(1280, 420)):
            dpg.add_text("FPS: 0", tag="profiler_fps")
            dpg.add_text("Entities: 0", tag="profiler_entities")
            dpg.add_text("Frame Time: 0.0ms", tag="profiler_frametime")
            dpg.add_text("VRAM: 0 MB", tag="profiler_vram")
            dpg.add_separator()
            with dpg.plot(label="FPS History", height=150, width=-1, tag="fps_plot"):
                dpg.add_plot_axis(dpg.mvXAxis, label="Frame", tag="fps_x_axis")
                dpg.add_plot_axis(dpg.mvYAxis, label="FPS", tag="fps_y_axis")
                dpg.add_line_series([], [], label="FPS", parent="fps_y_axis", tag="fps_series")
            self._fps_history: list[float] = []

    def _create_viewport_panel(self) -> None:
        with dpg.window(label="Viewport", tag="viewport_panel",
                        width=1000, height=630, pos=(280, 20)):
            with dpg.group(horizontal=True):
                dpg.add_button(label="▶ Play", tag="play_btn",
                               callback=self._on_play)
                dpg.add_button(label="⏸ Pause", tag="pause_btn",
                               callback=self._on_pause)
                dpg.add_button(label="⏹ Stop", tag="stop_btn",
                               callback=self._on_stop)
                dpg.add_separator()
                dpg.add_text("", tag="play_state_text")
            dpg.add_separator()
            # Render the DPG texture
            dpg.add_image("viewport_texture", width=800, height=600)
            dpg.add_text("Mode: Editor", tag="viewport_mode")

    # ─── Callbacks ────────────────────────────────────────────────

    def _on_add_entity(self) -> None:
        new_id = len(self.entities) + 1000
        name = f"Entity_{new_id}"
        self.entities.append({
            "id": new_id,
            "name": name,
            "position": [0.0, 0.0, 0.0],
            "rotation": [0.0, 0.0, 0.0],
            "scale": [1.0, 1.0, 1.0],
            "color": [1.0, 1.0, 1.0, 1.0],
        })
        if self.backend:
            try:
                eid = self.backend.spawn_entity(0.0, 0.0, 0.0)
                self.entities[-1]["id"] = eid
                self.entities[-1]["name"] = f"Entity_{eid}"
            except Exception as e:
                self.console.log(f"[Spawn] Error: {e}", "ERROR")

        cmd = AddEntityCommand(self, self.entities[-1])
        # Skip actual .do() execution since entity is already appended here for simplification,
        # but pushing to undo stack will handle undo/redo appropriately:
        self.undo_manager.execute(cmd)

        self._rebuild_hierarchy()
        self.console.log(f"[+] Added {name}", "INFO")

    def _rebuild_hierarchy(self) -> None:
        dpg.delete_item("hierarchy_list", children_only=True)
        if not self.entities:
            dpg.add_text("(empty scene)", parent="hierarchy_list")
            return
        for ent in self.entities:
            with dpg.group(horizontal=True):
                # We put the selectable in a group to attach a context menu safely
                selectable = dpg.add_selectable(
                    label=f"  {ent['name']}",
                    parent="hierarchy_list",
                    callback=self._on_select_entity,
                    user_data=ent,
                )
                with dpg.popup(selectable, mousebutton=dpg.mvMouseButton_Right):
                    dpg.add_menu_item(label="Duplicate", callback=self._on_duplicate_entity, user_data=ent)
                    dpg.add_menu_item(label="Delete", callback=self._on_delete_entity, user_data=ent)

    def _on_select_entity(self, sender, app_data, user_data) -> None:
        ent = user_data
        self.hierarchy.selected_id = ent["id"]
        dpg.set_value("inspector_title", f"Inspecting: {ent['name']}")
        dpg.set_value("info_id", f"Entity ID: {ent['id']}")

        pos = ent.get("position", [0, 0, 0])
        rot = ent.get("rotation", [0, 0, 0])
        scale = ent.get("scale", [1, 1, 1])

        dpg.set_value("pos_drag", pos)
        dpg.set_value("rot_drag", rot)
        dpg.set_value("scale_drag", scale)
        
        # Load material if exists
        color = ent.get("color", [1, 1, 1, 1])
        dpg.set_value("color_edit", [c * 255.0 for c in color])

        # Load behaviours
        dpg.delete_item("behaviours_list", children_only=True)
        behaviours = ent.get("behaviours", [])
        if not behaviours:
            dpg.add_text("No scripts attached", parent="behaviours_list")
        else:
            for b_path in behaviours:
                with dpg.group(horizontal=True, parent="behaviours_list"):
                    dpg.add_button(label="X", user_data={"id": ent["id"], "path": b_path}, callback=self._on_remove_behaviour)
                    dpg.add_text(f"📜 {os.path.basename(b_path)}")

        # Load native components
        dpg.delete_item("native_components_list", children_only=True)
        native_comps = ent.get("native_components", {})
        if not native_comps:
            dpg.add_text("No native components", parent="native_components_list")
        else:
            for c_type, c_data in native_comps.items():
                with dpg.tree_node(label=c_type, parent="native_components_list", default_open=True):
                    with dpg.group(horizontal=True):
                        dpg.add_button(label="Remove", user_data={"id": ent["id"], "type": c_type}, callback=self._on_remove_native_component)
                    if c_type in ["Health", "Mana"]:
                        dpg.add_drag_float(label="Current", default_value=c_data.get("current", 100.0), callback=self._on_native_comp_changed, user_data={"id": ent["id"], "type": c_type, "field": "current"}, speed=1.0)
                        dpg.add_drag_float(label="Max", default_value=c_data.get("max", 100.0), callback=self._on_native_comp_changed, user_data={"id": ent["id"], "type": c_type, "field": "max"}, speed=1.0)
                    elif c_type == "PointLight":
                        dpg.add_drag_float(label="Radius", default_value=c_data.get("radius", 10.0), callback=self._on_native_comp_changed, user_data={"id": ent["id"], "type": c_type, "field": "radius"}, speed=0.5)
                        dpg.add_color_edit(label="Color", default_value=[c*255.0 for c in c_data.get("color", [1,1,1])], callback=self._on_native_comp_changed, user_data={"id": ent["id"], "type": c_type, "field": "color"})

        self.console.log(f"[Select] {ent['name']}", "INFO")

    def _on_add_behaviour(self, sender, app_data) -> None:
        if self.hierarchy.selected_id is None: return
        path = dpg.get_value("new_behaviour_input").strip()
        if not path: return
        
        for ent in self.entities:
            if ent["id"] == self.hierarchy.selected_id:
                if "behaviours" not in ent:
                    ent["behaviours"] = []
                if path not in ent["behaviours"]:
                    ent["behaviours"].append(path)
                dpg.set_value("new_behaviour_input", "")
                self._on_select_entity(None, None, ent)
                self.console.log(f"Attached script: {path}", "INFO")
                break

    def _on_remove_behaviour(self, sender, app_data, user_data) -> None:
        ent_id = user_data["id"]
        path = user_data["path"]
        ent = next((e for e in self.entities if e["id"] == ent_id), None)
        if ent and "behaviours" in ent:
            ent["behaviours"].remove(path)
            self._on_select_entity(None, None, ent)
            self.console.log(f"Removed script: {path}", "INFO")

    def _on_add_native_component(self, sender, app_data) -> None:
        if self.hierarchy.selected_id is None: return
        comp_type = dpg.get_value("new_native_comp_combo")
        if not comp_type: return
        for ent in self.entities:
            if ent["id"] == self.hierarchy.selected_id:
                if "native_components" not in ent:
                    ent["native_components"] = {}
                if comp_type not in ent["native_components"]:
                    if comp_type == "Health":
                        ent["native_components"][comp_type] = {"current": 100.0, "max": 100.0}
                    elif comp_type == "Mana":
                        ent["native_components"][comp_type] = {"current": 50.0, "max": 50.0}
                    elif comp_type == "PointLight":
                        ent["native_components"][comp_type] = {"radius": 10.0, "color": [1.0, 1.0, 1.0]}
                    self._on_select_entity(None, None, ent)
                    self.console.log(f"Added Native Component: {comp_type}", "INFO")
                break

    def _on_remove_native_component(self, sender, app_data, user_data) -> None:
        ent_id = user_data["id"]
        c_type = user_data["type"]
        for ent in self.entities:
            if ent["id"] == ent_id:
                if "native_components" in ent and c_type in ent["native_components"]:
                    del ent["native_components"][c_type]
                    self._on_select_entity(None, None, ent)
                    self.console.log(f"Removed Native Component: {c_type}", "INFO")
                break

    def _on_native_comp_changed(self, sender, app_data, user_data) -> None:
        ent_id = user_data["id"]
        c_type = user_data["type"]
        field = user_data["field"]
        for ent in self.entities:
            if ent["id"] == ent_id:
                if field == "color":
                    ent["native_components"][c_type][field] = [c / 255.0 for c in app_data[:3]]
                else:
                    ent["native_components"][c_type][field] = app_data
                break

    def _on_duplicate_entity(self, sender, app_data, user_data) -> None:
        if not user_data: return
        ent = user_data.copy()
        
        new_id = len(self.entities) + 1000
        ent["id"] = new_id
        ent["name"] = ent["name"] + "_Copy"
        # Optional: offset slightly
        ent["position"] = [ent["position"][0] + 1.0, ent["position"][1], ent["position"][2] + 1.0]

        self.entities.append(ent)
        if self.backend:
            try:
                eid = self.backend.spawn_entity(*ent["position"])
                ent["id"] = eid
                ent["name"] = f"Entity_{eid}_Copy"
            except Exception: pass
            
        self._rebuild_hierarchy()
        self.console.log(f"[+] Duplicated {ent['name']}", "INFO")

    def _on_delete_entity(self, sender, app_data, user_data) -> None:
        if not user_data: return
        ent_id = user_data["id"]
        
        # We don't have a UndoRedo DeleteCommand yet, executing raw:
        self.entities = [e for e in self.entities if e["id"] != ent_id]
        if self.backend:
            try:
                self.backend.despawn_entity(ent_id)
            except Exception: pass
            
        if self.hierarchy.selected_id == ent_id:
            self.hierarchy.selected_id = None
            dpg.set_value("inspector_title", "No entity selected")
        
        self._rebuild_hierarchy()
        self.console.log(f"[-] Deleted Entity {ent_id}", "INFO")

    def _on_transform_changed(self, sender, app_data) -> None:
        if self.hierarchy.selected_id is None:
            return
        # Find entity
        for ent in self.entities:
            if ent["id"] == self.hierarchy.selected_id:
                if sender == "pos_drag":
                    ent["position"] = list(app_data)
                    if self.backend:
                        try:
                            self.backend.set_transform(ent["id"], *app_data[:3])
                        except Exception:
                            pass
                elif sender == "rot_drag":
                    ent["rotation"] = list(app_data)
                    if self.backend:
                        try:
                            self.backend.set_rotation(ent["id"], *app_data[:3])
                        except Exception:
                            pass
                elif sender == "scale_drag":
                    ent["scale"] = list(app_data)
                    if self.backend:
                        try:
                            self.backend.set_scale(ent["id"], *app_data[:3])
                        except Exception:
                            pass
                break

    def _on_color_changed(self, sender, app_data) -> None:
        if self.hierarchy.selected_id is None:
            return
        r, g, b, a = [c / 255.0 for c in app_data[:4]]
        for ent in self.entities:
            if ent["id"] == self.hierarchy.selected_id:
                ent["color"] = [r, g, b, a]
                if self.backend:
                    try:
                        self.backend.set_color(ent["id"], r, g, b, a)
                    except Exception:
                        pass
                break

    def _on_material_changed(self, sender, app_data) -> None:
        if self.hierarchy.selected_id is None:
            return
        for ent in self.entities:
            if ent["id"] == self.hierarchy.selected_id:
                if sender == "metallic_drag": ent["metallic"] = app_data
                elif sender == "roughness_drag": ent["roughness"] = app_data
                if self.backend:
                    try:
                        color = ent.get("color", [1,1,1,1])
                        # We use set_material since it was exposed, or fallback to properties.
                        self.backend.set_material(ent["id"], *color[:3], ent.get("roughness", 0.5), ent.get("metallic", 0.0))
                    except Exception: pass
                break

    def _apply_property_change(self, ent_id: int, prop: str, value: list) -> None:
        """Internal updater to bypass callbacks on Undo/Redo."""
        for ent in self.entities:
            if ent["id"] == ent_id:
                ent[prop] = value
                
                # Sync GUI if it's the selected entity
                if self.hierarchy.selected_id == ent_id:
                    if prop == "position": dpg.set_value("pos_drag", value)
                    elif prop == "rotation": dpg.set_value("rot_drag", value)
                    elif prop == "scale": dpg.set_value("scale_drag", value)
                    elif prop == "color": dpg.set_value("color_edit", [c * 255.0 for c in value])

                if self.backend:
                    try:
                        if prop == "position": self.backend.set_transform(ent_id, *value[:3])
                        elif prop == "rotation": self.backend.set_rotation(ent_id, *value[:3])
                        elif prop == "scale": self.backend.set_scale(ent_id, *value[:3])
                        elif prop == "color": self.backend.set_color(ent_id, *value)
                    except Exception: pass
                break

    def _on_item_activated(self, sender, app_data):
        item_id = app_data
        item_tag = dpg.get_item_alias(item_id) or str(item_id)
        if hasattr(self, "hierarchy") and self.hierarchy.selected_id is not None:
            self._drag_start_values[item_tag] = dpg.get_value(item_id)

    def _on_item_deactivated(self, sender, app_data):
        item_id = app_data
        item_tag = dpg.get_item_alias(item_id) or str(item_id)
        if self.hierarchy.selected_id is None:
            return

        old_val = self._drag_start_values.get(item_tag)
        new_val = dpg.get_value(item_id)
        
        # Determine property name
        prop_name = None
        if item_tag == "pos_drag": prop_name = "position"
        elif item_tag == "rot_drag": prop_name = "rotation"
        elif item_tag == "scale_drag": prop_name = "scale"
        elif item_tag == "color_edit": 
            prop_name = "color"
            old_val = [c / 255.0 for c in old_val[:4]] if old_val else [1,1,1,1]
            new_val = [c / 255.0 for c in new_val[:4]]
            
        if prop_name and old_val is not None and old_val != new_val:
            cmd = PropertyChangeCommand(self, self.hierarchy.selected_id, prop_name, old_val, new_val)
            self.undo_manager.undo_stack.append(cmd)  # Manually push without executing since it's already dragged
            self.undo_manager.redo_stack.clear()
            self._drag_start_values.pop(item_tag, None)

    def _on_undo_req(self):
        desc = self.undo_manager.undo()
        if desc: self.console.log(f"[Undo] {desc}", "INFO")

    def _on_redo_req(self):
        desc = self.undo_manager.redo()
        if desc: self.console.log(f"[Redo] {desc}", "INFO")

    def _on_undo_shortcut(self):
        if dpg.is_key_down(dpg.mvKey_Control) or dpg.is_key_down(dpg.mvKey_LControl) or dpg.is_key_down(dpg.mvKey_RControl):
            self._on_undo_req()

    def _on_redo_shortcut(self):
        if dpg.is_key_down(dpg.mvKey_Control) or dpg.is_key_down(dpg.mvKey_LControl) or dpg.is_key_down(dpg.mvKey_RControl):
            self._on_redo_req()

    def _on_history_up(self) -> None:
        if not dpg.is_item_focused("console_input") or not self._command_history:
            return
        self._history_idx = max(0, self._history_idx - 1)
        dpg.set_value("console_input", self._command_history[self._history_idx])

    def _on_history_down(self) -> None:
        if not dpg.is_item_focused("console_input") or not self._command_history:
            return
        if self._history_idx < len(self._command_history) - 1:
            self._history_idx += 1
            dpg.set_value("console_input", self._command_history[self._history_idx])
        else:
            self._history_idx = len(self._command_history)
            dpg.set_value("console_input", "")

    def _on_console_clear(self) -> None:
        self.console.clear()
        dpg.delete_item("console_log", children_only=True)

    def _on_console_exec(self, sender=None, app_data=None) -> None:
        text = dpg.get_value("console_input")
        if not text or not text.strip():
            return
        
        # History
        if not self._command_history or self._command_history[-1] != text.strip():
            self._command_history.append(text.strip())
        self._history_idx = len(self._command_history)
        
        dpg.set_value("console_input", "")
        self.console.log(f">>> {text}", "INFO")

        # Execute Python
        import io
        old_stdout = sys.stdout
        old_stderr = sys.stderr
        sys.stdout = captured = io.StringIO()
        sys.stderr = captured
        try:
            result = eval(text)
            if result is not None:
                self.console.log(str(result), "INFO")
        except SyntaxError:
            try:
                exec(text, {"backend": self.backend, "studio": self,
                             "entities": self.entities})
            except Exception as e:
                self.console.log(f"Error: {e}", "ERROR")
        except Exception as e:
            self.console.log(f"Error: {e}", "ERROR")
        finally:
            sys.stdout = old_stdout
            sys.stderr = old_stderr

        output = captured.getvalue()
        if output.strip():
            for line in output.strip().split("\n"):
                self.console.log(line, "INFO")

        # Render console
        self._render_console_logs()

    def _render_console_logs(self) -> None:
        dpg.delete_item("console_log", children_only=True)
        filter_val = dpg.get_value("console_filter") if dpg.does_item_exist("console_filter") else "ALL"
        for log in self.console.logs[-100:]:
            if filter_val != "ALL" and log["level"] != filter_val:
                continue
            color = {
                "INFO": (200, 200, 200),
                "WARN": (255, 200, 80),
                "ERROR": (255, 80, 80),
            }.get(log["level"], (200, 200, 200))
            dpg.add_text(log["text"], parent="console_log", color=color)

    def _on_asset_filter(self, sender=None, app_data=None) -> None:
        self._on_asset_refresh()

    def _on_asset_refresh(self) -> None:
        dpg.delete_item("asset_list", children_only=True)
        search = dpg.get_value("asset_search") if dpg.does_item_exist("asset_search") else ""

        if self.backend:
            try:
                files = self.backend.vfs_list_files("/")
                if search:
                    files = [f for f in files if search.lower() in f.lower()]
                for f in files[:200]:
                    fl = f.lower()
                    icon = "📄"
                    if fl.endswith((".glb", ".gltf", ".obj")): icon = "📦"
                    elif fl.endswith((".png", ".jpg", ".dds", ".jpeg")): icon = "🖼️"
                    elif fl.endswith((".wav", ".ogg", ".mp3")): icon = "🎵"
                    elif fl.endswith(".json"): icon = "🗄️"
                    elif fl.endswith((".py", ".rs", ".wgsl", ".md")): icon = "📜"
                    dpg.add_selectable(label=f"  {icon} {f}", parent="asset_list")
            except Exception as e:
                dpg.add_text(f"Error: {e}", parent="asset_list", color=(255, 80, 80))
        else:
            # Scan local assets directory
            assets_dir = os.path.join(_root, "assets")
            if os.path.isdir(assets_dir):
                found = []
                for root, _, files in os.walk(assets_dir):
                    for f in files:
                        rel = os.path.relpath(os.path.join(root, f), _root).replace("\\", "/")
                        if search and search.lower() not in rel.lower():
                            continue
                        found.append(rel)
                for f in sorted(found)[:200]:
                    fl = f.lower()
                    icon = "📄"
                    if fl.endswith((".glb", ".gltf", ".obj")): icon = "📦"
                    elif fl.endswith((".png", ".jpg", ".dds", ".jpeg")): icon = "🖼️"
                    elif fl.endswith((".wav", ".ogg", ".mp3")): icon = "🎵"
                    elif fl.endswith(".json"): icon = "🗄️"
                    elif fl.endswith((".py", ".rs", ".wgsl", ".md")): icon = "📜"
                    dpg.add_selectable(label=f"  {icon} {f}", parent="asset_list")
            else:
                dpg.add_text("No assets directory found", parent="asset_list")

    def _update_profiler(self) -> None:
        if self.frame_count == 0 and self.fps > 0:
            dpg.set_value("profiler_fps", f"FPS: {self.fps:.1f}")
            dpg.set_value("profiler_entities", f"Entities: {len(self.entities)}")
            dpg.set_value("profiler_frametime", f"Frame Time: {1000.0/max(self.fps, 1):.1f}ms")
            
            if self.backend:
                try:
                    vram_bytes = self.backend.get_vram_usage()
                    vram_mb = vram_bytes / (1024 * 1024)
                    dpg.set_value("profiler_vram", f"VRAM (Textures): {vram_mb:.1f} MB")
                except Exception:
                    pass

            self._fps_history.append(self.fps)
            if len(self._fps_history) > 120:
                self._fps_history = self._fps_history[-120:]

            dpg.set_value("fps_series", [list(range(len(self._fps_history))), self._fps_history])
            dpg.fit_axis_data("fps_x_axis")
            dpg.fit_axis_data("fps_y_axis")

    def _on_play(self) -> None:
        self.play_mode.play()
        dpg.set_value("viewport_mode", f"Mode: {self.play_mode.state}")
        dpg.set_value("play_state_text", "▶ PLAYING")
        dpg.bind_item_theme("viewport_panel", self.play_theme)
        self.console.log("[PlayMode] Playing", "INFO")
        
        # Build Runtime Behaviours
        self._runtime_behaviours = []
        import importlib.util
        import sys
        import os
        from starlight.framework import Entity as FrameworkEntity
        from starlight.framework import Transform
        from starlight.behaviour import StarlightBehaviour
        
        for ent_data in self.entities:
            # Native Components Injection
            native_comps = ent_data.get("native_components", {})
            for c_type, c_data in native_comps.items():
                if c_type == "Health":
                    try: self.backend.add_component_health(ent_data["id"], c_data.get("current", 100.0), c_data.get("max", 100.0))
                    except Exception: pass
                elif c_type == "Mana":
                    try: self.backend.add_component_mana(ent_data["id"], c_data.get("current", 100.0), c_data.get("max", 100.0))
                    except Exception: pass
                # Note: PointLight could spawn actual lights in backend if we expose add_point_light or modify global array

            behaviours_paths = ent_data.get("behaviours", [])
            if behaviours_paths:
                # Mock a framework Entity pointing to the backend ID
                fw_ent = FrameworkEntity.__new__(FrameworkEntity)
                fw_ent.id = ent_data["id"]
                fw_ent.name = ent_data["name"]
                
                pos = ent_data.get("position", [0,0,0])
                fw_ent.transform = Transform(*pos)
                
                for b_path in behaviours_paths:
                    if not os.path.exists(b_path):
                        self.console.log(f"Script not found: {b_path}", "ERROR")
                        continue
                        
                    module_name = os.path.splitext(os.path.basename(b_path))[0]
                    spec = importlib.util.spec_from_file_location(module_name, b_path)
                    if spec and spec.loader:
                        mod = importlib.util.module_from_spec(spec)
                        try:
                            spec.loader.exec_module(mod)
                            # Find subclass of StarlightBehaviour
                            for name in dir(mod):
                                obj = getattr(mod, name)
                                if isinstance(obj, type) and issubclass(obj, StarlightBehaviour) and obj is not StarlightBehaviour:
                                    inst = obj()
                                    inst.entity = fw_ent
                                    inst.on_start()
                                    self._runtime_behaviours.append(inst)
                                    self.console.log(f"Started behaviour {name} on {ent_data['name']}", "INFO")
                        except Exception as e:
                            self.console.log(f"Error loading script {b_path}: {e}", "ERROR")

        self._render_console_logs()

    def _on_pause(self) -> None:
        self.play_mode.pause()
        dpg.set_value("viewport_mode", f"Mode: {self.play_mode.state}")
        dpg.set_value("play_state_text", "⏸ PAUSED")
        self.console.log("[PlayMode] Paused", "INFO")
        self._render_console_logs()

    def _on_stop(self) -> None:
        self.play_mode.stop()
        
        # Cleanup Runtime Behaviours
        if hasattr(self, "_runtime_behaviours"):
            for b in self._runtime_behaviours:
                try:
                    b.on_destroy()
                except Exception: pass
            self._runtime_behaviours.clear()
            
        dpg.set_value("viewport_mode", f"Mode: {self.play_mode.state}")
        dpg.set_value("play_state_text", "")
        dpg.bind_item_theme("viewport_panel", self.edit_theme)
        self.console.log("[PlayMode] Stopped", "INFO")
        self._render_console_logs()


def main():
    studio = StarlightStudio()
    studio.run()


if __name__ == "__main__":
    main()
