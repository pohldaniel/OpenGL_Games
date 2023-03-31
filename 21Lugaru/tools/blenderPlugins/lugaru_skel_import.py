#!BPY

""" Registration for Blender menus
Name: 'Lugaru skeletons...'
Blender: 249b
Group: 'Import'
Tooltip: 'Import skeletons.'
"""
# lugaru_skel_import.py version 1.0
# Written in 2009 by Wolf MATHWIG -- wolf.mathwig@web.de
#
# You cannot claim this software as property
# You cannot claim copyright of this software
# You can use this software without any restrictions whatsoever, as long as you abide to the restrictions mentioned above


# This script imports Lugaru skeleton definition data
# The Lugaru skeleton format (c) David Rosen is a binary 3D skeleton format
# It holds
#         stuff

# This importer code is based upon reverse-engineering the format that's used in Lugaru
# Lugaru (c) Wolfire Games

import bpy
import Blender
import struct

from struct import *
from Blender import NMesh, Mesh, Object, Material, Texture, Image, Draw, BGL
from Blender.BGL import *

def event(evt, val):
    if evt == Draw.ESCKEY:
        Draw.Exit()
        return

def button_event(evt):
	global isbody,isweapon,isimmo
	arg = __script__['arg']
	if evt == 1:
		print("skeleton")
		Blender.Window.FileSelector(my_callback, "Import Skeleton", "skeleton")
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
    Draw.Text("Lugaru Skeleton Importer 1.0 ",'large')
    glRasterPos2i(210, 370)
    Draw.Text("(for Blender 2.49b)", 'small')
    glRasterPos2i(80, 355)
    Draw.Text("written by Wolf Mathwig, wolf.mathwig@web.de", 'small')
    glRasterPos2i(40, 330)
    Draw.Text("Import Lugaru skeletons into Blender", 'small')
    glRasterPos2i(40, 305)
    Draw.Text("Lugaru (c) Wolfire Games", 'small')
    glRasterPos2i(40, 293)
    Draw.Text("The Lugaru skeleton format (c) David Rosen", 'small')
    glRasterPos2i(40, 263)
    Draw.Text("Tipp: Select a body beforehand to set", 'small')
    glRasterPos2i(40, 251)
    Draw.Text("         vertex groups automatically", 'small')
    body_butt   = Draw.Button("Skeleton",  1,30, 150,  75, 30, "skeleton")
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

class skeletonImport:
	def parseSolidFloat(self, fourchars):
		f = struct.unpack('>f', str(fourchars))
		return f

	def __init__(self, filename):
		global my_path
		self.file = open(filename, "rb")
		#my_path = Blender.sys.dirname(filename)

	def Import(self):

		scene = bpy.data.scenes.active
		objs = Object.GetSelected()
		sel = 0
		if(len(objs)>0):
			sel=1
			obj=objs[0]
			selmesh = obj.getData(0, 1)
			obj.link(selmesh) # link


		scale = 250
		factor = 0.001*scale
		bytecount = 0
		print("Importing Lugaru Skeleton into Blender ...")
		scene = bpy.data.scenes.active

		bytes = self.file.read(4)
		bytecount += len(bytes)
		noofjoints = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
		print("  #joints: "+str(noofjoints))
		coords = []
		indices = []
		bytes = self.file.read(noofjoints*41)
		bytecount += len(bytes)

		# a joint position has 3 float components - xyz
		for i in range(0, noofjoints):
			# a float is 4 bytes.
			coords.append((self.parseSolidFloat(bytes[41*i]+bytes[41*i+1]+bytes[41*i+2]+bytes[41*i+3])[0]*factor))#x is NOT inverted
			coords.append((self.parseSolidFloat(bytes[41*i+4]+bytes[41*i+5]+bytes[41*i+6]+bytes[41*i+7])[0]*factor*-1.0))#y
			coords.append((self.parseSolidFloat(bytes[41*i+8]+bytes[41*i+9]+bytes[41*i+10]+bytes[41*i+11])[0]*factor*-1.0))#z
			indices.append(ord(bytes[41*i+31]))#joint index # TODO max 255

		skelvertlist = []

		for j in range(0, noofjoints):
			# assemble verts from coords
			skelvertlist.append((coords[3*j],coords[3*j+1],coords[3*j+2]))

		print("indices "+str(indices))

		bytes = self.file.read(4) # Number of relations
		bytecount += len(bytes)

		jvrel = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
		print("  #relations: "+str(jvrel))

		relfloats = [] # floats for a relation
		relvindices = [] # indices referencing body vertices for a relation
		relsindices = [] # indices referencing skeleton vertices for a relation
		for k in range(0, jvrel):
			bytes = self.file.read(24)
			bytecount += len(bytes)

			f1 = self.parseSolidFloat(bytes[0]+bytes[1]+bytes[2]+bytes[3])[0]
			f2 = self.parseSolidFloat(bytes[4]+bytes[5]+bytes[6]+bytes[7])[0]
			f3 = self.parseSolidFloat(bytes[8]+bytes[9]+bytes[10]+bytes[11])[0]
			f4 = self.parseSolidFloat(bytes[12]+bytes[13]+bytes[14]+bytes[15])[0]
			f5 = self.parseSolidFloat(bytes[16]+bytes[17]+bytes[18]+bytes[19])[0]
			f6 = self.parseSolidFloat(bytes[20]+bytes[21]+bytes[22]+bytes[23])[0] # maybe not a float?
			relfloats.append(((f1,f2,f3),(f4,f5,f6)))
			bytes = self.file.read(4)
			bytecount += len(bytes)

			noindices = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
			#print "#indices for rel "+str(k)+": "+str(noindices)
			myvindices = []
			for l in range(0, noindices):
				bytes = self.file.read(4)
				bytecount += len(bytes)

				myvindices.append(ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3]))
			relvindices.append((myvindices))
			bytes = self.file.read(1)
			bytecount += len(bytes)
			#print "-- delim "+str(ord(bytes[0]))

			bytes = self.file.read(8)
			bytecount += len(bytes)
			s1 = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
			s2 = ord(bytes[4])*256*256*256+ord(bytes[5])*256*256+ord(bytes[6])*256+ord(bytes[7])
			relsindices.append((s1,s2))

		#print "appendix follows"

		appendix = [] # 6 skeleton indices

		while(1):
			bytes = self.file.read(4)
			if(bytes == ""):
				break
			bytecount += len(bytes)
			a = ord(bytes[0])*256*256*256+ord(bytes[1])*256*256+ord(bytes[2])*256+ord(bytes[3])
			print(": "+str(a))
			appendix.append(a)


		#print "relational data follows"
		#print "-----------------------"
		#print "relfloats"
		#print str(relfloats)
		#print "rel v indices"
		#print str(relvindices)
		#print "rel s indices"
		#print str(relsindices)
		#print "-----------------------"
		#print "total bytecount "+str(bytecount)

		skelpoints = Mesh.New()
		skelpoints.verts.extend(skelvertlist)

		ragdollweights = Mesh.New()
		ragdollweights.verts.extend(skelvertlist)



		#print "#######"
		#print "ANALYZE"
		#print "#######"
		#print "skeleton vertex pairs with associated vertex indices follow"
		ragdolledges = []
		edges = []
		vpointers = []
		paircount = 0
		for n in range(0, len(relvindices)):
			if(len(relvindices[n])>0):
				print(str(relsindices[n]))
				paircount+=1
				edges.append(relsindices[n])
				if(sel!=0):
					vpointers.append(n)
			ragdolledges.append(relsindices[n])

		#print "total pairs "+str(paircount)

		skelpoints.faces.extend(edges)
		ragdollweights.faces.extend(ragdolledges)

		skel = scene.objects.new(skelpoints, "LugaruSkeleton")
		ragdoll = scene.objects.new(ragdollweights, "LugaruRagdoll")

		replace = Blender.Mesh.AssignModes.REPLACE
		for o in range(0, len(edges)):
			skelpoints.addVertGroup("s"+str(o))
			group = []
			for p in range(0, 2):
				group.append(edges[o][p])
			skelpoints.assignVertsToGroup("s"+str(o), group, 1.0, replace)
			if(sel!=0):
				selmesh.addVertGroup("s"+str(o))
				group = []
				for r in range(0, len(relvindices[vpointers[o]])):
					#group.append(relvindices[vpointers[o]][r])

					# preliminary code
					thisindex = []
					thisindex.append(relvindices[vpointers[o]][r])
					try:
						selmesh.assignVertsToGroup("s"+str(o), thisindex, 1.0, replace)
					except:
						print("dropped foul vertex "+str(thisindex[0]))

				#selmesh.assignVertsToGroup("s"+str(o), group, 1.0, replace)

		for n in range(0, len(appendix)//2):
			skelpoints.faces.extend((appendix[n*2],appendix[n*2+1]))

		ragdoll.setLocation(1.5,0,0)

		del coords
		del skelvertlist
		del bytes
		del objs
		del edges
		del ragdolledges
		del skelpoints
		del skel
		del ragdollweights
		del ragdoll
		del group
		del relvindices
		del vpointers

		print("... finished!")
		Blender.Redraw()

def my_callback(filename):
	skelimport = skeletonImport(filename)
	skelimport.Import()
	Draw.Redraw()

arg = __script__['arg']
