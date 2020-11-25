# Improbability blender exporter

# this stuff is info for blender to know about the plugin
bl_info = {
  "name" : "Export model for Improbability Engine (.idm, idmd)",
  "author" : "Nicholas Baldwin",
  "version": (1, 0),
  "blender": (2, 5, 7),
  "api": 36079,
  "location": "File > Export",
  "description": "Export to the Improbability Model Formats (.idm) or (.idmd)",
  "warning": "",
  "category": "Import-Export"}

# to draw the gui
import os
# to output binary data
import struct
# to output xml data
import xml.dom.minidom
# to interface with blender
import bpy
# blender property definitions
from bpy.props import *

# translates a vector to have y and z rotate about x
def FixVectorOrientation( vec3 ):
  return Vector3(vec3.x, vec3.z, vec3.y);

class Vector3:
  def __init__(self, x, y, z):
    self.x = x;
    self.y = y;
    self.z = z;

  def print_xml(self, parent_node):
    parent_node.setAttribute("x", "%f" % self.x);
    parent_node.setAttribute("y", "%f" % self.y);
    parent_node.setAttribute("z", "%f" % self.z);

  def print_binary(self, output_file):
    data = struct.pack("=fff", self.x, self.y, self.z);
    output_file.write(data);

  def binary_size(self):
    return struct.calcsize("=fff");

  def __iadd__(self, other):
    self.x += other.x;
    self.y += other.y;
    self.z += other.z;
    return self;

class Vector2:
  def __init__(self, x, y):
    self.x = x;
    self.y = y;

  def print_xml(self, parent_node):
    parent_node.setAttribute("u", "%f" % self.x);
    parent_node.setAttribute("v", "%f" % self.y);

  def print_binary(self, output_file):
    data = struct.pack("=ff", self.x, self.y);
    output_file.write(data);

  def binary_size(self):
    return struct.calcsize("=ff");

class Color:
  def __init__(self, r, g, b, a=1.0):
    self.r = r;
    self.g = g;
    self.b = b;
    self.a = a;

  def print_xml(self, parent_node):
    parent_node.setAttribute("r", "%f" % self.x);
    parent_node.setAttribute("g", "%f" % self.y);
    parent_node.setAttribute("b", "%f" % self.z);
    parent_node.setAttribute("a", "%f" % self.z);

  def print_binary(self, output_file):
    data = struct.pack("=ffff", self.r, self.g, self.b, self.a);
    output_file.write(data);

  def binary_size(self):
    return struct.calcsize("=ffff");

class Vertex:
  def __init__(self, position, normal):
    self.position = position;
    self.normal   = normal;
    self.bones    = [];
    self.weights  = [];

  def print_xml(self, parent_node, xml_doc):
    pos_node = xml_doc.createElement("position");
    self.position.print_xml(pos_node);
    parent_node.appendChild(pos_node);
    norm_node = xml_doc.createElement("normal");
    self.normal.print_xml(norm_node);
    parent_node.appendChild(norm_node);
    if hasattr(self, 'color'):
      color_node = xml_doc.createElement("color");
      self.color.print_xml(color_node);
      parent_node.appendChild(color_node);
    if hasattr(self, 'uv'):
      uv_node = xml_doc.createElement("texcoord");
      self.uv.print_xml(uv_node);
      parent_node.appendChild(uv_node);
    if hasattr(self, 'tangent') and hasattr(self, 'bitangent'):
      tan_node = xml_doc.createElement("tangent");
      self.tangent.print_xml(tan_node);
      parent_node.appendChild(tan_node);
      bit_node = xml_doc.createElement("bitangent");
      self.bitangent.print_xml(bit_node);
      parent_node.appendChild(bit_node);
    if self.bones and self.weights:
      bones_node = xml_doc.createElement("bones");
      for (bone, weight) in zip(self.bones, self.weights):
        bone_node = xml_doc.createElement("bone");
        bone_node.setAttribute("id", "%d" % bone);
        bone_node.setAttribute("weight", "%f" % weight);
        bones_node.appendChild(bone_node);
      parent_node.appendChild(bones_node);


  def print_binary(self, output_file):
    data = struct.pack("=c", b'v');
    output_file.write(data);
    data = struct.pack("=c", b'p');
    output_file.write(data);
    self.position.print_binary(output_file);
    data = struct.pack("=c", b'n');
    output_file.write(data);
    self.normal.print_binary(output_file);
    if hasattr(self, 'color'):
      data = struct.pack("=c", b'c');
      output_file.write(data);
      self.color.print_binary(output_file);
    if hasattr(self, 'uv'):
      data = struct.pack("=c", b'u');
      output_file.write(data);
      self.uv.print_binary(output_file);
    if hasattr(self, 'tangent') and hasattr(self, 'bitangent'):
      data = struct.pack("=c", b't');
      output_file.write(data);
      self.tangent.print_binary(output_file);
      self.bitangent.print_binary(output_file);
    if self.bones and self.weights:
      data = struct.pack("=c", b'b');
      output_file.write(data);
      data = struct.pack("=H", len(self.bones));
      output_file.write(data);
      for bone in self.bones:
        data = struct.pack("=H", bone);
        output_file.write(data);
      for weight in self.weights:
        data = struct.pack("=f", weight);
        output_file.write(data);

  def binary_size(self):
    size = struct.calcsize("=c");
    size += struct.pack("=c");
    self.position.binary_size();
    size += struct.pack("=c");
    self.normal.binary_size();
    if hasattr(self, 'color'):
      size += struct.pack("=c");
      self.color.binary_size();
    if hasattr(self, 'uv'):
      size += struct.pack("=c");
      self.uv.binary_size();
    if hasattr(self, 'tangent') and hasattr(self, 'bitangent'):
      size += struct.pack("=c");
      self.tangent.binary_size();
      self.bitangent.binary_size();
    if self.bones and self.weights:
      size += struct.pack("=c");
      size += struct.pack("=H");
      for bone in self.bones:
        size += struct.pack("=H");
      for weight in self.weights:
        size += struct.pack("=f");

class Face:
  def __init__(self, v1, v2, v3):
    self.v1 = v1;
    self.v2 = v2;
    self.v3 = v3;

  def print_xml(self, parent_node):
    parent_node.setAttribute("v1", "%d" % self.v1);
    parent_node.setAttribute("v2", "%d" % self.v2);
    parent_node.setAttribute("v3", "%d" % self.v3);
  
  def print_binary(self, output_file):
    data = struct.pack("=III", self.v1, self.v2, self.v3);
    output_file.write(data);

  def binary_size(self):
    return struct.calcsize("=III");

class Mesh:
  def __init__(self):
    self.name = "";
    self.faces = [];
    self.vertices = [];

  def print_xml(self, parent_node, xml_doc):
    mesh_node = xml_doc.createElement("mesh");
    mesh_node.setAttribute("name", self.name);
    vert_buff_node = xml_doc.createElement("vertex_buffer");
    vert_buff_node.setAttribute("vertex_count", "%d" % len(self.vertices));
    for vertex in self.vertices:
      vertex_node = xml_doc.createElement("vertex");
      vertex.print_xml(vertex_node, xml_doc);
      vert_buff_node.appendChild(vertex_node);
    mesh_node.appendChild(vert_buff_node);
    face_buff_node = xml_doc.createElement("index_buffer");
    num_indexes = len(self.faces) * 3;
    face_buff_node.setAttribute("index_count", "%d" % num_indexes);
    for face in self.faces:
      face_node = xml_doc.createElement("face");
      face.print_xml(face_node);
      face_buff_node.appendChild(face_node);
    mesh_node.appendChild(face_buff_node);
    parent_node.appendChild(mesh_node);

  def print_binary(self, output_file):
    data = struct.pack("=c%dp" % (len(self.name)), b'm', self.name);
    output_file.write(data);
    data = struct.pack("=I", len(self.faces));
    output_file.write(data);
    for face in self.faces:
      face.print_binary(output_file);
    data = struct.pack("=I", len(self.vertices));
    output_file.write(data);
    for vertex in self.vertices:
      vertex.print_binary(output_file);

  def binary_size(self):
    size = struct.calcsize("=c%dpII" % (len(self.name)));
    for face in self.faces:
      size += face.binary_size();
    for vertex in self.vertices:
      size += vertex.binary_size();
    return size;

class ExportSettings:
  def __init__(self,
               blender_context,
               file_path,
               fix_coordinates = True,
               use_directx_uvs = True,
               export_only_selected = False,
               output_to_binary = True,
               verbosity = 0,
               export_color = False,
               export_tan_bit = False,
               export_bones = False):
    self.blender_context = blender_context;
    self.file_path = file_path;
    self.fix_coordinates = fix_coordinates;
    self.use_directx_uvs = use_directx_uvs;
    self.export_only_selected = export_only_selected;
    self.output_to_binary = output_to_binary;
    self.verbosity = verbosity;
    self.export_color = export_color;
    self.export_tan_bit = export_tan_bit;
    self.export_bones = export_bones;

def Export(settings):
  if settings.export_only_selected is True:
    objects = [obj for obj in settings.blender_context.selected_objects
               if obj.type in ("MESH", "EMPTY")];
  else:
    objects = [obj for obj in settings.blender_context.scene.objects
               if obj.type in ("MESH", "EMPTY")];

  xml_doc = xml.dom.minidom.Document();

  meshes = [];
  meshes = prepare_meshes(settings, objects);

  meshes_node = xml_doc.createElement("mesh_file");

  for mesh in meshes:
    mesh.print_xml(meshes_node, xml_doc);

  xml_doc.appendChild(meshes_node);

  out_file = open(settings.file_path, "w");
  out_file.write(xml_doc.toprettyxml(indent="  "));

def prepare_verts(settings, obj):
  vertices = [];

  data = obj.data;

  for vert in data.vertices:
    pos = Vector3(vert.co.x, vert.co.y, vert.co.z);
    norm = Vector3(vert.normal.x, vert.normal.y, vert.normal.z);

    if settings.fix_coordinates:
      pos = FixVectorOrientation(pos);
      norm = FixVectorOrientation(norm);

    new_vert = Vertex(pos, norm);

    if settings.export_color:
      new_vert.color = Color(obj.color[0], obj.color[1], obj.color[2], obj.color[3]);
    
    vertices.append(new_vert);

  return vertices;

def prepare_faces(data):
  faces = [];

  for face in data.faces:
    indeces = [];

    for index in face.vertices:
      indeces.append(index);

    if len(indeces) != 3:
      raise ValueError("The model is not triangulated! Go To Edit Moode, Press Ctrl-T, and try again\n");
    
    faces.append(Face(indeces[0], indeces[1], indeces[2]));
  
  return faces;

# def prepare_uvs(settings, uv_data):


#   return uv_face_list;

def prepare_tangents_bitangents(vertices, faces):
  tangents = [None] * len(vertices);
  bitangents = [None] * len(vertices);
  accumulates = [None] * len(vertices);

  for face in faces:
    v0 = vertices[face.v0].position;
    v1 = vertices[face.v1].position;
    v2 = vertices[face.v2].position;

    w0 = vertices[face.v0].uv;
    w1 = vertices[face.v1].uv;
    w2 = vertices[face.v2].uv;

    x1 = v1.x - v0.x;
    x2 = v2.x - v0.x;
    y1 = v1.y - v0.y;
    y2 = v2.y - v0.y;
    z1 = v1.z - v0.z;
    z2 = v2.z - v0.z;

    s1 = w1.x - w0.x;
    s2 = w2.x - w0.x;
    t1 = w1.y - w0.y;
    t2 = w2.y - w0.y;

    if (s1 * t2 != s2 * t1):
      r = 1.0 / (s1 * t2 - s2 * t1);
    else:
      r = 0.0;

    sdir = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
    tdir = Vector3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

    tangent[face.v0] += sdir;
    tangent[face.v1] += sdir;
    tangent[face.v2] += sdir;

    bitangent[face.v0] += tdir;
    bitangent[face.v1] += tdir;
    bitangent[face.v2] += tdir;

    accumulates[face.v0] += 1;
    accumulates[face.v1] += 1;
    accumulates[face.v2] += 1;

def prepare_meshes(settings, objects):
  meshes = [];

  for obj in objects:
    if obj.type == "MESH":
      mesh = Mesh();
      mesh.name = obj.name;

      data = obj.data;

      # prepare the vertices
      mesh.vertices = prepare_verts(settings, obj);

      # prepare the faces
      mesh.faces = prepare_faces(data);

      # prepare the UVs
      uv_data = None;
      for uv_layers in data.uv_textures:
        if uv_layers.active_render:
          uv_data = uv_layers.data;
          break;

      uv_face_list = [];
      if uv_data:
        for face in uv_data:
          coords = [];
          for coord in face.uv:
            u = coord[0];
            v = coord[1];
            if settings.use_directx_uvs:
              v = 1.0 - v;
            coords.append(Vector2(u, v));
          uv_face_list.append(Face(coords[0],coords[1], coords[2]));

      if len(uv_face_list) > 0:
        for (tris, uvs) in zip(mesh.faces, uv_face_list):
          mesh.vertices[tris.v1].uv = uvs.v1;
          mesh.vertices[tris.v2].uv = uvs.v2;
          mesh.vertices[tris.v3].uv = uvs.v3;

      # prepare the tangent and bitangents
      if settings.export_tan_bit:
        if not uv_data:
          raise("There are no UV data to generate tangents and bitangents from!");
        (tangents, bitangents, accumulates) = prepare_tangents_bitangents();

        for (vertex, tangent, bitangent, accumulate) in (mesh.vertices, tangents, bitangents, accumulates):
          vertex.tangent = tangent / accumulate;
          vertex.bitangent = bitangent / accumulate;

      # prepare the bones and weights
      # if settings.export_bones:
      #   arm = obj.find_armature();
      #   if not arm:
      #     raise("There is no armature on the object!");

      #   (bones, weights) = prepare_bones(obj, arm);

      meshes.append(mesh);
  
  return meshes;

VerbosityOptions = [];
VerbosityOptions.append(("0", "No Verbose", ""));
VerbosityOptions.append(("1", "Normal Verbosity", ""));

class ExportMenu(bpy.types.Operator):
  """Export to Improbability Drive Model Formats""";

  # Blender Operator names
  bl_idname = "export.idm";
  bl_label = "Export Improbability Drive Model Format";

  # This is a file selector menu
  filepath = StringProperty(subtype="FILE_PATH");

  # Options
  fix_coordinates = BoolProperty(name="Fix Coordinates",
                                 description="Rotate about X axis to fix Y and Z",
                                 default=True);

  use_directx_uvs = BoolProperty(name="Map UVs to DirectX Form",
                                 description="V = 1-V, brings UVs to DirectX UV form",
                                 default=True);

  export_only_selected = BoolProperty(name="Export only Selected",
                                      description="If set, will only export the selected items, else, will export every item in the scene",
                                      default=False);

  output_to_binary = BoolProperty(name="Output As Binary",
                                  description="If set, compiles and outputs the model in binary form (.idm), else, outputs the model in human readable ascii (.idmd)",
                                  default=True);

  verbosity = EnumProperty(name="Verbosity",
                           description="Choose the verbosity of the exporter",
                           items=VerbosityOptions,
                           default="0");

  export_color = BoolProperty(name="Export Color",
                              description="Export the color of the object",
                              default=False);

  export_tan_bit = BoolProperty(name="Export Tangents and Bitangents",
                                description="Export the tangents and bitangents of the object",
                                default=False);

  export_bones = BoolProperty(name="Export Bones and Weights",
                              description="Export the bone ids and weights of each vertex, non-rigged, non-animated objects shouldn't do this",
                              default=False);

  # # Polls to see if there is an object to run on
  # def poll(self, cls, context):
  #   return context.object is not None;

  def execute(self, context):
    file_name = self.filepath;
    if not file_name.lower().endswith(".idm") and self.output_to_binary is True:
      file_name += ".idm";
    if not file_name.lower().endswith(".idmd") and self.output_to_binary is False:
      if file_name.lower().endswith(".idm"):
        file_name.replace('.idm', '.idmd');
      else:
        file_name += ".idmd";

    settings = ExportSettings(context, 
                              file_name, 
                              self.fix_coordinates, 
                              self.use_directx_uvs,
                              self.export_only_selected,
                              self.output_to_binary,
                              self.verbosity,
                              self.export_color,
                              self.export_tan_bit,
                              self.export_bones);

    Export(settings);
    return {"FINISHED"};

  def invoke(self, context, event):
    WindowManager = context.window_manager;
    WindowManager.fileselect_add(self);
    return {"RUNNING_MODAL"};


def menu_func(self, context):
  default_path = os.path.splitext(bpy.data.filepath)[0] + ".idm"
  self.layout.operator(ExportMenu.bl_idname, text="Improbability Model Exporter").filepath = default_path

def register():
  bpy.utils.register_module(__name__)
  bpy.types.INFO_MT_file_export.append(menu_func)


def unregister():
  bpy.utils.unregister_module(__name__)
  bpy.types.INFO_MT_file_export.remove(menu_func)


if __name__ == "__main__":
  register()