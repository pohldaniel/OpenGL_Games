uniform sampler2D tex;

void main()
{
    gl_FragColor = vec4( texture2D(tex,gl_TexCoord[0].st).rgb , gl_Color.a );
}