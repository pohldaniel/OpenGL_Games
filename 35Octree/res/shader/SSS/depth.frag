#version 410 core

out float fragmentdepth;
void main() {
	fragmentdepth = gl_FragCoord.z;
}

