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

const vec3 lightWPos = vec3( 2.0, 10.0, 4.0 );

void main(){
    outColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float density = 250.0;
    vec2  cellUV  = fract( fragUV * density );  // Create Quad cells
    vec2  midUV   = cellUV * 2.0 - 1.0;         // Remap 0:1 to -1:1

    // outColor.rg   = cellUV;
    // outColor.rg   = midUV;
    // outColor.rgb  = vec3( length( midUV ) );            
    // outColor.rgb  = vec3( 1.0 - length( cellUV - 0.5 ) ); // Nice looking quads

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    uvec2 freq    = uvec2( fragUV * density );
    uint seed     = freq.x + freq.y * 1000u; //* uint( density ); // * 1000u
    float fHash   = hash( seed );   // Random value between 0 - 1, uses as a Max Height for the cell
        
    // fHash = pow( fHash, 0.7 ); // Playing around curving the gradient value

    float fMaxH   = mix( 0.1, 0.9, fHash );
    // float fMaxH   = smoothstep( 0.4, 0.9, fHash ); // Makes things look more sparse

    // outColor.rgb  = vec3( fHash ) * instGrad; // Looks nice with random grayscale cylinders

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // float thickness = 1.0; // Very Grid Like
    float thickness = 2.0; // Increasing thickness over the cell limit fills the cell better

    // Create cylinder kinda look
    // if( length( midUV ) > thickness ) discard;

    // Simple taper, looks like christmas trees
    // if( length( midUV ) > thickness * ( 1.0 - instGrad ) ) discard;

    // Does the Height & Thickness check at the same time.
    // Does so by the distances between Max Height ( fHash ) & current shell height
    // As height increases the distance will decrease to 0, so will thickness since its being mul
    // if( length( midUV ) > thickness * ( fHash - instGrad ) ) discard;

    // Same as before, but playing around with the max height of the cell
    if( length( midUV ) > thickness * ( fMaxH - instGrad ) ) discard;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
    // Uber Basic Lighting based on Shell height
    // outColor.rgb = vec3( 0.0, 1.0, 0.0 ) * instGrad;

    // ----------------------------------------
    // Lambert Lighting
    float NdL = dot( fragNorm, normalize( lightWPos - fragWPos ) );
    NdL       = NdL * 0.5 + 0.5;                    // Remap -1:0 to 0:1
    NdL       = clamp( 0.0, 1.0, NdL );             // Help remove any midtone shadows, don't notice it using planes
    NdL       = NdL * NdL;                          // Valve's Half Lambert, just curves the light value

    // outColor.rgb = vec3( NdL );
    // outColor.rgb = vec3( 0.0, 1.0, 0.0 ) * NdL;  // Doesn't really work well for planes

    // ----------------------------------------
    // Fake Ambient Occlusion 
    float AC = pow( instGrad, 0.5 );    // Curve the Shell height gradient
    AC      += 0.1;                     // Creates a minimum starting value
    AC       = clamp( 0.0, 1.0, AC );   // Limit value between 0:1

    // outColor.rgb = vec3( AC );
    // outColor.rgb = vec3( 0.0, 1.0, 0.0 ) * AC;

    // ----------------------------------------
    // Final Lighting
    outColor.rgb = vec3( 0.0, 1.0, 0.0 ) * AC * NdL;
}