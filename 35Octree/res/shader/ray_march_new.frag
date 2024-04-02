#version 430 core

const int MAX_SPHERE_COUNT = 24;
const int SPHERE_RADIUS = 6;
const int MAX_NUM_LIGHTS = 10;
const int NULL_ID = -1;
const int SPHERE_ID = 0;
const int VSPHERE_ID = 1;

out vec4 FragColor;

in vec2 texCoord;
in vec4 vertColor;

uniform sampler2D uDiffuseTexture;
uniform sampler2D uDepthTexture;
uniform sampler2D uStickyDepthTexture;
uniform sampler2D uEnvMap;

uniform vec3 uCameraPosition;
uniform vec3 uCameraDirection;
uniform vec3 u_camright;
uniform vec3 u_camup;
uniform float u_scaleFactor;
uniform vec2 uCameraNearSize;
uniform float uCameraNear;
uniform float uCameraFar;
uniform float uCameraAspectRatio;
uniform vec2 uResolution;
uniform float uTime;
uniform uint u_sphereCount;


uniform float uRayMarchHitThreshold;
uniform float uMaxMarchDistance;
uniform float uExternalDistanceCutDiff;
uniform float uSmoothUnion;
uniform float uContactEdgeOffset;
uniform float uContactEdgeMin;
uniform float uContactEdgeMax;

uniform mat4 viewMatrix = mat4(1.0);

const float inf = uintBitsToFloat(0x7F800000u);

struct RayMarchSettings{
    int max_steps;   
    float min_distance;
	float max_distance;
    float depth_weight;
};

struct Ray{
    vec3 origin;
    vec3 direction;
    float cosA;
};

struct Sphere {
	vec3 position;
	vec4 quaternion;
	vec3 color;
	float radius;
};

struct DirectionalLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
};

struct AmbientLight {
	vec3 color;
	float intensity;
};

uniform DirectionalLight directionalLights[MAX_NUM_LIGHTS];
uniform uint uNumDirectionalLights;
uniform AmbientLight ambientLights[MAX_NUM_LIGHTS];
uniform uint uNumAmbientLights;

RayMarchSettings rm_settings = RayMarchSettings(128, 0.0001, 100000.0, 0.5);

layout(std140, binding = 4) uniform u_spheres{
    Sphere spheres[MAX_SPHERE_COUNT];
};


float easeInExpo(float x) {
	return 1.0 - pow(2.0, -10.0 * x);
}


vec3 filmicToneMapping(vec3 color) {
	color = max(vec3(0.), color - vec3(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return color;
}

vec3 encodeSRGB(vec3 linearRGB) {
	vec3 a = 12.92 * linearRGB;
	vec3 b = 1.055 * pow(linearRGB, vec3(1.0 / 2.4)) - 0.055;
	vec3 c = step(vec3(0.0031308), linearRGB);
	return mix(a, b, c);
}

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) {
    /* 1. find current tetrahedron T and it's four vertices */
    /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
    /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
      
    /* calculate s and x */
    vec3 s = floor(p + dot(p, vec3(F3)));
    vec3 x = p - s + dot(s, vec3(G3));
      
    /* calculate i1 and i2 */
    vec3 e = step(vec3(0.0), x - x.yzx);
    vec3 i1 = e*(1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy*(1.0 - e);
        
    /* x1, x2, x3 */
    vec3 x1 = x - i1 + G3;
    vec3 x2 = x - i2 + 2.0*G3;
    vec3 x3 = x - 1.0 + 3.0*G3;
      
    /* 2. find four surflets and store them in d */
    vec4 w, d;
      
    /* calculate surflet weights */
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);
      
    /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
    w = max(0.6 - w, 0.0);
      
    /* calculate surflet components */
    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);
      
    /* multiply d by w^4 */
    w *= w;
    w *= w;
    d *= w;
      
    /* 3. return the sum of the four surflets */
    return dot(d, vec4(52.0));
}

float fit(float unscaled, float originalMin, float originalMax, float minAllowed, float maxAllowed) {
	return (maxAllowed - minAllowed) * (unscaled - originalMin) / (originalMax - originalMin) + minAllowed;
}

// * Taken From Three.js
vec3 applyQuaternion(vec3 v, vec4 q) {

	// calculate quat * vector
	vec4 qv = vec4(
        q.w * v.x + q.y * v.z - q.z * v.y,
        q.w * v.y + q.z * v.x - q.x * v.z,
        q.w * v.z + q.x * v.y - q.y * v.x,
        -q.x * v.x - q.y * v.y - q.z * v.z
	);

	// calculate result * inverse quat
	return vec3(
        qv.x * q.w + qv.w * -q.x + qv.y * -q.z - qv.z * -q.y,
        qv.y * q.w + qv.w * -q.y + qv.z * -q.x - qv.x * -q.z,
        qv.z * q.w + qv.w * -q.z + qv.x * -q.y - qv.y * -q.x
	);
}

float getDepth(sampler2D sampler, vec2 uv, float near, float far){
	float z_ndc = texture2D( sampler, uv ).r * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near + z_ndc * (near - far));	//[near, far]
}

float getPixelDepth(sampler2D sampler, vec2 uv, vec3 pixelViewDirection) {
	float sceneDepth = getDepth(sampler, uv, uCameraNear, uCameraFar);
	float cameraDot = dot(pixelViewDirection, uCameraDirection);

	return sceneDepth / cameraDot;
}

vec4 sampleEnvMap(vec3 rayDirection, vec3 normal) {
	vec3 r = reflect(rayDirection, normal);
	float m = 2.0 * sqrt(
        pow(r.x, 2.0) +
        pow(r.y, 2.0) +
        pow(r.z + 1.0, 2.0)
	);
	vec2 reflectionUv = r.xy / m + 0.5;
	// vec4 envColor = textureCube(uEnvMap, vec3(r.x, r.yz));
	return texture(uEnvMap, reflectionUv);
}

vec2 calculateFragementRay(in vec2 uv, in float aspectRatio){ 
	uv = (uv * 2.0 - 1.0) * u_scaleFactor;
	uv.x *= aspectRatio;	
    return uv;
}

vec3 getCameraToPixelVector(vec2 coords) {
	vec2 fragment_ray = calculateFragementRay(coords, uCameraAspectRatio);
    vec3 ray_dir = normalize(vec3(fragment_ray, 1.0));
	vec3 pixelViewDirection = normalize(uCameraDirection + fragment_ray.x * u_camright + fragment_ray.y * u_camup);

	float depth = getPixelDepth(uDepthTexture, coords, pixelViewDirection);
	return pixelViewDirection * depth;
}

vec3 getPixelPosition(vec2 uv, float depth) {
	vec2 fragment_ray = calculateFragementRay(uv, uCameraAspectRatio);
    vec3 ray_dir = normalize(vec3(fragment_ray, 1.0));     
	vec3 pixelViewDirection = normalize(uCameraDirection + fragment_ray.x * u_camright + fragment_ray.y * u_camup);
	
	return uCameraPosition + pixelViewDirection * depth;
}

vec4 getPixelPositionViewSpace(vec2 coords, float depth) {
	vec3 pixelPosition = getPixelPosition(coords, depth);
	return vec4(viewMatrix * vec4(pixelPosition, 1));
}

vec4 getPixelPositionViewSpace(vec2 coords) {
	vec3 cameraToPixel = getCameraToPixelVector(coords);
	vec3 pixelPosition = getPixelPosition(coords, length(cameraToPixel));
	return vec4(viewMatrix * vec4(pixelPosition, 1));
}

struct Blend {
	float distance;
	float material;
};

// * https://iquilezles.org/articles/smin/
Blend smin( float a, float b, float k ) {

	float h =  max( k-abs(a-b), 0.0 )/k;
	float m = h*h*0.5;
	float s = m*k*(1.0/2.0);
	vec2 result = mix(vec2(b-s,1.0-m), vec2(a-s,m), vec2(a<b));
	return Blend(result.x, result.y);
}

struct Surface {
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float distance;
};

Surface smin(Surface a, Surface b, float smoothness) {

	Blend blend = smin(b.distance, a.distance, smoothness);
	return Surface(mix(b.diffuse, a.diffuse, blend.material),
				   mix(b.specular, a.specular, blend.material),
                   mix(b.shininess, a.shininess, blend.material),
                   blend.distance);

}

Surface smin(Surface a, Surface b, Blend blend) {

	return Surface(mix(b.diffuse, a.diffuse, blend.material),
                   mix(b.specular, a.specular, blend.material),
                   mix(b.shininess, a.shininess, blend.material),
                   blend.distance);

}


float sphere(vec3 p, float s) {
	return length(p)-s;
}

float lowerSphere(vec3 p, float s) {
	vec3 newPosition = p;
	newPosition.y += s / 2.0;

	return sphere(newPosition, s);
}

float box( vec3 p, vec3 b ) {
	vec3 q = abs(p) - b;
	return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float intersect(float shape1, float shape2) {
	return max(shape1, shape2);
}

float subtract(float base, float subtraction) {
	return intersect(base, -subtraction);
}

Surface getSpheresSurface(vec3 position) {

	float noise = simplex3d(position / (float(SPHERE_RADIUS)));

	const vec3 spec = vec3(1.0);
	const float shininess = 100.0;

	Surface finalSurf = Surface(vec3(1), spec, shininess, uMaxMarchDistance);

	for (int i = 0; i < u_sphereCount; i++) {

        // Sphere s = spheres[0];
        Sphere s = spheres[i];
        vec3 normal = normalize(position - s.position);
        // wobbly looking objects
        vec3 pos = position + noise * normal;
        Surface surf = Surface(s.color, spec, shininess, sphere(pos - s.position, s.radius));
        finalSurf = smin(surf, finalSurf, uSmoothUnion * 2.0);
	}
	return finalSurf;
}

float getBoxDistance(vec3 pos) {

      vec3 p = pos;
      p.x += 3.0;
      p.y += 46.0;

      return box(p, vec3(4));

    }

Surface getBoxSurface(vec3 position) {

	float distance = getBoxDistance(position);

	const vec3 color = vec3(1.0, 0., 0.);
	const vec3 spec = vec3(1);
	const float shininess = 10.0;

	return Surface(color, spec, shininess, distance);

}

Surface getSphereSingleSurface(vec3 position) {

	// float noise = simplex3d(position / 5.0 + sin(uTime));

	vec3 o = vec3(3.0 + sin(uTime * 2.0) * 5.0, -46.0, 0);
	vec3 p = position - o;

	vec3 normal = normalize(p);

	// wobbly looking objects
	// vec3 pos = p + noise * normal;
	vec3 pos = p;

	float distance = sphere(pos, 2.0);

	const vec3 color = vec3(0.1, 1, 0);
	const vec3 spec = vec3(1);
	const float shininess = 10.0;

	return Surface(color, spec, shininess, distance);
}

Surface getVirtualSurface(vec3 position) {

	Surface spheresSurface = getSpheresSurface(position);
	Surface sphereSurface = getSphereSingleSurface(position);
	Surface boxSurface = getBoxSurface(position);

	return smin(smin(sphereSurface, boxSurface, uSmoothUnion * 2.0), spheresSurface, uSmoothUnion * 2.0);
}

Surface getRealSurface(vec3 position, vec3 pixelPosition, vec3 pixelColor, float shininess) {
	float distance = distance(position, pixelPosition);
	return Surface(pixelColor, vec3(1), shininess, distance);
}

// * [final surface, virtual surface, real surface, mix surface]
Surface[4] getSurface(vec3 position, vec3 pixelPosition, vec3 pixelColor) {

	float cutOffset = max(uExternalDistanceCutDiff, uRayMarchHitThreshold + 0.01);

	// * virtual scene
	Surface virtual = getVirtualSurface(position);

	// * three.js scene
	Surface real = getRealSurface(position, pixelPosition, virtual.diffuse, virtual.shininess);
	// Surface real = getRealSurface(position, pixelPosition, pixelColor, virtual.shininess);

	// float smoothness = uSmoothUnion + sin(uTime / 3.0) / 3.0;
	float smoothness = uSmoothUnion;

	Blend blend = smin(real.distance, virtual.distance, smoothness);
	Surface mix = smin(virtual, real, blend);

	// // * cut three.js scene from final result
	float finalDistance = subtract(mix.distance , real.distance - cutOffset);
	// float finalDistance = mix.distance;

	Surface finalSurface = mix;
	finalSurface.distance = finalDistance;

	Surface[4] surfaceList = Surface[](
          finalSurface,
          virtual,
          real,
          mix
	);

	return surfaceList;
}

#define SDF(position) getSurface(position, stickyPixelPosition, pixelColor)

#define NORMAL_CALC_OFFSET vec2(0.001, 0)

#define SDF_NORMAL(func, position) normalize(vec3(\
	func(position - NORMAL_CALC_OFFSET.xyy)[0].distance - func(position + NORMAL_CALC_OFFSET.xyy)[0].distance, \
	func(position - NORMAL_CALC_OFFSET.yxy)[0].distance - func(position + NORMAL_CALC_OFFSET.yxy)[0].distance, \
	func(position - NORMAL_CALC_OFFSET.yyx)[0].distance - func(position + NORMAL_CALC_OFFSET.yyx)[0].distance \
	) \
) \

vec3 getDirectionalLightsColor(Surface surf, vec3 point, vec3 pixelPosition, vec3 normal) {

	vec3 lightColor = vec3(0);

	for (int i = 0; i < uNumDirectionalLights; i++) {
        DirectionalLight dirLight = directionalLights[i];

        vec3 lightPosition = dirLight.position; 
        vec3 color = dirLight.color;
        float intensity = dirLight.intensity;
        
        vec3 toLight = normalize(lightPosition - point); 
        float normalDotToLight = dot(toLight, normal);
        lightColor += color * intensity * clamp(normalDotToLight , 0., 1.); 
        float specularIntensity = pow(max(normalDotToLight , 0.0), surf.shininess);
        lightColor += color * specularIntensity; 
	}
	return lightColor;
}

vec3 getAmbientLightsColor() {
	vec3 lightColor = vec3(0);

	for (int i = 0; i < uNumAmbientLights; i++) {
        AmbientLight ambientLight = ambientLights[i];
        vec3 color = ambientLight.color;
        float intensity = ambientLight.intensity;
        lightColor += color * intensity; 
	}
	return lightColor;
}

vec4 getLightColor(Surface[4] sdf, vec3 point, vec3 pixelPosition, vec3 normal) {

	Surface surface = sdf[0];
	float dist = surface.distance;

	vec3 lightColor = vec3(0);

	// * directional lights
	vec3 directionalLightsColor = getDirectionalLightsColor(surface, point, pixelPosition, normal);

	// * ambient lights
	vec3 ambientLightsColor = getAmbientLightsColor();

	lightColor = directionalLightsColor + ambientLightsColor;


	vec3 baseColor = surface.diffuse;
	vec3 rayDirection = normalize(pixelPosition - uCameraPosition);

	// * env map
	vec4 envColor = sampleEnvMap(rayDirection, normal);

	// * fresnel
	float fresnel = clamp(dot(normal, rayDirection), 0., 1.);
	float fresnelBrightnessMask = clamp(pow(1.0 - fresnel, 4.0), 0.0, 1.0);

	// * alpha
	float realDistance = sdf[2].distance;
	float contactEdgeMask = fit(abs(dist - realDistance - uContactEdgeOffset), uContactEdgeMin, uContactEdgeMax, 0.0, 1.0);
	float contactEdgeMaskClamped = clamp(contactEdgeMask, 0., 1.);
	float alpha = clamp(easeInExpo(contactEdgeMaskClamped) - pow(fresnel, 2.0) / 2.0, 0., 1.);
      
	// * light color
	vec3 light = lightColor + fresnelBrightnessMask;

	// * mixing reflection in
	vec3 reflection = vec3(pow(1. - fresnel, 3.));
	vec3 outgoingLight = mix(baseColor, baseColor * envColor.rgb, reflection) * light;

	return vec4(outgoingLight, alpha);
}

vec3 getPixelNormal(sampler2D sampler, vec2 uv, vec3 pixelViewDirection) {

	float c0 = getDepth(sampler, uv + vec2(0,0) / uResolution, uCameraNear, uCameraFar);
	float l2 = getDepth(sampler, uv - vec2(2,0) / uResolution, uCameraNear, uCameraFar);
	float l1 = getDepth(sampler, uv - vec2(1,0) / uResolution, uCameraNear, uCameraFar);
	float r1 = getDepth(sampler, uv + vec2(1,0) / uResolution, uCameraNear, uCameraFar);
	float r2 = getDepth(sampler, uv + vec2(2,0) / uResolution, uCameraNear, uCameraFar);
	float b2 = getDepth(sampler, uv - vec2(0,2) / uResolution, uCameraNear, uCameraFar);
	float b1 = getDepth(sampler, uv - vec2(0,1) / uResolution, uCameraNear, uCameraFar);
	float t1 = getDepth(sampler, uv + vec2(0,1) / uResolution, uCameraNear, uCameraFar);
	float t2 = getDepth(sampler, uv + vec2(0,2) / uResolution, uCameraNear, uCameraFar);
        
	float dl = abs(l1*l2/(2.0*l2-l1)-c0);
	float dr = abs(r1*r2/(2.0*r2-r1)-c0);
	float db = abs(b1*b2/(2.0*b2-b1)-c0);
	float dt = abs(t1*t2/(2.0*t2-t1)-c0);
        
	vec3 ce = getPixelPosition(uv, c0);

	vec3 dpdx = (dl<dr) ?  ce - getPixelPosition(uv - vec2(1, 0) / uResolution, l1) : 
                           -ce + getPixelPosition(uv + vec2(1, 0) / uResolution, r1) ;
	vec3 dpdy = (db<dt) ?  ce - getPixelPosition(uv - vec2(0, 1) / uResolution, b1) : 
                           -ce + getPixelPosition(uv + vec2(0, 1) / uResolution, t1) ;

	return normalize(cross(dpdx,dpdy));
}

vec3 getPixelNormalViewSpace(sampler2D sampler, vec2 coords, vec3 pixelViewDirection) {
	vec3 pixelNormal = getPixelNormal(sampler, coords, pixelViewDirection);
	return vec4(viewMatrix * vec4(pixelNormal, 1)).xyz;
}

vec4 rayMarch(vec2 uv, vec3 pixelColor) {

	vec4 color = vec4(0);

	// * vector going from the camera to the pixel that is drawn
	vec3 pixelViewVector = getCameraToPixelVector(uv);

	// * ray march setup
	vec3 rayOrigin = uCameraPosition;
	vec3 rayDirection = normalize(pixelViewVector);

	// * depth buffer
	float depth = length(pixelViewVector);

	// * depth of sticky objects
	float stickyDepth = getPixelDepth(uDepthTexture, texCoord, rayDirection);

	// * pixel position of sticky objects
	vec3 stickyPixelPosition = uCameraPosition + rayDirection * stickyDepth;

	float traveled = SDF(rayOrigin)[0].distance;

	// const float EPS_MULTIPLIER = 1.;
	const float EPS_MULTIPLIER = 1.125;
	float eps = uRayMarchHitThreshold;

	const float OVERSHOOT_RELAXED = 1.125;
	// const float OVERSHOOT_RELAXED = 1.;
	float overshoot = 1.;
	float move = 0.0;

	while (traveled > eps && traveled < uMaxMarchDistance && traveled < depth) {

        // * march forward
		vec3 position = rayOrigin + rayDirection * traveled;

        Surface[4] sdf = SDF(position);
        float dist = sdf[0].distance;


        if(sign(dist) < 0.0) {

          // * we passed through something

          float moveBack = move - move / overshoot;
          traveled -= moveBack;

          position = rayOrigin + rayDirection * traveled;

          sdf = SDF(position);
          dist = sdf[0].distance;

          overshoot = 1.0; // * disable overshoot now

        } else {

          overshoot = OVERSHOOT_RELAXED;

        }

        if (dist < eps) {

          // * we hit something

          vec3 normal = SDF_NORMAL(SDF, position);
          vec4 lightColor = getLightColor(sdf, position, stickyPixelPosition, normal);

          return lightColor;

        }  

        move = dist * overshoot;
        traveled += move;
        eps *= EPS_MULTIPLIER; // Potential for further research here...

      }

      return color;
}

Ray getRay(in vec2 texccord, in float aspectRatio, in vec3 origin){

	vec2 fragment_ray = calculateFragementRay(texccord, aspectRatio);
    vec3 ray_dir = normalize(vec3(fragment_ray, 1.0));
	vec3 rot_ray_dir = normalize(uCameraDirection + fragment_ray.x * u_camright + fragment_ray.y * u_camup);
	
    vec3 ray_origin = origin;
    float cosA = ray_dir.z; // a.z/length(a), already normalized so length(a) == 1.0. 
    return Ray(ray_origin, rot_ray_dir, cosA);
}

float signedDistance(vec3 p, in Sphere sphere){
    return (length(p - sphere.position) - sphere.radius);
}

float signedDistance(vec3 ray_endpoint, out int obj_id){
	float distance = inf;	
	for(int i = 0; i < u_sphereCount; i++){
		distance =  min(signedDistance(ray_endpoint, spheres[i]), distance);
	}
	
    obj_id = SPHERE_ID;
    return distance;
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




void main(){
	   
	// * render image
	vec4 diffuseColor = texture(uDiffuseTexture, texCoord);
	vec4 rayMarchColor = rayMarch(texCoord, diffuseColor.rgb);

	// * apply tone mapping
	rayMarchColor.rgb = filmicToneMapping(rayMarchColor.rgb);

	// * gamma correction
	const float GAMMA = 2.2;
	vec3 color = pow(rayMarchColor.rgb, vec3(1.0 / GAMMA));
		rayMarchColor.rgb = color;

	// * sRGB conversion
	rayMarchColor.rgb = encodeSRGB(rayMarchColor.rgb);

	gl_FragColor = vec4(mix(diffuseColor.rgb, rayMarchColor.rgb, vec3(rayMarchColor.a)), 1);
}