#ifndef EMITTER_H_
#define EMITTER_H_
#include <vector>
#include <string_view>
// #include "Particle.h"
#include <ngl/Vec3.h>
#include <ngl/Vec4.h>
#include <ngl/MultiBufferVAO.h>
#include <memory>
#include <QObject>

class DropletVisualize : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int m_numPerFrame WRITE setNumPerFrame);


public :
        DropletVisualize(size_t _num,size_t _maxAlive, int _numPerFrame,ngl::Vec3 _pos);
    size_t size() const;
    void update(float _dt);
    void render() const;
    void move(float _dx, float _dy, float _dz);
    void setSpread(float _value);
    void drawTrailPoints(const std::vector<ngl::Vec4> &_points) const;
    void setShowTrailPoints(bool _show) { m_showTrailPoints = _show; }
    bool isShowingTrailPoints() const { return m_showTrailPoints; }

public slots:
    void setNumPerFrame(int _value){m_numPerFrame=_value;}

private :
        ngl::Vec3 m_pos;
    std::vector<ngl::Vec4> m_ppos;
    std::vector<ngl::Vec3> m_pdir;
    std::vector<ngl::Vec3> m_pcolour;
    std::vector<float> m_psize;
    std::vector<int> m_plife;
    enum class ParticleState : bool {Active,Dead};
    std::vector<ParticleState> m_state;
    size_t m_maxParticles;
    size_t m_maxAlive = 1000;
    int m_numPerFrame = 120;
    void resetParticle(size_t _i);
    void birthParticles();
    ngl::Vec3 randomVectorOnSphere(float _radius = 1.0f);
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    float m_spread = 1.0;
    bool m_showTrailPoints = true;
};
#endif
