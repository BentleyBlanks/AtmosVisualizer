#include "a3TypeData.h"
#include "log/a3Log.h"

a3EditorCameraData::a3EditorCameraData(ofCamera * cam, string name)
    :camera(cam), name(name)
{
    active = false;
    fov = 40.0f;

    distance = 100;

    a3Float3Set(origin, 0, 0, 0);
    a3Float3Set(up, 0, -1, 0);
    a3Float3Set(lookat, 0, 0, 1);

    dimension.set(768, 768);

    cam->setAspectRatio(dimension[0] / dimension[1]);
    cam->setPosition(a3Float3ToVec3(origin));
    cam->lookAt(a3Float3ToVec3(lookat), a3Float3ToVec3(up));
    cam->setFov(fov);
}

a3EditorCameraData::~a3EditorCameraData()
{
    //A3_SAFE_DELETE(camera);
}

void a3EditorCameraData::draw()
{
    ofPushStyle();

    // 本体
    camera->transformGL();

    ofPushMatrix();
    if(active)
        ofSetColor(20, 210, 20);
    else
        ofSetColor(255);

    ofScale(1, 1, 1);
    ofNode().draw();
    ofPopMatrix();

    camera->restoreTransformGL();

    // 视锥体中心点
    ofVec3f p = camera->getPosition() + distance * camera->getLookAtDir();

    // 宽高
    float aspectRatio = dimension[0] / dimension[1];
    float t0 = distance * tanf(ofDegToRad(camera->getFov() * aspectRatio / 2.0f));
    float t1 = t0 / aspectRatio;

    // 上右
    ofVec3f a0 = camera->getUpDir() * t1,
        a1 = camera->getSideDir() * t0;

    ofVec3f p0 = p + a1 + a0,
        p1 = p + a1 - a0,
        p2 = p - a1 - a0,
        p3 = p - a1 + a0;

    ofVec3f o = camera->getPosition();

    ofSetColor(220);
    ofLine(o, p0);
    ofLine(o, p1);
    ofLine(o, p2);
    ofLine(o, p3);

    ofLine(p0, p1);
    ofLine(p1, p2);
    ofLine(p2, p3);
    ofLine(p3, p0);

    ofPopStyle();
}

a3EditorModelData::a3EditorModelData(ofxAssimpModelLoader * loader, string modelPath, string name)
    :model(loader), name(name), materialIndex(-1)
{
    if(modelPath.length() <= A3_ADDRESS_PATH_LENGTH)
        strcpy(path, modelPath.c_str());
}

a3EditorModelData::~a3EditorModelData()
{
    //A3_SAFE_DELETE(model);
}

void a3EditorModelData::draw()
{
    ofPushStyle();
    model->drawWireframe();
    ofPopStyle();
}

a3EditorShapeData::a3EditorShapeData() : materialIndex(-1)
{
    type = A3_SHAPE_SPHERE;

    radius = 10.0f;

    a3Float3Set(position, 0, 0, 0);
    a3Float3Set(normal, 0, 1, 0);

    name = "Shape";

    sphere = NULL;
    plane = NULL;
}

a3EditorShapeData::~a3EditorShapeData()
{
    A3_SAFE_DELETE(sphere);
    A3_SAFE_DELETE(plane);
}

void a3EditorShapeData::draw()
{
    //ofPushStyle();
    switch(type)
    {
    case A3_SHAPE_SPHERE:
        //ofSetColor(180, 180, 50);
        sphere->drawWireframe();
        break;
    case A3_SHAPE_DISK:
        break;
    case A3_SHAPE_PLANE:
        plane->drawWireframe();
        break;
    case A3_SHAPE_TRIANGLE:
        break;
    case A3_SHAPE_INFINITE_PLANE:
        break;
    default:
        a3Log::warning("Shape Type not allowed\n");
        break;
    }
    //ofPopStyle();
}

a3EditorLightData::a3EditorLightData() : textureIndex(-1)
{
    type = A3_LIGHT_POINT;

    //cone = new ofConePrimitive();
    //cone->setResolutionHeight(1); 
    //cone->setResolutionRadius(20);
    //cone->setResolutionCap(0);
    node = new ofNode();

    coneAngle = ofDegToRad(40);
    falloffStart = ofDegToRad(5);

    coneHeight = 100;

    direction[0] = 0;
    direction[1] = 1;
    direction[2] = 0;

    //updatePosition();
}

a3EditorLightData::~a3EditorLightData()
{
}

void a3EditorLightData::updatePosition()
{
    //if(type == A3_LIGHT_SPOT)
    //    cone->setPosition(position + direction * coneHeight * 0.5f);

    node->setPosition(a3Float3ToVec3(position));
}

//void a3LightData::updateCone()
//{
//    //if(type == A3_LIGHT_SPOT)
//    //{
//    //    cone->set(tanf(coneAngle) * coneHeight, coneHeight);
//    //    updatePosition();
//    //    cone->lookAt(direction);
//    //}
//}

void a3EditorLightData::draw()
{
    switch(type)
    {
    case A3_LIGHT_POINT:
    case A3_LIGHT_AREA:
    {
        drawNode();
        break;
    }
    case A3_LIGHT_INFINITE_AREA:
        break;
    case A3_LIGHT_SPOT:
    {
        drawNode();

        ofPushStyle();
        ofSetColor(ofColor::green);
        ofDrawArrow(a3Float3ToVec3(position), 
                    a3Float3ToVec3(position) + coneHeight * a3Float3ToVec3(direction).getNormalized(), 5);
        ofPopStyle();
        //cone->drawWireframe();
        break;
    }
    default:
        break;
    }
}

void a3EditorLightData::drawNode()
{
    node->transformGL();
    node->draw();
    node->restoreTransformGL();
}

a3EditorTextureData::a3EditorTextureData()
{
}

a3EditorMaterialData::a3EditorMaterialData() : textureIndex(-1)
{
}

ofVec3f a3Float3ToVec3(float * f3)
{
    // 不做安全检查
    return ofVec3f(f3[0], f3[1], f3[2]);
}

void a3Float3Set(float * f3, float t1, float t2, float t3)
{
    f3[0] = t1;
    f3[1] = t2;
    f3[2] = t3;
}

void a3Float3Set(float * f3, ofVec3f v)
{
    f3[0] = v.x;
    f3[1] = v.y;
    f3[2] = v.z;
}