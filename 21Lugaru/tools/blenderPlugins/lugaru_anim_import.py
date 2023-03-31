#!BPY

""" Registration for Blender menus
Name: 'Lugaru animations...'
Blender: 249b
Group: 'Import'
Tooltip: 'Import animations.'
"""
# lugaru_anim_import.py version 1.0
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever,
# as long as you abide to the restrictions mentioned above


# This script imports Lugaru animation definition data
# The Lugaru animation format (c) David Rosen is a binary 3D animation format
# It holds
#         stuff

# This importer code is based upon reverse-engineering the format that's used in Lugaru
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct
import os

from struct import *
from Blender import Mesh, Object, Material, Texture, Image, Draw, BGL, Ipo, BezTriple
from Blender.BGL import *

def event(evt, val):
    if evt == Draw.ESCKEY:
        Draw.Exit()
        return

def button_event(evt):
    arg = __script__['arg']
    if evt == 1:
        Blender.Window.FileSelector(my_callback, "Import Animation", "animation")
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
    Draw.Text("Lugaru Animation Importer 1.0 ",'large')
    glRasterPos2i(210, 370)
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 355)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 330)
    Draw.Text("Import Lugaru animations into Blender", 'small')
    glRasterPos2i(40, 305)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 293)
    Draw.Text("The Lugaru animation format (c) David Rosen", 'small')
    glRasterPos2i(40, 270)
    Draw.Text("Select a skeleton beforehand to match the animation.", 'small')
    glRasterPos2i(40, 257)
    Draw.Text("The skeleton will be REPLACED by the animation.", 'small')
    glRasterPos2i(40, 242)
    Draw.Text("If the animation includes weaponry, the animated", 'small')
    glRasterPos2i(40, 229)
    Draw.Text("orientation mesh for it will be added as well.", 'small')
    body_butt   = Draw.Button("Animation",  1,30, 150,  75, 30, "animation")
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

class animationImport:
    def parseSolidFloat(self, fourchars):
        f = struct.unpack('>f', str(fourchars))
        return f

    def __init__(self, filename):
        global my_path
        self.file = open(filename, "rb")
        self.filename = filename
        #my_path = Blender.sys.dirname(filename)

    def Import(self):

        filename = self.filename.split(r"/")
        animtitle = filename[len(filename) - 1]
        scene = bpy.data.scenes.active

        #print animtitle

        scale = 250
        factor = 0.001*scale
        bytecount = 0
        #print "Importing Lugaru Animation into Blender ..."

        # We need this a lot
        r=256*256

        objs = Object.GetSelected()
        sel = 0
        if(len(objs)>0):
            sel=1
            obj=objs[0]
            # This mesh is derived from the skeleton and gets shape keys as animation data dictates
            mesh = obj.getData(0, 1)
            obj.link(mesh) # link

            skeledges = []
            for e in mesh.edges:
                sedge = []
                sedge.append(e.v1.index)
                sedge.append(e.v2.index)
                skeledges.append(sedge)

        else:
            print("Select a skeleton!")
            return

        bytes = self.file.read(4)
        bytecount += len(bytes)
        noofframes = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
        #print "  #frames: "+str(noofframes)
        bytes = self.file.read(4)
        noofjoints = ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])
        #print "  #joints: "+str(noofjoints)
        indices = []
        chunksize = noofjoints*3*4+26*4# byterange of a frame
        bytes = self.file.read(noofframes *chunksize)# all frame data
        bytecount += len(bytes)

        obj.name = animtitle

        timemarks = []

        # Get old timemarks if present
        try:
            for t in obj.properties['lugaru_anim_speeds']:
                timemarks.append(t)
        except:
            print("No previous timemarks found.")


        lastaddr = 0

        firstnew = 0
        try:
            oldblocks = mesh.key.blocks
            firstnew = len(oldblocks)

            # Try to fill the unset timemarks (if there are any)
            try:
                for k in range(len(obj.properties['lugaru_anim_speeds']), firstnew):
                    timemarks.append(10.0)
            except:
                print("Could not fill rest of timemarks.")
        except:
            print("Has no previous blocks")


        for i in range(0, noofframes):

            f = []
            for c in range(0, chunksize):
                f.append(bytes[c+(chunksize*i)])

            coords = []
            # a joint position has 3 float components - xyz
            for j in range(0, noofjoints):
                # a float is 4 bytes.
                coords.append((self.parseSolidFloat(\
                    f[12*j]+f[12*j+1]+f[12*j+2]+f[12*j+3])[0]*factor))#x
                coords.append((self.parseSolidFloat(\
                    f[12*j+4]+f[12*j+5]+f[12*j+6]+f[12*j+7])[0]*factor*-1.0))#y
                coords.append((self.parseSolidFloat(\
                    f[12*j+8]+f[12*j+9]+f[12*j+10]+f[12*j+11])[0]*factor*-1.0))#z

            # go to frame, set vertices, insert shape key
            bpy.data.scenes.active.getRenderingContext().cFrame=(i+firstnew+1)*20
            for k in range(0, noofjoints):
                # assemble verts from coords
                mesh.verts[k]=Blender.Mesh.MVert(coords[3*k],coords[3*k+1],coords[3*k+2])

            obj.insertShapeKey()

            # Name "LKey "+decadestring+str(i)

            # get final 4 bytes of current frame
            timemarks.append(self.parseSolidFloat(\
                f[len(f)-4]+f[len(f)-3]+f[len(f)-2]+f[len(f)-1])[0])
            lastaddr = len(f)

        # TODO Set globally later, set an object property
        cyclic = 1

        if cyclic:

            # Repeat first shape key for cyclic animation
            # (See NOTEs below)
            f=[]
            for c in range(0,chunksize):f.append(bytes[c])
            coords=[]
            for j in range(0, noofjoints):
                coords.append((self.parseSolidFloat(\
                    f[12*j]+f[12*j+1]+f[12*j+2]+f[12*j+3])[0]*factor))#x
                coords.append((self.parseSolidFloat(\
                    f[12*j+4]+f[12*j+5]+f[12*j+6]+f[12*j+7])[0]*factor*-1.0))#y
                coords.append((self.parseSolidFloat(\
                    f[12*j+8]+f[12*j+9]+f[12*j+10]+f[12*j+11])[0]*factor*-1.0))#z
            vertlist = []
            bpy.data.scenes.active.getRenderingContext().cFrame=(firstnew+noofframes+1)*20
            for k in range(0, noofjoints):
                mesh.verts[k]=Blender.Mesh.MVert(coords[3*k],coords[3*k+1],coords[3*k+2])
            obj.insertShapeKey()
            # NOTE
            #         This repeated key is _just_ for cyclic stuff
            # NOTE
            #         TODO Give exporter knowledge of this key via object property


        if cyclic:
            keyedframes = noofframes
        else:
            keyedframes = noofframes-1

        # IPO is needed to read out RVK upon export!
        try:
            ipo = mesh.key.ipo
            ipo.name = animtitle[:8]+"_IPO"
        except:
            print("No IPO, adding it")
            ipo = Ipo.New("Key", animtitle[:8]+"_IPO")
            mesh.key.ipo = ipo
        #ipo = Ipo.New("Key", animtitle+"_IPO")
        #mesh.key.ipo = ipo

        keyblocks = mesh.key.blocks
        decadestring= ""

        block_i = -1
        dbg = "DBG: "

        print(dir(ipo))
        if firstnew == 0:
            curve = ipo.addCurve(keyblocks[0].name)
            print("New IPO generated")
        else:
            curve = ipo.getCurve(keyblocks[0].name)
            print(curve)
            if not str(curve) == "None":
                print("Existing IPO used, adding frames")
            else:
                curve = ipo.addCurve(keyblocks[0].name)
                print("Existing IPO was NoneType, built new IPO")
        if not len(curve.bezierPoints)>0:
            curve.append(BezTriple.New(1,0,0))

        for block in keyblocks:
            block_i +=1
            if block_i < firstnew:

                print("Skipping block "+str(block_i))
                continue

            while len(str(len(keyblocks)))  - len(str(block_i)) - len(decadestring) > 0:
                decadestring += "0"
                dbg += ". "
            while len(decadestring) > len(str(len(keyblocks))) - len(str(block_i)):
                decadestring = decadestring[:len(decadestring)-1]
                dbg += " # "
            block.name = decadestring+str(block_i)
            print("blockname "+block.name)

            #counting = 0

            for i in range(0, block_i):
            #counting = counting + timemarks[i]
            #curve.append(BezTriple.New(counting*0.2, (i+1)*0.1, 0))
                curve.append(BezTriple.New((i+1)*2+1, (i+1)*0.1, 0))
        print(dbg)
        mesh.name = animtitle+"_MESH"
        obj.properties['lugaru_anim_speeds'] = timemarks


        #__ Weapon orientation junk
        #   Also sound ints and something else (appendixh, wprefix); these end up as object props!

        # discard 00 00 00 00*noofframes*noofjoints garbage
        self.file.read(noofframes * noofjoints * 4)
        bytes = 0
        bytecount = 0

        debugstring = animtitle
        # appendix header; always present (always ints)
        # Sound references as ints
        # Dynamic, can't say what's played at which int, really
        appendixh = []
        for i in range(0, noofframes):
            bytes = self.file.read(4)
            appendixh.append(ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3]))
        obj.properties['lugaru_anim_sounds'] = appendixh
        debugstring = debugstring+": "+str(appendixh)

        # appendix header extra; only present if weapon
        # Not clear what it does
        bytes = self.file.read(4)
        if bytes!="":
            obj.properties['lugaru_anim_wprefix'] = \
                ord(bytes[0])*r*256+ord(bytes[1])*r+ord(bytes[2])*256+ord(bytes[3])

            bpy.data.scenes.active.getRenderingContext().cFrame=1

            # weapon coords
            wcoords = []
            wmesh = Mesh.New()
            wmesh.name = animtitle+"_wmesh"
            wmesh.verts.extend(0.0,0.0,0.0)
            wobj = scene.objects.new(wmesh, animtitle+"_w")
            wf = []
            for j in range(0, noofframes):
                bytes = self.file.read(12)
                #if bytes == "":
                #    break
                #bytecount += len(bytes)
                # Weapon coord triplet xyz
                wcoords.append((\
                    self.parseSolidFloat(bytes[0]+bytes[1]+bytes[2]+bytes[3])[0]*factor,\
                    self.parseSolidFloat(bytes[4]+bytes[5]+bytes[6]+bytes[7])[0]*factor*-1.0,\
                    self.parseSolidFloat(bytes[8]+bytes[9]+bytes[10]+bytes[11])[0]*factor*-1.0))#xyz
                # for cyclic stuff (below)
                if j==0:
                    wf.append(wcoords[len(wcoords)-1])

                # go to frame, set vertices, insert shape key
                bpy.data.scenes.active.getRenderingContext().cFrame=(j+1)*20
                # assemble verts from coords
                wmesh.verts[0]=Blender.Mesh.MVert(wcoords[j][0],wcoords[j][1],wcoords[j][2])
                wobj.insertShapeKey()

            if cyclic:
                # Repeat first shape key for cyclic animation
                # (See NOTEs below)
                bpy.data.scenes.active.getRenderingContext().cFrame=(noofframes+1)*20
                wmesh.verts[0]=Blender.Mesh.MVert(wf[0][0],wf[0][1],wf[0][2])
                wobj.insertShapeKey()
                # NOTE
                #         This repeated key is _just_ for cyclic stuff
                # NOTE
                #         TODO Give exporter knowledge of this key via object property

            wobj.LocX = obj.LocX
            wobj.LocY = obj.LocY
            wobj.LocZ = obj.LocZ

            # IPO
            wipo = Ipo.New("Key", animtitle+"_w_IPO")
            wmesh.key.ipo = wipo
            wkeyblocks = wmesh.key.blocks
            for block in wkeyblocks:
                wcurve = wipo.addCurve(block.name)
                wcurve.append(BezTriple.New(1,0,0))
                #counting = 0
                for i in range(0, keyedframes):
                    #counting = counting + timemarks[i]
                    #curve.append(BezTriple.New(counting*0.2, (i+1)*0.1, 0))
                    wcurve.append(BezTriple.New((i+1)*2+1, (i+1)*0.1, 0))



            debugstring = debugstring+" -- "+str(obj.properties['lugaru_anim_wprefix'])
            #print "wcoord data     "+str(wcoords)

            obj.properties['lugaru_anim_orientationmesh'] = animtitle+"_w"
        else:
            obj.properties['lugaru_anim_orientationmesh'] = "NONE"

        #__ Weapon mesh movement junk done

        bpy.data.scenes.active.getRenderingContext().cFrame=1

        print(debugstring)

        del f
        del coords
        if cyclic:
            del vertlist
        del bytes
        #print "... finished!"

        Blender.Redraw()

        return


def my_callback(filename):
    # __ DEBUG Junk for importing all anims at once
    #splitted = filename.split(r"/")
    #dir="/"
    #for i in range(0, len(splitted)-1):
    #    dir=dir+splitted[i]+"/"
    #
    #files = os.listdir(dir)
    #
    #for f in files:
    #filename=dir+f
    # __ DEBUG End
    animimport = animationImport(filename)
    animimport.Import()
    Draw.Redraw()

arg = __script__['arg']
