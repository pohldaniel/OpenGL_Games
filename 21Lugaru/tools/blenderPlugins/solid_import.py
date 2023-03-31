#!BPY

""" Registration for Blender menus
Name: 'Lugaru models (.solid)...'
Blender: 249b
Group: 'Import'
Tooltip: 'Import from SOLID format.'
"""
# solid_import.py version 1.0
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever, as long as you abide to the restrictions mentioned above


# This script imports SOLID mesh definition data
# The SOLID format (c) David Rosen is a binary 3D mesh format
# It holds
#         XYZ vertices (float float float)
#         ABC triangle associations of vertices (short short short)
#         UV mapping coordinates (float float)

# BODY SOLIDs are a special case, there is additional data after the mesh definition data
# This additional data is relevant only for the game the SOLID came from
# This importer code is based upon reverse-engineering the format that's used in Lugaru
# Note that this code is INCOMPATIBLE with Black Shades SOLIDs
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct
import math  #<<

from math import pi  #<<
from struct import *
from Blender import Mesh, Object, Material, Texture, Image, Draw, BGL
from Blender.BGL import *

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
        Blender.Window.FileSelector(my_callback, "Import Solid", "BODYMESH.solid")
        #Draw.Exit()
    if evt == 2:
        print("weapon")
        isbody = 0
        isweapon = 1
        isimmo = 0
        Blender.Window.FileSelector(my_callback, "Import Solid", "WEAPONMESH.solid")
        #Draw.Exit()
    if evt == 3:
        print("immo")
        isbody = 0
        isweapon = 0
        isimmo = 1
        Blender.Window.FileSelector(my_callback, "Import Solid", "IMMOBILEMESH.solid")
        #Draw.Exit()

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
    Draw.Text("Solid Importer 1.0 ",'large')
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 365)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 340)
    Draw.Text("Import Lugaru models into Blender", 'small')
    glRasterPos2i(40, 315)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 303)
    Draw.Text("The SOLID format (c) David Rosen", 'small')
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

class solidImport:
    def parseSolidFloat(self, fourchars):
        f = struct.unpack('>f', str(fourchars))
        return f

    def __init__(self, filename):
        global my_path
        self.file = open(filename, "rb")
        #my_path = Blender.sys.dirname(filename)

    def Import(self):

        if(isbody == 1):
            body = 1
            scale = 10
            rotx = pi/2.0  #<<
            roty = 0.0  #<<
            rotz = -pi/2.0  #<<
        if(isweapon == 1):
            body = 0
            scale = 1
            rotx = 0.0  #<<
            roty = 0.0  #<<
            rotz = pi  #<<
        if(isimmo == 1):
            body = 0
            scale = 200
            rotx = pi  #<<
            roty = 0.0  #<<
            rotz = 0.0  #<<
        factor = 0.001*scale
        bytecount = 0
        print("Importing Solid into Blender ...")
        scene = bpy.data.scenes.active

        bytes = self.file.read(2) # TODO Unclear whether Python default encoding does the job on PPC
        bytecount += len(bytes)
        noofverts = ord(bytes[0])*256+ord(bytes[1])
        bytes = self.file.read(2)
        bytecount += len(bytes)
        nooftris = ord(bytes[0])*256+ord(bytes[1])
        print("  #verts: "+str(noofverts))
        print("  #tris:  "+str(nooftris))
        coords = []
        bytes = self.file.read(noofverts*4*3)
        bytecount += len(bytes)

        # a vert has 3 float components - xyz
        for i in range(0, noofverts*3):
            # a float is 4 bytes.
            coords.append((self.parseSolidFloat(bytes[4*i]+bytes[4*i+1]+bytes[4*i+2]+bytes[4*i+3])[0]*factor))
        vertlist = []

        for j in range(0, noofverts):
            # assemble verts from coords
            vertlist.append((coords[3*j],coords[3*j+1],coords[3*j+2]))

        tris = []
        texmap = []
        # a float is 4 bytes
        bytes = self.file.read((nooftris*2*6)+(nooftris*4*6)) # Includes trimap, texmap, garbage data

        bytecount += len(bytes)

        for k in range(0, nooftris):
            texmapoffset = k*6*4 # byterange of texmap with garbage
            o = k*12 + texmapoffset # byterange of triangle indices and texmap with garbage

            # Note that garbage is implicitly omitted here!
            i0 = ord(bytes[o])*256+ord(bytes[o+1])
            i1 = ord(bytes[o+2])*256+ord(bytes[o+3])
            i2 = ord(bytes[o+6])*256+ord(bytes[o+7])

            if(scale>1):
                if(body==1):
                    tris.append((i0,i2,i1))
                else:
                    tris.append((i0,i1,i2))
            else:
                tris.append((i0,i2,i1))

            # texmap follows

            # Note that garbage is implicitly omitted here as well!
            f0 = self.parseSolidFloat(bytes[o+12]+bytes[o+13]+bytes[o+14]+bytes[o+15])[0]
            f1 = self.parseSolidFloat(bytes[o+16]+bytes[o+17]+bytes[o+18]+bytes[o+19])[0]
            f2 = self.parseSolidFloat(bytes[o+20]+bytes[o+21]+bytes[o+22]+bytes[o+23])[0]
            f3 = self.parseSolidFloat(bytes[o+24]+bytes[o+25]+bytes[o+26]+bytes[o+27])[0]
            f4 = self.parseSolidFloat(bytes[o+28]+bytes[o+29]+bytes[o+30]+bytes[o+31])[0]
            f5 = self.parseSolidFloat(bytes[o+32]+bytes[o+33]+bytes[o+34]+bytes[o+35])[0]

            if(scale>1):
                if(body==1):
                    texmap.append(((f0,1.0-f3),(f2,1.0-f5),(f1,1.0-f4)))
                else:
                    texmap.append(((f0,f3),(f1,f4),(f2,f5)))
            else:
                texmap.append(((f0,f3),(f2,f5),(f1,f4)))

        print("  total bytecount: "+str(bytecount))
        mesh = Mesh.New()
        mesh.verts.extend(vertlist)
        mesh.faces.extend(tris)

        # Setting UV mapping
        mesh.faceUV = True
        counter = 0
        for f in mesh.faces:
            current = f.uv
            for l in range(0, 3):
                current[l].x = texmap[counter][l][0]
                current[l].y = texmap[counter][l][1]
            counter+=1
        mobj = scene.objects.new(mesh)  #<<
        mobj.RotX=rotx  #<<
        mobj.RotY=roty  #<<
        mobj.RotZ=rotz  #<<
        del coords
        del vertlist
        del tris
        del texmap
        del bytes
        print("... finished!")


def my_callback(filename):
    if not filename.lower().endswith('.solid'): print("Not a .solid extension!")
    solidimport = solidImport(filename)
    solidimport.Import()
    Draw.Redraw()

arg = __script__['arg']
