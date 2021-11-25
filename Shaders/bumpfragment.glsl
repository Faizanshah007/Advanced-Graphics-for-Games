#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D grassTex;
uniform sampler2D waterTex;
uniform sampler2D areaMapTex;
uniform sampler2D bumpTex;
//uniform samplerCube cubeTex;


uniform  vec3    cameraPos;
uniform  vec4    lightColour;
uniform  vec3    lightPos;
uniform  float   lightRadius;

in  Vertex {
vec3  colour;
vec2  texCoord;
vec3  normal;
vec3  tangent;
vec3  binormal;
vec3  worldPos;
float height;
} IN;
out  vec4  fragColour;

void  main(void)    {


vec3  incident   = normalize(lightPos  - IN.worldPos );
vec3  viewDir    = normalize(cameraPos  - IN.worldPos );
vec3  halfDir    = normalize(incident + viewDir );
mat3  TBN         = mat3(normalize(IN.tangent),
normalize(IN.binormal), normalize(IN.normal ));
 vec4  diffuse       = texture(diffuseTex , IN.texCoord );
vec3  bumpNormal    = texture(bumpTex , IN.texCoord ).rgb;
bumpNormal   = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

vec2 scaledTexCoord = vec2(IN.texCoord.x/16.0,IN.texCoord.y/16.0);
 vec4 mountain = texture ( diffuseTex, IN.texCoord );
 vec4 grass = texture ( grassTex, IN.texCoord );
 vec4 water = texture ( waterTex, IN.texCoord );//mix(texture(cubeTex, -viewDir), texture ( waterTex, IN.texCoord ), 0.1);
 water.w = 0.3;
 vec4 mapping = texture( areaMapTex, scaledTexCoord);
 fragColour = vec4(0,0,0,1);
 if(mapping.g > 0.0 && mapping.r == 0) discard;
 else if(mapping.g > 0.0 && mapping.r > 0.0) {
	fragColour = mix(grass, water, 1-mapping.r);
 }
 else if(mapping.r > 0.0 && IN.height < 30) {
	fragColour = grass;
 }
 else if(mapping.b > 0.0 || IN.height >= 30) {
	fragColour = mix( grass, mountain, 3.0/5.0 * smoothstep(0.0,400.0,IN.height) + 2.0/5.0 * (1-dot(bumpNormal, vec3(0,1,0))) );
 }


  float  lambert          = max(dot(incident , bumpNormal), 0.0f);
float  distance         = length(lightPos  - IN.worldPos );
float  attenuation     = 1.0f - clamp(distance / lightRadius ,0.0 ,1.0);
float  specFactor   = clamp(dot(halfDir , bumpNormal ) ,0.0 ,1.0);
specFactor          = pow(specFactor , 60.0 );
vec3 light = lightColour.rgb * lightColour.w;
vec3 specLight = vec3(1,1,0)*20.0*lightColour.w;
   vec3  surface    = (diffuse.rgb * light);
fragColour.rgb += surface * lambert * attenuation;
fragColour.rgb += (specLight * specFactor )* attenuation *0.33;//
fragColour.rgb +=  surface * 0.1f;
//fragColour.a    = diffuse.a;
}