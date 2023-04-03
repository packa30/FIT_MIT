/**
 * @file    tree_mesh_builder.h
 *
 * @author  FULL NAME <xgrofc00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    DATE
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include "base_mesh_builder.h"
#include <omp.h>

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }
    unsigned underdog(unsigned gridSize, Vec3_t<float> cubeOffset, const ParametricScalarField &field);

    std::vector<Triangle_t> mTriangles; ///< Temporary array of triangles
    std::vector<Triangle_t> *mTriangles_thread;
};

#endif // TREE_MESH_BUILDER_H
