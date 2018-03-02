#version  430 compatibility 

uniform sampler2D earthDiffuseMap; 
uniform sampler2D earthNormalMap; 

in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
in vec3 Position_FS_in;
uniform float time;
in vec3 uPos_FS_in;
in float EyeToVertexDistance_FS_in;
in float distance_FS_in;

in vec3 tangent;
in vec3 binormal;
uniform vec4 lightPosition; 
uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrixInverse;
uniform mat4 osg_NormalMatrix;
uniform vec4 gEyeWorldPos;
out vec4 FragColor; 

vec3 Specular_Fresnel_Schlick( in vec3 SpecularColor, in vec3 PixelNormal, in vec3 LightDir )
{
    float NdotL = max( 0, dot( PixelNormal, LightDir ) );
    return SpecularColor + ( 1 - SpecularColor ) * pow( ( 1 - NdotL ), 5 );
}

float getFogFactor(float d)
{
    const float FogMax = 2000.0;
    const float FogMin = 100.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}
float fogFactorExp2(  const float dist,  const float density)
{
  const float LOG2 = -1.442695;
  float d = density * dist;
  return 1.0 - clamp(exp2(d * d * LOG2), 0.0, 1.0);
}
float fogFactorExp( const float dist, const float density)
{
  return 1.0 - clamp(exp(-density * dist), 0.0, 1.0);
}

void main (void) 
     { 
		vec4 vertexInWorld = osg_ViewMatrixInverse * vec4(Position_FS_in,1.0); 

		//float distance = clamp(distance_FS_in,0.0,1.0);
		float d = distance(gEyeWorldPos, vertexInWorld);
		//float alpha = getFogFactor(d);
		float density = 0.00009;
		float alpha = fogFactorExp(d,density);
        

		vec3 normal = normalize(Normal_FS_in); 

		vec2 texcoords = TexCoord_FS_in.xy ;
	
		 vec4 base = texture2D(earthDiffuseMap, texcoords.xy); 
         
		 vec3 bump = texture2D(earthNormalMap, texcoords.xy).xyz; 

         bump = normalize(bump * 2.0 - 1.0); 
		 
				
		 vec3 finalBump = normalize(bump);// + bump2  );



		
        mat3 rotation = mat3(tangent, binormal, normal); 
        vec4 vertexInEye = osg_ModelViewMatrix * vec4(Position_FS_in,1.0); 
        vec4 lightPosEye = normalize(lightPosition); 
        vec3 lightDir = vec3(lightPosEye.xyz - vertexInEye.xyz); 
        lightDir = normalize(rotation * normalize(lightDir)); 

	

		vec4 specularColour = vec4(1.0,1.0,1.0,1.0);
		 //vec3 fresnelColour = Specular_Fresnel_Schlick(specularColour.xyz,finalBump,lightDir );


			vec4 skyblue = vec4(0.0, 0.7652, 0.993,1.0) ;    
         float lambert = max(dot(finalBump, lightDir), 0.0); 

		//vec4 colour;
        // compute the intensity as the dot product
    	// the max prevents negative intensity values
    	float intensity = max(dot(finalBump, lightDir), 0.0);
 
    	// Compute the color per vertex
    	vec4 colour = intensity * base;
		FragColor = colour;



  }

