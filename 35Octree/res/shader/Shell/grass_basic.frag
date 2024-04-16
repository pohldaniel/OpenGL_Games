#version 410 core

in vec3  fragNorm;
in vec3  fragLPos;
in vec3  fragWPos;
in vec3  fragVPos;
in vec2  fragUV;

in float instGrad;

out vec4  outColor;

float hash( uint n ){
    n = ( n << 13u ) ^ n;
    n = n * ( n * n * 15731u + 789221u ) + 1376312589u;
    return float( n & uint( 0x7fffffffU ) ) / float( 0x7fffffff );
}

void main(){
    outColor = vec4( 0.0, 0.0, 0.0, 1.0 );

    float density = 20.0;
    uvec2 freq    = uvec2( fragUV * density );
    uint seed     = freq.x + freq.y * uint( density );

        // outColor.rgb  = vec3( hash( seed ) );

    float fHash   = hash( seed );
    if( fHash >= instGrad ){
        outColor.rgb = vec3( 0.0, 1.0, 0.0 ) * instGrad;
    }else{
        discard;
    }
}