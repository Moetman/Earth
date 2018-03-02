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
out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in; 
uniform float time;

in vec3 tangent;
in vec3 binormal;
out vec3 lightDir;

uniform sampler2D normalMap; 

uniform vec4 lightPosition; 
    
void main() 
{ 

 TexCoord_FS_in = TexCoord;
 Normal_FS_in = Normal;
 vPosition = Position;
 gl_Position = gl_ModelViewProjectionMatrix * vec4(Position,1.0);


}


