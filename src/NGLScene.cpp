#include <QMouseEvent>
#include <QGuiApplication>
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/Util.h>
#include <iostream>
#include <QApplication>
#include <QCoreApplication> // For QCoreApplication::processEvents()
#include <ngl/VAOFactory.h>

NGLScene::NGLScene(QWidget *_parent) :QOpenGLWidget(_parent)
{
    setFocusPolicy(Qt::StrongFocus); // Allow the widget to receive focus by tabbing and clicking
    setFocus(); // Give focus to the widget immediately
}

NGLScene::~NGLScene()
{
    //std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}


void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project=ngl::perspective(45.0f, float(m_win.width)/float(m_win.height), 0.001f,10000.0f);
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  ngl::VAOPrimitives::createSphere("sphere",1.0f,20);
  ngl::VAOPrimitives::createLineGrid("floor",100,100,50);
  m_emitter=std::make_unique<Emitter>(10000,10000,800,ngl::Vec3(0,0,0));

  m_plane = std::make_unique<Plane>(300, 300, 1.0f);

  ngl::ShaderLib::loadShader("HeightColourShader","shaders/HeightColourVertex.glsl","shaders/HeightColourFragment.glsl");
    ngl::ShaderLib::loadShader("ColourShader","shaders/ColourVertex.glsl","shaders/ColourFragment.glsl");


  m_view = ngl::lookAt({150.0f, 100.0f, 450.0f}, {150.0f, 0.0f, 150.0f}, {0.0f, 1.0f, 0.0f});
  m_previousTime=std::chrono::steady_clock::now();

   //m_text = std::make_unique<ngl::Text>("fonts/DejaVuSansMono.ttf",16);
  // m_text->setScreenSize(width(),height());
  // m_text->setColour(1,1,1);
  startTimer(10);
  emit glInitialized();
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  glViewport(0,0,m_win.width,m_win.height);
  auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);
  auto mouseRotation = rotX * rotY;
  mouseRotation.m_m[3][0]=m_modelPos.m_x;
  mouseRotation.m_m[3][1]=m_modelPos.m_y;
  mouseRotation.m_m[3][2]=m_modelPos.m_z;
  ngl::ShaderLib::use("HeightColourShader");
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);
  ngl::ShaderLib::setUniform("maxTerrainHeight", m_plane->getTerrainHeight());

 // Wireframe switch
  glPolygonMode(GL_FRONT_AND_BACK, m_wireframeMode ? GL_LINE : GL_FILL);
  m_plane->render();

  ngl::ShaderLib::use("ColourShader");
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);

  m_emitter->drawTrailPoints(m_plane->getDropletTrailPoints());

  //doneCurrent();

  //m_plane->render();


  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);
  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
 // ngl::VAOPrimitives::draw("floor");

  //ngl::ShaderLib::use(ngl::nglTextShader);
  // m_text->renderText(10,700,"Particle System");


}

//rest of NGLScene such as keypress events, process keys, timerEvent, updateTerrainFrequency, updateTerrainOctaves

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
  m_keysPressed -=(Qt::Key)_event->key();
    // switch (_event->key())
  // {
  // case Qt::Key_L :

  //     if (m_plane)
  //     {
  //         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  //         m_plane->render();
  //     }
  // }
}

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    //std::cout << "NGLScene::keyPressEvent called. Key: " << _event->key() << std::endl; // <-- ADD THIS LINE

    // ... rest of your keyPressEvent logic ...
    switch (_event->key())
    {
        // ... (your other existing key cases like Key_W, Key_S, etc.) ...

   case Qt::Key_E :
        if (m_plane)
        {

            const int totalDroplets = 40000;
            const int dropletsPerUpdate = 1000;
            const int iterations = totalDroplets / dropletsPerUpdate; // 40 iterations

            for (int i = 0; i < iterations; i++)
            {
                // Apply erosion for just dropletsPerUpdate droplets
                m_plane->applyHydraulicErosion(dropletsPerUpdate, 30);
                makeCurrent();
                m_plane->refreshGPUAssets();

                update();
                QApplication::processEvents();

            }
            doneCurrent(); // Release context for the FINAL GPU update

        }
        break;

          case Qt::Key_W :

        if (m_plane)
        {
                  m_wireframeMode = !m_wireframeMode;
                  update();
                  break;
        }
          case Qt::Key_V:
              m_emitter->setShowTrailPoints(!m_emitter->isShowingTrailPoints());
              update();
              break;
    default :
        break;
    }
}

void NGLScene::process_keys()
{
  ////std::cout<<"Set Size " << m_keysPressed.size()<<"\n";
  float dx=0.0f;
  float dy=0.0f;
  float dz=0.0f;
  const float inc=0.2f;
  for(auto key : m_keysPressed)
  {
    switch(key)
    {
      case Qt::Key_Left : dx -= inc; break;
      case Qt::Key_Right : dx +=inc; break;
      case Qt::Key_Up : dz += inc; break;
      case Qt::Key_Down : dz -=inc; break;
    }
  }
m_emitter->move(dx,dy,dz);
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
  auto now = std::chrono::steady_clock::now();
  auto delta = std::chrono::duration<float,std::chrono::seconds::period>(now-m_previousTime);
  m_previousTime=now;
  ////std::cout<<"time delta" << delta.count()<<'\n';
  if(m_animate)
  {
    process_keys();
    // m_emitter->update(delta.count());
  }
 // update();
}

void NGLScene::setSpread(double _value)
{
    m_emitter->setSpread(static_cast<float>(_value));
        //update();
}


void NGLScene::updateGridDepth(int depth)
{
    if (m_plane) {
        m_plane->setDepth(depth);
        makeCurrent();
        m_plane->regenerate();
        doneCurrent();

        update(); // Tell the NGL widget to repaint itself
    }
}

void NGLScene::updateGridWidth(int width)
{
    if (m_plane) {
        m_plane->setWidth(width);
        makeCurrent();
        m_plane->regenerate();
        doneCurrent();

        update();
    }
}
void NGLScene::updateTerrainFrequency(float freq)
{
    if (m_plane) {
        m_plane->setNoiseFrequency(freq);
        makeCurrent();
        m_plane->regenerate();
        doneCurrent();
        update();

    }
}
void NGLScene::updateTerrainOctaves(int octaves)
{
    if (m_plane) {
        m_plane->setNoiseOctaves(octaves);
        makeCurrent();
        m_plane->regenerate();
        doneCurrent();

        update();

    }
}
void NGLScene::updateTerrainHeight(int height)
{
    if (m_plane) {
        m_plane->setTerrainHeight(height);
        makeCurrent();

        ngl::ShaderLib::setUniform("maxTerrainHeight", m_plane->getTerrainHeight());
        m_plane->regenerate();
        doneCurrent();

        update();

    }
}

void NGLScene::callErosionEvent(int maxDroplets, int lifetime)
{
    if (m_plane)
    {
        std::cout << "Erosion droplets " << maxDroplets << std::endl;
        std::cout << "Droplet Lifetime " << lifetime << std::endl;


        makeCurrent();
        m_plane->applyHydraulicErosion(maxDroplets, lifetime);
        makeCurrent();
        m_plane->refreshGPUAssets();
    }
}
