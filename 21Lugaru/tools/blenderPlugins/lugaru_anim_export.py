#!BPY

""" Registration for Blender menus
Name: 'Lugaru animations...'
Blender: 249b
Group: 'Export'
Tooltip: 'Export animations.'
"""
# lugaru_anim_export.py version 1.0
# Written in 2010 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever,
# as long as you abide to the restrictions mentioned above


# This script exports Lugaru animation definition data
# The Lugaru animation format (c) David Rosen is a binary 3D animation format
# It holds
#         stuff

# This exporter code is based upon reverse-engineering the format that's used in Lugaru
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct
import os
from struct import *
from Blender import Mesh, Object, Material, Texture, Image, Draw, BGL, Ipo, BezTriple, NMesh, Key
from Blender.BGL import *

lugaru_anim_frames = 0
lugaru_anim_sounds = []
lugaru_anim_speeds = []
lugaru_anim_hasweapon = 0  # has no fallback - just a convenience variable
lugaru_anim_wprefix = 0
lugaru_anim_orientationmesh = ""

page = 0

objectnamelist = []

o = "" # The selected object

def event(evt, val):
    if evt == Draw.ESCKEY:
        Draw.Exit()
        return

def button_event(evt):
    global page, lugaru_anim_frames
    arg = __script__['arg']
    if evt == 1:
        Blender.Window.FileSelector(my_callback, "Export Animation", "animation")
        Draw.Redraw(1)
        #Draw.Exit()

    if evt == 2:
        Draw.Redraw(1)

    if evt == 6:
        if page>0:
            page=page-1
        else:
            page=lugaru_anim_frames/15
        Draw.Redraw(1)
    if evt == 7:
        if page<lugaru_anim_frames/15:
            page=page+1
        else:
            page=0
        Draw.Redraw(1)


    if evt == 9:
        Draw.Exit()
        return

def toggler(evt, val):
    global o
    #100*m+n+100 => 100 - 1099 --> EVT OFFSET, COLUMN, ROW

    evt = evt-100  #evt off => 0-999
    if evt<100:
        m=0
    else:
        m=int(str(evt)[0])  #col is centennial [SOUND]
    evt = evt-m*100
    n=evt  #row is rest [FRAME]
    if lugaru_anim_sounds[n]==m:
        o.properties['lugaru_anim_sounds'][n] = 0
    else:
        o.properties['lugaru_anim_sounds'][n] = m
    Draw.Redraw()

    return

def setspeed(evt, val):
    #n+1100 => 1100 - 1199 --> EVT OFFSET, ROW
    evt = evt-1100  #evt off => 0-99
    o.properties['lugaru_anim_speeds'][evt] = val
    Draw.Redraw()
    return

def defaultsounds():
    global lugaru_anim_sounds, lugaru_anim_frames
    lugaru_anim_sounds = []
    for i in range(0, lugaru_anim_frames):
        lugaru_anim_sounds.append(0)

def sounds():
    global o, lugaru_anim_sounds, lugaru_anim_frames
    lugaru_anim_sounds = []
    try:
        for i in range(0, len(o.properties['lugaru_anim_sounds'])):
            lugaru_anim_sounds.append(o.properties['lugaru_anim_sounds'][i])
            if lugaru_anim_sounds[i]>9 or lugaru_anim_sounds[i]<0:
                lugaru_anim_sounds[i]=0 # Fix for broken shit like Landing
        while len(lugaru_anim_sounds)<lugaru_anim_frames:
            lugaru_anim_sounds.append(0)
        o.properties['lugaru_anim_sounds'] = lugaru_anim_sounds
    except:
        defaultsounds()
        o.properties['lugaru_anim_sounds'] = lugaru_anim_sounds
    return lugaru_anim_sounds

def defaultspeeds():
    global lugaru_anim_speeds
    lugaru_anim_speeds = []
    for i in range(0, lugaru_anim_frames):
        lugaru_anim_speeds.append(10.0)

def speeds():
    global o, lugaru_anim_speeds, lugaru_anim_frames
    lugaru_anim_speeds = []
    try:
        for i in range(0, len(o.properties['lugaru_anim_speeds'])):
            lugaru_anim_speeds.append(o.properties['lugaru_anim_speeds'][i])
        while len(lugaru_anim_speeds)<lugaru_anim_frames:
            lugaru_anim_speeds.append(10.0)
        o.properties['lugaru_anim_speeds'] = lugaru_anim_speeds
    except:
        defaultspeeds()
        o.properties['lugaru_anim_speeds'] = lugaru_anim_speeds
    return lugaru_anim_speeds

def wprefix():
    global o, lugaru_anim_wprefix
    try:
        lugaru_anim_wprefix = o.properties['lugaru_anim_wprefix']
    except:
        lugaru_anim_wprefix = "NONE"
        o.properties['lugaru_anim_wprefix'] = lugaru_anim_wprefix
    return lugaru_anim_wprefix

def orientationmesh():
    global o, lugaru_anim_orientationmesh
    try:
        lugaru_anim_orientationmesh = o.properties['lugaru_anim_orientationmesh']
    except:
        lugaru_anim_orientationmesh = "NONE"
        o.properties['lugaru_anim_orientationmesh'] = lugaru_anim_orientationmesh
    return lugaru_anim_orientationmesh

def draw():
    global o, lugaru_anim_frames, lugaru_anim_sounds, lugaru_anim_speeds,\
            lugaru_anim_hasweapon, lugaru_anim_wprefix,\
            lugaru_anim_orientationmesh, objectnamelist, page

    glClearColor(0.55,0.6,0.6,1)
    glClear(BGL.GL_COLOR_BUFFER_BIT)
    #external box
    glColor3f(0.2,0.3,0.3)
    ystart = 502
    rect(10,502,304,482)
    rect(12,500,300,180)

    glColor3f(0.8,.8,0.6)
    glRasterPos2i(20, ystart - 22)
    Draw.Text("Lugaru Animation Exporter 1.0 ",'large')
    glRasterPos2i(210, ystart - 32)
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, ystart - 47)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, ystart - 67)
    Draw.Text("Export Lugaru animations from Blender", 'small')
    glRasterPos2i(40, ystart - 87)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, ystart -100)
    Draw.Text("The Lugaru animation format (c) David Rosen", 'small')
    glRasterPos2i(40, ystart -116)
    Draw.Text("ONLY select the animated skeleton to export.", 'small')
    glRasterPos2i(40, ystart -129)
    Draw.Text("The selected file will be REPLACED.", 'small')
    glRasterPos2i(40, ystart -144)
    Draw.Text("If the animation includes weaponry, the animated", 'small')
    glRasterPos2i(40, ystart -157)
    Draw.Text("orientation mesh for it needs to be included .", 'small')
    glRasterPos2i(40, ystart -171)
    Draw.Text("Set appropriate properties beforehand.", 'small')
    labelstart = ystart -200
    xstart = 144
    ystart = labelstart-12

    genericboxheader = ystart-50
    genericbox = ystart-62

    refresh_butt= Draw.Button("Refresh",  2,12+151, ystart-32,  149, 30, "refresh")
    exit_butt   = Draw.Button("Exit",  9,12,22, 65, 234, "exit")


    try:
        o = Object.GetSelected()[0]
        k = Object.GetSelected()[0].getData(0,1).key
        # exception assignment
        lugaru_anim_frames         = len(k.blocks)
    except:
        glRasterPos2i(xstart-100, labelstart)
        Draw.Text("ERROR: Select an animation, click Refresh!")
        return

    # Here, fails result in defaults
    lugaru_anim_sounds         = sounds()        # DEFAULT [0,0,0,0...]
    lugaru_anim_speeds     = speeds()    # DEFAULT [2.0,2.0,2.0...]
    lugaru_anim_wprefix        = wprefix()        # DEFAULT "NONE"
    lugaru_anim_hasweapon    = o.properties['lugaru_anim_wprefix']!= "NONE"
    lugaru_anim_orientationmesh= orientationmesh()    # DEFAULT "NONE"

    glRasterPos2i(xstart+14, genericboxheader)
    Draw.Text("S O U N D      FRAME", 'normalfix')
    anim_butt   = Draw.Button("Export",  1,12, ystart-32,  149, 30, "animation")

    nb = len(k.blocks)
    p = page*15

    for m in range(0, 10):
        glRasterPos2i(xstart+m*10+2, genericbox)
        if m<9:
            Draw.Text(str(m), 'normalfix')
        else:
            Draw.Text(str(m)+"   SPEED", 'normalfix')

        for n in range(0, 15):
            if n+p>=nb:
                break
            if m==0:
                if n==0:
                    glRasterPos2i(xstart-57, genericbox-13)
                    #a
                    Draw.Text("FRAME "+str(n+1+p), 'normalfix')

                else:
                    glRasterPos2i(xstart-15, genericbox-13-n*12)
                    #a
                    Draw.Text(str(n+1+p), 'normalfix')

                #b
                #Draw.Text(

                #100*m+n+1100 => 1100 - 2099 --> EVT OFFSET, COLUMN, ROW
                Draw.Slider("", 100*m+n+p+1100, xstart+102, genericbox-15-n*12, 60, 11, \
                    lugaru_anim_speeds[n+p],0.1, 30.0, 0, \
                    "Frame speed", setspeed)

            #100*m+n+100 => 100 - 1099 --> EVT OFFSET, COLUMN, ROW
            Draw.Toggle("", \
                100*m+n+p+100, xstart+m*10, genericbox-15-n*12,\
                11,11, lugaru_anim_sounds[n+p]==m, "This sound at this frame", toggler)


    rect(12+65+6,22+230,226,15*15-10)

    if nb>15:
        Draw.Button("<", 6, 12+65+8, 24, 60, 10, "Previous Page")
        Draw.Button(">", 7, 226+12+65+6-60-2, 24, 60, 10, "Next Page")
        glRasterPos2i(12+65+8+60+32, 26)
        Draw.Text("Page "+str(page+1), 'normal')
    Draw.Toggle("Affects Weaponry", \
        3, 12, ystart,110,25, lugaru_anim_hasweapon, "Toggle", toggleweapon)
    if lugaru_anim_hasweapon:
        Draw.Toggle("Weapon Prefix: "+str(lugaru_anim_wprefix), \
            4, 12+110+2, ystart,110,25, lugaru_anim_wprefix, "Toggle", toggleprefix)
        orichoice = "Orientation Mesh %t|"
        counter = 0
        themesh = 1
        objectnamelist = []
        for i in Blender.Scene.GetCurrent().objects:
            counter = counter + 1
            objectnamelist.append(i.name)
            if i.name == o.name:
                continue
            orichoice = orichoice+i.name+"%x"+str(counter)+"|"
            if i.name == lugaru_anim_orientationmesh:
                themesh = counter

        Draw.Menu(orichoice, 5, 12+110*2+4, ystart, 76, 25, themesh, "Orientation Mesh for the Weapon", setOri)

def toggleweapon(evt, val):
    global o
    if val:
        o.properties['lugaru_anim_wprefix'] = 0
    else:
        o.properties['lugaru_anim_wprefix'] = "NONE"
    Draw.Redraw()

def toggleprefix(evt, val):
    global o
    o.properties['lugaru_anim_wprefix'] = val
    Draw.Redraw()

def setOri(evt, val):
    global o, objectnamelist
    o.properties['lugaru_anim_orientationmesh'] = objectnamelist[val-1]

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

class animationExport:
    def toSolidFloat(self, fl):
        fp = struct.pack('>f', fl)
        return fp

    def toSolidInt(self, i):
        ip = struct.pack('>i', i)
        return ip

    def __init__(self, filename):
        global my_path
        self.filename = filename
        #my_path = Blender.sys.dirname(filename)

    def Export(self):
        global o, lugaru_anim_frames, lugaru_anim_sounds, lugaru_anim_speeds,\
                lugaru_anim_hasweapon, lugaru_anim_wprefix,\
                lugaru_anim_orientationmesh
        file = open(self.filename, "wb")

        #print "Exporting Lugaru Animation from Blender ..."

        mesh = o.getData(0, 1)

        file.write(self.toSolidInt(lugaru_anim_frames))
        file.write(self.toSolidInt(len(mesh.verts)))

        k = mesh.key

        c1 = 0

        for kb in k.blocks:
            for v in kb.data:
                file.write(self.toSolidFloat(v.x*4.0))
                file.write(self.toSolidFloat(v.y*-4.0))
                file.write(self.toSolidFloat(v.z*-4.0))
            for i in range(0,25):
                file.write(self.toSolidInt(0))
            file.write(self.toSolidFloat(lugaru_anim_speeds[c1]))
            c1 = c1+1

        for i in range(0, lugaru_anim_frames*len(mesh.verts)):
            file.write(self.toSolidInt(0))
        for i in lugaru_anim_sounds:
            file.write(self.toSolidInt(i))
        if lugaru_anim_hasweapon:
            file.write(self.toSolidInt(lugaru_anim_wprefix))

            w = Object.Get(lugaru_anim_orientationmesh)
            wm= w.getData(0,1)
            wk= wm.key

            c1=0
            for kb in wk.blocks:
                for v in kb.data:
                    file.write(self.toSolidFloat(v.x*4.0))
                    file.write(self.toSolidFloat(v.y*-4.0))
                    file.write(self.toSolidFloat(v.z*-4.0))

        #print "... finished!"
        file.close()
        return


def my_callback(filename):
    # __ DEBUG Junk for exporting all anims at once
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
    animexport = animationExport(filename)
    #try:
    animexport.Export()
    #except:
    #    print "Error upon export! Aborted."
    Draw.Redraw()

arg = __script__['arg']
