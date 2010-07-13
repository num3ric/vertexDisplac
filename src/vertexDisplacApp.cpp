#include "cinder/app/AppBasic.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ArcBall.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Vector.h"
#include "cinder/Rect.h"
#include "cinder/Utilities.h"

#define SIDE 1000
#define PLANEWIDTH 1600.0f

using namespace ci;
using namespace ci::app;
using namespace std;

class vertexDisplacApp : public AppBasic {
public:
	void	prepareSettings( Settings *settings );
	void	setup();
	void	resize( int width, int height );
	void    mouseDown( MouseEvent event );
	void    mouseDrag( MouseEvent event );
	void	draw();

	CameraPersp		mCam;
	Arcball			mArcball;
	gl::Fbo			mFBO;
	
	gl::Texture		mTexture;
	gl::VboMesh		mVboMesh;
	gl::GlslProg	mShader;
};

void vertexDisplacApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1000, 800 );
}

void vertexDisplacApp::setup()
{	
	mArcball.setQuat( Quatf( Vec3f( 0.0577576f, -0.956794f, 0.284971f ), 3.68f ) );
	
	//====== Shader ======
	try {
		mShader = gl::GlslProg( loadResource( "vDispl.vert" ), loadResource( "vDispl.frag" ));
	}
	catch( ci::gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
	
	//====== Texture ======
	mTexture = gl::Texture( loadImage( loadResource( "nebula.jpg" ) ) );
	mTexture.setWrap( GL_REPEAT, GL_REPEAT );
	mTexture.setMinFilter( GL_NEAREST );
	mTexture.setMagFilter( GL_NEAREST );
	
	//======  FBO ======
	gl::Fbo::Format format;
	//format.enableDepthBuffer(false);
	format.enableColorBuffer(true);
	format.setColorInternalFormat( GL_RGBA32F_ARB );
	mFBO = gl::Fbo( SIDE, SIDE, format );
	
	mFBO.bindFramebuffer();
	
	gl::setMatricesWindow( mFBO.getSize(), false );
	gl::setViewport( mFBO.getBounds() );
	
	mTexture.enableAndBind();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	gl::drawSolidRect( mFBO.getBounds() );
	mTexture.unbind();
	mFBO.unbindFramebuffer();
	//console()<<"Fbo target"<< mFBO.getTarget()<<std::endl;
	//console()<<"Texture target"<< mTexture.getTarget()<<std::endl;
	console()<<SIDE*SIDE<<" vertices!!!"<<std::endl;
	
	//====== VboMesh the same size as the texture ======
	
	/* THE VBO HAS TO BE DRAWN AFTER FBO! I almost lost all hope
	 on this, but after having googled "vbo fbo conflict",
	 I stumbled upon this thread
	 http://www.openframeworks.cc/forum/viewtopic.php?f=9&t=2443
	 which led me to put the code in current order and voilà!
	*/
	float half = SIDE/2.0f;
	int totalVertices = SIDE * SIDE;
	vector<Vec2f> texCoords;
	vector<Vec3f> vCoords, normCoords;
	vector<uint32_t> indices;
	gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticNormals();
	mVboMesh = gl::VboMesh( totalVertices, totalVertices, layout, GL_POINTS); // GL_LINES GL_QUADS
	for( int x = 0; x < SIDE; ++x ) {
		for( int y = 0; y < SIDE; ++y ) {	
			indices.push_back( x * SIDE + y );
			texCoords.push_back( Vec2f( x/(float)SIDE, y/(float)SIDE ) );
			vCoords.push_back( PLANEWIDTH*Vec3f( (x-half)/(float)SIDE, 0, (y-half)/(float)SIDE ) );
			normCoords.push_back( Vec3f( 0.0f, 1.0f, 0.0f ));
		}
	}
	mVboMesh.bufferIndices( indices );
	mVboMesh.bufferTexCoords2d( 0, texCoords );
	mVboMesh.bufferPositions( vCoords );
	mVboMesh.bufferNormals( normCoords );
	
	gl::enableDepthRead();
	glColor3f(1.0f, 1.0f, 1.0f);
}

void vertexDisplacApp::resize( int width, int height )
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( getWindowHeight() / 2.0f );
	
	mCam.lookAt( Vec3f( 0.0f, 0.0f, -450.0f ), Vec3f::zero() );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 2000.0f );
	gl::setMatrices( mCam );
}

void vertexDisplacApp::mouseDown( MouseEvent event )
{
    mArcball.mouseDown( event.getPos() );
}

void vertexDisplacApp::mouseDrag( MouseEvent event )
{
    mArcball.mouseDrag( event.getPos() );
}


void vertexDisplacApp::draw()
{
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	glEnable( mFBO.getTarget() );
	mFBO.bindTexture();
	mShader.bind();
	mShader.uniform("colorMap", 0 );
	mShader.uniform("displacementMap", 0 );
	
	gl::pushModelView();
		gl::translate( Vec3f( 0.0f, 0.0f, getWindowHeight() / 2.0f ) );
		gl::rotate( mArcball.getQuat() );
		gl::draw( mVboMesh );
    gl::popModelView();
	
	mShader.unbind();
	mFBO.unbindTexture();
//	gl::drawString( toString( SIDE*SIDE ) + " vertices!", Vec2f(32.0f, 32.0f));
//	gl::drawString( toString((int) getAverageFps()) + " fps", Vec2f(32.0f, 52.0f));
}


CINDER_APP_BASIC( vertexDisplacApp, RendererGl )