#version 400
uniform  mat4  modelMatrix;
uniform  mat4  viewMatrix;
uniform  mat4  projMatrix;
in vec3   position;
in vec2   texCoord;
in vec4   jointWeights;
in  ivec4  jointIndices;
in   vec3  normal;
uniform  mat4    joints [128];
out  Vertex {
vec2  texCoord;
vec3  worldPos;
vec3 normal;
//vec4 weights;
} OUT;
void  main(void) {
vec4  worldPos   = (modelMatrix * vec4(position ,1));
OUT.worldPos    = worldPos.xyz;
mat3  normalMatrix = transpose(inverse(mat3(modelMatrix )));
OUT.normal = normalize(normalMatrix * normalize(normal));
vec4  localPos   = vec4(position , 1.0f);
vec4  skelPos    = vec4 (0,0,0,0);
//OUT.weights = jointWeights;
for(int i = 0; i < 4; ++i) {
int    jointIndex   = jointIndices[i];
float  jointWeight = jointWeights[i];
skelPos  +=  joints[jointIndex] * localPos * jointWeight;
}
mat4  mvp = projMatrix * viewMatrix * modelMatrix;
gl_Position = mvp * vec4(skelPos.xyz , 1.0);
OUT.texCoord = texCoord;
}