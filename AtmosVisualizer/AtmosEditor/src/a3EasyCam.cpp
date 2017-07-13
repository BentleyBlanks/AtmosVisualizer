#include "a3EasyCam.h"
#include "ofMath.h"
#include "ofUtils.h"

// when an a3EasyCam is moving due to momentum, this keeps it
// from moving forever by assuming small values are zero.
static const float minDifference = 0.1e-5f;

// this is the default on windows os
static const unsigned long doubleclickTime = 200;

//----------------------------------------
a3EasyCam::a3EasyCam()
{
    lastTap = 0;
    lastDistance = 0;
    // drag the axis after mouse is released
    drag = 0.0f;
    sensitivityRot = 1.0f;//when 1 moving the mouse from one side to the other of the arcball (min(viewport.width, viewport.height)) will rotate 180degrees. when .5, 90 degrees.
    sensitivityXY = .5f;
    sensitivityZ = .7f;

    mouseTriggerRect.set(0, 0, 500, 500);

    bDistanceSet = false;
    bMouseInputEnabled = true;
    bDoRotate = false;
    bApplyInertia = false;
    bDoTranslate = false;
    bDoScrollZoom = false;
    bInsideArcball = true;
    bEnableMouseMiddleButton = true;
    bAutoDistance = true;
    doTranslationKey = 'm';
    bEventsSet = false;
    events = nullptr;
    rotationFactor = 0.0f;

    reset();
}

//----------------------------------------
a3EasyCam::~a3EasyCam()
{
    disableMouseInput();
}

//----------------------------------------
void a3EasyCam::update(ofEventArgs & args)
{
    viewport = getViewport(this->viewport);
    if(!bDistanceSet && bAutoDistance)
    {
        setDistance(getImagePlaneDistance(viewport), true);
    }
    if(bMouseInputEnabled)
    {

        rotationFactor = sensitivityRot * 180 / min(viewport.width, viewport.height);

        if(events->getMousePressed()) prevMouse = ofVec2f(events->getMouseX(), events->getMouseY());

        if(bDoRotate)
        {
            updateRotation();
        }
        else if(bDoTranslate || bDoScrollZoom)
        {
            updateTranslation();
            bDoScrollZoom = false;
        }
    }
}

//----------------------------------------
void a3EasyCam::begin(ofRectangle _viewport)
{
    if(!bEventsSet)
    {
        setEvents(ofEvents());
    }
    viewport = getViewport(_viewport);
    ofCamera::begin(viewport);
}

//----------------------------------------
void a3EasyCam::reset()
{
    target.resetTransform();

    target.setPosition(0, 0, 0);
    lookAt(target);

    resetTransform();
    setPosition(0, 0, lastDistance);

    xRot = 0;
    yRot = 0;
    zRot = 0;

    moveX = 0;
    moveY = 0;
    moveZ = 0;

    bApplyInertia = false;
    bDoTranslate = false;
    bDoRotate = false;
}

//----------------------------------------
void a3EasyCam::setTarget(const ofVec3f& targetPoint)
{
    target.setPosition(targetPoint);
    lookAt(target);
}

//----------------------------------------
void a3EasyCam::setTarget(ofNode& targetNode)
{
    target = targetNode;
    lookAt(target);
}

//----------------------------------------
ofNode& a3EasyCam::getTarget()
{
    return target;
}

//----------------------------------------
void a3EasyCam::setDistance(float distance)
{
    setDistance(distance, true);
}

//----------------------------------------
void a3EasyCam::setDistance(float distance, bool save)
{//should this be the distance from the camera to the target?
    if(distance > 0.0f)
    {
        if(save)
        {
            this->lastDistance = distance;
        }
        setPosition(target.getPosition() + (distance * getZAxis()));
        bDistanceSet = true;
    }
}

//----------------------------------------
float a3EasyCam::getDistance() const
{
    return target.getPosition().distance(getPosition());
}

//----------------------------------------
void a3EasyCam::setAutoDistance(bool bAutoDistance)
{
    this->bAutoDistance = bAutoDistance;
    if(bAutoDistance)
    {
        bDistanceSet = false;
    }
}

//----------------------------------------
void a3EasyCam::setDrag(float drag)
{
    this->drag = drag;
}

//----------------------------------------
float a3EasyCam::getDrag() const
{
    return drag;
}

//----------------------------------------
void a3EasyCam::setTranslationKey(char key)
{
    doTranslationKey = key;
}

//----------------------------------------
char a3EasyCam::getTranslationKey()
{
    return doTranslationKey;
}

//----------------------------------------
void a3EasyCam::setMouseTriggerRegion(ofRectangle mouseTriggerRegion)
{
    mouseTriggerRect = mouseTriggerRegion;
}

//----------------------------------------
ofRectangle a3EasyCam::getMouseTriggerRegion()
{
    return mouseTriggerRect;
}

//----------------------------------------
void a3EasyCam::enableMouseInput()
{
    if(!bMouseInputEnabled && events)
    {
        ofAddListener(events->update, this, &a3EasyCam::update);
        ofAddListener(events->mouseDragged, this, &a3EasyCam::mouseDragged);
        ofAddListener(events->mousePressed, this, &a3EasyCam::mousePressed);
        ofAddListener(events->mouseReleased, this, &a3EasyCam::mouseReleased);
        ofAddListener(events->mouseScrolled, this, &a3EasyCam::mouseScrolled);
    }
    // if enableMouseInput was called within ofApp::setup()
    // `events` will still carry a null pointer, and bad things
    // will happen. Therefore we only update the flag. 
    bMouseInputEnabled = true;
    // setEvents() is called upon first load, and will make sure 
    // to enable the mouse input once the camera is fully loaded.
}

//----------------------------------------
void a3EasyCam::disableMouseInput()
{
    if(bMouseInputEnabled && events)
    {
        ofRemoveListener(events->update, this, &a3EasyCam::update);
        ofRemoveListener(events->mouseDragged, this, &a3EasyCam::mouseDragged);
        ofRemoveListener(events->mousePressed, this, &a3EasyCam::mousePressed);
        ofRemoveListener(events->mouseReleased, this, &a3EasyCam::mouseReleased);
        ofRemoveListener(events->mouseScrolled, this, &a3EasyCam::mouseScrolled);
    }
    // if disableMouseInput was called within ofApp::setup()
    // `events` will still carry a null pointer, and bad things
    // will happen. Therefore we only update the flag. 
    bMouseInputEnabled = false;
    // setEvents() is called upon first load, and will make sure 
    // to enable the mouse input once the camera is fully loaded.
}

//----------------------------------------
void a3EasyCam::setEvents(ofCoreEvents & _events)
{
    // If en/disableMouseInput were called within ofApp::setup(),
    // bMouseInputEnabled will tell us about whether the camera
    // mouse input needs to be initialised as enabled or disabled.
    // we will still set `events`, so that subsequent enabling
    // and disabling can work.

    // we need a temporary copy of bMouseInputEnabled, since it will 
    // get changed by disableMouseInput as a side-effect.
    bool wasMouseInputEnabled = bMouseInputEnabled;
    disableMouseInput();
    events = &_events;
    if(wasMouseInputEnabled)
    {
        // note: this will set bMouseInputEnabled to true as a side-effect.
        enableMouseInput();
    }
    bEventsSet = true;
}

//----------------------------------------
bool a3EasyCam::getMouseInputEnabled()
{
    return bMouseInputEnabled;
}

//----------------------------------------
void a3EasyCam::enableMouseMiddleButton()
{
    bEnableMouseMiddleButton = true;
}

//----------------------------------------
void a3EasyCam::disableMouseMiddleButton()
{
    bEnableMouseMiddleButton = false;
}

//----------------------------------------
bool a3EasyCam::getMouseMiddleButtonEnabled()
{
    return bEnableMouseMiddleButton;
}

void a3EasyCam::lookAt(const ofVec3f & lookAtPosition)
{
    auto relPosition = (lookAtPosition - getGlobalPosition());
    auto radius = relPosition.length();
    if(radius>0)
    {
        auto latitude = ofRadToDeg(acos(relPosition.y / radius)) - 90;
        auto longitude = ofRadToDeg(atan2(relPosition.x, relPosition.z));
        ofQuaternion q(latitude, ofVec3f(1, 0, 0), longitude, ofVec3f(0, 1, 0), 0, ofVec3f(0, 0, 1));
        setGlobalOrientation(q);
    }
}

void a3EasyCam::lookAt(const ofVec3f & lookAtPosition, ofVec3f upVector)
{
    if(parent) upVector = upVector * ofMatrix4x4::getInverseOf(parent->getGlobalTransformMatrix());
    ofVec3f zaxis = (lookAtPosition - getGlobalPosition()).getNormalized();
    if(zaxis.length() > 0)
    {
        ofVec3f xaxis = upVector.getCrossed(zaxis).getNormalized();
        ofVec3f yaxis = zaxis.getCrossed(xaxis);

        ofMatrix4x4 m;
        m._mat[0].set(xaxis.x, xaxis.y, xaxis.z, 0);
        m._mat[1].set(yaxis.x, yaxis.y, yaxis.z, 0);
        m._mat[2].set(zaxis.x, zaxis.y, zaxis.z, 0);

        setGlobalOrientation(m.getRotate());
    }
}

void a3EasyCam::lookAt(const ofNode & lookAtNode)
{
    lookAt(lookAtNode.getGlobalPosition());
}

void a3EasyCam::lookAt(const ofNode & lookAtNode, const ofVec3f & upVector)
{
    lookAt(lookAtNode.getGlobalPosition(), upVector);
}

//----------------------------------------
void a3EasyCam::updateTranslation()
{
    if(bApplyInertia)
    {
        moveX *= drag;
        moveY *= drag;
        moveZ *= drag;
        if(ABS(moveX) <= minDifference && ABS(moveY) <= minDifference && ABS(moveZ) <= minDifference)
        {
            bApplyInertia = false;
            bDoTranslate = false;
        }
        move((getXAxis() * moveX) + (getYAxis() * moveY) + (getZAxis() * moveZ));
    }
    else
    {
        setPosition(prevPosition + ofVec3f(prevAxisX * moveX) + (prevAxisY * moveY) + (prevAxisZ * moveZ));
    }
}

//----------------------------------------
void a3EasyCam::updateRotation()
{
    if(bApplyInertia)
    {
        xRot *= drag;
        yRot *= drag;
        zRot *= drag;

        if(ABS(xRot) <= minDifference && ABS(yRot) <= minDifference && ABS(zRot) <= minDifference)
        {
            bApplyInertia = false;
            bDoRotate = false;
        }
        curRot = ofQuaternion(xRot, getXAxis(), yRot, getYAxis(), zRot, getZAxis());
        setPosition((getGlobalPosition() - target.getGlobalPosition())*curRot + target.getGlobalPosition());
        rotate(curRot);
    }
    else
    {
        curRot = ofQuaternion(xRot, prevAxisX, yRot, prevAxisY, zRot, prevAxisZ);
        setPosition((prevPosition - target.getGlobalPosition())*curRot + target.getGlobalPosition());
        setOrientation(prevOrientation * curRot);
    }
}

void a3EasyCam::mousePressed(ofMouseEventArgs & mouse)
{
    ofRectangle viewport = getViewport(this->viewport);
    //if(viewport.inside(mouse.x, mouse.y))
    if(mouseTriggerRect.inside(mouse.x, mouse.y))
    {
        lastMouse = mouse;
        prevMouse = mouse;
        prevAxisX = getXAxis();
        prevAxisY = getYAxis();
        prevAxisZ = getZAxis();
        prevPosition = ofCamera::getGlobalPosition();
        prevOrientation = ofCamera::getGlobalOrientation();

        if((bEnableMouseMiddleButton && mouse.button == OF_MOUSE_BUTTON_MIDDLE) || events->getKeyPressed(doTranslationKey) || mouse.button == OF_MOUSE_BUTTON_RIGHT)
        {
            bDoTranslate = true;
            bDoRotate = false;
        }
        else if(mouse.button == OF_MOUSE_BUTTON_LEFT)
        {
            bDoTranslate = false;
            bDoRotate = true;
            if(ofVec2f(mouse.x - viewport.x - (viewport.width / 2), mouse.y - viewport.y - (viewport.height / 2)).length() < min(viewport.width / 2, viewport.height / 2))
            {
                bInsideArcball = true;
            }
            else
            {
                bInsideArcball = false;
            }
        }
        bApplyInertia = false;
    }
}

void a3EasyCam::mouseReleased(ofMouseEventArgs & mouse)
{
    unsigned long curTap = ofGetElapsedTimeMillis();
    ofRectangle viewport = getViewport(this->viewport);

    // disable double click reset
    //if(lastTap != 0 && curTap - lastTap < doubleclickTime)
    //{
    //    reset();
    //    return;
    //}
    //lastTap = curTap;
    bApplyInertia = true;
    mouseVel = mouse - prevMouse;

    updateMouse(mouse);
    ofVec2f center(viewport.width / 2, viewport.height / 2);
    int vFlip;
    if(isVFlipped())
    {
        vFlip = -1;
    }
    else
    {
        vFlip = 1;
    }
    zRot = -vFlip * ofVec2f(mouse.x - viewport.x - center.x, mouse.y - viewport.y - center.y).angle(prevMouse - ofVec2f(viewport.x, viewport.y) - center);
}

void a3EasyCam::mouseDragged(ofMouseEventArgs & mouse)
{
    mouseVel = mouse - lastMouse;

    updateMouse(mouse);
}

void a3EasyCam::mouseScrolled(ofMouseEventArgs & mouse)
{
    ofRectangle viewport = getViewport(this->viewport);
    prevPosition = ofCamera::getGlobalPosition();
    prevAxisZ = getZAxis();
    moveZ = mouse.scrollY * 30 * sensitivityZ * (getDistance() + FLT_EPSILON) / viewport.height;
    bDoScrollZoom = true;
}

void a3EasyCam::updateMouse(const ofMouseEventArgs & mouse)
{
    ofRectangle viewport = getViewport(this->viewport);
    int vFlip;
    if(isVFlipped())
    {
        vFlip = -1;
    }
    else
    {
        vFlip = 1;
    }
    if(bDoTranslate)
    {
        moveX = 0;
        moveY = 0;
        moveZ = 0;
        if(mouse.button == OF_MOUSE_BUTTON_RIGHT)
        {
            moveZ = mouseVel.y * sensitivityZ * (getDistance() + FLT_EPSILON) / viewport.height;
        }
        else
        {
            moveX = -mouseVel.x * sensitivityXY * (getDistance() + FLT_EPSILON) / viewport.width;
            moveY = vFlip * mouseVel.y * sensitivityXY * (getDistance() + FLT_EPSILON) / viewport.height;
        }
    }
    else
    {
        xRot = 0;
        yRot = 0;
        zRot = 0;
        if(bInsideArcball)
        {
            xRot = vFlip * -mouseVel.y * rotationFactor;
            yRot = -mouseVel.x * rotationFactor;
        }
        else
        {
            ofVec2f center(viewport.width / 2, viewport.height / 2);
            zRot = -vFlip * ofVec2f(mouse.x - viewport.x - center.x, mouse.y - viewport.y - center.y).angle(lastMouse - ofVec2f(viewport.x, viewport.y) - center);
        }
    }
}
