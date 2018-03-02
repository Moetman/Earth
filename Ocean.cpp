#include "Ocean.h"



GLuint verticesLoc = 1;
GLuint textureLoc = 0;

class EyePositionCallback: public osg::Uniform::Callback
{
    public:
        EyePositionCallback(osg::ref_ptr<osgViewer::Viewer> viewer) :_viewer(viewer){}

        virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv )
        {

       	osg::Vec3f eye,centre,up;
		//Camera position
		//osg::Matrixd mat = viewer->getCamera()->getViewMatrix();	

		//getViewMatrixAsLookAt (osg::Vec3 &eye, osg::Vec3 &center, osg::Vec3 &up, float lookDistance=1.0f)
		_viewer->getCamera()->getViewMatrixAsLookAt (eye, centre,up);

		osg::Vec4f eye_vec = osg::Vec4f(eye.x(), eye.y(), eye.z(), 1.0);
		uniform->set(eye_vec);
		//mat.getLookAt (eye, centre,up);
		
		//uniform->set(eye);
		
		//float limit = 10e6f;		

		float dx = eye.x();
		float dy = eye.y();
		float dz = eye.z();

	//	osg::notify(osg::NOTICE)<<"\nEyePositionUpdateCallback: eye.x :"  << eye.x() << " "<< eye.y() << " "<< eye.z() <<std::endl;  

        }

    private:
       osg::ref_ptr<osgViewer::Viewer> _viewer; 
};

class TimeCallback: public osg::Uniform::Callback
{
    public:
    
        
    
	TimeCallback() :theLastFrameNumber(-1)
	{
	
	}

    virtual void operator() ( osg::Uniform* uniform,osg::NodeVisitor* nv )
    {
		//osg::notify(osg::NOTICE)<<"\nTimeCallback: "<<std::endl;  
	
		bool frameNumberChanged = false;
		if(nv->getFrameStamp())
   		{
      		frameNumberChanged = nv->getFrameStamp()->getFrameNumber() != theLastFrameNumber;
      		theLastFrameNumber = nv->getFrameStamp()->getFrameNumber();
   		}

		uniform->set((float)nv->getFrameStamp()->getReferenceTime() * float(.000150));
		//osg::notify(osg::NOTICE)<<"\nTime: "<<nv->getFrameStamp()->getReferenceTime()<<std::endl;
	}

    private:
		unsigned int theLastFrameNumber;
 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LightCallback: public osg::Uniform::Callback
{
    public:
    
        
    
	LightCallback() :theLastFrameNumber(-1),time(0.0)
	{
	
	}

    virtual void operator() ( osg::Uniform* uniform,osg::NodeVisitor* nv )
    {
		//osg::notify(osg::NOTICE)<<"\nLightCallback: "<<std::endl;  
	
		bool frameNumberChanged = false;
		if(nv->getFrameStamp())
   		{
      		frameNumberChanged = nv->getFrameStamp()->getFrameNumber() != theLastFrameNumber;
      		theLastFrameNumber = nv->getFrameStamp()->getFrameNumber();
   		}
		 std::cout<<"Orbit"<<std::endl;
             float radius  = 10.0;
	     osg::Vec3 center(0.0,0.0,0.0);	 

	     float time = (float)nv->getFrameStamp()->getReferenceTime() * float(10.03150) ;
	     float yaw = 0.5;
		osg::Vec3 pos(center.x() + sinf(time)*radius,
			      center.y() + cosf(time)*radius,
			      center.z() = 100);



		uniform->set(pos);
		//osg::notify(osg::NOTICE)<<"\nLight Position: "<<nv->getFrameStamp()->getReferenceTime()<<std::endl;
		std::cout<<"\nLight Position: x: "<<pos.x()<<"  y:  "<<pos.y()<<"  z: "<<pos.z()<<std::endl;
	}

    private:
    		float time;
		unsigned int theLastFrameNumber;
 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ComputeTangentVisitor : public osg::NodeVisitor
	{
	public:
		void apply( osg::Node& node ) { traverse(node); }
	    
		void apply( osg::Geode& node )
		{
			for ( unsigned int i=0; i<node.getNumDrawables(); ++i )
			{
				osg::Geometry* geom = dynamic_cast<osg::Geometry*>( node.getDrawable(i) );
				if ( geom ) generateTangentArray( geom ); //setGeometryVertexArray(geom);
			}
			traverse( node );
		}
		
		void generateTangentArray( osg::Geometry* geom )
		{
			osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
			tsg->generate( geom );
			geom->setVertexAttribArray( 6, tsg->getTangentArray() );
			geom->setVertexAttribBinding( 6, osg::Geometry::BIND_PER_VERTEX );
			geom->setVertexAttribArray( 7, tsg->getBinormalArray() );
			geom->setVertexAttribBinding( 7, osg::Geometry::BIND_PER_VERTEX );
		}
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////


osg::ref_ptr<osg::Geode> generateSphere(float radius, unsigned int rings, unsigned int sectors, float scale)
{

  osg::notify(osg::NOTICE) << "generateSphere " << std::endl;
  osg::notify(osg::NOTICE) << " " << radius << " " << rings << " " << sectors
                           << " " << scale <<std::endl;

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();

  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
  geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
  std::vector<int> faces;


	std::vector<GLfloat> vertices2;
    std::vector<GLfloat> normals2;
    std::vector<GLfloat> texcoords2;
    std::vector<GLushort> indices;

        float const R = 1./(float)(rings-1);
        float const S = 1./(float)(sectors-1);
        int r, s;

        //vertices.resize(rings * sectors * 3);
        //normals.resize(rings * sectors * 3);
        //texcoords.resize(rings * sectors * 2);
        std::vector<GLfloat>::iterator v = vertices2.begin();
        std::vector<GLfloat>::iterator n = normals2.begin();
        std::vector<GLfloat>::iterator t = texcoords2.begin();
      

		for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
                float const y = (sin( -M_PI_2 + M_PI * r * R )) * scale;
                float const x = (cos(2*M_PI * s * S) * sin( M_PI * r * R ) ) *scale;
                float const z = (sin(2*M_PI * s * S) * sin( M_PI * r * R )) * scale;

                texCoords->push_back(osg::Vec2(s*S,r*R));
                vertices->push_back(osg::Vec3( x * radius,y * radius,z * radius));
	        normals->push_back(osg::Vec3(x,y,z));
	        //normals->push_back(osg::Vec3(0.0,1.0,1.0));


          }

		osg::DrawElementsUInt *coordIndices =
	      new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

        indices.resize(rings * sectors * 4);
        std::vector<GLushort>::iterator i = indices.begin();
        for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
                coordIndices->push_back( r * sectors + s);
                coordIndices->push_back(r * sectors + (s+1));
                coordIndices->push_back((r+1) * sectors + (s+1));
                coordIndices->push_back((r+1) * sectors + s);
        }
		 geometry->setVertexArray(vertices);
		 geometry->setNormalArray(normals);
		 geometry->setTexCoordArray(textureLoc, texCoords);
		
		  int numFaces = 0;
		  for (int y = 0; y < sectors - 1; y++) {
		    for (int x = 0; x < rings - 1; x++) {
		      // 1, 0, 2
		      faces.push_back(y * rings + (x + 1));
		      faces.push_back(y * rings + x);
		      faces.push_back((y + 1) * rings + x);

		      // 3, 1, 2
		      faces.push_back((y + 1) * rings + (x + 1));
		      faces.push_back(y * rings + (x + 1));
		      faces.push_back((y + 1) * rings + x);

		      numFaces += 2;
		    }
		  }

  int IndexCount = faces.size();
 geometry->addPrimitiveSet(new osg::DrawElementsUInt(
      osg::PrimitiveSet::PATCHES, IndexCount, (GLuint *)&faces[0]));

  // testing against quads
  // geometry->addPrimitiveSet(coordIndices);

  geometry->setVertexAttribArray(verticesLoc, vertices);
  geometry->setVertexAttribArray(textureLoc, texCoords);
  geometry->setVertexAttribBinding(textureLoc, osg::Geometry::BIND_PER_VERTEX);
  geometry->setVertexAttribBinding(verticesLoc, osg::Geometry::BIND_PER_VERTEX);
  geometry->setVertexAttribArray(2, normals);
  geometry->setVertexAttribBinding(2, osg::Geometry::BIND_PER_VERTEX);

  osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg =
      new osgUtil::TangentSpaceGenerator;
  tsg->generate(geometry, 2);
  // geometry->setVertexAttribData(6,
  // osg::Geometry::ArrayData(tsg->getTangentArray(),
  // osg::Geometry::BIND_PER_VERTEX,GL_FALSE));
  // geometry->setVertexAttribData(5,
  // osg::Geometry::ArrayData(tsg->getBinormalArray(),
  // osg::Geometry::BIND_PER_VERTEX,GL_FALSE));
  geometry->setVertexAttribArray(5, tsg->getTangentArray());
  geometry->setVertexAttribArray(6, tsg->getBinormalArray());

  // osg::Geometry::setVertexAttribArray(unsigned int, osg::Array*)

  geometry->setUseVertexBufferObjects(true);
  geometry->setUseDisplayList(false);
  geometry->setTexCoordArray(textureLoc, texCoords);

  geode->addDrawable(geometry);

  return geode;

}

///////////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::Geode> generateQuad(int tilesx, int tilesy, float scale )
{
osg::notify(osg::NOTICE) <<"generateQuad "<<std::endl;
osg::notify(osg::NOTICE) <<" "<<tilesx<<" "<<tilesy<<" "<<scale<<std::endl;

osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();


	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
	std::vector<int> faces;
	
	float htilesx = tilesx/2.0;
	float htilesy = tilesy/2.0;

	for(int x=-htilesx; x<htilesx; x++)
	{
		for(int y=-htilesy; y<htilesy; y++)
		{
		float u = (float)x / (float)(tilesx);
		float v = (float)y / (float)(tilesy);

		vertices->push_back(osg::Vec3(x * scale / (tilesx-1) - scale ,   y * scale / (tilesy-1) - scale, 0.0f));
	
		normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

		texCoords->push_back(osg::Vec2(u, v));
		 //texCoords->push_back( osg::Vec2((float)(x/tilesx),(float)y/tilesy));
			
		}
	} 
	//To test against a quad
	
	int numIndices=tilesx+1;
  
	osg::DrawElementsUInt* coordIndices =   new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	
	
	for(int iy=-htilesy;iy<htilesy;++iy)// -2 removes the shelf artifact: ie vertices with no values
	{
        	for(int ix=-htilesx;ix<htilesx;++ix)
        	{
            	// four vertices per quad.
            	coordIndices->push_back(ix    +(iy+1)*numIndices);
            	coordIndices->push_back(ix    +iy*numIndices);
            	coordIndices->push_back((ix+1)+iy*numIndices);
            	coordIndices->push_back((ix+1)+(iy+1)*numIndices);
           
        	}
    	}
    	
	
	
	geometry->setVertexArray(vertices);
	geometry->setNormalArray(normals);
	geometry->setTexCoordArray(textureLoc,texCoords);


	



	int numFaces = 0;
	for(int y=0; y<tilesy-1; y++)
	{
		for(int x=0; x<tilesx-1; x++)
		{
			// 1, 0, 2
			faces.push_back(y * tilesx + (x+1));
			faces.push_back(y * tilesx + x);
			faces.push_back((y+1) * tilesx + x);

			// 3, 1, 2
			faces.push_back((y+1) * tilesx + (x+1));
			faces.push_back(y * tilesx + (x+1));
			faces.push_back((y+1) * tilesx + x);
			
			numFaces += 2;
		}
	}

	int IndexCount = faces.size();
	
	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES, IndexCount, (GLuint* )&faces[0]));
	
	
	//testing against quads
	//geometry->addPrimitiveSet(coordIndices);
	
	 
	  geometry->setVertexAttribArray(verticesLoc,vertices);
	  geometry->setVertexAttribArray(textureLoc,texCoords);
	  geometry->setVertexAttribBinding( textureLoc ,osg::Geometry::BIND_PER_VERTEX); 
	  geometry->setVertexAttribBinding( verticesLoc ,osg::Geometry::BIND_PER_VERTEX); 
	  geometry->setVertexAttribArray(2,normals);
	  geometry->setVertexAttribBinding(2,osg::Geometry::BIND_PER_VERTEX);
	  
	   osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
			    tsg->generate(geometry, 2);
			    //geometry->setVertexAttribData(6, osg::Geometry::ArrayData(tsg->getTangentArray(), osg::Geometry::BIND_PER_VERTEX,GL_FALSE));
			    //geometry->setVertexAttribData(5, osg::Geometry::ArrayData(tsg->getBinormalArray(), osg::Geometry::BIND_PER_VERTEX,GL_FALSE));	
			    geometry->setVertexAttribArray( 5,tsg->getTangentArray());
			    geometry->setVertexAttribArray( 6,tsg->getBinormalArray());	
				
				//osg::Geometry::setVertexAttribArray(unsigned int, osg::Array*)
				
			   
	  
	geometry->setUseVertexBufferObjects( true );
	geometry->setUseDisplayList(false);
	geometry->setTexCoordArray(textureLoc,texCoords);

	geode->addDrawable(geometry);



	return geode;
}

///////////////////////////////////////////////////////////////////////////////////////

Ocean::Ocean(int wave_number, int scale, int num_grids):number_of_waves(wave_number),_scale(scale),_num_grids(num_grids)
{
	 directions = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "directions",number_of_waves);
	   wavelengths = new osg::Uniform(osg::Uniform::FLOAT, "wavelengths",number_of_waves);
	     amplitudes = new osg::Uniform(osg::Uniform::FLOAT, "amplitudes", number_of_waves);
	       frequencys = new osg::Uniform(osg::Uniform::FLOAT, "frequency", number_of_waves);
	         speeds = new osg::Uniform(osg::Uniform::FLOAT, "speeds", number_of_waves);
		   		steepness = new osg::Uniform(osg::Uniform::FLOAT, "steepness",number_of_waves);


				//for(int i=0;i<number_of_waves;++i)
				//	setWaveAttributes(i, osg::Vec3(1.0,1.0,1.0), 1, 1, 1, 1);


}

Ocean::~Ocean(){}

///////////////////////////////////////////////////////////////////////////////////////
void Ocean::createProgram(){
	program = new osg::Program();
	//program->addShader(new osg::Shader(osg::Shader::VERTEX,vertSource));
	//program->addShader(new osg::Shader(osg::Shader::TESSCONTROL,tessControlSource));
	//program->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,tessEvalSource));
	//program->addShader(new osg::Shader(osg::Shader::GEOMETRY,geomSource));
	//program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragSource));
	//program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragSourceSIMPLE));

	program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("data/shaders/tessellation.vert")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSCONTROL, osgDB::findDataFile("data/shaders/tessellation.tesc")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSEVALUATION, osgDB::findDataFile("data/shaders/tessellation.tese")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::GEOMETRY, osgDB::findDataFile("data/shaders/tessellation.geom")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("data/shaders/tessellation.frag")));


	program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	program->setParameter(GL_PATCH_VERTICES,3);

	//program->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    //program->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_QUADS );
    //program->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GGL_QUADS);



}
void Ocean::setWaveAttributes(unsigned int waveNumber,osg::Vec3 direction, float amplitude, float wavelength, float speed, float steep )
{
	amplitudes->setElement(waveNumber, amplitude);
	wavelengths->setElement(waveNumber, wavelength);
	speeds->setElement(waveNumber, speed);
	directions->setElement(waveNumber, direction);
	steepness->setElement(waveNumber, steep);

}

///////////////////////////////////////////////////////////////////////////////////////
void Ocean::init(osg::ref_ptr<osgViewer::Viewer> viewer)
{
  

  createProgram();


  group = new osg::Group;
  
  //group->addChild(generateQuad(_num_grids, _num_grids,_scale));
  
  //group->addChild(generateQuad(64, 64, 1024));
 float _radius = 6.0;
  group->addChild(generateSphere(_radius, _num_grids, _num_grids,_scale));
  
  osg::StateSet *state;
  state = group->getOrCreateStateSet();
  state->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE ); 
  state->setAttribute(program.get());
  
  
	osg::Vec3 dominantdir(0.9,0.1,0.0);
	
		   
		  

	eyePositionUniform = new osg::Uniform("gEyeWorldPos", osg::Vec4(0.0,1.0,1.0,1.0) );
	eyePositionUniform->setUpdateCallback(new EyePositionCallback(viewer));
	state->addUniform( eyePositionUniform );
	
	
	timeUniform = new osg::Uniform("time", 0.0f ); 	
	timeUniform->setUpdateCallback(new TimeCallback());
	state->addUniform( timeUniform );


	numWavesUniform = new osg::Uniform("numWaves",number_of_waves ); 	
	state->addUniform(numWavesUniform);

	//lightUniform = new osg::Uniform("LightPosition", osg::Vec3(0.0,800.0,7800.0)); 
	lightUniform = new osg::Uniform("LightPosition", osg::Vec3(0.0,0.0,200.0)); 
	//lightUniform->setUpdateCallback(new LightCallback());
	state->addUniform(lightUniform);

		
	    state->addUniform(directions); 
	   state->addUniform(wavelengths); 
	  state->addUniform(amplitudes); 
	 state->addUniform(speeds); 
	state->addUniform(steepness); 

	 
	 
	 
	 
	
	 program->addBindAttribLocation( "Tangent" , 6 );
	 program->addBindAttribLocation( "Binormal", 5 );
	   
	 timeUniform = new osg::Uniform("time", 0.0f);
  	 timeUniform->setUpdateCallback(new TimeCallback());
	 state->addUniform(timeUniform);
   
	  osg::ref_ptr<osg::Texture2D> basetex2D = new osg::Texture2D;
	  basetex2D->setTextureSize(1024, 1024);
	  basetex2D->setInternalFormat(GL_RGBA);
	  //basetex2D->setDataVariance(osg::Object::DYNAMIC);
	  basetex2D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);//NEAREST
	  basetex2D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	  basetex2D->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);//osg::Texture::CLAMP
	  basetex2D->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT); // load an image by reading a file:
		   
 	  osg::Image *image = osgDB::readImageFile("/home/paul/workspace/projects/ocean-tess/index.jpeg");//earthmap1k.jpg
	  if (!image) {
	    std::cout << " couldn't find texture basetexture, quiting." << std::endl;
	    exit(1);
	  }
   
		   
	  // Assign the texture to the image we read from file:
	  basetex2D->setImage(image);
	  state->setTextureAttributeAndModes(1, basetex2D, osg::StateAttribute::ON);
  	  state->addUniform(new osg::Uniform("baseTexture", 1));

	//Displacement map


	osg::Texture2D* DisplacementMap = new osg::Texture2D;
   	DisplacementMap->setTextureSize(1024, 1024);
	DisplacementMap->setDataVariance(osg::Object::DYNAMIC); 
   	DisplacementMap->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	DisplacementMap->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	DisplacementMap->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
	DisplacementMap->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	DisplacementMap->setInternalFormat(GL_RGBA_FLOAT32_ATI); //or 
	DisplacementMap->setInternalFormat(GL_LUMINANCE_FLOAT32_ATI); 	


	osg::Image* displacementimage = osgDB::readImageFile("/home/paul/workspace/projects/ocean-tess/data/ocean_01_h.jpg");//ocean_01_h.jpg//test.jpeg Height0001.jpg Test_h.jpg"

   	if (!displacementimage)
   	{
      	std::cout << " couldn't find  displacementimage  texture, quiting." << std::endl;
      	exit(1);
   	}
        
   	DisplacementMap->setImage(displacementimage);
   	state->setTextureAttributeAndModes(3, DisplacementMap,osg::StateAttribute::ON);
	state->addUniform( new osg::Uniform("oceanHeightmap",3) );
  
	//Normal maps
	  osg::ref_ptr<osg::Texture2D> heightMap = new osg::Texture2D;
	  heightMap->setTextureSize(1024, 1024);
	  heightMap->setInternalFormat(GL_RGBA);
	  basetex2D->setDataVariance(osg::Object::DYNAMIC);
	  heightMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);//NEAREST
	  heightMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	  heightMap->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	  heightMap->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT); // load an image by reading a file:
		   
 	  osg::Image *oceanHeightImage = osgDB::readImageFile("/home/paul/workspace/projects/ocean-tess/data/ocean_01_n.jpg");//ocean_01_n.jpg Test_n.jpg"
	  if (!oceanHeightImage) {
	    std::cout << " couldn't find Normal map texture , quiting." << std::endl;
	    exit(1);
	  }
   
		   
	  // Assign the texture to the image we read from file:
	  heightMap->setImage(oceanHeightImage);
	  state->setTextureAttributeAndModes(2, heightMap, osg::StateAttribute::ON);
  	  state->addUniform(new osg::Uniform("oceanNormalmap", 2));
   
  
}
