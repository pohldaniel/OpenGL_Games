#!BPY

""" Registration for Blender menus
Name: 'Lugaru maps...'
Blender: 249b
Group: 'Import'
Tooltip: 'Import maps.'
"""
# lugaru_map_import.py version 1.1
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever,
# as long as you abide to the restrictions mentioned above


# This script imports Lugaru map definition data
# The Lugaru map format (c) David Rosen is a binary 3D map format
# It holds
#         stuff

# This importer code is based upon reverse-engineering the format that's used in Lugaru
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct
import os
import math

from math import pi
from struct import *
from Blender import Mesh, Object, Material, Texture, Image, Draw, BGL, Ipo, BezTriple, Mathutils
from Blender.Object import ProtectFlags
from Blender.BGL import *
from Blender.Mathutils import *


global error_s_1
global error_s_2
error_s_1 = ""
error_s_2 = ""


def event(evt, val):
    if evt == Draw.ESCKEY:
        Draw.Exit()
        return

def button_event(evt):
    arg = __script__['arg']
    if evt == 1:
        Blender.Window.FileSelector(my_callback, "Import Map", "map")
        Draw.Redraw(1)
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
    Draw.Text("Lugaru Map Importer 1.1 ",'large')
    glRasterPos2i(210, 370)
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 355)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 330)
    Draw.Text("Import Lugaru maps into Blender", 'small')
    glRasterPos2i(40, 305)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 293)
    Draw.Text("The Lugaru map format (c) David Rosen", 'small')
    if not error_s_1 == "":
        glRasterPos2i(30, 137)
        Draw.Text("NOTE: "+error_s_1, 'small')
        glRasterPos2i(30, 124)
        Draw.Text(error_s_2, 'small')
    body_butt   = Draw.Button("Map", 1, 30, 150, 75, 30, "map")
    exit_butt   = Draw.Button("Exit", 9, 220, 30, 75, 30, "exit")

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

def loadModels(modelpath, scene, instance):
    rotx = pi
    roty = 0.0
    rotz = 0.0
    importSolid(modelpath, "Box", scene, instance, 0)
    importSolid(modelpath, "Weird", scene, instance, 1)
    importSolid(modelpath, "Spike", scene, instance, 2)
    importSolid(modelpath, "Treetrunk", scene, instance, 3)
    importSolid(modelpath, "Leaves", scene, instance, 4)
    importSolid(modelpath, "Bush", scene, instance, 5)
    importSolid(modelpath, "Rock", scene, instance, 6)
    importSolid(modelpath, "Wall", scene, instance, 7)
    importSolid(modelpath, "Chimney", scene, instance, 8)
    importSolid(modelpath, "Platform", scene, instance, 9)
    # Hidden is already in the scene
    importSolid(modelpath, "Tunnel", scene, instance, 11)
    importSolid(modelpath, "Cool", scene, instance, 12)
    # Fire is already in the scene

def importSolid(filename, fprefix, scene, inst, meshindex):
    file = open(filename+fprefix+".solid", "rb")

    body = 0
    scale = 1#200
    factor = 1.0/3.0 #0.001*scale

    scene = bpy.data.scenes.active

    bytes = file.read(2)
    noofverts = ord(bytes[0])*256+ord(bytes[1])
    bytes = file.read(2)
    nooftris = ord(bytes[0])*256+ord(bytes[1])
    coords = []
    bytes = file.read(noofverts*4*3)

    for i in range(0, noofverts*3):
        coords.append((inst.parseSolidFloat(bytes[4*i]+bytes[4*i+1]+bytes[4*i+2]+bytes[4*i+3])[0]*factor))
    vertlist = []

    for j in range(0, noofverts):
        vertlist.append((coords[3*j],coords[3*j+1],coords[3*j+2]))

    tris = []
    texmap = []
    bytes = file.read((nooftris*2*6)+(nooftris*4*6))

    for k in range(0, nooftris):
        texmapoffset = k*6*4
        o = k*12 + texmapoffset
        i0 = ord(bytes[o])*256+ord(bytes[o+1])
        i1 = ord(bytes[o+2])*256+ord(bytes[o+3])
        i2 = ord(bytes[o+6])*256+ord(bytes[o+7])

        tris.append((i0,i2,i1))

        f0 = inst.parseSolidFloat(bytes[o+12]+bytes[o+13]+bytes[o+14]+bytes[o+15])[0]
        f1 = inst.parseSolidFloat(bytes[o+16]+bytes[o+17]+bytes[o+18]+bytes[o+19])[0]
        f2 = inst.parseSolidFloat(bytes[o+20]+bytes[o+21]+bytes[o+22]+bytes[o+23])[0]
        f3 = inst.parseSolidFloat(bytes[o+24]+bytes[o+25]+bytes[o+26]+bytes[o+27])[0]
        f4 = inst.parseSolidFloat(bytes[o+28]+bytes[o+29]+bytes[o+30]+bytes[o+31])[0]
        f5 = inst.parseSolidFloat(bytes[o+32]+bytes[o+33]+bytes[o+34]+bytes[o+35])[0]

        texmap.append(((f0,1.0-f3),(f2,1.0-f5),(f1,1.0-f4)))

    mesh = ""
    try:
        mesh = Mesh.Get("m_"+fprefix)
    except:
        print(fprefix+": No such mesh exists yet.")
        mesh = Mesh.New("m_"+fprefix)
    mesh.verts.delete(mesh.verts)
    #mesh.faces = []
    mesh.verts.extend(vertlist)
    mesh.faces.extend(tris)
    mesh.faceUV = True
    counter = 0
    proto = ""
    for f in mesh.faces:
        current = f.uv
        for l in range(0, 3):
            current[l].x = texmap[counter][l][0]
            current[l].y = texmap[counter][l][1]
        counter+=1
    # Fix mesh rx, rz and sxyz so that mapping is pixel perfect
    #print "PPT"
    zalt = 1.0
    if meshindex == 9:
        zalt = -1.0
    rxm = RotationMatrix(1.0, 4, "y")
    rzm = RotationMatrix(zalt, 4, "z")
    sxyzm = ScaleMatrix(0.9, 4)
    mesh.transform(rxm)
    mesh.transform(rzm)
    mesh.transform(sxyzm)
    #print "APPT"
    try:
        proto = Object.Get("The_"+fprefix)
        #proto.mesh = mesh
    except:
        proto = scene.objects.new(mesh, "The_"+fprefix)
    proto.layers = [2]
    Blender.Redraw()
    del coords
    del vertlist
    del tris
    del texmap
    del bytes
    return mesh


Draw.Register(draw, event, button_event)

def caprot(rot):
    while rot > 360.0:
        rot -= 360.0
    while rot < 0.0:
        rot += 360.0
    return rot

def introt(rot):
    rot = caprot(rot)
    return float(int(rot))

def r1fix(rot):
    rot = caprot(rot)
    return float(int(rot/30.0))*30.0

class mapImport:
    def parseSolidFloat(self, fourchars):
        f = struct.unpack('>f', str(fourchars))
        return f

    def __init__(self, filename):
        global my_path
        self.file = open(filename, "rb")
        self.filename = filename
        #my_path = Blender.sys.dirname(filename)

    def Import(self):
        global error_s_1, error_s_2

        error_s_1 = ""
        error_s_2 = ""
        filename = self.filename.split(r"/")
        filename = self.filename.split("\\")
        maptitle = filename[len(filename) - 1]
        scene = bpy.data.scenes.active

        modelpath = os.path.split(os.path.split(self.filename)[0])[0]+"/Models/"
        print("-- -- #################### import "+maptitle)
        debugtext = maptitle

        bytecount = 0

        try:
            loadModels(modelpath, scene, self)
        except:
            error_s_1 = "Failed loading models from"
            error_s_2 = modelpath

        #print "Importing Lugaru Map into Blender ..."

        # We need this a lot
        r=256*256

        # Character position and other junk
        bytes = self.file.read(65)
        bytecount += len(bytes)
        #print str(bytes)
        # Whether or not we have a +4 offset
        bytes = self.file.read(4)
        bytecount += len(bytes)
        # offset
        o=0
        if ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3]) > 0:
            bytes = self.file.read(4)
            bytecount += len(bytes)
            #print "O!"
        bytes = self.file.read(11*4)
        bytecount += len(bytes)
        # Number of clothing textures
        bytes = self.file.read(4)
        bytecount += len(bytes)
        ctex = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
        # Irrelevant
        bytes = self.file.read(8)
        bytecount += len(bytes)
        # Whether we have dialogue
        bytes = self.file.read(4)
        bytecount += len(bytes)
        dialoguesections = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
        debugtext += " Number of dialogue sections: "+str(dialoguesections)
        if dialoguesections>0:
            debugtext += "("
        for i in range(0, dialoguesections):
            if i>0:
                debugtext +=", "
            # number of lines
            bytes = self.file.read(4)
            bytecount += len(bytes)
            #print "L "+str(bytecount)
            d = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
            debugtext +=str(d)
            # Crap we don't care about
            bytes = self.file.read(184)
            bytecount += len(bytes)
            for i in range(0, d):
                # Number of letters for text
                bytes = self.file.read(4)
                bytecount += len(bytes)
                l = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
                #print str(l)
                # Actual text
                bytes = self.file.read(l)
                bytecount += len(bytes)
                # Speakername letters
                bytes = self.file.read(4)
                bytecount += len(bytes)
                s = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
                # Actual speakername with D at end
                bytes = self.file.read(s+1)
                bytecount += len(bytes)
                # Rest of line junk
                if(i+1<d):
                    bytes = self.file.read(167)
                else:
                    bytes = self.file.read(147)
                    #print "LAST"
                bytecount += len(bytes)
                #print str(bytes)+"--"+str(bytecount)

        if dialoguesections>0:
            debugtext += ")"

        for i in range(0, ctex):
            # Clothing texture reference letters count
            bytes = self.file.read(4)
            bytecount += len(bytes)
            ct = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
            #print str(bytes)+"##"+str(ct)
            bytes = self.file.read(ct)
            bytecount += len(bytes)
            # Something irrelevant
            bytes = self.file.read(12)
            bytecount += len(bytes)
        # Unknown what this is
        bytes = self.file.read(4)
        bytecount += len(bytes)
        # Number of objects
        bytes = self.file.read(4)
        bytecount += len(bytes)
        no = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
        debugtext += " Objects: "+str(no)

        # All objects end up in here
        impobj = []

        # Go through all objects
        for i in range(0, no):
            bytes = self.file.read(28)
            bytecount += len(bytes)
            # Type
            type = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
            # rotation X
            rx = self.parseSolidFloat(bytes[4]+bytes[5]+bytes[6]+bytes[7])[0]
            # rotation Y
            ry = self.parseSolidFloat(bytes[8]+bytes[9]+bytes[10]+bytes[11])[0]
            # position X
            px = self.parseSolidFloat(bytes[12]+bytes[13]+bytes[14]+bytes[15])[0]
            # position Y
            py = self.parseSolidFloat(bytes[16]+bytes[17]+bytes[18]+bytes[19])[0]
            # position Z
            pz = self.parseSolidFloat(bytes[20]+bytes[21]+bytes[22]+bytes[23])[0]
            # scale
            sc = self.parseSolidFloat(bytes[24]+bytes[25]+bytes[26]+bytes[27])[0]
            # DBG NO OBJS
            #continue
            ref = ""
            rpx = 90.0
            rpy = 180.0
            rpz = 0.0
            type_s = "UNDEFINED"
            if type == 0:
                type_s = "Box"
            elif type == 1:
                type_s = "Weird"
            elif type == 2:
                type_s = "Spike"
            elif type == 3:
                type_s = "Treetrunk"
            elif type == 4:
                # DBG NO TREE LEAVES
                #continue
                type_s = "Leaves"
            elif type == 5:
                # DBG NO BUSHES
                #continue
                type_s = "Bush"
            elif type == 6:
                ry -= rx*7.0
                type_s = "Rock"
            elif type == 7:
                type_s = "Wall"
            elif type == 8:
                type_s = "Chimney"
            elif type == 9:
                ry += 90.0
                type_s = "Platform"
            elif type == 10:
                type_s = "Hidden"
            elif type == 11:
                type_s = "Tunnel"
            elif type == 12:
                type_s = "Cool"
            elif type == 13:
                type_s = "Fire"

            ref = scene.objects.new(Mesh.Get("m_"+type_s), type_s)
            ref.properties['lugaru_map_elem_type'] = type
            ref.properties['lugaru_map_elem_type_s'] = type_s
            facLoc=0.25
            facSc =1.0/3.0
            #ref.LocX = pz*facLoc
            #ref.LocY = px*facLoc
            #ref.LocZ = py*facLoc
            ref.LocX = pz
            ref.LocY = px
            ref.LocZ = py
            # Still named Size instead of Scale
            # This might change from 2.5 and up!
            #ref.SizeX = sc*facSc
            #ref.SizeY = sc*facSc
            #ref.SizeZ = sc*facSc
            ref.SizeX = sc
            ref.SizeY = sc
            ref.SizeZ = sc
            #print type_s+": "+str(rx)+"-"+str(ry)+" "+str(sc)[:5]+" ("\
            #        +str(px)[:5]+", "+str(py)[:5]+", "+str(pz)[:5]+")"
            ref.RotY = rpz*(pi/180.0)
            ref.protectFlags = ref.protectFlags or ProtectFlags.ROTY
            ref.RotX = caprot(ry+rpy)*(pi/180.0)
            ref.RotZ = caprot(rx+rpx)*(pi/180.0)

        Blender.Redraw()

        print(debugtext)
        #print "-- "+str(impobj)
        return


def my_callback(filename):
    mapimport = mapImport(filename)
    mapimport.Import()
    Draw.Redraw()

arg = __script__['arg']
