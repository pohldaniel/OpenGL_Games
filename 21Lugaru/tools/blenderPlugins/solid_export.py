#!BPY

""" Registration for Blender menus
Name: 'Lugaru models (.solid)...'
Blender: 249b
Group: 'Export'
Tooltip: 'Export to SOLID format.'
"""
# solid_export.py version 1.0
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever, as long as you abide to the restrictions mentioned above


# This script exports SOLID mesh definition data
# The SOLID format (c) David Rosen is a binary 3D mesh format
# It holds
#         XYZ vertices (float float float)
#         ABC triangle associations of vertices (short short short)
#         UV mapping coordinates (float float)

# BODY SOLIDs are a special case, there is additional data after the mesh definition data
# This additional data is relevant only for the game the SOLID came from
# This exporter code is based upon reverse-engineering the format that's used in Lugaru
# Note that this code is INCOMPATIBLE with Black Shades SOLIDs
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct

from struct import *
from Blender import NMesh, Mesh, Object, Material, Texture, Image, Draw, BGL
from Blender.BGL import *

# Meant for detected User errors in the context of Solid export
class SolidUserError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

def event(evt, val):
    if evt == Draw.ESCKEY:
        Draw.Exit()
        return

def button_event(evt):
    global isbody,isweapon,isimmo
    arg = __script__['arg']
    if evt == 1:
        print("body")
        isbody = 1
        isweapon = 0
        isimmo = 0
        Blender.Window.FileSelector(my_callback, "Export Solid", "BODYMESH.solid")
        #Draw.Exit()
    if evt == 2:
        print("weapon")
        isbody = 0
        isweapon = 1
        isimmo = 0
        Blender.Window.FileSelector(my_callback, "Export Solid", "WEAPONMESH.solid")
        #Draw.Exit()
    if evt == 3:
        print("immo")
        isbody = 0
        isweapon = 0
        isimmo = 1
        Blender.Window.FileSelector(my_callback, "Export Solid", "IMMOBILEMESH.solid")
        #Draw.Exit()
    Draw.Redraw()
    if evt == 9:
        Draw.Exit()
        return

def draw():
    glClearColor(0.55,0.6,0.6,1)
    glClear(BGL.GL_COLOR_BUFFER_BIT)
    #external box
    glColor3f(0.2,0.3,0.3)
    rect(10,402,300,382)

    glColor3f(0.8,.8,0.6)
    glRasterPos2i(20, 380)
    Draw.Text("Solid Exporter 1.0 ",'large')
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 365)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 340)
    Draw.Text("Export selected Lugaru meshes from Blender", 'small')
    glRasterPos2i(40, 315)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 303)
    Draw.Text("The SOLID format (c) David Rosen", 'small')
    glRasterPos2i(40, 280)
    Draw.Text("Make sure it's just triangles and you've got a UV map!", 'small')
    glRasterPos2i(40, 268)
    Draw.Text("Also, BODY meshes don't work ingame yet.", 'small')
    glRasterPos2i(40, 248)
    Draw.Text("If you select multiple Objects, they will be exported as", 'small')
    glRasterPos2i(40, 236)
    Draw.Text("FILENAME_i - FILENAME being your chosen", 'small')
    glRasterPos2i(40, 224)
    Draw.Text("filename and i being a growing index number.", 'small')
    glRasterPos2i(108, 123)
    Draw.Text("Note that this program OVERWRITES files.", 'small')
    body_butt   = Draw.Button("Body",  1,30, 150,  75, 30, "body")
    weapon_butt = Draw.Button("Weapon",2,30, 110,  75, 30, "weapon")
    immo_butt   = Draw.Button("Immo",  3,30, 70, 75, 30, "immo")
    exit_butt   = Draw.Button("Exit",  9,220,30, 75, 30, "exit")

def rect(x,y,width,height):
    glBegin(GL_LINE_LOOP)
    glVertex2i(x,y)
    glVertex2i(x+width,y)
    glVertex2i(x+width,y-height)
    glVertex2i(x,y-height)
    glEnd()

def rectFill(x,y,width,height):
    glBegin(GL_POLYGON)
    glVertex2i(x,y)
    glVertex2i(x+width,y)
    glVertex2i(x+width,y-height)
    glVertex2i(x,y-height)
    glEnd()


Draw.Register(draw, event, button_event)

class solidExport:

    def toSolidFloat(self, fl):
        fp = struct.pack('>f', fl)
        return fp

    def toSolidShort(self, i):
        ip = struct.pack('>h', i)
        return ip

    def __init__(self, filename):
        global my_path
        self.filename = filename
        #my_path = Blender.sys.dirname(filename)

    def exportimpl(self, f, obj, factor):
        try:
            self.file = open(f, "wb")
            mesh = NMesh.GetRawFromObject(obj.name)
            noofverts = self.toSolidShort(len(mesh.verts))
            nooffaces = self.toSolidShort(len(mesh.faces))
            self.file.write(noofverts+nooffaces)
            if(isbody==1):
                tri0=0
                tri1=2
                tri2=1
                tm0=(0,0)
                tm1=(2,0)
                tm2=(1,0)
                tm3=(0,1)
                tm4=(2,1)
                tm5=(1,1)
                inv=1.0
                off=1.0
            if(isweapon==1):
                tri0=0
                tri1=2
                tri2=1
                tm0=(0,0)
                tm1=(2,0)
                tm2=(1,0)
                tm3=(0,1)
                tm4=(2,1)
                tm5=(1,1)
                inv=-1.0
                off=0.0
            if(isimmo==1):
                tri0=0
                tri1=1
                tri2=2
                tm0=(0,0)
                tm1=(1,0)
                tm2=(2,0)
                tm3=(0,1)
                tm4=(1,1)
                tm5=(2,1)
                inv=-1.0
                off=0.0
            for v in mesh.verts:
                self.file.write(self.toSolidFloat(v.co[0]*factor))
                self.file.write(self.toSolidFloat(v.co[1]*factor))
                self.file.write(self.toSolidFloat(v.co[2]*factor))
            for fa in mesh.faces:
                self.file.write(self.toSolidShort(fa.v[tri0].index))
                self.file.write(self.toSolidShort(fa.v[tri1].index))
                self.file.write(self.toSolidShort(fa.v[tri1].index))
                self.file.write(self.toSolidShort(fa.v[tri2].index))
                self.file.write(self.toSolidShort(fa.v[tri2].index))
                self.file.write(self.toSolidShort(0))
                current = fa.uv
                try:
                    self.file.write(self.toSolidFloat(current[tm0[0]][tm0[1]]))
                    self.file.write(self.toSolidFloat(current[tm1[0]][tm1[1]]))
                    self.file.write(self.toSolidFloat(current[tm2[0]][tm2[1]]))
                    self.file.write(self.toSolidFloat(off-(current[tm3[0]][tm3[1]]*inv)))
                    self.file.write(self.toSolidFloat(off-(current[tm4[0]][tm4[1]]*inv)))
                    self.file.write(self.toSolidFloat(off-(current[tm5[0]][tm5[1]]*inv)))
                except IndexError:
                    # Could have actually tested with faceUV.. meh
                    raise SolidUserError("You probably forgot to make a UV map!")
        except:
            self.file.close()
            raise
        self.file.close()

    def Export(self):

        scene = bpy.data.scenes.active
        obj = Object.GetSelected()
        if(len(obj)<1):
            raise SolidUserError("Select at least one Object to export.")
        else:
            s=""
            multiple = 0
            if(len(obj)>1):
                s="s"
                multiple = 1
            print("You selected "+str(len(obj))+" Object"+s+".")

        if(isbody == 1):
            factor = 100
        if(isweapon == 1):
            factor = 1000
        if(isimmo == 1):
            factor = 5

        print("Exporting Solid"+s+" from Blender ...")
        if(multiple==1):
            i = 0
            belowtenprefix = ""
            belowhundredprefix = ""
            iprefix = ""
            if(len(obj)>10):
                if(len(obj)>100):
                    belowtenprefix = "00"
                    belowhundredprefix = "0"
                else:
                    belowtenprefix = "0"
            for o in obj:
                # We skip over non-meshes, naturally
                if o.type == 'Mesh':
                    i+=1
                    if(i<10):
                        iprefix = belowtenprefix
                    else:
                        if(i<100):
                            iprefix = belowhundredprefix
                    self.exportimpl(self.filename+"_"+iprefix+str(i),o,factor)
        else:
            if(obj[0].type == 'Mesh'):
                self.exportimpl(self.filename,obj[0],factor)
            else:
                raise SolidUserError("You have to select at least one Mesh.")

        print("... finished!")
        return

def my_callback(filename):
    if not filename.lower().endswith('.solid'): print("Not a .solid extension!")
    solidexport = solidExport(filename)
    try:
        solidexport.Export()
    except:
        raise

arg = __script__['arg']
