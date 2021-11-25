# version 330 core
uniform sampler2D diffuseTex;
uniform float blend;

in Vertex {
vec2 texCoord ;
//float snowAlpha;
//vec4 colour;
vec4 weights;
} IN;

out vec4 fragColour ;
void main ( void ) {
 vec4 value = texture ( diffuseTex, IN.texCoord);
 if(value.a == 0.0) discard;
 fragColour = value;//mix(value,IN.weights,1.0f);//value;//mix(value,IN.weights,1.0f);//mix(value, IN.colour, IN.snowAlpha);
 }