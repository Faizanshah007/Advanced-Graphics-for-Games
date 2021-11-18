# version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D grassTex;
uniform sampler2D areaMapTex;

uniform float blend;

in Vertex {
vec2 texCoord ;
float height;
//float snowAlpha;
//vec4 colour;
//vec4 weights;
} IN;

out vec4 fragColour ;
void main ( void ) {
 vec2 scaledTexCoord = vec2(IN.texCoord.x/16.0,IN.texCoord.y/16.0);
 vec4 mountain = texture ( diffuseTex, IN.texCoord );
 vec4 grass = texture ( grassTex, IN.texCoord );
 //if(value.a == 0.0) discard;
 vec4 mapping = texture( areaMapTex, scaledTexCoord);
 if(mapping.r > 0.0 && IN.height < 30) {
	fragColour = grass;
 }
 else if(mapping.b > 0.0 || IN.height >= 30) {
	fragColour = mix(mountain,grass,1.0-clamp(IN.height,0,0.3));
 }
 //fragColour = value;//mix(value,IN.weights,1.0f);//mix(value, IN.colour, IN.snowAlpha);
 }