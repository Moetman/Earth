#version  430 compatibility


uniform vec4 gEyeWorldPos;


layout(vertices = 3) out;
in vec3 vPosition[];
out vec3 tcPosition[];
uniform float TessLevelInner;
uniform float TessLevelOuter;
in vec2 TexCoord_CS_in[]; 
in vec3 Normal_CS_in[]; 
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];

in float distance_CS_in[];
out float distance_ES_in[];

#define ID gl_InvocationID
/*  
//For a small plane                        

float GetTessLevel(float Distance0, float Distance1)                                            
{                                                                                               
    float AvgDistance = (Distance0 + Distance1) / 2.0;                                          
                                                                                                
    if (AvgDistance <= 2.0) {                                                                   
        return 7.0;                                                                            
        }                                                                                           
    else if (AvgDistance <= 5.0) {                                                              
        return 3.0;                                                                             
    }                                                                                           
    else {                                                                                      
        return 1.0;                                                                             
    }                                                                                           
}                                                                                               

*/ 
/*                
//For a medium plane                        

float GetTessLevel(float Distance0, float Distance1)                                            
{                                                                                               
    float AvgDistance = (Distance0 + Distance1) / 32.0;                                          
	
	if (AvgDistance <= 10.0) {                                                              
        return 128.0;                                                                             
    } 
    if (AvgDistance <= 20.0) {                                                              
        return 64.0;                                                                             
    }                                                                                               
    else if (AvgDistance <= 50.0) {                                                                   
        return 32.0;                                                                            
        }                                                                                           
    else if (AvgDistance <= 100.0) {                                                              
        return 16.0;                                                                             
    }                                                                                           
    else {                                                                                      
        return 1.0;                                                                             
    }                                                                                           
}                                                                                               
                  
*/

//For a medium plane                        

float GetTessLevel(float Distance0, float Distance1)                                            
{                                                                                               
    double AvgDistance = (Distance0 + Distance1) / 16.0;                                          

	if (AvgDistance <= 10.0) {                                                              
        return 128.0;                                                                             
    } 

	else if (AvgDistance <= 20.0) {                                                              
        return 80.0;                                                                             
    } 
    else if (AvgDistance <= 30.0) {                                                              
        return 64.0;                                                                             
    }                                                                                               
    else if (AvgDistance <= 150.0) {                                                                   
        return 32.0;                                                                            
        }                                                                                           
    else if (AvgDistance <= 200.0) {                                                              
        return 4.0;                                                                             
    }                                                                                           
    else {                                                                                      
        return 1.0;                                                                             
    }                                                                                           
}                 


/*
//For a large sphere

float GetTessLevel(float Distance0, float Distance1)                                            
{                                                                                               
    float AvgDistance = (Distance0 + Distance1) / 2.0;                                          

	if (AvgDistance <= 2.50) {                                                                   
        return 64.0;                                                                            
        }    
    else if (AvgDistance <= 8.50) {                                                                   
        return 16.0;                                                                            
        }                                                                                           
    else if (AvgDistance <= 20.0) {                                                              
        return 4.0;                                                                             
    }   
                 
   else {                                                                                      
        return 1.0;                                                                             
    }                                                                                           
}                                                                                               
                  
*/


void main(){
    tcPosition[ID] = vPosition[ID];
    Normal_ES_in[ID]   = Normal_CS_in[ID];          
    distance_ES_in[ID] = distance_CS_in[ID];
    
    if (ID == 0) {

	
	 // Calculate the distance from the camera to the three control points                 
	float EyeToVertexDistance0 = distance(gEyeWorldPos.xyz, vPosition[0]);                     
	float EyeToVertexDistance1 = distance(gEyeWorldPos.xyz, vPosition[1]);                     
	float EyeToVertexDistance2 = distance(gEyeWorldPos.xyz, vPosition[2]);                     

	//distance_FS_in[ID] = EyeToVertexDistance0;

        //gl_TessLevelInner[0] = TessLevelInner;
        //gl_TessLevelOuter[0] = TessLevelOuter;
        //gl_TessLevelOuter[1] = TessLevelOuter;
        //gl_TessLevelOuter[2] = TessLevelOuter;
	// Calculate the tessellation levels                                                        
    	gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);            
    	gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);            
    	gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);            
    	gl_TessLevelInner[0] = gl_TessLevelOuter[2];                                                

    }
     TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];  
    


}
