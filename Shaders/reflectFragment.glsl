#version 330 core

uniform sampler2D	diffuseTex;
uniform sampler2D	bumpTex;
uniform samplerCube cubeTex;
uniform sampler2D	areaMapTex;
uniform vec3		cameraPos;
uniform float		clarity;

in  Vertex {
vec4  colour;
vec2  texCoord;
vec3  normal;
vec3  tangent;
vec3  binormal;
vec3  worldPos;
} IN;
out  vec4  fragColour;

void  main(void)    {

vec3  viewDir    = normalize(cameraPos  - IN.worldPos );
mat3  TBN         = mat3(normalize(IN.tangent),
normalize(IN.binormal), normalize(IN.normal ));
 //vec4  diffuse       = texture(diffuseTex , IN.texCoord );
 vec4 diffuse = mix(texture(cubeTex, -viewDir), texture ( diffuseTex, IN.texCoord ), 3.0);
vec3  bumpNormal    = texture(bumpTex , IN.texCoord ).rgb;
bumpNormal   = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

vec2 scaledTexCoord = vec2(IN.texCoord.x/16.0,IN.texCoord.y/16.0);
vec4 mapping = texture( areaMapTex, scaledTexCoord);


vec3  reflectDir   = reflect(-viewDir ,bumpNormal);
vec4  reflectTex   = texture(cubeTex ,reflectDir );

fragColour         =   reflectTex + (diffuse * 0.25f);

fragColour.a = clarity;
 

}