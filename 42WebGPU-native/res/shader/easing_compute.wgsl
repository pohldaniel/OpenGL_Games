  const PARTICLE_NUM: u32 = 60000u;

 struct Info {
   startValue: f32,
   endValue: f32,
   easeType: f32,
   value: f32,
 };

 struct InfoGroup {
   infoX: Info,
   infoY: Info,
   infoZ: Info,
 };

 struct Particle {
   startTime: f32,
   life: f32,
   valuePosition: vec4<f32>,
   valueScale: vec4<f32>,
   infoPosition: InfoGroup,
   infoScale: InfoGroup,
   infoAlpha: Info,
 };

 struct SimParams {
   time: f32,
   minLife: f32,
   maxLife: f32,
 };

 @group(0) @binding(0) var<uniform> params: SimParams;

 @group(0) @binding(1) var<storage, read_write> particlesA: array<Particle, PARTICLE_NUM>;

 fn rand(co: vec2<f32>) -> f32 {
   return fract(sin(dot(co, vec2<f32>(12.9898, 78.233))) * 43758.5453);
 }

 const PI: f32 = 3.141592653589793;
 const HPI: f32 = PI * 0.5;
 const PI2: f32 = PI * 2.0;

 fn calEasing(n_in: f32, type_in: f32) -> f32 {
   var n = n_in;
   switch (i32(type_in)) {
     // linear
     case 0: { }
     // QuintIn
     case 1: { n = n * n * n * n * n; }
     // QuintOut
     case 2: { n = ((n - 1.0) * n * n * n * n) + 1.0; }
     // QuintInOut
     case 3: {
       n = n * 2.0;
       if (n < 1.0) {
         n = n * n * n * n * n * 0.5;
       } else {
         n = n - 2.0;
         n = 0.5 * (n * n * n * n * n + 2.0);
       }
     }
     // BackIn
     case 4: { n = n * n * (n * 1.70158 + n - 1.70158); }
     // BackOut
     case 5: { n = (n - 1.0) * n * (n * 1.70158 + n + 1.70158) + 1.0; }
     // BackInOut
     case 6: {
       n = n * 2.0;
       if (n < 1.0) {
         n = 0.5 * n * n * (n * 1.70158 + n - 1.70158);
       } else {
         n = n - 2.0;
         n = 0.5 * (n * n * (n * 1.70158 + n + 1.70158) + 2.0);  // Note: Added +2 for consistency with other InOut patterns
       }
     }
     // CircIn
     case 7: { n = -1.0 * (sqrt(1.0 - n * n) - 1.0); }
     // CircOut
     case 8: { n = sqrt(1.0 - (n - 1.0) * n); }
     // CircInOut
     case 9: {
       n = n * 2.0;
       if (n < 1.0) {
         n = -0.5 * (sqrt(1.0 - n * n) - 1.0);
       } else {
         n = n - 2.0;
         n = 0.5 * (sqrt(1.0 - n * n) + 1.0);
       }
     }
     // CubicIn
     case 10: { n = n * n * n; }
     // CubicOut
     case 11: { n = ((n - 1.0) * n * n) + 1.0; }
     // CubicInOut
     case 12: {
       n = n * 2.0;
       if (n < 1.0) {
         n = n * n * n * 0.5;
       } else {
         n = n - 2.0;
         n = 0.5 * (n * n * n + 2.0);
       }
     }
     // ExpoIn
     case 13: {
       if (n == 0.0) {
         n = 0.0;
       } else {
         n = pow(2.0, 10.0 * (n - 1.0));
       }
     }
     // ExpoOut
     case 14: {
       if (n == 1.0) {
         n = 1.0;
       } else {
         n = -pow(2.0, -10.0 * n) + 1.0;
       }
     }
     // ExpoInOut
     case 15: {
       n = n * 2.0;
       if (n < 1.0) {
         if (n == 0.0) {
           n = 0.0;
         } else {
           n = 0.5 * pow(2.0, 10.0 * (n - 1.0));
         }
       } else {
         if (n == 2.0) {
           n = 1.0;
         } else {
           n = -0.5 * pow(2.0, -10.0 * (n - 1.0)) + 1.0;
         }
       }
     }
     // QuadIn
     case 16: { n = n * n; }
     // QuadOut
     case 17: { n = (2.0 - n) * n; }
     // QuadInOut
     case 18: {
       n = n * 2.0;
       if (n < 1.0) {
         n = n * n * 0.5;
       } else {
         n = n - 1.0;
         n = 0.5 * ((2.0 - n) * n + 1.0);
       }
     }
     // QuartIn
     case 19: { n = n * n * n * n; }
     // QuartOut
     case 20: { n = 1.0 - ((n - 1.0) * n * n * n); }
     // QuartInOut
     case 21: {
       n = n * 2.0;
       if (n < 1.0) {
         n = n * n * n * n * 0.5;
       } else {
         n = n - 2.0;
         n = 1.0 - (n * n * n * n * 0.5);
       }
     }
     // SineIn
     case 22: { n = -cos(n * HPI) + 1.0; }
     // SineOut
     case 23: { n = sin(n * HPI); }
     // SineInOut
     case 24: { n = (-cos(n * PI) + 1.0) * 0.5; }
     // ElasticIn
     case 25: {
       if (n == 0.0) {
         n = 0.0;
       } else if (n == 1.0) {
         n = 1.0;
       } else {
         n = n - 1.0;
         n = -1.0 * pow(2.0, 10.0 * n) * sin((n - 0.075) * PI2 / 0.3);
       }
     }
     // ElasticOut
     case 26: {
       if (n == 0.0) {
         n = 0.0;
       } else if (n == 1.0) {
         n = 1.0;
       } else {
         n = pow(2.0, -10.0 * n) * sin((n - 0.075) * PI2 / 0.3) + 1.0;
       }
     }
     // ElasticInOut
     case 27: {
       if (n == 0.0) {
         return 0.0;
       }
       if (n == 1.0) {
         return 1.0;
       }
       n = n * 2.0;
       if (n < 1.0) {
         n = n - 1.0;
         n = -0.5 * pow(2.0, 10.0 * n) * sin((n - 0.075) * PI2 / 0.3);
       } else {
         n = n - 1.0;
         n = 0.5 * pow(2.0, -10.0 * n) * sin((n - 0.075) * PI2 / 0.3) + 1.0;
       }
     }
     default: { }
   }
   return n;
 }
 
 @compute @workgroup_size(256)
 fn main(@builtin(global_invocation_id) global_id: vec3<u32>) {
   let index = global_id.x;
   if (index >= PARTICLE_NUM) {
     return;
   }

   var targetParticle = particlesA[index];

   var n: f32;
   let age = params.time - targetParticle.startTime;
   var lifeRatio = age / targetParticle.life;

   if (lifeRatio >= 1.0) {
     particlesA[index].startTime = params.time;
     var t0 = rand(vec2<f32>(params.minLife, params.maxLife) + params.time) * params.maxLife;
     t0 = max(params.minLife, t0);
     particlesA[index].life = t0;
     lifeRatio = 0.0;
   }

   // position
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoPosition.infoX.easeType);
   particlesA[index].valuePosition.x = targetParticle.infoPosition.infoX.startValue + (targetParticle.infoPosition.infoX.endValue - targetParticle.infoPosition.infoX.startValue) * n;
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoPosition.infoY.easeType);
   particlesA[index].valuePosition.y = targetParticle.infoPosition.infoY.startValue + (targetParticle.infoPosition.infoY.endValue - targetParticle.infoPosition.infoY.startValue) * n;
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoPosition.infoZ.easeType);
   particlesA[index].valuePosition.z = targetParticle.infoPosition.infoZ.startValue + (targetParticle.infoPosition.infoZ.endValue - targetParticle.infoPosition.infoZ.startValue) * n;

   // scale
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoScale.infoX.easeType);
   particlesA[index].valueScale.x = targetParticle.infoScale.infoX.startValue + (targetParticle.infoScale.infoX.endValue - targetParticle.infoScale.infoX.startValue) * n;
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoScale.infoY.easeType);
   particlesA[index].valueScale.y = targetParticle.infoScale.infoY.startValue + (targetParticle.infoScale.infoY.endValue - targetParticle.infoScale.infoY.startValue) * n;
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoScale.infoZ.easeType);
   particlesA[index].valueScale.z = targetParticle.infoScale.infoZ.startValue + (targetParticle.infoScale.infoZ.endValue - targetParticle.infoScale.infoZ.startValue) * n;

   // alpha
   n = lifeRatio;
   n = calEasing(n, targetParticle.infoAlpha.easeType);
   particlesA[index].infoAlpha.value = targetParticle.infoAlpha.startValue + (targetParticle.infoAlpha.endValue - targetParticle.infoAlpha.startValue) * n;
 }