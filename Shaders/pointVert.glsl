# version 330 core
uniform mat4 modelMatrix ;
uniform mat4 textureMatrix ;

in vec3 normal;
in vec3 position;
in vec2 texCoord;

 out Vertex {
 vec2 texCoord;
 vec3 normal;
 } OUT ;

 void main ( void ) {
OUT.texCoord = ( textureMatrix * vec4 ( texCoord , 0.0 , 1.0)).xy;
 gl_Position = //projMatrix * viewMatrix *
				modelMatrix * vec4(position,1.0);

	mat3  normalMatrix = transpose(inverse(mat3(modelMatrix )));
	vec3  wNormal    =   normalize(normalMatrix * normalize(normal ));
	OUT.normal = wNormal;
 }