#!BPY

""" Registration for Blender menus
Name: 'Lugaru skeleton merge...'
Blender: 249b
Group: 'Export'
Tooltip: 'Merges Body SOLIDs with skeletons.'
"""
# lugaru_skel_merge.py version 1.1
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever, as long as you abide to the restrictions mentioned above


# This bakes Lugaru Body SOLID vertices via their vertex groups into Lugaru skeletons
# The SOLID format (c) David Rosen is a binary 3D mesh format
# The Lugaru skeletons (c) David Rosen hold
#         XYZ vertices (float float float) for joint positional data
#         Ragdoll weights per vertex pair
#         Associated vertex indices of Body SOLIDs

# This merger code is based upon reverse-engineering the format that's used in Lugaru
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
        print("merge rig")
        Blender.Window.FileSelector(rig_callback, "Merge with Skeleton", "SKELETON")
        #Draw.Exit()
        Draw.Redraw()
        return
    if evt == 2:
        print("merge jointlocs")
        Blender.Window.FileSelector(loc_callback, "Merge with Skeleton", "SKELETON")
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
    Draw.Text("Skeleton Merger 1.1 ",'large')
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 365)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 340)
    Draw.Text("Merge the selected Lugaru mesh with a skeleton", 'small')
    glRasterPos2i(40, 315)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 303)
    Draw.Text("All Lugaru formats (c) David Rosen", 'small')
    glRasterPos2i(40, 280)
    Draw.Text("Make sure you've set all vertex groups correctly!", 'small')
    glRasterPos2i(40, 268)
    Draw.Text("If you forgot vertices, it'll be apparent in-game.", 'small')
    glRasterPos2i(40, 248)
    Draw.Text("Note that you need to select a Body mesh beforehand", 'small')
    glRasterPos2i(40, 236)
    Draw.Text("for rig merge and a Skeleton mesh for joint location merge.", 'small')
    glRasterPos2i(40, 224)
    Draw.Text("The new file will be called FILENAME_", 'small')
    glRasterPos2i(40, 212)
    Draw.Text("Refer to the forums for further instructions.", 'small')
    glRasterPos2i(40, 157)
    Draw.Text("NOTE: This program OVERWRITES files.", 'small')
    rig_butt   = Draw.Button("Rig merge",  1,80, 111,  120, 30, "Merge a rig setup into skeleton")
    rig_butt   = Draw.Button("Joint location merge",  2,80, 79,  120, 30, \
        "Merge a skeleton's joint locations into skeleton")
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

class skelMergeRig:

    def __init__(self, filename):
        global my_path
        self.filename = filename
        #my_path = Blender.sys.dirname(filename)

    def mergeimpl(self, f, obj):
        debug = 1
        try:
            self.reference = open(f, "rb")
            self.result = open(f+"_", "wb")

            mesh = obj.getData(0, 1)
            obj.link(mesh) # link

            vgnames = mesh.getVertGroupNames()
            #print "vgnames "+str(vgnames)
            indices = []
            for u in range(0, len(vgnames)):
                indices.append(-1)

            # Fix the order of joints
            for u in range(0, len(vgnames)):
                indexint = int(vgnames[u][1:])
                indices[indexint]=mesh.getVertsFromGroup(vgnames[u])

            print("Copying Lugaru Skeleton header into result file...")
            bytes = self.reference.read(4)
            noofjoints = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
            print("  #joints: "+str(noofjoints))
            self.result.write(bytes)

            #bytes = self.reference.read(noofjoints*41)
            #self.result.write(bytes)

            for i in range(0, noofjoints):
                #print str(i)
                loc=self.reference.read(12) # old location is preserved
                pbytes = self.reference.read(30-12)
                jbyte = self.reference.read(1) # joint index
                ji = ord(jbyte[0])
                sbytes = self.reference.read(41-31) # the remaining bytes are copied
                #self.result.write(toSolidFloat(mesh.verts[ji].co.x*4.0))
                #self.result.write(toSolidFloat(mesh.verts[ji].co.y*-4.0))
                #self.result.write(toSolidFloat(mesh.verts[ji].co.z*-4.0))
                #self.result.write(toSolidFloat(mesh.verts[i].co.x*4.0))
                #self.result.write(toSolidFloat(mesh.verts[i].co.y*4.0))
                #self.result.write(toSolidFloat(mesh.verts[i].co.z*4.0))
                self.result.write(loc)
                self.result.write(pbytes)
                self.result.write(jbyte)
                self.result.write(sbytes)

            bytes = self.reference.read(4)
            # joint-vertex relation
            jvrel = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
            self.result.write(bytes)
            print("  #relations: "+str(jvrel))

            print("Header done.")

            validpaircounter = 0
            for k in range(0, jvrel):
                bytes = self.reference.read(24) # floats for this relation
                self.result.write(bytes)
                bytes = self.reference.read(4)
                print("b "+str(ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])))
                noindices = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
                if(noindices>0):
                    #print "noindices "+str(noindices)
                    noofnewindices = len(indices[validpaircounter])
                    self.result.write(toSolidInt(noofnewindices))
                    for v in range(0, noofnewindices):
                        self.result.write(toSolidInt(indices[validpaircounter][v]))
                    crap = self.reference.read(4*noindices) # discard

                    if(noofnewindices>0):
                        self.result.write(toSolidByte(1))
                    else:
                        self.result.write(toSolidByte(0))
                    validpaircounter+=1
                else:
                    self.result.write(bytes)
                    self.result.write(toSolidByte(0))
                crap = self.reference.read(1) # discard
                bytes = self.reference.read(8) # skel indices for rel, copy
                self.result.write(bytes)

            while(1):
                bytes = self.reference.read(4) # appendix skel indices, copy
                if(bytes == ""):
                    break
                self.result.write(bytes)

            print("... done!")

        except:
            self.result.close()
            self.reference.close()
            raise
        self.result.close()
        self.reference.close()

    def Merge(self):

        scene = bpy.data.scenes.active
        obj = Object.GetSelected()
        if(len(obj)<1):
            raise SolidUserError("Select exactly one Object to merge with a skeleton.")
        else:
            if(len(obj)>1):
                print("Picked the first selected Object automatically.")

        obj = obj[0]

        print("Baking indices into skeleton...")
        if(obj.type == 'Mesh'):
            self.mergeimpl(self.filename,obj)
        else:
            raise SolidUserError("Select an Object that is a Mesh!")

        return


class skelMergeLoc:

    def __init__(self, filename):
        global my_path
        self.filename = filename
        #my_path = Blender.sys.dirname(filename)

    def mergeimpl(self, f, obj):
        debug = 1
        try:
            self.reference = open(f, "rb")
            self.result = open(f+"_", "wb")

            mesh = obj.getData(0, 1)
            obj.link(mesh) # link

            vgnames = mesh.getVertGroupNames()
            #print "vgnames "+str(vgnames)
            indices = []
            for u in range(0, len(vgnames)):
                indices.append(-1)

            # Fix the order of joints
            for u in range(0, len(vgnames)):
                indexint = int(vgnames[u][1:])
                indices[indexint]=mesh.getVertsFromGroup(vgnames[u])


            print("indices "+str(indices))

            print("Merging Lugaru Skeleton header into result file...")
            bytes = self.reference.read(4)
            noofjoints = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
            print("  #joints: "+str(noofjoints))
            self.result.write(bytes)

            #bytes = self.reference.read(noofjoints*41)
            #self.result.write(bytes)

            for i in range(0, noofjoints):
                #print str(i)
                self.reference.read(12) # old location is discarded
                pbytes = self.reference.read(30-12)
                jbyte = self.reference.read(1) # joint index
                ji = ord(jbyte[0])
                sbytes = self.reference.read(41-31) # the remaining bytes are copied
                #self.result.write(toSolidFloat(mesh.verts[ji].co.x*4.0))
                #self.result.write(toSolidFloat(mesh.verts[ji].co.y*-4.0))
                #self.result.write(toSolidFloat(mesh.verts[ji].co.z*-4.0))
                self.result.write(toSolidFloat(mesh.verts[i].co.x*4.0))
                self.result.write(toSolidFloat(mesh.verts[i].co.y*-4.0))
                self.result.write(toSolidFloat(mesh.verts[i].co.z*-4.0))
                self.result.write(pbytes)
                self.result.write(jbyte)
                self.result.write(sbytes)

            # Copy the rest
            while(1):
                bytes = self.reference.read(4)
                if(bytes == ""):
                    break
                self.result.write(bytes)

            print("... done!")

        except:
            self.result.close()
            self.reference.close()
            raise
        self.result.close()
        self.reference.close()

    def Merge(self):

        scene = bpy.data.scenes.active
        obj = Object.GetSelected()
        if(len(obj)<1):
            raise SolidUserError("Select exactly one Object to merge with a skeleton.")
        else:
            if(len(obj)>1):
                print("Picked the first selected Object automatically.")

        obj = obj[0]

        print("Baking indices into skeleton...")
        if(obj.type == 'Mesh'):
            self.mergeimpl(self.filename,obj)
        else:
            raise SolidUserError("Select an Object that is a Mesh!")

        return

def rig_callback(filename):
    skelmerge = skelMergeRig(filename)
    try:
        skelmerge.Merge()
    except:
        raise

def loc_callback(filename):
    skelmerge = skelMergeLoc(filename)
    try:
        skelmerge.Merge()
    except:
        raise

arg = __script__['arg']
