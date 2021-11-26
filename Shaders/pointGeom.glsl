#version 330 core
uniform mat4 viewMatrix ;
uniform mat4 projMatrix ;
uniform float theta;
uniform int mode;

layout(triangles) in;
layout(triangle_strip , max_vertices = 20) out;
//layout(points, max_vertices = 3) out;

in Vertex {
vec2 texCoord;
vec3 normal;
} IN[];

 out Vertex {
 vec2 texCoord;
 flat vec4 finalColour;
 vec3  normal;
vec3  worldPos;
 } OUT;

 void main() {
  vec4 newPos;
 mat4 vp = projMatrix * viewMatrix;

 for(int i = 0; i < gl_in.length (); ++i) {
	newPos = gl_in[ i ]. gl_Position;
	OUT.worldPos = newPos.xyz;
	gl_Position = vp * newPos;
	OUT.finalColour = vec4(0,0,0,1);//vec4(0.643f, 0.455f, 0.286f,1.0f);
	OUT.texCoord = IN[i].texCoord;
	OUT.normal = IN[i].normal;
	EmitVertex ();
 }
 EndPrimitive ();
 if(mode == 0) {
 newPos = gl_in[ 2 ]. gl_Position;
 newPos.x += 13 * cos(theta) + 10;
 newPos.y +=  int(53 * theta) % 200;
 OUT.worldPos = newPos.xyz;
 newPos.z += 13 * sin(theta);
 gl_Position = vp * newPos;
 OUT.finalColour = vec4(0.0f,0.0f,0.0f,10.0f);
 OUT.normal = IN[2].normal;
 EmitVertex();

 newPos = gl_in[ 2 ]. gl_Position;
 newPos.x += 13 * cos(theta);
 newPos.y +=  int(53 * theta) % 200;
 OUT.worldPos = newPos.xyz;
 newPos.z += 13 * sin(theta) + 10;
 gl_Position = vp * newPos;
 OUT.finalColour = vec4(0.0f,0.0f,0.0f,10.0f);
 OUT.normal = IN[2].normal;
 EmitVertex();

 newPos = gl_in[ 2 ]. gl_Position;
 newPos.x += 13 * cos(theta);
 newPos.y +=  int(53 * theta) % 200;
 OUT.worldPos = newPos.xyz;
 newPos.z += 13 * sin(theta);
 gl_Position = vp * newPos;
 OUT.finalColour = vec4(0.0f,0.0f,0.0f,10.0f);
 OUT.normal = IN[2].normal;
 EmitVertex();
 }
 else if(mode == 1) {
	 newPos = gl_in[ 2 ]. gl_Position;
	 newPos.x += 13 * cos(theta) + 10;
	 newPos.y +=  53 * (abs(sin(theta) + 1.5f));
	 OUT.worldPos = newPos.xyz;
	 newPos.z += 13 * sin(theta);
	 gl_Position = vp * newPos;
	 OUT.finalColour = vec4(1.0f,0.0f,0.0f,1.0f);
	 OUT.normal = IN[2].normal;
	 EmitVertex();
	newPos = gl_in[ 0 ]. gl_Position;
	gl_Position = vp * newPos;
	OUT.worldPos = newPos.xyz;
	OUT.finalColour = vec4(0.8863,0.3451,0.1333, 1);
	OUT.normal = IN[0].normal;
	EmitVertex();
	
	 newPos = gl_in[ 1 ]. gl_Position;
	gl_Position = vp * newPos;
	OUT.finalColour = vec4(0.8863,0.3451,0.1333, 1);
	OUT.worldPos = newPos.xyz;
	OUT.normal = IN[1].normal;
	EmitVertex();
	EndPrimitive ();
 }
}