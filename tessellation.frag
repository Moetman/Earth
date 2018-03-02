#version  430 compatibility


const float pi = 3.14159;


out vec4 FragColor;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec4 gColor;
in float gPrimitive;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
in vec3 Position_FS_in;
in vec3 uPos_FS_in;

in vec3 lightv_FS_in;
in vec3 viewv_FS_in;

uniform vec4 light_position_world;

uniform vec4 gEyeWorldPos;

uniform vec3 DiffuseMaterial;
uniform vec3 AmbientMaterial;

in vec3 Tangent;
in vec3 Binormal;

uniform sampler2D baseTexture;

uniform sampler2D oceanHeightmap;
uniform sampler2D oceanNormalmap;

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrixInverse;
uniform mat4 osg_NormalMatrix;


const int numWaves = 5;

uniform float time;
uniform float amplitudes[numWaves];
uniform float wavelengths[numWaves];
uniform float speeds[numWaves];
uniform vec3 directions[numWaves];
uniform float steepness[numWaves];

const vec4 diffuse_color = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 specular_color = vec4(1.0, 1.0, 1.0, 1.0);

     
void main(){
	
	//Transform normal to eye space
	vec3 facetNormal = Normal_FS_in;//gFacetNormal;// normalize(gl_NormalMatrix * gFacetNormal);//  Normal_FS_in;  //gFacetNormal;
	//vec3 N =  normalize( Normal_FS_in);//  Normal_FS_in;  //gFacetNormal;

	vec4 oceanblue = vec4(0.0, 0.0, 0.2, 1.0);
	vec4 skyblue = vec4(0.39, 0.52, 0.93,1.0) ;


	vec2 texcoords = TexCoord_FS_in.xy * 20;

	vec4 baseColour = texture2D(oceanHeightmap, texcoords.st ); //works
	vec2 index = vec2(1.0 - texcoords.x,1.0 - texcoords.y);
	index.y += 1.0 * time;


	vec4 finalColour = oceanblue;//mix(baseColour,baseColour2,0.5);


	//load Normal maps

	// lookup normal from normal map, move from [0,1] to  [-1, 1] range, normalize
	vec4 normal01 = 2.0 *texture2D(oceanNormalmap, texcoords.st )- 1.0;; //works
	vec4 normal02 = 2.0 *texture2D(oceanNormalmap, index.st )- 1.0;; //works

	vec4 finalNormal = normalize( normal01 + normal02);

	//facet normal
	vec4 N = finalNormal;//vec4(Normal_FS_in,0.0);//finalNormal;
	const float maxVariance = 0.5; // Mess around with this value to increase/decrease normal perturbation
	const float minVariance = maxVariance / 0.5;
	N = normalize(N * maxVariance - minVariance);

	vec3 B, T;
	
	float step = 1 / 1024; //where 1024 is texture width, i.e. heightmap width
	
	mat4 modelMatrix = osg_ViewMatrixInverse * osg_ModelViewMatrix;
	mat3 modelMatrix3x3 = mat3(modelMatrix);

	//Pos into worldspace
	//vec4 position_worldspace = modelMatrix * vec4(Position_FS_in,1.0);
	vec4 position_worldspace =  osg_ViewMatrixInverse * vec4(uPos_FS_in,1.0);

	vec4 vertexPos = position_worldspace;
	
	//next we offset our neighbours in their relative directions

	vec4 prev_vertex_x = vertexPos -= step;
	vec4 prev_vertex_y = vertexPos -= step;
	vec4 next_vertex_x = vertexPos += step;
	vec4 next_vertex_y = vertexPos +=step;

  

	T = next_vertex_x.xyz - prev_vertex_x.xyz;
	
	B = next_vertex_y.xyz - prev_vertex_y.xyz;

	//normal = normalize( cross(tangent, bitangent) );
	mat3 TBNmat = mat3(T, B, N);

				// osg_ModelViewMatrix keeps the light still

	vec4 eyeWorldPos = gEyeWorldPos;

	vec4 lightPosition = ( light_position_world);
	vec3 light_vector = position_worldspace.xyz - lightPosition.xyz;

	vec3 view = normalize(eyeWorldPos.xyz - position_worldspace.xyz);
	
	float light_distance = length(light_vector);
	
	vec3 light = -normalize(light_vector);


	vec3 normal_vector = normalize(N.xyz);

	vec3 binormal_vector = normalize(B);
	vec3 bitangent_vector = normalize(T);
	mat3 tbn_matrix = mat3(normalize(binormal_vector), normalize(bitangent_vector), normalize(normal_vector));
	vec3 normal_map_value = normalize(2.0 * texture2D(oceanNormalmap, texcoords).xyz - 1.0);
	normal_vector = normalize(tbn_matrix * normal_map_value);

	light_vector = normalize(light);
	vec3 view_vector = normalize(view);
	vec3 reflect_vector = normalize(reflect(-light_vector, normal_vector));

	float normal_dot_light_vector = 0.0;
	float normal_dot_half_vector = 0.0;
	float light_factor = 0.0;

	vec4 material_color = texture2D(oceanHeightmap, texcoords.st ); //works

	FragColor = material_color;

	normal_dot_light_vector = max(dot(normal_vector, light_vector), 0.0);

	vec4 light_parameters = vec4(0.4,0.3,0.2,1.0);
	vec4 light_color = vec4(0.8,0.8,0.8,1.0);

	float material_specularity = 0.5;

	if (normal_dot_light_vector > 0.0)
	{
	float attenuation_factor = (1.0 / (light_parameters.x + light_parameters.y * light_distance + light_parameters.z * light_distance * light_distance));
	attenuation_factor *= (1.0 - pow((light_distance / light_parameters.w), 2.0));

	light_factor += normal_dot_light_vector * attenuation_factor;

	normal_dot_half_vector = max(dot(reflect_vector, view_vector), 0.0);
	FragColor += pow(normal_dot_half_vector, 16.0) * material_specularity * attenuation_factor;
	}

	FragColor *= light_color * light_factor;

}


/*

	//lighting

	float specular_intensity;
	float diffuse_intensity;

	//Problem is either Position or Normal

	vec4 vertex_position_camera = osg_ModelViewMatrix * vec4(uPos_FS_in,1.0);
	vec4 normal_camera = normalize(N);
	vec4 light_position_camera = osg_ModelViewMatrix * light_position_world;

	vec3 light_vert = normalize(vec3(light_position_camera - vertex_position_camera));
	vec3 light_refl = normalize(reflect(light_vert, normal_camera.xyz));

	// diffuse light
	diffuse_intensity = max(dot(light_vert, normal_camera.xyz), 0.0);

	// specular light
	specular_intensity = max(dot(light_refl, normalize(vec3(vertex_position_camera))), 0.0);
	specular_intensity = pow(specular_intensity, 6.0);

	gl_FragColor =
		diffuse_color * diffuse_intensity +
		specular_color * specular_intensity;


*/

