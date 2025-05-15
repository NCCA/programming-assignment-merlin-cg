#include "Emitter.h"
#include <iostream>
#include <ngl/Random.h>
#include <algorithm>
#include <fstream>
#include <ngl/VAOPrimitives.h>
#include <ngl/Mat4.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/Util.h>
#include <ngl/VAOFactory.h>
#include <algorithm>


Emitter::Emitter(size_t _num, size_t _maxAlive, int _numPerFrame, ngl::Vec3 _pos) :
m_maxParticles{_num}, m_maxAlive{_maxAlive}, m_numPerFrame{_numPerFrame},m_pos{_pos}
{


    m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
      ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_POINTS)
      );
    m_vao->bind();
    m_vao->setData(ngl::MultiBufferVAO::VertexData(0,0)); // index 0 points
    m_vao->setData(ngl::MultiBufferVAO::VertexData(0,0)); // index 1 colours
    m_vao->unbind();
}

size_t Emitter::size() const
{
  return m_maxParticles;
}

void Emitter::birthParticles()
{

}

void Emitter::update(float _dt)
{

    birthParticles();


}

void Emitter::drawTrailPoints(const std::vector<ngl::Vec4> &_points) const
{
    if (_points.empty())
        return;

    // Position data (as is)
    std::vector<ngl::Vec4> pointData = _points;

    // Color data — use w (lifetime) as red channel
    std::vector<ngl::Vec3> colourData;
    colourData.reserve(_points.size());

    for (const auto &p : _points)
    {
        float red = p.m_w / 100.0f; // Using lifetime as colour
        red = std::clamp(red, 0.0f, 100.0f);
        colourData.emplace_back(red, 0.0f, 0.0f);
    }

    m_vao->bind();

    // Upload positions
    m_vao->setData(0, ngl::MultiBufferVAO::VertexData(pointData.size() * sizeof(ngl::Vec4),
                                                      pointData[0].m_x));
    m_vao->setVertexAttributePointer(0, 4, GL_FLOAT, 0, 0);

    // Upload colors
    m_vao->setData(1, ngl::MultiBufferVAO::VertexData(colourData.size() * sizeof(ngl::Vec3),
                                                      colourData[0].m_x));
    m_vao->setVertexAttributePointer(1, 3, GL_FLOAT, 0, 0);

    m_vao->setNumIndices(_points.size());

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(1.0f);
    m_vao->draw();
    glDisable(GL_PROGRAM_POINT_SIZE);
    m_vao->unbind();
}

void Emitter::resetParticle(size_t _i)
{

}


ngl::Vec3 Emitter::randomVectorOnSphere(float _radius)
{
  auto phi = ngl::Random::randomPositiveNumber(M_PI * 2.0f);
  auto costheta = ngl::Random::randomNumber();
  auto u = ngl::Random::randomPositiveNumber();
  auto theta = std::acos(costheta);
  auto r = _radius * std::cbrt(u);
  return ngl::Vec3(r * std::sin(theta) * std::cos(phi),
                   r * std::sin(theta) * std::sin(phi),
                   r * std::cos(theta));
}

void Emitter::render() const
{
  m_vao->bind();
  m_vao->setData(0,ngl::MultiBufferVAO::VertexData(
          m_ppos.size() * sizeof(ngl::Vec4),
          m_ppos[0].m_x
    ));
  m_vao->setVertexAttributePointer(0,4,GL_FLOAT,0,0);

  m_vao->setData(1,ngl::MultiBufferVAO::VertexData(
    m_pcolour.size() * sizeof(ngl::Vec3),
    m_pcolour[0].m_x
  ));

  m_vao->setVertexAttributePointer(1,3,GL_FLOAT,0,0);

  m_vao->setNumIndices(m_maxParticles);
  glEnable(GL_PROGRAM_POINT_SIZE);
  m_vao->draw();
  glDisable(GL_PROGRAM_POINT_SIZE);
  m_vao->unbind();

}

void Emitter::move(float _dx, float _dy, float _dz)
{
  m_pos.m_x +=_dx;
  m_pos.m_y +=_dy;
  m_pos.m_z +=_dz;
}

void Emitter::setSpread(float _value)
{
    m_spread = _value;
}
