#include "a3TypeData.h"
#include "log/a3Log.h"

a3EditorCameraData::a3EditorCameraData(ofCamera * cam, string name)
    :camera(cam), name(name)
{
    active = false;
    fov = 40.0f;

    distance = 100;

    origin.set(0, 0, 0);
    up.set(0, -1, 0);
    lookAt.set(0, 0, 1);

    dimension.set(768, 768);

    cam->setAspectRatio(dimension[0] / dimension[1]);
    cam->setPosition(origin);
    cam->lookAt(lookAt, up);
    cam->setFov(fov);
}

a3EditorCameraData::~a3EditorCameraData()
{
    A3_SAFE_DELETE(camera);
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

a3EditorModelData::~a3EditorModelData()
{
    A3_SAFE_DELETE(model);
}

void a3EditorModelData::draw()
{
    ofPushStyle();
    model->drawWireframe();
    ofPopStyle();
}

a3EditorShapeData::a3EditorShapeData()
{
    type = A3_SHAPE_SPHERE;

    radius = 10.0f;

    position.set(0, 0, 0);
    normal.set(0, 1, 0);

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

a3EditorLightData::a3EditorLightData()
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

    direction.set(0, 1, 0);

    //updatePosition();
}

a3EditorLightData::~a3EditorLightData()
{
}

void a3EditorLightData::updatePosition()
{
    //if(type == A3_LIGHT_SPOT)
    //    cone->setPosition(position + direction * coneHeight * 0.5f);

    node->setPosition(position);
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
        ofDrawArrow(position, position + coneHeight * direction.getNormalized(), 5);
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

string getShapeTypeName(a3ShapeType type)
{
    switch(type)
    {
    case A3_SHAPE_SPHERE:
        return "Sphere";
    case A3_SHAPE_DISK:
        return "Disk";
    case A3_SHAPE_PLANE:
        return "Plane";
    case A3_SHAPE_TRIANGLE:
        return "Triangle";
    case A3_SHAPE_INFINITE_PLANE:
        return "Infinite Plane";
    default:
        return "Unknown Shape Type";
    }
}

string getLightTypeName(a3LightType type)
{
    switch(type)
    {
    case A3_LIGHT_POINT:
        return "Point Light";
    case A3_LIGHT_AREA:
        return "Area Light";
    case A3_LIGHT_INFINITE_AREA:
        return "Infinite Light";
    case A3_LIGHT_SPOT:
        return "Spot Light";
    default:
        return "Unknow Light Type";
    }
}