#version 460

#define MAX_LIGHT_SOURCES 64

struct lightInfo {
	mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    vec3 lightDir;
    float intensity;
};


layout(binding = 4) uniform LightMatrix {
    lightInfo lightInfos[MAX_LIGHT_SOURCES];
//    float sin_of_angle;
};

layout(binding = 5) uniform sampler2D texSampler[2];

//layout(binding = 5) uniform sampler2D normalMap;

layout(binding = 6) uniform cameraInfo{
    vec3 cameraPos;
    vec3 viewDir;
    vec3 viewPos;

    vec4 flashlight_diffused;
} camera;

layout(binding = 7) uniform sampler2DArray shadowMap;

layout(binding = 8) uniform lightGlobalInfo {
    uint lightSourcesCount;
};

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 pass_normal;
layout(location = 3) in vec3 pass_position;
layout(location = 4) in vec3 pass_velocity;
//layout(location = 5) in vec3 pass_lightDir;
//layout(location = 6) in vec3 pass_TBN_lightDir;
//layout(location = 7) in float pass_theta;
//layout(location = 8) in float pass_milleage;
layout(location = 5) in mat3 TBN;

layout(location = 0) out vec4 outColor;

float near_plane = 0.1f;
float far_plane = 100.f;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

float ShadowCalculation(vec3 normal) {
    float shadow = 0.0;
    for (int layerIndex = 0; layerIndex < lightSourcesCount; layerIndex++) {
        // perform perspective divide
        lightInfo current_lightInfo = lightInfos[layerIndex];
        vec4 fragPosLightSpace = (biasMat * current_lightInfo.proj * current_lightInfo.view) * vec4(pass_position, 1.0);
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;
        if ((currentDepth > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0 || currentDepth < 0.0 || projCoords.x < 0.0 || projCoords.y < 0.0)) {
            continue;
        }
        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
//        if(currentDepth > 1.0)
//            return 0.0;
        // calculate bias (based on depth map resolution and slope)
        vec3 lightDir = normalize(current_lightInfo.lightDir);
        float closestDepth = (texture(shadowMap, vec3(projCoords.xy, layerIndex)).r); 
//        //normalize(light.lightPos - pass_position);
        float bias = max(0.00005 * (1.0 - dot(normal, lightDir)), 0.0000225);
        // check whether current frag pos is in shadow
//        if (currentDepth - bias > closestDepth && currentDepth <= 1.0 && currentDepth >= 0) {
//            return .9;
//        }
        // PCF
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
        float currentShadow = 0.0;
        for(int x = -1; x <= 1; x++)
        {
            for(int y = -1; y <= 1; y++)
            {
                float pcfDepth = (texture(shadowMap, vec3((projCoords.xy + vec2(x, y) * texelSize), layerIndex)).r); 
                currentShadow += ((currentDepth - bias) > pcfDepth ? 0.9 : 0.0);        
            }
        }
        currentShadow /= 9.0;
        shadow += (currentShadow - current_lightInfo.intensity);
    }
    return shadow;
}

const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 1000.0;
const float flashlightPower = 1.0;
const float beamThickness = 25.0;
const vec3 diffuseColor = vec3(1., 1., 1.);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float shininess = 16.0;
const float screenGamma = 1; // Assume the monitor is calibrated to the sRGB color space
float flashlightAngle = 0.52359877559;

void main() {

    vec3 normal = pass_normal;
//    vec3 T = dFdx(pass_position);
//    vec3 B = dFdy(pass_position);
//    vec3 N = normalize(cross(B, T));
//    mat3 local_TBN = mat3(T, B, N);//TBN by dF

//    bool flashlight = camera.flashlight_diffused[0] != 0.f;
//    bool diffused = camera.flashlight_diffused[1] != 0.f;
	vec3 color = texture(texSampler[0], fragTexCoord).rgb;
    float alpha = texture(texSampler[0], fragTexCoord)[3];

    vec3 lightDir = normalize(lightInfos[0].lightPos - pass_position);; 
//    if  (TBN[2] == vec3(0)) {
//        lightDir = pass_lightDir;
//    }
//    normalize(light.lightPos - pass_position);
    float theta = dot(lightDir, -lightInfos[0].lightDir);
//    dot(lightDir, -light.lightDir);
    
    //diffuse
    vec3 ambientColor = 0.1 * color;

    float mileage = length(lightInfos[0].lightPos - pass_position) + 5.0;
//    length(light.lightPos - pass_position) + 5.0;
//    float power = 2.2; //GAMMA
//    vec3 linearNormalMap = vec3(pow(texture(normalMap, fragTexCoord).r * 255, power), pow(texture(normalMap, fragTexCoord).g * 255, power), pow(texture(normalMap, fragTexCoord).b * 255, power));
    vec3 normal_from_map = ((texture(texSampler[1], fragTexCoord).rgb) * 2.0) - 1.0;
    if(TBN[0] != vec3(0.0)) {
        normal = (TBN * normalize(normal_from_map));
    }
//    vec3 specular;
//
//    if(flashlight){
//        if(theta > cos(flashlightAngle)){
//            float diff = max(dot(lightDir, normal), 0.0);
//            vec3 diffuse = diff * color * flashlightPower * pow(theta, beamThickness / flashlightAngle);
//            outColor = vec4(ambientColor + (diffuse) * lightPower / (mileage * mileage), alpha);
//        } else {
//            outColor = vec4(ambientColor, alpha);
//        }
//    } else if (diffused) {
//        //diffuse
//        float diff = max(dot(lightDir, normal), 0.0);
//        vec3 diffuse = diff * color;
//        // specular
//        vec3 editViewDir = normalize(camera.viewPos - pass_position);
//        vec3 reflectDir = reflect(-lightDir, normal);
//        float spec = 0.0;
//        //blinn phong
//        vec3 halfwayDir = normalize(lightDir + editViewDir);  
//        float specAngle = max(dot(halfwayDir, normal), 0.0);
//        spec = pow(specAngle, shininess);
//
//        vec3 specular = vec3(0.1) * spec; // assuming bright white light color
//        outColor = vec4((ambientColor + diffuse + specular) * lightPower / 5 / (mileage * mileage), alpha);
//    } else {
//        outColor = vec4(ambientColor * lightPower / (mileage * mileage), alpha);
//    }
//    float number = 0.5;
//    float prec = 0.01;
////    if(texture(normalMap, fragTexCoord).x > number - prec && texture(normalMap, fragTexCoord).x < number + prec)
//    outColor = vec4(vec3(0), 1.);
//if(ShadowCalculation() == (0.0 - lightInfos[0].intensity))
//    outColor = vec4(texture(texSampler[0], fragTexCoord).rgb, 1.);
    outColor = vec4(texture(texSampler[0], fragTexCoord).rgb * (-ShadowCalculation(normal)), 1.);

    vec4 coords = gl_FragCoord;
    coords.xy -= 500;
    float radius = 250;
    if((coords.x * coords.x + coords.y * coords.y) < (radius * radius))
    {
        outColor = vec4(1, 0, 1, 1);
    }


//    outColor = vec4(normal, 1);
//if (pass_index == 0)
//    outColor = vec4(pass_velocity, 1);
}