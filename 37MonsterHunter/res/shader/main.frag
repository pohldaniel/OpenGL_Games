#version 410 core

#define FLIP_HORIZONTAL 8u
#define FLIP_VERTICAL 4u
#define FLIP_DIAGONAL 2u

const float epsilon = 0.0;

in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2D u_texture;
uniform usampler2D u_lookupMap;

uniform vec2 u_tileSize = vec2(64.0);
uniform vec2 u_tilesetCount = vec2(6.0, 7.0);
uniform vec2 u_tilesetScale = vec2(1.0);
uniform float u_opacity = 1.0;

out vec4 color;

void main(void){
	uvec2 values = texture(u_lookupMap, v_texCoord).rg;
        if(values.r > 0u)
        {
            float index = float(values.r) - 1.0;
            vec2 position = vec2(mod(index + epsilon, u_tilesetCount.x), floor((index / u_tilesetCount.x) + epsilon)) / u_tilesetCount;
            vec2 offsetCoord = (v_texCoord * (textureSize(u_lookupMap, 0) * u_tilesetScale)) / u_tileSize;
            
            vec2 texelSize = vec2(1.0) / textureSize(u_lookupMap, 0);
            vec2 offset = mod(v_texCoord, texelSize);
            vec2 ratio = offset / texelSize;
            offset = ratio * (1.0 / u_tileSize);
            offset *= u_tileSize / u_tilesetCount;

            if(values.g != 0u)
            {
                vec2 tileSize = vec2(1.0) / u_tilesetCount;
                if((values.g & FLIP_DIAGONAL) != 0u)
                {
                    float temp = offset.x;
                    offset.x = offset.y;
                    offset.y = temp;
                    temp = tileSize.x / tileSize.y;
                    offset.x *= temp;
                    offset.y /= temp;
                    offset.x = tileSize.x - offset.x;
                    offset.y = tileSize.y - offset.y;
                }
                if((values.g & FLIP_VERTICAL) != 0u)
                {
                    offset.y = tileSize.y - offset.y;
                }
                if((values.g & FLIP_HORIZONTAL) != 0u)
                {
                    offset.x = tileSize.x - offset.x;
                }
            }
            color = texture(u_texture, position + offset);
            color.a = min(color.a, u_opacity);
        }
        else
        {
             color = vec4(0.0);
        }
}
