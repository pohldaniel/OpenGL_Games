// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tom� Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tom� Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David �abata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#version 130

////////////////////////////////////////////////////////////////////////////////
// Kresleni probiha mimo Scene, pouzijeme jen vstupy ktere se nam hodi,
// zbytek si dohledame sami (namisto ShaderManageru) volanim getUniformLocation.
// Kreslit se budou ctverce, v texture bude jediny pevne dany font.
////////////////////////////////////////////////////////////////////////////////
in vec2 position;
in vec2 texpos;


out vec2 texcoords;
/////////////////////////////////////////////////


void main() {	
	texcoords = texpos;
	gl_Position = vec4(position, 0, 1);
}
