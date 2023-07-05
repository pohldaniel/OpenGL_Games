import struct
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

    def unpackFloatLE(self, fourchars):
        f = struct.unpack('f', fourchars)
        return f

    def unpackFloatBE(self, fourchars):
        f = struct.unpack('>f', fourchars)
        return f[0]
    
    def unpackShortLE(self, fourchars):
        s = struct.unpack('h', fourchars)
        return s[0]

    def unpackShortBE(self, fourchars):
        s = struct.unpack('>h', fourchars)
        return s[0]

    def ImportExport(self):

        if(self.isBody == 1):
            body = 1
            scale = 10
        if(self.isWeapon == 1):
            body = 0
            scale = 1

        factor = 0.001*scale
        bytecount = 0

        byteArray = self.file.read(2)
        bytecount += len(byteArray)
        noofverts = self.unpackShortBE(byteArray)
        byteArray = self.file.read(2)
        bytecount += len(byteArray)
        nooftris = self.unpackShortBE(byteArray)
        print("  #verts: "+str(noofverts))
        print("  #tris:  "+str(nooftris))

        coords = []
        byteArray = self.file.read(noofverts*4*3)
        bytecount += len(byteArray)

        # a vert has 3 float components - xyz
        for i in range(0, noofverts*3):
            coords.append((self.unpackFloatBE([byteArray[4*i], byteArray[4*i+1], byteArray[4*i+2], byteArray[4*i+3]]) * factor))
        vertlist = []

        for j in range(0, noofverts):
            vertlist.append((coords[3*j],coords[3*j+1],coords[3*j+2]))

        tris = []
        texmap = []
        uvcoords = []
        uvtris = []
        byteArray = self.file.read((nooftris*2*6)+(nooftris*4*6)) # Includes trimap, texmap, garbage data

        bytecount += len(byteArray)

        for k in range(0, nooftris):
            o = k*12 + k*6*4 # byterange of triangle indices and texmap with garbage

            # Note that garbage is implicitly omitted here!
            i0 = self.unpackShortBE(bytes([byteArray[o],   byteArray[o+1]]))
            i1 = self.unpackShortBE(bytes([byteArray[o+4], byteArray[o+5]]))
            i2 = self.unpackShortBE(bytes([byteArray[o+8], byteArray[o+9]]))

            if(scale>1):
                if(body==1):
                    tris.append((i0,i2,i1))
                else:
                    tris.append((i0,i1,i2))
            else:
                tris.append((i0,i2,i1))

            # texmap
            f0 = self.unpackFloatBE(bytes([byteArray[o+12], byteArray[o+13], byteArray[o+14], byteArray[o+15]]))
            f1 = self.unpackFloatBE(bytes([byteArray[o+16], byteArray[o+17], byteArray[o+18], byteArray[o+19]]))
            f2 = self.unpackFloatBE(bytes([byteArray[o+20], byteArray[o+21], byteArray[o+22], byteArray[o+23]]))
            f3 = self.unpackFloatBE(bytes([byteArray[o+24], byteArray[o+25], byteArray[o+26], byteArray[o+27]]))
            f4 = self.unpackFloatBE(bytes([byteArray[o+28], byteArray[o+29], byteArray[o+30], byteArray[o+31]]))
            f5 = self.unpackFloatBE(bytes([byteArray[o+32], byteArray[o+33], byteArray[o+34], byteArray[o+35]]))

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
        del byteArray
        print("... finished!")

importer = SolidImportExport("../../Data/Models/Sword.solid", "Sword.obj", "sword", "Sword.jpg", False, True)
importer.ImportExport()

importer = SolidImportExport("../../Data/Models/Body.solid", "Rabbit.obj", "rabbit", "FurBlackWhite.jpg", True, False)
importer.ImportExport()
