 #version  430 compatibility
layout (location = 1) in vec3 Position; 
layout (location = 0) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrixInverse;
uniform mat4 osg_NormalMatrix;
uniform vec4 gEyeWorldPos;

out vec3 vPosition;
out vec2 TexCoord_CS_in;
out vec3 Normal_CS_in; 
uniform float time;

in vec3 tangent;
in vec3 binormal;
out vec3 lightDir;
out float distance_CS_in;


uniform vec4 lightPosition; 
    
void main() 
{ 

	/////

	//Transform vertices in eye coordinates
	//vec4 eyeVertexPos = osg_ModelViewMatrix  * vec4(Position.xyz,1.0);
	//vec4 eyeMyPointPos = osg_ModelViewMatrix * gEyeWorldPos; 
	//float dist = distance(eyeMyPointPos,eyeVertexPos);
	//distance_CS_in = dist;

	float dx = Position.x - gEyeWorldPos.x;
	float dy = Position.y - gEyeWorldPos.y;
	float dz = Position.z - gEyeWorldPos.z;
	
	distance_CS_in = clamp((dx*dx)+(dy*dy)+(dz*dz),0.0,1.0);

   	//vec4 P = vec4(Position,1.0);
	
	//vec4 cs_position = osg_ModelViewMatrix * gl_Vertex;
    //distance_FS_in = -cs_position.z;

    //gl_Position = osg_ProjectionMatrix * cs_position;

	//float distance = length(gEyeWorldPos, Position_FS_in);
	//distance = clamp(distance,0.0,1.0);

	//float dx = P.x - gEyeWorldPos.x;
	//float dy = P.y - gEyeWorldPos.y;
	//float dz = P.z - gEyeWorldPos.z;
	
	//float distance = clamp((dx*dx)+(dy*dy)+(dz*dz),0.0,1.0);

	//vec4 vInEye =  vec4(Position_FS_in,1.0); 

	//vec3 temp = vec3(1.0,1.0,1.0);
    //float depth = (length( gEyeWorldPos.xyz - vInEye.xyz ) ) ;
	//float opacity = distance;//clamp(distance / 1000, 0, 1);
	
	//depth = normalize(depth);
	//float distance = length(P);

	//float distance = length( Position.xyz);
	//distance_FS_in = distance;



	/////




	vPosition =Position.xyz;
	TexCoord_CS_in = TexCoord;
	//TexCoord_CS_in.x += 1.0 * time;

	Normal_CS_in = Normal;
	vec3 normal = normalize(gl_Normal); 


         }


