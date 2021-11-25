# version 330 core
uniform mat4 modelMatrix ;
uniform mat4 viewMatrix ;
uniform mat4 projMatrix ;
uniform mat4 textureMatrix ;

in vec3 position ;
in vec2 texCoord ;

 out Vertex {
 vec2 texCoord ;
 //float snowAlpha;
 //vec4 colour;

 } OUT ;

 void main ( void ) {
 mat4 mvp = projMatrix * viewMatrix * modelMatrix;

 gl_Position = mvp * vec4 ( position , 1.0);
// if(position.y > 400.0f) {
//  OUT.snowAlpha = 1.0f;
//  OUT.colour = vec4(1,1,1,1);
// }
// else{
//  OUT.snowAlpha = 1.0f;
//  float val = 0.99/400 * position.y;
//  OUT.colour = vec4(0,0,0,1);
//  if(position.y > 200.0f && position.y < 270.0f) {
//   OUT.colour = vec4(val,0,0,1);
//  }
// }
 OUT.texCoord = ( textureMatrix * vec4 ( texCoord , 0.0 , 1.0)).xy;
 }