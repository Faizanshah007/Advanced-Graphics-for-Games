#version  330  core
uniform  sampler2D  diffuseTex;

uniform  vec3    cameraPos;
uniform  vec4    lightColour;
uniform  vec3    lightPos;
uniform  float   lightRadius;

//uniform  int         useTexture;
in  Vertex    {
vec2  texCoord;
vec4  colour;
vec3 worldPos;
vec3 normal;
} IN;
out  vec4  fragColour;
void  main(void)    {
fragColour = IN.colour;
//if(useTexture  > 0) {
fragColour   =  texture(diffuseTex , IN.texCoord );

vec3  incident   = normalize(lightPos  - IN.worldPos );
vec3  viewDir    = normalize(cameraPos  - IN.worldPos );
vec3  halfDir    = normalize(incident + viewDir );

float  lambert          = max(dot(incident , IN.normal), 0.0f);
float  distance         = length(lightPos  - IN.worldPos );
float  attenuation     = 1.0f - clamp(distance / lightRadius ,0.0 ,1.0);
float  specFactor   = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
specFactor          = pow(specFactor , 60.0 );
vec3 light = lightColour.rgb * lightColour.w;
vec3 specLight = light;//vec3(0.8314,0.6863,0.2157)*20.0*lightColour.w;
   vec3  surface    = (fragColour.rgb * light);
fragColour.rgb += surface * lambert * attenuation;
fragColour.rgb += (specLight * specFactor )* attenuation *0.33;//
fragColour.rgb +=  surface * 0.1f;
//}
}