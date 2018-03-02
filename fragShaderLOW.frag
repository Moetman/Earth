#version  430 compatibility 

in vec3 tangent;
in vec3 binormal;
in vec3 vPosition;

uniform float time;
out vec4 FragColor; 
uniform sampler2D diffuseMap; 
uniform sampler2D normalMap; 

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrixInverse;
uniform mat4 osg_NormalMatrix;

in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
//in vec3 lightDir;
uniform vec4 lightPosition; 

void main (void) 
{ 
	vec3 normal = normalize(Normal_FS_in); 
	vec2 texcoords = TexCoord_FS_in.xy;
	vec4 base = texture2D(diffuseMap, texcoords.xy); 
	vec3 bump = texture2D(normalMap, texcoords.xy).xyz; 
    bump = normalize(bump * 2.0 - 1.0); 

	//vec3 lightDirection = -lightPosition.xyz

	mat3 rotation = mat3(tangent, binormal, normal); 
    vec4 vertexInEye = osg_ModelViewMatrix * vec4(vPosition,1.0); 
    vec4 lightPosEye = normalize(lightPosition); 
    vec3 lightDir = lightPosition.xyz;// vec3(lightPosEye.xyz - vertexInEye.xyz); 
    lightDir = normalize(rotation * normalize(lightDir)); 

	vec4 skyblue = vec4(0.0, 0.7652, 0.993,1.0) ;    
    float lambert = max(dot(bump, lightDir), 0.0); 
	
	// compute the intensity as the dot product
    // the max prevents negative intensity values
    float intensity = max(dot(bump, lightDir), 0.0);
 
    // Compute the color per vertex
    vec4 colour = intensity * base;
	
	//vec4 colour;

	//if (lambert > 0.0) 
	//{ 
	//colour = base  * lambert; //* vec4(fresnelColour,1.0)
	//colour += base * pow(lambert, 2.0);
    //}

	FragColor = colour;//vec4(bump,1.0);// colour;//vec4(Normal_FS_in,1.0);//vec4(mlightDir,1.0);//base* lambert;//vec4(0.0,1.0,0.0,1.0); 

}

