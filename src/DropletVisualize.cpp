#include "DropletVisualize.h"
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


DropletVisualize::DropletVisualize(size_t _num, size_t _maxAlive, int _numPerFrame, ngl::Vec3 _pos) :
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


void DropletVisualize::drawTrailPoints(const std::vector<ngl::Vec4> &_points) const
{
    if (!_points.empty() && m_showTrailPoints == false)
        return;
    float alpha = 0.1;
    // Position data (as is)
    std::vector<ngl::Vec4> pointData = _points;

    // Color data — use w (lifetime) as red channel
    std::vector<ngl::Vec4> colourData;
    colourData.reserve(_points.size());

    for (const auto &p : _points)
    {
        float red = p.m_w / 100.0f; // Using lifetime as colour
        red = std::clamp(red, 0.0f, 100.0f);
        colourData.emplace_back(red, 0.0f, 0.0f, alpha);
    }

    m_vao->bind();

    // Upload positions
    m_vao->setData(0, ngl::MultiBufferVAO::VertexData(pointData.size() * sizeof(ngl::Vec4),
                                                      pointData[0].m_x));
    m_vao->setVertexAttributePointer(0, 4, GL_FLOAT, 0, 0);

    // Upload colors
    m_vao->setData(1, ngl::MultiBufferVAO::VertexData(colourData.size() * sizeof(ngl::Vec4),
                                                      colourData[0].m_x));
    m_vao->setVertexAttributePointer(1, 4, GL_FLOAT, 0, 0);

    m_vao->setNumIndices(_points.size());

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(3.0f);
    m_vao->draw();
    glDisable(GL_PROGRAM_POINT_SIZE);
    m_vao->unbind();
}





