#version 330 core

uniform sampler2D	diffuseTex;
uniform samplerCube cubeTex;
uniform sampler2D	areaMapTex;
uniform vec3		cameraPos;
in  Vertex {
vec4  colour;
vec2  texCoord;
vec3  normal;
vec3  worldPos;
} IN;
out  vec4  fragColour;

void  main(void)    {

vec2 scaledTexCoord = vec2(IN.texCoord.x/16.0,IN.texCoord.y/16.0);
vec4 mapping = texture( areaMapTex, scaledTexCoord);

//if(mapping.g == 0.0) discard;

vec3  viewDir      = normalize(cameraPos  - IN.worldPos );
vec4 diffuse = mix(texture(cubeTex, -viewDir), texture ( diffuseTex, IN.texCoord ), 1.0);

vec3  reflectDir   = reflect(-viewDir ,normalize(IN.normal ));
vec4  reflectTex   = texture(cubeTex ,reflectDir );

fragColour         =   reflectTex + (diffuse * 0.25f);
fragColour.a = 0.3;
 

}