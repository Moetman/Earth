#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <osgUtil/TangentSpaceGenerator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/io_utils>
#include <osgDB/FileUtils>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgViewer/Viewer>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osg/io_utils>
#include <osgDB/FileUtils>
#include <osg/PositionAttitudeTransform>
#include <iostream>

const int NUM_WAVES = 20 ;


//using namespace osg;

osg::ref_ptr<osg::Uniform> lightUniform;
GLuint verticesLoc = 1;
GLuint textureLoc = 0;



///////////////////////////////////////////////////////////////////////////////////
float uniformRandomInRange(float min, float max) {
    assert(min < max);
    double n = (double) rand() / (double) RAND_MAX;
    double v = min + n * (max - min);
    return v;
}

///////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////

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

		uniform->set((float)nv->getFrameStamp()->getReferenceTime() * float(.0050));
		//osg::notify(osg::NOTICE)<<"\nTime: "<<nv->getFrameStamp()->getReferenceTime()<<std::endl;
	}

    private:
		unsigned int theLastFrameNumber;
 };


///////////////////////////////////////////////////////////////////////////////////

class ComputeTangentVisitor : public osg::NodeVisitor
{
public:
    void apply( osg::Node& node ) { traverse(node); }
    
    void apply( osg::Geode& node )
    {
        for ( unsigned int i=0; i<node.getNumDrawables(); ++i )
        {
            osg::Geometry* geom = dynamic_cast<osg::Geometry*>( node.getDrawable(i) );
            if ( geom ) generateTangentArray( geom );
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

///////////////////////////////////////////////////////////////////////////////////
/*
osg::ref_ptr<osg::Group> startup_tesselated_sphere(osg::ref_ptr<osgViewer::Viewer> viewer)
{

	int scale = 1;
	int num_grids = 32;
	int wave_number = 5;

	osg::ref_ptr<Ocean> ocean = new Ocean(wave_number, scale, num_grids);
	ocean->init(viewer);
	 

	return ocean->group;
}
*/
/*
///////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::Group> startup_baseline()
{
	osg::ref_ptr<osg::Group> scene = new osg::Group();

	osg::ref_ptr<osg::Geode> sphereGeode = new osg::Geode();
	//sphere->addDrawable(new osg::ShapeDrawable(new osg::ShapeDrawable(osg::Vec3(), 1)));

	osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(),1);
	osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(sphere); 
	sphereGeode->addDrawable(unitSphereDrawable);


	osg::ref_ptr<osg::StateSet> sphereStateSet = sphereGeode->getOrCreateStateSet();
	sphereStateSet->ref();

	osg::ref_ptr<osg::Program> programObject = new osg::Program();
	osg::ref_ptr<osg::Shader>  vertexObject = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader>  fragmentObject = new osg::Shader(osg::Shader::FRAGMENT);
	programObject->addShader(fragmentObject);
	programObject->addShader(vertexObject);

	sphereStateSet->addUniform(lightUniform);

	vertexObject->loadShaderSourceFromFile("gouraud.vert");
	fragmentObject->loadShaderSourceFromFile("gouraud.frag");

	sphereStateSet->setAttributeAndModes(programObject, osg::StateAttribute::ON);

	scene->addChild(sphereGeode);

	return scene;
}

*/


///////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::Group> Base()
{
  
    //Sphere 
    osg::ref_ptr<osg::Group> oceanSphereGroup = new osg::Group();
    osg::ref_ptr<osg::Geode> sphereGeode = new osg::Geode();
    //sphere->addDrawable(new osg::ShapeDrawable(new osg::ShapeDrawable(osg::Vec3(), 1)));
    osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(),10);
    osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(sphere); 
    sphereGeode->addDrawable(unitSphereDrawable);
    osg::ref_ptr<osg::StateSet> sphereStateSet = sphereGeode->getOrCreateStateSet();
    sphereStateSet->ref();
    oceanSphereGroup->addChild(sphereGeode);


    ComputeTangentVisitor ctv;
    ctv.setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
    sphereGeode->accept( ctv );
   
    osg::ref_ptr<osg::Program> program = new osg::Program();
    osg::ref_ptr<osg::Shader>  vertexObject = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader>  fragmentObject = new osg::Shader(osg::Shader::FRAGMENT);
    program->addShader(fragmentObject);
    program->addShader(vertexObject);

    vertexObject->loadShaderSourceFromFile("vertShader.vert");
    fragmentObject->loadShaderSourceFromFile("fragShader.frag");


    //osg::ref_ptr<osg::Program> program = new osg::Program;
    //program->addShader( new osg::Shader(osg::Shader::VERTEX, vertSource) );
    //program->addShader( new osg::Shader(osg::Shader::FRAGMENT, fragSource) );
    //program->addBindAttribLocation( "tangent", 6 );
    //program->addBindAttribLocation( "binormal", 7 );
    
    
    osg::ref_ptr<osg::Texture2D> colorTex = new osg::Texture2D;
    colorTex->setImage( osgDB::readImageFile("data/whitemetal_diffuse.jpg") );
    
    osg::ref_ptr<osg::Texture2D> normalTex = new osg::Texture2D;
    normalTex->setImage( osgDB::readImageFile("data/whitemetal_normal.jpg") );
    
    osg::StateSet* stateset = sphereGeode->getOrCreateStateSet();
    stateset->addUniform( new osg::Uniform("colorTex", 0) );
    stateset->addUniform( new osg::Uniform("normalTex", 1) );
    stateset->setAttributeAndModes( program.get() );
    
    osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE;
    stateset->setTextureAttributeAndModes( 0, colorTex.get(), value );
    stateset->setTextureAttributeAndModes( 1, normalTex.get(), value );

       
    return oceanSphereGroup;


}

///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr<osg::Group> startup_baseline()
{
	osg::ref_ptr<osg::Group> scene = new osg::Group();

	osg::ref_ptr<osg::Geode> sphereGeode = new osg::Geode();
	//sphere->addDrawable(new osg::ShapeDrawable(new osg::ShapeDrawable(osg::Vec3(), 1)));

	osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(),10);
	osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(sphere); 
	sphereGeode->addDrawable(unitSphereDrawable);


	osg::ref_ptr<osg::StateSet> sphereStateSet = sphereGeode->getOrCreateStateSet();
	sphereStateSet->ref();

	scene->addChild(sphereGeode);

	return scene;


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	
	float htilesx = tilesx;
	float htilesy = tilesy;

	for(int x=0; x<htilesx; x++)
	{
		for(int y=0; y<htilesy; y++)
		{
		float u = (float)x / (float)(tilesx);
		float v = (float)y / (float)(tilesy);

		vertices->push_back(osg::Vec3(x * scale / (tilesx-1) - scale ,   y * scale / (tilesy-1) - scale, 0.0f));
	
		normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

		texCoords->push_back(osg::Vec2(u, v));
		 //texCoords->push_back( osg::Vec2((float)(x/tilesx),(float)y/tilesy));
			
		}
	} 
	//To test against a quad
	
	int numIndices=tilesx+1;
  
	osg::DrawElementsUInt* coordIndices =   new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	
	
	for(int iy=0;iy<htilesy;++iy)// -2 removes the shelf artifact: ie vertices with no values
	{
        	for(int ix=0;ix<htilesx;++ix)
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

                texCoords->push_back(osg::Vec2(1.0 - s*S,r*R));
                vertices->push_back(osg::Vec3( x * radius,z * radius,y * radius));
	        normals->push_back(osg::Vec3(x,z,y));
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::Geode> buildSphere( const double radius,
                                          const unsigned int rings,
                                          const unsigned int sectors )
    {
        osg::ref_ptr<osg::Geode>      sphereGeode = new osg::Geode;
        osg::ref_ptr<osg::Geometry>   sphereGeometry = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array>  sphereVertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array>  sphereNormals = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec2Array>  sphereTexCoords = new osg::Vec2Array;

        float const R = 1. / static_cast<float>( rings - 1 );
        float const S = 1. / static_cast<float>( sectors - 1 );

        sphereGeode->addDrawable( sphereGeometry );

        // Establish texture coordinates, vertex list, and normals
        for( unsigned int r( 0 ); r < rings; ++r ) {
            for( unsigned int s( 0) ; s < sectors; ++s ) {
                float const y = sin( -M_PI_2 + M_PI * r * R );
                float const x = cos( 2 * M_PI * s * S) * sin( M_PI * r * R );
                float const z = sin( 2 * M_PI * s * S) * sin( M_PI * r * R );

                sphereTexCoords->push_back( osg::Vec2( s * R, r * R ) );

                sphereVertices->push_back ( osg::Vec3( x * radius,
                                                       z * radius,
                                                       y * radius) )
                ;
                sphereNormals->push_back  ( osg::Vec3( x, z, y ) );

            }
        }

        sphereGeometry->setVertexArray  ( sphereVertices  );
        //sphereGeometry->setTexCoordArray( 0, sphereTexCoords );
		sphereGeometry->setNormalArray(sphereNormals);
		sphereGeometry->setTexCoordArray(textureLoc,sphereTexCoords );

		sphereGeometry->setUseVertexBufferObjects(true);
		sphereGeometry->setUseDisplayList(false);

		sphereGeometry->setVertexAttribArray(verticesLoc, sphereVertices);
		sphereGeometry->setVertexAttribArray(textureLoc, sphereTexCoords);
		sphereGeometry->setVertexAttribBinding(textureLoc, osg::Geometry::BIND_PER_VERTEX);
		sphereGeometry->setVertexAttribBinding(verticesLoc, osg::Geometry::BIND_PER_VERTEX);
		sphereGeometry->setVertexAttribArray(2, sphereNormals);
		sphereGeometry->setVertexAttribBinding(2, osg::Geometry::BIND_PER_VERTEX);

        // Generate quads for each face.
        for( unsigned int r( 0 ); r < rings - 1; ++r ) {
            for( unsigned int s( 0 ); s < sectors - 1; ++s ) {

                osg::ref_ptr<osg::DrawElementsUInt> face =
                        new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS,
                                                   4 )
                ;
                // Corners of quads should be in CCW order.
                face->push_back( ( r + 0 ) * sectors + ( s + 0 ) );
                face->push_back( ( r + 0 ) * sectors + ( s + 1 ) );
                face->push_back( ( r + 1 ) * sectors + ( s + 1 ) );
                face->push_back( ( r + 1 ) * sectors + ( s + 0 ) );

                sphereGeometry->addPrimitiveSet( face );
            }
        }

        return sphereGeode;
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr<osg::Group> createLowPolyEarth()
{
		osg::ref_ptr<osg::Group> scene = new osg::Group();

		// create a sphere
		osg::ref_ptr<osg::Geode> sphere = buildSphere(6371,32,32);

		//sphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 100)));

		// create a simple material
		//osg::Material *material = new osg::Material();
		//material->setEmission(osg::Material::FRONT, osg::Vec4(0.8, 0.8, 0.8, 1.0));
	
	
		// create a texture
		// load image for texture
		osg::Image *image = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/earthmap1k.jpg");
		if (!image) {
			std::cout << "Couldn't load texture from createLowPolyEarth." << std::endl;
			return NULL;
		}
		osg::Texture2D *texture = new osg::Texture2D;
		texture->setDataVariance(osg::Object::DYNAMIC);
		texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
		texture->setImage(image);

		// assign the material and texture to the sphere
		osg::StateSet *sphereStateSet = sphere->getOrCreateStateSet();
		sphereStateSet->ref();
		//sphereStateSet->setAttribute(material);
		sphereStateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		sphereStateSet->addUniform(new osg::Uniform("diffuseMap", 0));
	 	//add shaders 
	 
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("vertShaderLOW.vert")));
		program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("fragShaderLOW.frag")));

		sphereStateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE ); 
		sphereStateSet->setAttribute(program.get());

		scene->addChild(sphere);

	return scene;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//    GERSTNER
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void generateWaveData(osg::ref_ptr<osg::StateSet> stateset)
{
	
	osg::Vec3 dominantdir(0.9,0.1,0.0);
	osg::Vec3 direction(0.0,0.0,0.0);
	
	 osg::Uniform *directions = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "directions",NUM_WAVES);
	  osg::Uniform *wavelengths = new osg::Uniform(osg::Uniform::FLOAT, "wavelengths",NUM_WAVES);
	   osg::Uniform *amplitudes = new osg::Uniform(osg::Uniform::FLOAT, "amplitudes", NUM_WAVES);
	     osg::Uniform *frequencys = new osg::Uniform(osg::Uniform::FLOAT, "frequency", NUM_WAVES);
	       osg::Uniform *speeds = new osg::Uniform(osg::Uniform::FLOAT, "speeds", NUM_WAVES);
		 osg::Uniform *steepness = new osg::Uniform(osg::Uniform::FLOAT, "steepness",NUM_WAVES);
	        // initialize random seed: 
		  srand ( time(NULL) );
	       
		  
		  float spread = 360.;
		  
		  
		  for (int i = 0; i < NUM_WAVES; ++i)
		  {
		    
		      

			// generate secret number: 
			float num = (rand() % 100 + 1) * .0008101;
			float amplitude = num;
			//float amplitude = 0.0005f / (i * 0.001 + 1);

			std::cout << "Amplitudes: "<<amplitude<<std::endl;
			amplitudes->setElement(i, amplitude);
			
				
			//float wavelength =  (rand() % 10 + 1) * ((rand() % 100 + 1) * 0.1);
			 float wavelength = ( (rand() % 100 + 1 )* amplitude)  ;

			std::cout << "WavelLengths: "<<wavelength<<std::endl;
			wavelengths->setElement(i, wavelength);
			
			 float speed = 0.0005f * sqrt(M_PI/wavelength);
			//float speed = ( (rand() % 10 + 1) * 0.09);
			//float speed = 0.01f + 2*i;

			std::cout << "Speed: "<<amplitude<<std::endl;
			speeds->setElement(i, speed);
			      
			float steep = .5f*( (rand() % 100) * .001f + .1f);
			std::cout << "Steepness: "<<steep<<std::endl;
			steepness->setElement(i, steep);
			
			//osg::Vec3 direction = osg::Vec3( sqrt((rand() % 20 + 1)) * ((rand() % 10 + 1) * 0.01), sqrt((rand() % 100 + 1)) * ((rand() % 10 + 1) * 0.01)  , 0.0);
			
			//direction = dominantdir + osg::Vec3(  ( (rand() % 10 ) * 0.1),( ( (rand() % 10 ) * 0.1))   , ( ( (rand() % 10 ) * 0.1)));
			
			
			 float angle = uniformRandomInRange(-M_PI/3, M_PI/3);
			 float x = ((rand() % 500 + 1 )* 0.1) ;
			 float y = ((rand() % 500 + 1 )* 0.1) ;
			  
			  osg::Vec3 direction = osg::Vec3(sin(x), cos(y) , 0.0);
			  //direction.x() += (spread *  (rand() % 10 * 0.1) - spread * 0.5 * .100 )* M_PI/180;
			  //direction.y() += (spread *  (rand() % 10 * 0.1) - spread * 0.5 * .100 )* M_PI/180;
			  //direction.z() += ( (sqrt(2.0)) * 0.5 + 0.1)/ 360;
			std::cout << "Direction: "<<direction.x()<<" "<<direction.y()<<" "<<std::endl;
			
			
			//osg::Vec3 direction = osg::Vec3(0.36,1.0, 0.0);
			directions->setElement(i, direction);
			
			std::cout<<std::endl;
			
		  }
		  
		 stateset->addUniform(directions); 
	   stateset->addUniform(wavelengths); 
	  stateset->addUniform(amplitudes); 
	 stateset->addUniform(speeds); 



}

osg::ref_ptr<osg::Group> generateOceanSphere(osg::ref_ptr<osgViewer::Viewer> viewer)
{
 osg::ref_ptr<osg::Group> oceanSphereGroup = new osg::Group();

//Sphere with earth radius
    oceanSphereGroup->addChild(generateSphere(6371.0, 128, 128, 1.0));

	osg::ref_ptr<osg::Program> program = new osg::Program();
	//program->addShader(new osg::Shader(osg::Shader::VERTEX,vertSource));
	//program->addShader(new osg::Shader(osg::Shader::TESSCONTROL,tessControlSource));
	//program->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,tessEvalSource));
	//program->addShader(new osg::Shader(osg::Shader::GEOMETRY,geomSource));
	//program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragSource));
	//program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragSourceSIMPLE));

	program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("vertShader.vert")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSCONTROL, osgDB::findDataFile("tessellation.tesc")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSEVALUATION, osgDB::findDataFile("tessellation.tese")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::GEOMETRY, osgDB::findDataFile("tessellation.geom")));
	//program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("data/shaders/tessellation.frag")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("fragShader.frag")));


	program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	program->setParameter(GL_PATCH_VERTICES,3);

	osg::ref_ptr<osg::StateSet> stateset = oceanSphereGroup->getOrCreateStateSet();
	 stateset->setAttributeAndModes( program.get() );

	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE ); 
	stateset->setAttribute(program.get());

	osg::ref_ptr<osg::Uniform> eyePositionUniform = new osg::Uniform("gEyeWorldPos", osg::Vec4(0.0,1.0,1.0,1.0) );
	eyePositionUniform->setUpdateCallback(new EyePositionCallback(viewer));
	stateset->addUniform( eyePositionUniform );
	
	
	osg::ref_ptr<osg::Uniform> timeUniform = new osg::Uniform("time", 0.0f ); 	
	timeUniform->setUpdateCallback(new TimeCallback());
	stateset->addUniform( timeUniform );


	//height map
   	osg::Texture2D* DisplacementMap = new osg::Texture2D;
   	DisplacementMap->setTextureSize(1024, 1024);
	DisplacementMap->setDataVariance(osg::Object::DYNAMIC); 
   	DisplacementMap->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	DisplacementMap->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	DisplacementMap->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
	DisplacementMap->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	DisplacementMap->setInternalFormat(GL_RGBA_FLOAT32_ATI); //or 
	DisplacementMap->setInternalFormat(GL_LUMINANCE_FLOAT32_ATI); 	
	osg::Image* displacementimage = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/ocean_01_h_h.jpg");//ocean_01_h_h.jpg ocean_01_h.jpg//test.jpeg Height0001.jpg Test_h.jpg"

   	if (!displacementimage)
   	{
      	std::cout << " couldn't find  displacementimage  texture, quiting." << std::endl;
      	exit(1);
   	}
        
   	DisplacementMap->setImage(displacementimage);
   	stateset->setTextureAttributeAndModes(0, DisplacementMap,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	stateset->addUniform( new osg::Uniform("oceanHeightmap",0) );

	 //Normal maps
	 osg::ref_ptr<osg::Texture2D> heightMap = new osg::Texture2D;
	 heightMap->setTextureSize(1024, 1024);
	 heightMap->setInternalFormat(GL_RGBA);
	 heightMap->setDataVariance(osg::Object::DYNAMIC);
	 heightMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);//NEAREST
	 heightMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	 heightMap->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	 heightMap->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT); // load an image by reading a file:
		   
 	 osg::Image *oceanHeightImage = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/ocean_01_h_n.jpg");//ocean_01_h_n ocean_01_n.jpg Test_n.jpg"
	 if (!oceanHeightImage) {
	    std::cout << " couldn't find Normal map texture , quiting." << std::endl;
	    exit(1);
	 }
   
		   
	 // Assign the texture to the image we read from file:
	 heightMap->setImage(oceanHeightImage);
	 stateset->setTextureAttributeAndModes(1, heightMap, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
  	 stateset->addUniform(new osg::Uniform("oceanNormalmap", 1));



 	//Normal map 2
	 osg::ref_ptr<osg::Texture2D> heightMap2 = new osg::Texture2D;
	 heightMap2->setTextureSize(1024, 1024);
	 heightMap2->setInternalFormat(GL_RGBA);
	 heightMap2->setDataVariance(osg::Object::DYNAMIC);
	 heightMap2->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);//NEAREST
	 heightMap2->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	 heightMap2->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	 heightMap2->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT); // load an image by reading a file:
		   
 	 osg::Image *oceanHeightImage2 = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/ocean_01_h_n.jpg");//ocean_01_h_n ocean_01_n.jpg Test_n.jpg"
	 if (!oceanHeightImage2) {
	    std::cout << " couldn't find Normal map 2 texture , quiting." << std::endl;
	    exit(1);
	 }
   
		   
	 // Assign the texture to the image we read from file:
	 heightMap2->setImage(oceanHeightImage2);
	 stateset->setTextureAttributeAndModes(2, heightMap2, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
  	 stateset->addUniform(new osg::Uniform("oceanNormalmap2", 2));


	 generateWaveData(stateset);	


	return oceanSphereGroup;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr<osg::Group> generateEarthSphere(osg::ref_ptr<osgViewer::Viewer> viewer)
{
 osg::ref_ptr<osg::Group> oceanSphereGroup = new osg::Group();

//Sphere with earth radius
    oceanSphereGroup->addChild(generateSphere(6371.0, 128, 128, 1.0));

	osg::ref_ptr<osg::Program> program = new osg::Program();
	
	program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("vertShader.vert")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSCONTROL, osgDB::findDataFile("tessellation.tesc")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::TESSEVALUATION, osgDB::findDataFile("tessellation.tese")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::GEOMETRY, osgDB::findDataFile("tessellation.geom")));
	//program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("data/shaders/tessellation.frag")));
	program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("fragShader.frag")));


	program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	program->setParameter(GL_PATCH_VERTICES,3);

	osg::ref_ptr<osg::StateSet> stateset = oceanSphereGroup->getOrCreateStateSet();
	 stateset->setAttributeAndModes( program.get() );

	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE ); 
	stateset->setAttribute(program.get());

	osg::ref_ptr<osg::Uniform> eyePositionUniform = new osg::Uniform("gEyeWorldPos", osg::Vec4(0.0,1.0,1.0,1.0) );
	eyePositionUniform->setUpdateCallback(new EyePositionCallback(viewer));
	stateset->addUniform( eyePositionUniform );
	
	
	osg::ref_ptr<osg::Uniform> timeUniform = new osg::Uniform("time", 0.0f ); 	
	timeUniform->setUpdateCallback(new TimeCallback());
	stateset->addUniform( timeUniform );


	//height map
   	osg::Texture2D* DisplacementMap = new osg::Texture2D;
   	DisplacementMap->setTextureSize(1024, 1024);
	DisplacementMap->setDataVariance(osg::Object::DYNAMIC); 
   	DisplacementMap->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	DisplacementMap->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	DisplacementMap->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
	DisplacementMap->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	DisplacementMap->setInternalFormat(GL_RGBA_FLOAT32_ATI); //or 
	DisplacementMap->setInternalFormat(GL_LUMINANCE_FLOAT32_ATI); 	
	osg::Image* displacementimage = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/ocean_01_h_h.jpg");//ocean_01_h_h.jpg ocean_01_h.jpg//test.jpeg Height0001.jpg Test_h.jpg"

   	if (!displacementimage)
   	{
      	std::cout << " couldn't find  displacementimage  texture, quiting." << std::endl;
      	exit(1);
   	}
        
   	DisplacementMap->setImage(displacementimage);
   	stateset->setTextureAttributeAndModes(0, DisplacementMap,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	stateset->addUniform( new osg::Uniform("oceanHeightmap",0) );

	 //Normal maps
	 osg::ref_ptr<osg::Texture2D> heightMap = new osg::Texture2D;
	 heightMap->setTextureSize(1024, 1024);
	 heightMap->setInternalFormat(GL_RGBA);
	 heightMap->setDataVariance(osg::Object::DYNAMIC);
	 heightMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);//NEAREST
	 heightMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	 heightMap->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	 heightMap->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT); // load an image by reading a file:
		   
 	 osg::Image *oceanHeightImage = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/ocean_01_h_n.jpg");//ocean_01_h_n ocean_01_n.jpg Test_n.jpg"
	 if (!oceanHeightImage) {
	    std::cout << " couldn't find Normal map texture , quiting." << std::endl;
	    exit(1);
	 }
   
		   
	 // Assign the texture to the image we read from file:
	 heightMap->setImage(oceanHeightImage);
	 stateset->setTextureAttributeAndModes(1, heightMap, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
  	 stateset->addUniform(new osg::Uniform("oceanNormalmap", 1));



 	//Normal map 2
	 osg::ref_ptr<osg::Texture2D> heightMap2 = new osg::Texture2D;
	 heightMap2->setTextureSize(1024, 1024);
	 heightMap2->setInternalFormat(GL_RGBA);
	 heightMap2->setDataVariance(osg::Object::DYNAMIC);
	 heightMap2->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);//NEAREST
	 heightMap2->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	 heightMap2->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	 heightMap2->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT); // load an image by reading a file:
		   
 	 osg::Image *oceanHeightImage2 = osgDB::readImageFile("/home/paul/workspace/projects/lighting/data/ocean_01_h_n.jpg");//ocean_01_h_n ocean_01_n.jpg Test_n.jpg"
	 if (!oceanHeightImage2) {
	    std::cout << " couldn't find Normal map 2 texture , quiting." << std::endl;
	    exit(1);
	 }
   
		   
	 // Assign the texture to the image we read from file:
	 heightMap2->setImage(oceanHeightImage2);
	 stateset->setTextureAttributeAndModes(2, heightMap2, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
  	 stateset->addUniform(new osg::Uniform("oceanNormalmap2", 2));


	return oceanSphereGroup;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::LOD> Base2(osg::ref_ptr<osgViewer::Viewer> viewer)
{
  
    //Sphere 
       osg::ref_ptr<osg::Group> earthSphereGroupLowDetail = new osg::Group();
       osg::ref_ptr<osg::Group> oceanSphereGroupHighDetail = new osg::Group();
       osg::ref_ptr<osg::Group> earthSphereGroupHighDetail = new osg::Group();
    
	float _radius = 6.0;

	     //oceanSphereGroup->addChild(generateQuad(64, 64, 2048));

	oceanSphereGroupHighDetail->addChild(generateOceanSphere(viewer));
	
	//earthSphereGroupHighDetail->addChild(generateEarthSphere(viewer));

	//earthSphereGroupLowDetail->addChild(createLowPolyEarth());


	//generate Earth LOD
	osg::ref_ptr<osg::LOD> earthLOD = new osg::LOD;
	//Tessellated Ocean sphere - High detail
	earthLOD->addChild(oceanSphereGroupHighDetail);
	//earthLOD->setRange(0,0,12000);
	earthLOD->setRange(0,0,912000);
	
	//Tessellated Land sphere - High detail
	earthLOD->addChild(earthSphereGroupHighDetail);
	earthLOD->setRange(1,0,12000);

	//low detail
  	earthLOD->addChild(earthSphereGroupLowDetail);
	earthLOD->setRange(2,12000,1000000);
  

       
	
       
    return earthLOD;


}


///////////////////////////////////////////////////////////////////////////////////
//  MAIN
///////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

    osg::ArgumentParser arguments( &argc, argv );
    osg::ref_ptr<osg::Group> group = new osg::Group;	

      //group->addChild(scene.get());
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
    viewer->setUpViewInWindow(300, 100, 640, 480);

      
    osg::ref_ptr<osg::Group> oceanSphereGroup = new osg::Group;	


    //Add main Group here
    oceanSphereGroup->addChild(Base2(viewer));
    //End 	



    //Add group to the SceneGraph
    group->addChild(oceanSphereGroup);
    //End
    
    


    //end setup

    osg::ref_ptr<osg::StateSet> stateset = oceanSphereGroup->getOrCreateStateSet();	
    //setup lighting
    osg::Vec4 LightPosition = osg::Vec4(0.0,100.0,7000.0,1.0);
    lightUniform = new osg::Uniform("lightPosition", LightPosition); 
    stateset->addUniform(lightUniform);


    //visualize light position
    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->addChild(startup_baseline());
    pat->setPosition(osg::Vec3f(LightPosition.x(),LightPosition.y(),LightPosition.z()));

    group->addChild(pat);
    //group->addChild();

    
   
	//load a model    ///home/paul/workspace/models/battleship.osg
 	osg::ref_ptr<osg::Group> unit = (osg::Group *)osgDB::readNodeFile("/home/paul/workspace/igx64/repositories/osgw2/data/dumptruck.osg");
	if (!unit) {
    	osg::notify(osg::NOTICE) << ":loadedModel No data loaded battleship" << std::endl;
    exit(1);
  	}
	
	osg::ref_ptr<osg::PositionAttitudeTransform> unitpat = new osg::PositionAttitudeTransform;
    unitpat->addChild(unit);
    unitpat->setPosition(osg::Vec3f(-50,-50,.20));

	group->addChild(unitpat);
	// "/home/paul/workspace/models/battleship.osg"
       
	
	
	
	
	
	viewer->setSceneData( group.get() );



 
	 
    osgGA::KeySwitchMatrixManipulator*  m_keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
    
    char keyForAnimationPath = '5';
    std::string pathfile;
    osgGA::AnimationPathManipulator* apm;
    
     while (arguments.read("-p",pathfile))
	{
		//osg::notify(osg::INFO)<<"PATH FOUND"<<std::endl;

		apm = new osgGA::AnimationPathManipulator(pathfile);
		if (apm || !apm->valid()) 
		{
	                //unsigned int num = m_keyswitchManipulator->getNumMatrixManipulators();
			m_keyswitchManipulator->addMatrixManipulator( keyForAnimationPath, "Path", apm );
			m_keyswitchManipulator->selectMatrixManipulator(keyForAnimationPath);
			++keyForAnimationPath;
			std::cout<<"PATH: "<<pathfile<<" Key: "<<keyForAnimationPath<<std::endl; 
		}
        }
    
    	 osgGA::NodeTrackerManipulator::TrackerMode trackerMode = osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION;
		 osgGA::NodeTrackerManipulator::RotationMode rotationMode = osgGA::NodeTrackerManipulator::TRACKBALL;

	
        m_keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        m_keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
		//m_keyswitchManipulator->addMatrixManipulator( '3', "WASD", pManipulator );
		//m_keyswitchManipulator->addMatrixManipulator( '3', "FP-WASD", new FirstPersonManipulator() );
		//m_keyswitchManipulator->addMatrixManipulator( '4', "Tracker", tm );

	 
	 
	 

    osgViewer::Viewer::Windows windows;
	viewer->getWindows(windows);

	for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
	    itr != windows.end();
	    ++itr)
	{
		    osg::State *s=(*itr)->getState();
		    s->setUseModelViewAndProjectionUniforms(true);
		    s->setUseVertexAttributeAliasing(true);
	}

	viewer->setCameraManipulator( m_keyswitchManipulator );
    //viewer->setCameraManipulator(new osgGA::TrackballManipulator());
    viewer->addEventHandler(new osgViewer::WindowSizeHandler);	
    viewer->addEventHandler(new osgViewer::StatsHandler);
    viewer->addEventHandler(new osgViewer::ThreadingHandler);
    viewer->addEventHandler(new osgViewer::HelpHandler);
    viewer->addEventHandler(new osgViewer::RecordCameraPathHandler);
    viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));


	//Solar system
	double zNear = 1.0, zMid = 1e4, zFar = 2e8;
    arguments.read( "--depth-partition", zNear, zMid, zFar );
    
    osg::ref_ptr<osgViewer::DepthPartitionSettings> dps = new osgViewer::DepthPartitionSettings;
    dps->_mode = osgViewer::DepthPartitionSettings::FIXED_RANGE;
    dps->_zNear = zNear;
    dps->_zMid = zMid;
    dps->_zFar = zFar;


	//viewer->setUpDepthPartition(dps.get());
	
    	
    return viewer->run();


}
