#version 410 core

in vec2 texCoord;

uniform vec3 seed;
uniform float perlinAmplitude = 0.5;
uniform float perlinFrequency = 0.8;
uniform float perlinScale = 100.0;
uniform int perlinOctaves = 4;

out vec4 color;

float random2D( in vec2 st ) {
	return fract( sin(dot( st.xy, vec2(12.9898,78.233 ) + seed.xy ) ) * 43758.5453123);
}

float noiseInterpolation(in vec2 i_coord, in float i_size) {

	vec2 grid = i_coord * i_size;
    
    vec2 randomInput = floor( grid );
    vec2 weights     = fract( grid );
    
    
    float p0 = random2D( randomInput );
    float p1 = random2D( randomInput + vec2( 1.0, 0.0  ) );
    float p2 = random2D( randomInput + vec2( 0.0, 1.0 ) );
    float p3 = random2D( randomInput + vec2( 1.0, 1.0 ) );
    
    weights = smoothstep( vec2( 0.0, 0.0 ), vec2( 1.0, 1.0 ), weights ); 
    
    return p0 +
           ( p1 - p0 ) * ( weights.x ) +
           ( p2 - p0 ) * ( weights.y ) * ( 1.0 - weights.x ) +
           ( p3 - p1 ) * ( weights.y * weights.x );    
}

float perlinNoise(vec2 uv, float sc, float f, float a, int o) {

    float noiseValue = 0.0;
    
    float localAplitude  = a;
    float localFrecuency = f;

    for( int index = 0; index < o; index++ )
    {
     	       
        noiseValue += noiseInterpolation( uv, sc * localFrecuency ) * localAplitude;
    
        localAplitude   *= 0.25;
        localFrecuency  *= 3.0;
    }    

	return noiseValue * noiseValue;
}


void main(void){
	color = vec4(1.0, 0.0, 0.0, 1.0);
	vec2 suv = vec2(texCoord.x + 5.5, texCoord.y + 5.5);
	
	float cloudType = clamp(perlinNoise(suv, perlinScale*3.0, 0.3, 0.7,10), 0.0, 1.0);
	//float cloudType_ = clamp(perlinNoise(suv.xy, perlinScale/ 8., 0.3, 0.7,10), 0.0, 1.0);
	//cloudType = cloudType*0.5 + cloudType_*0.5;

	float coverage = perlinNoise(texCoord, perlinScale*0.95, perlinFrequency, perlinAmplitude, 4);
	vec4 weather = vec4( clamp(coverage, 0, 1), cloudType, 0, 1);	
	
	color = weather;
}
