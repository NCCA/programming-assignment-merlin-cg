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
  m_ppos.resize(m_maxParticles);
  m_pcolour.resize(m_maxParticles);
  m_pdir.resize(m_maxParticles);
  m_plife.resize(m_maxParticles);
  m_psize.resize(m_maxParticles);
  m_state.resize(m_maxParticles);
  for(size_t i=0; i<m_maxParticles; ++i)
  {
    resetParticle(i);
  }

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
  auto births = static_cast<int>(ngl::Random::randomPositiveNumber(m_numPerFrame));

  for(size_t i=0; i<births; ++i)
  {
    for(size_t p=0; p<m_maxParticles; ++p)
    {
      if(m_state[p] == ParticleState::Dead)
      {
        resetParticle(p);
        m_state[p] = ParticleState::Active;
        break;
      }
    }
 }
}

void Emitter::update(float _dt)
{
  const ngl::Vec3 gravity(0.0f,-9.81f,0.0f);

  auto numAlive = std::count_if(std::begin(m_state),std::end(m_state),
                                [](auto p){ return p == ParticleState::Active;});

  if(numAlive < m_maxAlive)
  {
    birthParticles();
  }
// #pragma omp parallel for
 for(size_t i=0; i<m_maxParticles; ++i)
 {
    if(m_state[i] == ParticleState::Dead)
      continue;
    m_pdir[i] +=gravity * _dt * -0.1;
    m_ppos[i] += m_pdir[i];
    m_psize[i]+=0.1f;
    m_psize[i] = std::clamp(m_psize[i],0.0f, 5.0f);
    m_ppos[i].m_w=m_psize[i];
    if(--m_plife[i] <=0 || m_ppos[i].m_y <=0.0f)
    {
      resetParticle(i);
    }
  }


}



void Emitter::resetParticle(size_t _i)
{
  ngl::Vec3 emitDir(0.0f,0.1f,0.0f);
  m_ppos[_i].set(m_pos.m_x,m_pos.m_y,m_pos.m_z,0.0f);
  m_pdir[_i] = emitDir * ngl::Random::randomPositiveNumber()+randomVectorOnSphere() * m_spread;
  m_pdir[_i].m_y=std::abs(m_pdir[_i].m_y);
  m_psize[_i]=0.005f;
  m_plife[_i] = 20 + static_cast<int>(ngl::Random::randomPositiveNumber(100));
  m_pcolour[_i]=ngl::Random::getRandomColour3();
  m_state[_i]= ParticleState::Dead;
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
