import bpy

obj = bpy.context.selected_objects[0]
polys = obj.data.polygons
verts = obj.data.vertices

collector = ""
collector += str(len(polys.values())*9) + "\n\n"
for poly in polys:
    vertsIndexes = poly.vertices
    vertsPoly = [verts[vertsIndexes[0]], verts[vertsIndexes[1]], verts[vertsIndexes[2]]]
    for vert in vertsPoly:
        collector += "%s, %s, %s, \n" % ("{:.8f}".format(float(vert.co.x)), "{:.8f}".format(float(vert.co.z)), "{:.8f}".format(float(vert.co.y)))

f = open("out.tris", "w")
f.write(collector)
f.close()
