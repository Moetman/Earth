#version  430 compatibility

#define textureSize 2014.0
#define texelSize 1.0 / 2048.0


const float pi = 3.14159;
const float g = 19.6f; /** In UU, 9.8f in meters; */

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;
in vec2 TexCoord_ES_in[];
out vec2 TexCoord_GS_in;
in vec3 Normal_ES_in[];    

out vec3 Normal_GS_in;

out vec3 lightv_GS_in;
out vec3 viewv_GS_in;
out vec3 uPos_GS_in;

in vec3 Tangent[];
in vec3 Binormal[];

uniform vec4 gEyeWorldPos;

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrixInverse;
uniform mat4 osg_NormalMatrix;
uniform float time;

out float EyeToVertexDistance_GS_in;
in float distance_ES_in[];
out float distance_GS_in;

const int numWaves = 5;


uniform sampler2D earthHeightMap;
//uniform int numWaves;
uniform vec4 LightPosition;
uniform float waterHeight;

uniform float amplitudes[numWaves];
uniform float wavelengths[numWaves];
uniform float speeds[numWaves];
uniform vec3 directions[numWaves];
//uniform float directions[numWaves];
uniform float steepness[numWaves];


//uniform vec4 waveCharacteristics[numWaves];



//-------------------------------------------------------------------
void OffsetVertexXPosition(inout vec4 vertex, in float offsetAmount){
vertex.x += offsetAmount;
}
//-------------------------------------------------------------------
void OffsetVertexZPosition(inout vec4 vertex, in float offsetAmount){
vertex.z += offsetAmount;
}
//-------------------------------------------------------------------


vec4 texture2D_bilinear( sampler2D tex, vec2 uv )
{
	vec2 f = fract( uv.xy * textureSize );
	vec4 t00 = texture2D( tex, uv );
	vec4 t10 = texture2D( tex, uv + vec2( texelSize, 0.0 ));
	vec4 tA = mix( t00, t10, f.x );
	vec4 t01 = texture2D( tex, uv + vec2( 0.0, texelSize ) );
	vec4 t11 = texture2D( tex, uv + vec2( texelSize, texelSize ) );
	vec4 tB = mix( t01, t11, f.x );
	return mix( tA, tB, f.y );
}
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)    
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}      
                                                                                                
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)                                                   
{                                                                                               
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;   
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        

void main(){
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    tePatchDistance = gl_TessCoord;
    tePosition = (p0 + p1 + p2);
    //tePosition = (osg_ModelViewMatrix * vec4(normalize(p0 + p1 + p2), 1.0);

    vec3 n0 = gl_TessCoord.x * Normal_ES_in[0];
    vec3 n1 = gl_TessCoord.y * Normal_ES_in[1];
    vec3 n2 = gl_TessCoord.z * Normal_ES_in[2];
    //vec3 N = normalize(n0 + n1 + n2);
    vec3 N = interpolate3D(n0 , n1 , n2);

    //Tangent
    vec3 t0 = gl_TessCoord.x * Tangent[0];
    vec3 t1 = gl_TessCoord.y * Tangent[1];
    vec3 t2 = gl_TessCoord.z * Tangent[2];
    vec3 T =  normalize(t0 + t1 + t2);

    //Binormal
    vec3 bn0 = gl_TessCoord.x * Binormal[0];
    vec3 bn1 = gl_TessCoord.y * Binormal[1];
    vec3 bn2 = gl_TessCoord.z * Binormal[2];
    vec3 B = normalize(bn0 + bn1 + bn2);


     vec3 normal = (n0 + n1 + n2)/3;
	//N = normal;
     TexCoord_GS_in = interpolate2D(TexCoord_ES_in[0], TexCoord_ES_in[1], TexCoord_ES_in[2]);
    
     //TexCoord_GS_in.s = TexCoord_GS_in.s * time;
      
     vec3 P;
    
     P = tePosition.xyz;
    //vec2 texcoords = TexCoord_GS_in.xy * 20;
	
		//vec2 texcoords2 = TexCoord_GS_in.xy;

			
  	    //scale the size of the deformed mesh 
		float scale = 50.050;	




		float textureScale = 1.0;
		vec2 texcoords = TexCoord_GS_in.xy * textureScale;
		float waveSpeed = 1.0;
		//texcoords.x += waveSpeed * time;
		//texcoords.y += waveSpeed * time;
		//TexCoord_GS_in = texcoords;
		//--------------------------------------------------------

		vec4 dv = texture2D_bilinear( earthHeightMap, texcoords.xy );
	
		//converts a RGB value to a grey value
		float df = 0.2989*dv.r + 0.5870*dv.g + 0.1140*dv.b;
		//vec4 newP = vec4(normal, 0.0) + vec4(P,0.0) + (df * scale);
		//newP.xyzw += df * scale;	
		
		vec4 newPosition =  vec4(P,1.0) +  vec4(N,1.0)  * (df * scale);

		//---------------------------------------------------------

	
		P = (newPosition.xyz) ;//(newP.xyz );//+ newP2.xyz)/2;

		//P.x += df * scale;
		//P.y += df2 * scale;
		
//---------------------





	   //wave_function( time, tePosition.xyz, P, N, B, T);


	    uPos_GS_in = P;

	    //facet normal	
	    Normal_GS_in = normal;//N;	
		
	    //Transforms to clip space
	    //The projection matrix is multiplied by the modelview matrix for every vertex	
	    gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix *  vec4(P,1.0);
		
	
	
		EyeToVertexDistance_GS_in = distance(gEyeWorldPos.xyz, gl_Position.xyz);                     
	


}







