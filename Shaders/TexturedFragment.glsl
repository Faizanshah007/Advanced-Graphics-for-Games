# version 330 core
uniform sampler2D diffuseTex;
//uniform float blend;

uniform  vec3    cameraPos;
uniform  vec4    lightColour;
uniform  vec3    lightPos;
uniform  float   lightRadius;

in Vertex {
vec2 texCoord ;
flat vec4 finalColour;
vec3  normal;
vec3  worldPos;
} IN;

out vec4 fragColour ;
void main ( void ) {
 //vec4 value = texture ( diffuseTex, IN.texCoord);
 //if(value.a == 0.0) discard;

 vec3  incident   = normalize(lightPos  - IN.worldPos );
 vec3  viewDir    = normalize(cameraPos  - IN.worldPos );
 vec3  halfDir    = normalize(incident + viewDir );

 if(IN.finalColour == vec4(0,0,0,1)) fragColour = texture(diffuseTex, IN.texCoord);
 else fragColour = IN.finalColour;//value;//mix(value,IN.weights,1.0f);//value;//mix(value,IN.weights,1.0f);//mix(value, IN.colour, IN.snowAlpha);

 float  lambert          = max(dot(incident, normalize(IN.normal)), 0.0f);
	float  distance         = length(lightPos  - IN.worldPos );
	float  attenuation     = 1.0f - clamp(distance / lightRadius ,0.0 ,1.0);
	float  specFactor   = clamp(dot(halfDir , normalize(IN.normal) ) ,0.0 ,1.0);
	specFactor          = pow(specFactor , 60.0 );
	vec3 light = lightColour.rgb * lightColour.w;
	vec3 specLight = vec3(1,1,0)*20.0*lightColour.w;
	   vec3  surface    = (fragColour.rgb * light);
	fragColour.rgb += surface * lambert * attenuation;
	fragColour.rgb += (specLight * specFactor )* attenuation *0.33;//
	fragColour.rgb +=  surface * 0.1f;
 }