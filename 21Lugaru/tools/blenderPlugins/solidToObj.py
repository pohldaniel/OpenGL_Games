import struct
import math
import os
from struct import *

class SolidImportExport:
   
    def __init__(self, filename, outFile, matName, textureName, isBody, isWeapon):
        global my_path
        self.file = open(filename, "rb")
        self.isBody = isBody
        self.isWeapon = isWeapon
        self.outFile = outFile
        self.matName = matName
        self.textureName = textureName

    def parseSolidFloat(self, fourchars):
        f = struct.unpack('>f', str(fourchars))
        return f

    def ImportExport(self):

        if(self.isBody == 1):
            body = 1
            scale = 10
        if(self.isWeapon == 1):
            body = 0
            scale = 1


        
        factor = 0.001*scale
        bytecount = 0
        print("Importing Solid into Blender ...")
#        scene = bpy.data.scenes.active

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
        uvcoords = []
        uvtris = []
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

            # texmap
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
     
        counter = 0
        for f in tris:
            uvtri = []
            for l in range(0, 3):
                try:
                    uvtri.append(uvcoords.index((texmap[counter][l][0], texmap[counter][l][1])))
                except ValueError:
                    uvtri.append(len(uvcoords))
                    uvcoords.append((texmap[counter][l][0], texmap[counter][l][1]))

            uvtris.append(uvtri)
            counter+=1

        filepath = self.outFile
        absPath = os.path.abspath("../../Data/Textures")
       
        with open(filepath, 'w') as f:
            f.write("# OBJ file\n")
            f.write("mtllib " + self.matName + ".mtl\n")
            for v in vertlist:
                f.write("v {:1.6f} {:1.6f} {:1.6f}\n".format(v[0],v[1],v[2]))
            for uv in uvcoords:
                f.write("vt {:1.6f} {:1.6f}\n".format(uv[0],uv[1]))

            counter = 0
            f.write("usemtl Material\n")
            for t in tris:           
                f.write("f {0}/{3} {1}/{4} {2}/{5} \n".format(t[0] + 1,t[1] + 1,t[2] + 1, uvtris[counter][0] + 1, uvtris[counter][1] + 1, uvtris[counter][2] + 1)) 
                counter+=1

        with open(self.matName + ".mtl", 'w') as f:     
            f.write("newmtl Material\n")      
            f.write("Ns 10.000000\n")
            f.write("Ka 0.000000 0.000000 0.000000\n")
            f.write("Kd 1.000000 1.000000 1.000000\n")
            f.write("Ks 0.000000 0.000000 0.000000\n")
            f.write("Ni 1.000000\n")
            f.write("d 1.000000\n")
            f.write("illum 1\n")
            f.write("map_Kd " + absPath + "\\" + self.textureName)


        del coords
        del vertlist
        del tris
        del texmap
        del bytes
        print("... finished!")

importer = SolidImportExport("../../Data/Models/Sword.solid", "Sword.obj", "sword", "Sword.jpg", False, True)
importer.ImportExport()

importer = SolidImportExport("../../Data/Models/Body.solid", "Rabbit.obj", "rabbit", "FurBlackWhite.jpg", True, False)
importer.ImportExport()