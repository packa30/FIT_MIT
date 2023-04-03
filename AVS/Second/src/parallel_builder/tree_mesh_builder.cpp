                    /**
 * @file    tree_mesh_builder.cpp
 *
 * @author  FULL NAME <xgrofc00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    DATE
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"

#define SQRT sqrt(3)/2

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{
  TreeMeshBuilder:mTriangles_thread = new std::vector<Triangle_t>[omp_get_max_threads()];
}

unsigned TreeMeshBuilder::underdog(unsigned gridSize, Vec3_t<float> cubeOffset, const ParametricScalarField &field){
  unsigned totalTriangles = 0;
  unsigned gridSize2=gridSize>>1;
  unsigned gridSize4=gridSize2>>1;
  float misolvl = mIsoLevel+(SQRT*gridSize)*mGridResolution;
  if(gridSize > 1 ){
    for (size_t i = 0; i < 8; i++) {
      // if (gridSize==mGridSize && i >3) {
      //   break;
      // }
      Vec3_t<float> cubeOffsetTODO(cubeOffset.x+(sc_vertexNormPos[i].x*gridSize2),
                                   cubeOffset.y+(sc_vertexNormPos[i].y*gridSize2),
                                   cubeOffset.z+(sc_vertexNormPos[i].z*gridSize2));
      /*one point in space as middle point for sphere*/
      Vec3_t<float> cubeOffsetcenterTODO(((cubeOffsetTODO.x+gridSize4)*mGridResolution),
                                         ((cubeOffsetTODO.y+gridSize4)*mGridResolution),
                                         ((cubeOffsetTODO.z+gridSize4)*mGridResolution));
      unsigned cubeIndex = unsigned(evaluateFieldAt(cubeOffsetcenterTODO, field) < misolvl);

      /*middle cube corner points as 8 middle points for 8 spheres
        -> also correct, but a bit slower cause of 16 || 8 iterations on every recursion lvl*/
      // Vec3_t<float> cubeOffsetcenter((cubeOffsetTODO.x+gridSize4),
      //                                (cubeOffsetTODO.y+gridSize4),
      //                                (cubeOffsetTODO.z+gridSize4));
      // CubeCornerVerts_t cubeCorners;
      // transformCubeVertices(cubeOffsetcenter, sc_vertexNormPos, cubeCorners);
      //
      // unsigned cubeIndex = 0;
      // std::array<float, 8> cubeCornerValues;
      // for(unsigned i = 0; i < 8; ++i)
      // {
      //     cubeCornerValues[i] = evaluateFieldAt(cubeCorners[i], field);
      //     cubeIndex |= unsigned(cubeCornerValues[i] < mIsoLevel+(SQRT*(gridSize/2))*mGridResolution) << i;
      //     // std::cout << cubeCornerValues[i] << '\t' << misolvl << '\n';
      // }
      if (cubeIndex == 0) {
      }else{
        #pragma omp task shared(totalTriangles) if(gridSize2 > 2)
        totalTriangles += underdog(gridSize2, cubeOffsetTODO, field);
      }
    }
  }else{
    // totalTriangles += 1;
    totalTriangles += buildCube(cubeOffset, field);
  }
  #pragma omp taskwait
  return totalTriangles;
}


unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    unsigned totalTriangles = 0;
    Vec3_t<float> cubeOffset(0,0,0);
    #pragma omp parallel
    {
      #pragma omp single
      totalTriangles = underdog(mGridSize, cubeOffset, field);
    }
    for (size_t i = 0; i < omp_get_max_threads(); i++) {
      mTriangles.insert(mTriangles.end(),mTriangles_thread[i].begin(), mTriangles_thread[i].end());
    }
    // std::cout << mGridSize*mGridSize*mGridSize << '\n';
    // std::cout << totalTriangles << '\n';
    return totalTriangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
  // NOTE: This method is called from "buildCube(...)"!

  // 1. Store pointer to and number of 3D points in the field
  //    (to avoid "data()" and "size()" call in the loop).
  const Vec3_t<float> *pPoints = field.getPoints().data();
  const unsigned count = unsigned(field.getPoints().size());

  float value = std::numeric_limits<float>::max();

  // 2. Find minimum square distance from points "pos" to any point in the
  //    field.
  for(unsigned i = 0; i < count; ++i)
  {
      float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
      distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
      distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

      // Comparing squares instead of real distance to avoid unnecessary
      // "sqrt"s in the loop.
      value = std::min(value, distanceSquared);
  }

  // 3. Finally take square root of the minimal square distance to get the real distance
  return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    TreeMeshBuilder:mTriangles_thread[omp_get_thread_num()].push_back(triangle);
    // mTriangles.push_back(triangle);
}
