#version 410 core
out vec4 FragColor;

in vec2 texCoord;
in vec4 vertColor;

uniform sampler2D u_screen_texture;
uniform sampler2D u_depth_texture;

uniform vec3 u_campos;
uniform vec3 u_viewdir;
uniform vec3 u_camright;
uniform vec3 u_camup;
uniform float u_scaleFactor;
uniform float u_aspectRatio;

const float inf = uintBitsToFloat(0x7F800000u);

struct Ray{
    vec3 origin;
    vec3 direction;
    float cosA;
};

vec2 calculateFragementRay(in vec2 uv, in float aspectRatio){ 
	uv = (uv * 2.0 - 1.0) * u_scaleFactor;
	uv.x *= aspectRatio;	
    return uv;
}

Ray getRay(in vec2 texccord, in float aspectRatio, in vec3 origin){

	vec2 fragment_ray = calculateFragementRay(texccord, aspectRatio);
    vec3 ray_dir = normalize(vec3(fragment_ray, 1.0));
	vec3 rot_ray_dir = normalize(u_viewdir + fragment_ray.x * u_camright + fragment_ray.y * u_camup);
	
    vec3 ray_origin = origin;
    float cosA = ray_dir.z; // a.z/length(a), already normalized so length(a) == 1.0. 
    return Ray(ray_origin, rot_ray_dir, cosA);
}

struct RayMarchSettings{
    int max_steps;
    float max_distance;
    float min_distance;
    float depth_weight;
};

struct Sphere{
    vec3 origin;
    float radius;
};

float signedDistance(vec3 p, in Sphere sphere){
    return (length(p - sphere.origin) - sphere.radius);
}

const int NULL_ID = -1;
const int SPHERE_ID = 0;
const int VSPHERE_ID = 1;

Sphere test_sphere = Sphere(vec3(0.0, 4.0, 4.0),1.0);
Sphere volume_sphere = Sphere(vec3(0.0, 1.0, 0.0),2.0);

float signedDistance(vec3 ray_endpoint, out int obj_id){
    float d0 = signedDistance(ray_endpoint, test_sphere);
    float d1 = signedDistance(ray_endpoint, volume_sphere);
    float minval = min(d0, d1);
    obj_id = minval == d0 ? SPHERE_ID : VSPHERE_ID;
    return minval;
}

bool rayMarch(Ray ray, in RayMarchSettings settings, in float rasterized_depth, out float out_depth, out int out_obj_id){
    float depth = 0.0;
    for( int i = 0; i < settings.max_steps; ++i){
        vec3 ray_endpoint = ray.origin + ray.direction * depth;
        int obj_id;
        float ray_distance = signedDistance(ray_endpoint, obj_id);
        depth += ray_distance * settings.depth_weight;

        if( depth * ray.cosA > rasterized_depth){
            break;
        }
        if(ray_distance <= settings.min_distance){
            out_depth = depth;
            out_obj_id = obj_id;
            return true;
        }

        if(depth > settings.max_distance){
            break;
        }
    }
    out_depth = inf;
    out_obj_id = NULL_ID;
    return false;
}

float checkerBoardTexture(in vec2 uv){
    vec2 q = floor(uv);
    return clamp(mod( q.x+q.y, 2.0 ), 0.25, 1.0);
}
float fogify(in float dist){
    return  1.0/ (1.0 + dist*dist * 0.1);
}

RayMarchSettings rm_settings = RayMarchSettings(128, 100000.0, 0.0001, 0.5);


void main(){
	   
	vec3 origin = u_campos;
	Ray ray = getRay(texCoord, u_aspectRatio, origin);

    vec4 texture_color = texture(u_screen_texture, texCoord);
    float rasterized_depth = texture(u_depth_texture, texCoord).r;
	
    float depth = inf;
    int obj_id = NULL_ID;


    vec4 color;
    if(rayMarch(ray, rm_settings, rasterized_depth, depth, obj_id)){
        vec3 endpoint = ray.origin + ray.direction * depth;
        float pattern_value = checkerBoardTexture(endpoint.xz * 4.0);

        switch(obj_id){
            case SPHERE_ID:
            color = vec4(1.0)*pattern_value;
            break;
            case VSPHERE_ID:
            color = vec4(0.0,  1.0, 0.0, 1.0) * pattern_value;
            break;
            default:
            color = vec4(1.0, 0.0, 1.0, 1.0) * pattern_value;
        }
	}else{       
        color = texture_color;
	}

    FragColor = color;
}