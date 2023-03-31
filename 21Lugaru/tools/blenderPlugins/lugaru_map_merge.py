#!BPY

""" Registration for Blender menus
Name: 'Lugaru map merge...'
Blender: 249b
Group: 'Export'
Tooltip: 'Merge maps.'
"""
# lugaru_map_merge.py version 1.1
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever,
# as long as you abide to the restrictions mentioned above


# This script merges Lugaru map definition data
# The Lugaru map format (c) David Rosen is a binary 3D map format
# It holds
#         stuff

# This merger code is based upon reverse-engineering the format that's used in Lugaru
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct
import os
import math

from math import pi
from struct import *
from Blender import Mesh, Object, Material, Texture, Image, Draw, BGL, Ipo, BezTriple
from Blender.BGL import *


global error_s_1
global error_s_2
global error_s_3
global error_s_4
error_s_1 = ""
error_s_2 = ""
error_s_3 = ""
error_s_4 = ""


def event(evt, val):
    if evt == Draw.ESCKEY:
        Draw.Exit()
        return

def button_event(evt):
    arg = __script__['arg']

    if evt == 1:
        Blender.Window.FileSelector(my_callback_merge, "Merge Map", "map")
        Draw.Redraw(1)

    if evt == 2:
        Blender.Window.FileSelector(my_callback_export, "Export Map", "map")
        Draw.Redraw(1)

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
    # TODO Make this an exporter as well
    Draw.Text("Lugaru Map Merger 1.1 ",'large')
    glRasterPos2i(210, 370)
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 355)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 330)
    Draw.Text("Merge Lugaru maps from Blender", 'small')
    glRasterPos2i(40, 305)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 293)
    Draw.Text("The Lugaru map format (c) David Rosen", 'small')
    glRasterPos2i(40, 261)
    Draw.Text("Merged files automatically get the suffix \".merge\"!", 'small')
    glRasterPos2i(40, 248)
    Draw.Text("If such a .merge file already exists, it is overwritten.", 'small')
    if not error_s_1 == "":
        glRasterPos2i(30, 137-32)
        Draw.Text("NOTE: "+error_s_1, 'small')
        glRasterPos2i(30, 124-32)
        Draw.Text("            "+error_s_2, 'small')
    if not error_s_3 == "":
        glRasterPos2i(30, 124-32-13)
        Draw.Text("NOTE: "+error_s_3, 'small')
        glRasterPos2i(30, 124-32-13-13)
        Draw.Text("            "+error_s_4, 'small')
    merge_butt    = Draw.Button("Merge with...",    1, 30,150, 80, 30, \
        "Select a map to merge with")
    #export_butt    = Draw.Button("Export",    2, 30,150-32, 80, 30, "map")
    exit_butt    = Draw.Button("Exit",    9,220, 30, 75, 30, "exit")

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

def toSolidFloat(fl):
    fp = struct.pack('>f', fl)
    return fp

def toSolidShort(i):
    ip = struct.pack('>h', i)
    return ip

def toSolidInt(i):
    ip = struct.pack('>i', i)
    return ip

def toSolidByte(b):
    bp = struct.pack('b', b)
    return bp

def parseInt(bytes):
    r=256*256
    return ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])

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


class mapMerge:

    def __init__(self, src, dest):
        global my_path
        self.file = open(src, "rb")
        self.filename = src
        self.destname = dest

    def Merge(self):
        global error_s_1, error_s_2, error_s_3, error_s_4

        error_s_3 = error_s_1
        error_s_4 = error_s_2
        filename = self.filename.split(r"/")
        filename = self.filename.split("\\")
        maptitle = filename[len(filename) - 1]
        scene = bpy.data.scenes.active
        dst = self.destname

        print("-- -- #################### merge "+maptitle)
        try:
            dst = open(self.destname, "wb")
            debugtext = maptitle

            bytecount = 0




            # Character position and other junk
            bytes = self.file.read(65)
            bytecount += len(bytes)
            dst.write(bytes)
            #print str(bytes)
            # Whether or not we have a +4 offset
            bytes = self.file.read(4)
            bytecount += len(bytes)
            dst.write(bytes)
            # offset
            o=0
            if parseInt(bytes) > 0:
                bytes = self.file.read(4)
                bytecount += len(bytes)
                dst.write(bytes)
                #print "O!"
            bytes = self.file.read(11*4)
            bytecount += len(bytes)
            dst.write(bytes)
            # Number of clothing textures
            bytes = self.file.read(4)
            bytecount += len(bytes)
            dst.write(bytes)
            ctex = parseInt(bytes)
            # Irrelevant
            bytes = self.file.read(8)
            bytecount += len(bytes)
            dst.write(bytes)
            # Whether we have dialogue
            bytes = self.file.read(4)
            bytecount += len(bytes)
            dst.write(bytes)
            dialoguesections = parseInt(bytes)
            debugtext += " Number of dialogue sections: "+str(dialoguesections)
            if dialoguesections>0:
                debugtext += "("
            for i in range(0, dialoguesections):
                if i>0:
                    debugtext +=", "
                # number of lines
                bytes = self.file.read(4)
                bytecount += len(bytes)
                dst.write(bytes)
                #print "L "+str(bytecount)
                d = parseInt(bytes)
                debugtext +=str(d)
                # Crap we don't care about
                bytes = self.file.read(184)
                bytecount += len(bytes)
                dst.write(bytes)
                for i in range(0, d):
                    # Number of letters for text
                    bytes = self.file.read(4)
                    bytecount += len(bytes)
                    dst.write(bytes)
                    l = parseInt(bytes)
                    #print str(l)
                    # Actual text
                    bytes = self.file.read(l)
                    bytecount += len(bytes)
                    dst.write(bytes)
                    # Speakername letters
                    bytes = self.file.read(4)
                    bytecount += len(bytes)
                    dst.write(bytes)
                    s =    parseInt(bytes)
                    # Actual speakername with D at end
                    bytes = self.file.read(s+1)
                    bytecount += len(bytes)
                    dst.write(bytes)
                    # Rest of line junk
                    if(i+1<d):
                        bytes = self.file.read(167)
                    else:
                        bytes = self.file.read(147)
                        #print "LAST"
                    bytecount += len(bytes)
                    dst.write(bytes)
                #print str(bytes)+"--"+str(bytecount)

            if dialoguesections>0:
                debugtext += ")"

            for i in range(0, ctex):
                # Clothing texture reference letters count
                bytes = self.file.read(4)
                bytecount += len(bytes)
                dst.write(bytes)
                ct = parseInt(bytes)
                #print str(bytes)+"##"+str(ct)
                bytes = self.file.read(ct)
                bytecount += len(bytes)
                dst.write(bytes)
                # Something irrelevant
                bytes = self.file.read(12)
                bytecount += len(bytes)
                dst.write(bytes)
            # Unknown what this is
            bytes = self.file.read(4)
            bytecount += len(bytes)
            dst.write(bytes)
            # Number of objects
            bytes = self.file.read(4)
            bytecount += len(bytes)
            no = parseInt(bytes)
            debugtext += " Objects: "+str(no)

            # Go through all objects in source file to skip the section
            for i in range(0, no):
                bytes = self.file.read(28)
                bytecount += len(bytes)

            # CUSTOM OBJECT PLACEMENT IS WRITTEN HERE

            rpx = -90.0
            rpy = -180.0
            rpz = 0.0
            facLoc= 4.0
            facSc = 3.33333333
            rd = 180.0 / pi
            rg = pi / 180.0
            to_export = [[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
            total = 0
            for o in scene.objects:
                try:
                    type = o.properties['lugaru_map_elem_type']
                    if type < 14:
                        to_export[type].append(o)
                        total += 1
                except:
                    # Not meant for export => ignore
                    pass
            totalfix = total
            if total>299:
                error_s_1 = "299 objects maximum exceeded."
                if total-299>1:
                    s="s"
                else:
                    s=""
                error_s_2 = str(total-299)+" object"+s+" ignored. Delete that amount."
                totalfix = 299
            print("Writing "+str(totalfix)+" objects.")
            dst.write(toSolidInt(totalfix))
            count=0
            for cat in to_export:
                for o in cat:
                    count+=1
                    if count>299:
                        break
                    type = o.properties['lugaru_map_elem_type']
                    dst.write(toSolidInt(type))
                    rx = rd * o.RotZ
                    ry = rd * o.RotX
                    if type == 6:
                        ry += rx*7.0
                    elif type == 9:
                        ry -= 90.0
                    dst.write(toSolidFloat(caprot(rx+rpx)))
                    dst.write(toSolidFloat(caprot(ry+rpy)))
                    px = o.LocY# * facLoc
                    py = o.LocZ# * facLoc
                    pz = o.LocX# * facLoc
                    dst.write(toSolidFloat(px))
                    dst.write(toSolidFloat(py))
                    dst.write(toSolidFloat(pz))
                    sc = o.SizeX# * facSc
                    dst.write(toSolidFloat(sc))


            # CUSTOM OBJECT PLACEMENT WRITE END

            # Copy the rest of the source file into dst
            rest = "rest"
            while not rest == "":
                rest = self.file.read(2)
                bytecount += len(rest)
                dst.write(rest)

            dst.close()

        except:
            error_s_3 = error_s_1
            error_s_4 = error_s_2

            error_s_1 = "Error while merging."
            print(error_s_1)
            self.file.close()
            dst.close()
            return
            #raise
        print("Merge success.")
        self.file.close()
        #print debugtext
        #print "-- "+str(impobj)
        return


def my_callback_export(filename):
    mapexport = mapExport(filename)
    mapexport.Export()
    Draw.Redraw()

def my_callback_merge(filename):
    global error_s_1, error_s_2, error_s_3, error_s_4
    error_s_1 = ""
    error_s_2 = ""
    error_s_3 = ""
    error_s_4 = ""
    original = filename
    filename = os.path.splitext(filename)
    if filename[1]==".merge":
        #DBG versioning
        #filename = filename[0]
        dest = original
        original = filename[0]
    else:
        #DBG versioning
        #filename = filename[0]+filename[1]
        dest = filename[0]+".merge"
#DBG versioning
#    if os.path.exists(filename+".merge"):
#        raw = os.path.splitext(filename)
#        nodot = 1
#        if not raw[1]=="":
#            # this filename is a versioned one
#            try:
#                nodot = int(raw[1][1:])
#                noversions = raw[0]
#            except:
#                noversions = raw[0]+raw[1]
#            #print str(nodot)
#        else:
#            noversions = raw[0]
#        while os.path.exists(noversions+"."+str(nodot)+".merge"):
#            nodot+=1
#        dest = noversions+"."+str(nodot)+".merge"
#        #if os.path.exists(noversions):
#        #    filename=noversions
#    else:
#        dest = filename+".merge"
    if os.path.exists(original):
        print("  -  "+original+" -> "+dest)
        mapmerge = mapMerge(original, dest)
        mapmerge.Merge()
    else:
        error_s_1 = "No such file exists: \"..."+filename[:20]+"\""
        error_s_2 = "Choose an existing mapfile to merge with."
    Draw.Redraw()

arg = __script__['arg']
