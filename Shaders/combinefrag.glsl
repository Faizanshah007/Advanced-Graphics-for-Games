#version 330 core
uniform  sampler2D  diffuseTex;
uniform  sampler2D  diffuseLight;
uniform  sampler2D  specularLight;
in  Vertex {
vec2  texCoord;
} IN;
out  vec4  fragColour;
void  main(void)    {
vec3  diffuse          = texture(diffuseTex  , IN.texCoord ).xyz;
vec3  light            = texture(diffuseLight , IN.texCoord ).xyz;
vec3  specular         = texture(specularLight , IN.texCoord ).xyz;
fragColour.xyz     = diffuse * 0.1;   // ambient1
fragColour.xyz    +=  diffuse * light*10;    // lambert1
fragColour.xyz    +=  specular;         // Specular2
fragColour.a       = 1.0;
}