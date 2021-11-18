#version 330 core

uniform  sampler2D  diffuseTex;
uniform  vec3    cameraPos;
uniform  vec4    lightColour[4];
uniform  vec3    lightPos[4];
uniform  float   lightRadius[4];

in  Vertex {
vec3  colour;
vec2  texCoord;
vec3  normal;
vec3  worldPos;
} IN;

out  vec4  fragColour;

void  main(void)    {
   vec3  viewDir    = normalize(cameraPos  - IN.worldPos );
   vec4  diffuse    = texture(diffuseTex , IN.texCoord );
   //vec3 directionLightVec = normalize(vec3(0,-1,0)); //For direction light
   for(int i = 0; i < 4; ++i) {
	 vec3  incident   = normalize(lightPos[i]  - IN.worldPos ); //directionLightVec;//
	 vec3  halfDir    = normalize(incident + viewDir );
	 float  lambert      = max(dot(incident , IN.normal), 0.0f);
	 float  distance     = length(lightPos[i]  - IN.worldPos );
	 float  attenuation = 1.0 - clamp(distance / lightRadius[i] , 0.0,  1.0);
	 float  specFactor   = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
	 specFactor    = pow(specFactor , 60.0 );
	 vec3 light = lightColour[i].rgb * lightColour[i].w; //lightColour[i].w -> light intensity
	 vec3  surface = (diffuse.rgb * light);
	 fragColour.rgb += surface * lambert * attenuation;
	 fragColour.rgb += (vec3(1,1,0)*20.0 * specFactor )* attenuation *0.33;//light
	 fragColour.rgb +=  surface * 0.1f;   // ambient!
  }
  fragColour.a = diffuse.a;
}