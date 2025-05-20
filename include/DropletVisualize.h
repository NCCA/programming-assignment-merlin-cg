
#ifndef DROPLET_VISUALIZE_H_
#define DROPLET_VISUALIZE_H_
#include <vector>
#include <string_view>
// #include "Particle.h"
#include <ngl/Vec3.h>
#include <ngl/Vec4.h>
#include <ngl/MultiBufferVAO.h>
#include <memory>
#include <QObject>

/**
* Provides visualization for water droplet movement
 * Renders trail points to visualize the path and behavior
 * of water droplets during the hydraulic erosion simulation.
 */


class DropletVisualize : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int m_numPerFrame WRITE setNumPerFrame);


public :
    /**
 * Constructor for droplet visualization
 * @param _num Maximum number of particles (only used for VAO initialization)
 * @param _maxAlive Maximum number of active particles (unused in current implementation)
 * @param _numPerFrame Number of particles to emit per frame (unused in current implementation)
 * @param _pos Initial position (unused in current implementation)
 */
    DropletVisualize(size_t _num,size_t _maxAlive, int _numPerFrame,ngl::Vec3 _pos);

    
    /**
     * Draw trail points for droplet visualization
     * @param _points Vector of points to visualize (x,y,z,lifetime)
     */
    void drawTrailPoints(const std::vector<ngl::Vec4> &_points) const;

    /**
     * Enable or disable trail point visualization
     * @param _show Whether to show trail points
     */
    void setShowTrailPoints(bool _show) { m_showTrailPoints = _show; }

    /**
     * Check if trail points are currently being shown
     * @return True if trail points are visible
     */
    bool isShowingTrailPoints() const { return m_showTrailPoints; }

public slots:
    void setNumPerFrame(int _value){m_numPerFrame=_value;}

private :
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    bool m_showTrailPoints = true;

    // From old emitter code, might use later
    size_t m_maxParticles;
    int m_numPerFrame = 120;
    ngl::Vec3 m_pos;
    size_t m_maxAlive = 1000;
};
#endif
