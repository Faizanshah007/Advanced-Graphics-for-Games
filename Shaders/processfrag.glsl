#version 330 core
uniform  sampler2D  sceneTex;
uniform  int  isVertical;
in  Vertex    {
vec2  texCoord;
} IN;
out  vec4  fragColor;
const  float  scaleFactors [7] =
//float [](-3,-2,-1,0,1,2,3);
float [](0.006 ,  0.061,  0.242,  0.383,  0.242,  0.061,  0.006);
//float [](0,0,0,1,0,0,0);
void  main(void)    {
fragColor = vec4 (0,0,0,1);
vec2  delta = vec2 (0,0);
if(isVertical  == 1) {
delta = dFdy(IN.texCoord );
}
else{
delta = dFdx(IN.texCoord );
}
vec4 mainColour;
for(int i = 0; i < 7; i++ )   {
vec2  offset = delta * (i - 3);
vec4  tmp = texture2D(sceneTex , IN.texCoord.xy + offset );
if(i==3) mainColour = tmp;
vec4  tmp2 = tmp;//texture2D(sceneTex, IN.texCoord.xy - 0.1);
fragColor  += (tmp+tmp2)/2 * scaleFactors[i];
}
fragColor = mix(fragColor,mainColour,0.5f);
}