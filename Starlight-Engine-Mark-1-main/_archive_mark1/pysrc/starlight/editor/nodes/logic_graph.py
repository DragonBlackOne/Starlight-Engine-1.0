import dearpygui.dearpygui as dpg

class LogicGraphEditor:
    
    def __init__(self, studio=None):
        self.studio = studio
        self.window_tag = "logic_graph_window"
        self.node_editor_tag = "logic_node_editor"
        self.links = [] # list of (id1, id2, link_id)
        
        self.logical_nodes = {} # node_id -> dict
        self.pin_to_node = {} # pin_id -> node_id
        self.pin_to_name = {} # pin_id -> name
        
        self._next_id = 2000
    
    def show(self):
        if dpg.does_item_exist(self.window_tag):
            dpg.focus_item(self.window_tag)
            return

        with dpg.window(label="Logic Graph (Blueprint)", tag=self.window_tag, width=900, height=600):
            with dpg.group(horizontal=True):
                dpg.add_button(label="Play/Execute Logic", callback=self.execute_graph)
                dpg.add_separator()
                dpg.add_button(label="+ Event: On Start", callback=lambda: self.add_event_node("On Start"))
                dpg.add_button(label="+ Event: On Update", callback=lambda: self.add_event_node("On Update"))
                dpg.add_button(label="+ Action: Print", callback=lambda: self.add_action_node("Print"))
                dpg.add_button(label="+ Action: Set Position", callback=lambda: self.add_action_node("Set Position"))
                dpg.add_button(label="+ Flow: Branch/If", callback=self.add_branch_node)

            # The main canvas
            with dpg.node_editor(tag=self.node_editor_tag, callback=self.link_callback, delink_callback=self.delink_callback):
                pass
                
    def _register_node(self, node_id, ntype, exec_inputs, exec_outputs, data_inputs, data_outputs):
        self.logical_nodes[node_id] = {
            "type": ntype,
            "exec_in": exec_inputs,
            "exec_out": exec_outputs,
            "data_in": data_inputs,
            "data_out": data_outputs,
            "data_values": {} # stores id from dpg.add_input_text etc
        }
        for (name, pid) in exec_inputs + data_inputs + exec_outputs + data_outputs:
            self.pin_to_node[pid] = node_id
            self.pin_to_name[pid] = name

    def link_callback(self, sender, app_data):
        id1, id2 = app_data[0], app_data[1]
        
        node1 = self.pin_to_node.get(id1)
        node2 = self.pin_to_node.get(id2)
        if not node1 or not node2: return
        
        # Link logic: one must be output, one input
        is_id1_out = any(pid == id1 for (n, pid) in self.logical_nodes[node1]["exec_out"] + self.logical_nodes[node1]["data_out"])
        out_pin, in_pin = (id1, id2) if is_id1_out else (id2, id1)

        link = dpg.add_node_link(out_pin, in_pin, parent=sender)
        self.links.append((out_pin, in_pin, link))

    def delink_callback(self, sender, app_data):
        dpg.delete_item(app_data)
        self.links = [l for l in self.links if l[2] != app_data]

    def add_event_node(self, event_type="On Start"):
        with dpg.node(label=f"Event: {event_type}", parent=self.node_editor_tag, pos=[50, 100]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output, shape=dpg.mvNode_PinShape_TriangleFilled) as pin_exec_out:
                dpg.add_text("Exec")
        self._register_node(node_id, event_type, [], [("Exec", pin_exec_out)], [], [])

    def add_action_node(self, action_type="Print"):
        with dpg.node(label=f"Action: {action_type}", parent=self.node_editor_tag, pos=[250, 100]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_TriangleFilled) as pin_exec_in:
                dpg.add_text("Exec In")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output, shape=dpg.mvNode_PinShape_TriangleFilled) as pin_exec_out:
                dpg.add_text("Exec Out")
                
            data_inputs = []
            if action_type == "Print":
                with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_CircleFilled) as pin_data:
                    val_id = dpg.add_input_text(label="Message", default_value="Hello Starlight", width=150)
                data_inputs.append(("Message", pin_data))
                self._register_node(node_id, action_type, [("Exec In", pin_exec_in)], [("Exec Out", pin_exec_out)], data_inputs, [])
                self.logical_nodes[node_id]["data_values"]["Message"] = val_id

            elif action_type == "Set Position":
                with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_CircleFilled) as pin_ent:
                    ent_id = dpg.add_input_int(label="Entity ID", default_value=1, width=100)
                with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_CircleFilled) as pin_pos:
                    pos_id = dpg.add_input_floatx(label="Position", size=3, width=150)
                data_inputs.extend([("Entity ID", pin_ent), ("Position", pin_pos)])
                self._register_node(node_id, action_type, [("Exec In", pin_exec_in)], [("Exec Out", pin_exec_out)], data_inputs, [])
                self.logical_nodes[node_id]["data_values"]["Entity ID"] = ent_id
                self.logical_nodes[node_id]["data_values"]["Position"] = pos_id

    def add_branch_node(self):
        with dpg.node(label="Branch (If)", parent=self.node_editor_tag, pos=[250, 250]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_TriangleFilled) as pin_exec_in:
                dpg.add_text("Exec In")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_CircleFilled) as pin_cond:
                cond_id = dpg.add_checkbox(label="Condition")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output, shape=dpg.mvNode_PinShape_TriangleFilled) as pin_true:
                dpg.add_text("True")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output, shape=dpg.mvNode_PinShape_TriangleFilled) as pin_false:
                dpg.add_text("False")
                
        self._register_node(node_id, "Branch", [("Exec", pin_exec_in)], [("True", pin_true), ("False", pin_false)], [("Condition", pin_cond)], [])
        self.logical_nodes[node_id]["data_values"]["Condition"] = cond_id

    def execute_graph(self):
        # Extremely basic tree-walking MVP interpreter
        if self.studio:
            self.studio.console.log("Starting Graph Execution...", "INFO")
        else:
            print("Executing Graph...")
            
        # Find entry points (On Start)
        for node_id, data in self.logical_nodes.items():
            if data["type"] == "On Start":
                self._execute_node(node_id)
                
    def _execute_node(self, node_id):
        if node_id not in self.logical_nodes:
            return
            
        n_data = self.logical_nodes[node_id]
        ntype = n_data["type"]
        
        # Execute logic
        if ntype == "Print":
            msg = dpg.get_value(n_data["data_values"]["Message"])
            if self.studio:
                self.studio.console.log(f"[Logic] {msg}", "INFO")
            else:
                print(f"[Logic] {msg}")
        elif ntype == "Set Position":
            ent = dpg.get_value(n_data["data_values"]["Entity ID"])
            pos = dpg.get_value(n_data["data_values"]["Position"])
            if self.studio and self.studio.backend:
                try:
                    self.studio.backend.set_transform(ent, pos[0], pos[1], pos[2])
                    self.studio.console.log(f"[Logic] Moved Entity {ent} to {pos}", "INFO")
                except Exception as e:
                    self.studio.console.log(f"Set Position failed: {e}", "ERROR")

        # Flow to next connected node via Exec out
        for (out_name, out_pid) in n_data["exec_out"]:
            next_node = self._find_connected_node(out_pid)
            if next_node:
                # Special cases for flow control
                if ntype == "Branch":
                    cond = dpg.get_value(n_data["data_values"]["Condition"])
                    if out_name == "True" and cond:
                        self._execute_node(next_node)
                    elif out_name == "False" and not cond:
                        self._execute_node(next_node)
                else:
                    self._execute_node(next_node)

    def _find_connected_node(self, out_pin_id):
        for o_pin, i_pin, l_id in self.links:
            if o_pin == out_pin_id:
                return self.pin_to_node.get(i_pin)
        return None
