#include "Shape.h"
//#include "Triangle.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm>
#include <functional>
#include <vector>
#include "a3Log.h"

Triangle triangle;

bool loadModel(std::vector<Triangle*>& triangles, const aiScene* scene)
{
    if(!scene)
    {
        a3Log::error("找不到场景aiScene\n");
        return false;
    }

    if(scene->mNumMeshes < 0)
    {
        a3Log::error("场景网格数量过低: %d\n", scene->mNumMeshes);
        return false;
    }

    for(int m = 0; m < scene->mNumMeshes; m++)
    {
        const struct aiMesh* mesh = scene->mMeshes[m];

        for(int i = 0; i < mesh->mNumFaces; i++)
        {
            int numIndices = mesh->mFaces[i].mNumIndices;
            if(numIndices != 3)
            {
                a3Log::error("图元顶点个数不支持: %d\n", numIndices);
                return false;
            }

            Triangle* triangle = new Triangle();

            int indexV0 = mesh->mFaces[i].mIndices[0], indexV1 = mesh->mFaces[i].mIndices[1], indexV2 = mesh->mFaces[i].mIndices[2];

            // init vertices
            triangle->setV1(mesh->mVertices[indexV0].x, mesh->mVertices[indexV0].y, mesh->mVertices[indexV0].z);
            triangle->setV2(mesh->mVertices[indexV1].x, mesh->mVertices[indexV1].y, mesh->mVertices[indexV1].z);
            triangle->setV3(mesh->mVertices[indexV2].x, mesh->mVertices[indexV2].y, mesh->mVertices[indexV2].z);

            triangle->setCenter();

            triangles.push_back(triangle);
        }
    }

    return true;
}

bool sortPoints(const ofVec3f & m1, const ofVec3f & m2)
{
    ofVec3f xDir(-1, 0, 0);
    ofVec3f cross1 = xDir.crossed(m1.getNormalized());
    ofVec3f cross2 = xDir.crossed(m2.getNormalized());

    if(cross1.z >= 0)
    {
        if(cross2.z < 0)
        {
            return true;
        }
        else if(cross2.z > 0)
        {
            float cos1 = m1.getNormalized().dot(xDir);
            float cos2 = m2.getNormalized().dot(xDir);

            if(cos1 > cos2)
                return true;
            else
                return false;
        }
        else
            return false;
    }
    else
    {
        if(cross2.z < 0)
        {
            float cos1 = m1.getNormalized().dot(xDir);
            float cos2 = m2.getNormalized().dot(xDir);

            if(cos1 >= cos2)
                return false;
            else
                return true;
        }
        else if(cross2.z > 0)
            return false;
        else
            return true;
    }

    //if(cross1.z > 0 && cross2.z < 0)
    //{
    //    return true;
    //}
    //else if(cross2.z > 0 && cross1.z < 0)
    //{
    //    return false;
    //}
    //else if(cross1.z > 0 && cross2.z > 0)
    //{
    //    float cos1 = m1.getNormalized().dot(xDir);
    //    float cos2 = m2.getNormalized().dot(xDir);

    //    if(cos1 > cos2)
    //        return true;
    //    else
    //        return false;
    //}
    //else
    //{
    //    float cos1 = m1.getNormalized().dot(xDir);
    //    float cos2 = m2.getNormalized().dot(xDir);

    //    if(cos1 > cos2)
    //        return false;
    //    else
    //        return true;
    //}
}

Shape::Shape()
{
    triangle.set(ofVec3f(10, 0, 0), ofVec3f(200, 500, 0), ofVec3f(-500, 0, 0), ofVec3f(200, -500, 0));
}

Shape::~Shape()
{
    for(auto t : triangles)
    {
        delete t;
        t = NULL;
    }

    triangles.clear();
}

bool Shape::load(const char * path)
{
    // models
    if(model.loadModel(path))
    {
        model.setPosition(0, 0, 0);

        // triangles
        return loadModel(triangles, model.getAssimpScene());
    }
    else
    {
        a3Log::error("找不到模型%s\n", path);
        return false;
    }
}

std::vector<ofVec3f>* Shape::getPointsWithZ(float z)
{
    //points.clear();
    std::vector<ofVec3f>* points = new std::vector<ofVec3f>();

    TopologicalStructure t;
    for(int i = 0; i < triangles.size(); i++)
    {
        triangle.setZ(z);

        t = judge_triangle_topologicalStructure(&triangle, triangles[i]);
        if(t == INTERSECT)
        {
            ofVec3f pointProject(triangles[i]->center[0], triangles[i]->center[1], z);

            points->push_back(pointProject);
        }
    }

    if(!points->empty())
    {
        // 根据点与x轴角度排序
        sort(points->begin(), points->end(), sortPoints);
    }

    return points;
}

ofPolyline* Shape::getPolylineWithZ(float z)
{
    ofPolyline* polyline = new ofPolyline();

    std::vector<ofVec3f>* points = getPointsWithZ(z);

    if(!points->empty())
    {
        // 根据点与x轴角度排序
        sort(points->begin(), points->end(), sortPoints);

        polyline->addVertices(*points);
    }

    return polyline;
}

void Shape::draw()
{
    ofPushStyle();
    ofNoFill();

    for(auto t : triangles)
    {
        t->draw();
    }

    ofPopStyle();
}
