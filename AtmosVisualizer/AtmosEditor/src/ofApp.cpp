#include "ofApp.h"
#include "log/a3Log.h"
#include "log/a3Settings.h"

#define A3_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

string generateLabel(string name, string anonymous)
{
    string label = name;
    label += "##";
    label += anonymous;
    return label;
}

string getShapeTypeName(a3ShapeType type)
{
    switch(type)
    {
    case SPHERE:
        return "Sphere";
    case DISK:
        return "Disk";
    case PLANE:
        return "Plane";
    case TRIANGLE:
        return "Triangle";
    case INFINITE_PLANE:
        return "Infinite Plane";
    default:
        return "Unknown Shape Type";
    }
}

// 左手坐标系
void a3DrawAxis(float axisLength)
{
    // axis
    ofPushStyle();

    ofVec3f x(axisLength, 0, 0), y(0, -axisLength, 0), z(0, 0, axisLength);

    ofSetColor(ofColor::red);
    ofLine(ofPoint(), x);

    ofSetColor(ofColor::green);
    ofLine(ofPoint(), y);

    ofSetColor(ofColor::blue);
    ofLine(ofPoint(), z);

    ofPopStyle();

    // axis name
    ofDrawBitmapString("x", x);
    ofDrawBitmapString("y", y);
    ofDrawBitmapString("z", z);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableDepthTest();

    gui.setup();
    
    freeCamPreview = true;
    freeCam.lookAt(freeCam.getLookAtDir(), ofVec3f(0, -1, 0));

    activeCameraIndex = -1;

    // preview 
    ground = new Graph3D(650, 650, 30, 30);

    logo.loadImage("./logo.png");
    logoButtonID = gui.loadImage(logo);

    openCameraWindow = true;
    openShapeWindow = true;
    openViewWindow = true;
    openLightWindow = true;
    openModelWindow = true;
    openAboutWindow = false;
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(65, 65, 85);

    // -----------------------------------cam begin-----------------------------------
    getActiveCamera()->begin();

    // 左手坐标系
    float axisLength = 550;
    a3DrawAxis(axisLength);
    //ofDrawAxis(axisLength);

    ofPushStyle();
    ofSetColor(200, 200, 200, 150);
    ground->draw();
    ofPopStyle();

    // models
    for(auto m : modelList)
        m->draw();

    // camera
    for(auto c : cameraList)
        c->draw();

    for(auto s : shapeList)
        s->draw();
        
    // -----------------------------------cam end-----------------------------------
    getActiveCamera()->end();

    guiDraw();
}

//--------------------------------------------------------------
void ofApp::guiDraw()
{
    gui.begin();

    // -----------------------------------menu-----------------------------------
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open", NULL)) { /*--! in the future*/ }

            if(ImGui::MenuItem("Save", NULL)) { /*--! in the future*/ }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Render"))
        {
            if(ImGui::MenuItem("Render Local", NULL)) { /*--! in the future*/ }

            if(ImGui::MenuItem("Render IPC", NULL))
            {

            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Window"))
        {
            if(ImGui::MenuItem("Shape Window", NULL)) { openShapeWindow = !openShapeWindow; }

            if(ImGui::MenuItem("Light Window", NULL)) { openLightWindow = !openLightWindow; }

            if(ImGui::MenuItem("Model Window", NULL)) { openModelWindow = !openModelWindow; }

            if(ImGui::MenuItem("View Window", NULL)) { openViewWindow = !openViewWindow; }

            if(ImGui::MenuItem("Camera Window", NULL)) { openCameraWindow = !openCameraWindow; }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("About")) 
        { 
            openAboutWindow = !openAboutWindow;
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // -----------------------------------windows-----------------------------------
    modelWindow();
    viewWindow();
    cameraWindow();
    lightWindow();
    shapeWindow();

    aboutWindow();

    gui.end();
}

//--------------------------------------------------------------
void ofApp::modelWindow()
{
    if(openModelWindow)
    {
        ImGui::Begin("Models");

        bool continued = true;
        for(vector<a3ModelData*>::iterator it = modelList.begin(); it != modelList.end();)
        {
            if(ImGui::TreeNode((*it)->name.c_str()))
            {
                if(ImGui::Button("Delete"))
                {
                    a3ModelData* temp = *it;

                    it = modelList.erase(it);
                    continued = false;

                    A3_SAFE_DELETE(temp);
                }
                ImGui::TreePop();
            }

            if(continued) it++;
        }

        if(ImGui::Button(generateLabel("+", "Model").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 30)))
        {
            ofFileDialogResult result = ofSystemLoadDialog("Import Model");
            if(result.bSuccess)
            {
                ofxAssimpModelLoader* loader = new ofxAssimpModelLoader();
                if(loader->loadModel(result.filePath))
                {
                    bool find = false;
                    for(auto m : modelList)
                    {
                        if(result.fileName == m->name)
                        {
                            find = true;
                            break;
                        }
                    }

                    if(!find)
                    {
                        a3Log::success("Successfully loded model:%s\n", result.filePath.c_str());
                        a3ModelData* data = new a3ModelData(loader, result.filePath, result.fileName);
                        modelList.push_back(data);
                    }
                    else
                    {
                        a3Log::error("Model Named:%s exsited\n", result.filePath.c_str());
                        A3_SAFE_DELETE(loader);
                    }
                }
                else
                {
                    a3Log::error("Failed to load model:%s\n", result.filePath.c_str());
                    A3_SAFE_DELETE(loader);
                }
            }
        }

        ImGui::End();
    }
}

//--------------------------------------------------------------
void ofApp::shapeWindow()
{
    if(openShapeWindow)
    {
        ImGui::Begin("Shapes");

        for(vector<a3ShapeData*>::iterator it = shapeList.begin(); it != shapeList.end();)
        {
            bool continued = true;

            a3ShapeData* s = *it;
            if(ImGui::TreeNode(s->name.c_str()))
            {
                ImGui::TextWrapped(getShapeTypeName(s->type).c_str());

                switch(s->type)
                {
                case SPHERE:
                {
                    if(ImGui::DragFloat(generateLabel("Radius", s->name).c_str(), &s->radius))
                        s->sphere->setRadius(s->radius);
                    
                    if(ImGui::DragFloat3(generateLabel("Position", s->name).c_str(), &s->position[0]))
                        s->sphere->setPosition(s->position);

                    break;
                }
                case DISK:
                    break;
                case PLANE:
                    break;
                case TRIANGLE:
                    break;
                case INFINITE_PLANE:
                    break;
                }

                if(ImGui::Button(generateLabel("Delete", "Shape").c_str()))
                {
                    it = shapeList.erase(it);
                    continued = false;
                    A3_SAFE_DELETE(s);
                }

                ImGui::TreePop();
            }

            if(continued) it++;
        }

        if(ImGui::Button(generateLabel("+", "Shape").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 30)))
            ImGui::OpenPopup("Add Shape");

        if(ImGui::BeginPopupModal("Add Shape", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static int item = 0;
            ImGui::Combo(generateLabel("Type", "Shape").c_str(), &item, "Sphere\0Disk\0Plane\0Triangle\0InfinityPlane\0");

            if(ImGui::Button(generateLabel("Ok", "Shape").c_str(), ImVec2(120, 0))) 
            {
                a3ShapeData* data = new a3ShapeData();
                data->type = (a3ShapeType)item;
                data->name = "Shape" + ofToString(shapeList.size());
                switch(data->type)
                {
                case SPHERE:
                    data->sphere = new ofSpherePrimitive();
                    break;
                case DISK:
                    break;
                case PLANE:
                    data->plane = new ofPlanePrimitive();
                    break;
                case TRIANGLE:
                    break;
                case INFINITE_PLANE:
                    break;
                }

                shapeList.push_back(data);

                ImGui::CloseCurrentPopup(); 
            }

            ImGui::SameLine();
            if(ImGui::Button(generateLabel("Cancel", "Shape").c_str(), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::End();
    }
}

//--------------------------------------------------------------
void ofApp::lightWindow()
{
    if(openLightWindow)
    {
        ImGui::Begin("Lights");

        ImGui::End();
    }
}

//--------------------------------------------------------------
void ofApp::viewWindow()
{
    if(openViewWindow)
    {
        ImGui::Begin("Views");

        ImGui::TextWrapped("FPS:%.2f", ofGetFrameRate());

        if(ImGui::TreeNode("Preview Camera"))
        {
            if(freeCamPreview)
                ImGui::TextWrapped("Mode: Free Camera");
            else
                ImGui::TextWrapped("Mode: User Defined Camera");

            ofCamera* activeCamera = getActiveCamera();
            ofVec3f up = activeCamera->getUpDir();
            ofVec3f lookAt = activeCamera->getLookAtDir();
            ImGui::TextWrapped("Look At: (%.2f, %.2f, %.2f)", lookAt.x, lookAt.y, lookAt.z);
            ImGui::TextWrapped("Up: (%.2f, %2f, %.2f)", up.x, up.y, up.z);
            ImGui::TextWrapped("Aspect Ratio: %.2f", activeCamera->getAspectRatio());

            ImGui::TreePop();
        }

        ImGui::End();
    }
}

//--------------------------------------------------------------
void ofApp::cameraWindow()
{
    if(openCameraWindow)
    {
        ImGui::Begin("Cameras");

        if(ImGui::TreeNode("Free Camera"))
        {
            if(ImGui::Checkbox("Lock Mouse Input", &lockMouseInput))
            {
                if(lockMouseInput)
                    freeCam.disableMouseInput();
                else
                    freeCam.enableMouseInput();
            }

            ImGui::TreePop();
        }

        for(vector<a3CameraData*>::iterator it = cameraList.begin(); it != cameraList.end();)
        {
            bool continued = true;

            if(ImGui::TreeNode((*it)->name.c_str()))
            {
                //if(ImGui::Checkbox(generateLabel("Lock Mouse Input", it->name).c_str(), &it->lockMouseInput))
                //{
                //    if(it->lockMouseInput)
                //        it->camera->disableMouseInput();
                //    else
                //        it->camera->enableMouseInput();
                //}

                // active camera to preview
                if(ImGui::Checkbox(generateLabel("Active", (*it)->name).c_str(), &(*it)->active))
                {
                    // 被激活摄像机索引
                    activeCameraIndex = it - cameraList.begin();
                }

                if(ImGui::DragFloat("Width", &(*it)->dimension[0], 1.0f, 1.0f, 1920.0f))
                {
                    (*it)->camera->setAspectRatio((*it)->dimension[0] / (*it)->dimension[1]);
                }

                if(ImGui::DragFloat("Height", &(*it)->dimension[1], 1.0f, 1.0f, 1080.0f))
                {
                    (*it)->camera->setAspectRatio((*it)->dimension[0] / (*it)->dimension[1]);
                }

                // camera or(*it)ation
                //(*it)->lookAt = (*it)->camera->getLookAtDir();
                if(ImGui::DragFloat3(generateLabel("Look At", (*it)->name).c_str(), &(*it)->lookAt[0]))
                {
                    (*it)->camera->lookAt((*it)->lookAt, (*it)->up);
                }

                //(*it)->up = (*it)->camera->getUpDir();
                if(ImGui::DragFloat3(generateLabel("Up", (*it)->name).c_str(), &(*it)->up[0]))
                {
                    (*it)->camera->lookAt((*it)->lookAt, (*it)->up);
                }

                (*it)->origin = (*it)->camera->getPosition();
                if(ImGui::DragFloat3(generateLabel("Origin", (*it)->name).c_str(), &(*it)->origin[0]))
                {
                    (*it)->camera->setPosition((*it)->origin);
                }

                // fov
                (*it)->fov = (*it)->camera->getFov();
                if(ImGui::DragFloat(generateLabel("FOV", (*it)->name).c_str(), &(*it)->fov))
                {
                    (*it)->camera->setFov((*it)->fov);
                }

                // 视锥体距离
                ImGui::DragFloat(generateLabel("Distance", (*it)->name).c_str(), &(*it)->distance);

                if(ImGui::Button(generateLabel("Delete", "Camera").c_str()))
                {
                    a3CameraData* temp = *it;

                    it = cameraList.erase(it);
                    if(cameraList.size() <= 0)
                        activeCameraIndex = -1;
                    continued = false;

                    A3_SAFE_DELETE(temp);
                }

                ImGui::TreePop();
            }

            if(continued)
                it++;
        }

        ImGui::Separator();

        if(ImGui::Button("Change Camera To Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 30)))
        {
            if(cameraList.size() <= 0)
            {
                a3Log::warning("There is no user defined Camera\n");
            }
            else
            {
                if(activeCameraIndex == -1)
                    a3Log::warning("No user defined Camera actived\n");
                else
                {
                    // 另一相机后台停止事件更新
                    freeCamPreview = !freeCamPreview;
                }
            }
        }

        if(ImGui::Button(generateLabel("+", "Camera").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 30)))
            ImGui::OpenPopup("Add Camera");
        if(ImGui::BeginPopupModal("Add Camera", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char buffer[32];
            string temp = "Camera";
            
            temp += ofToString(cameraList.size());
            strcpy(buffer, temp.c_str());
            
            ImGui::InputText(generateLabel("Name", "Init Camera").c_str(), buffer, A3_ARRAYSIZE(buffer));

            if(ImGui::Button(generateLabel("OK", "Camera").c_str(), ImVec2(120, 0)))
            {
                bool find = false;
                for(auto c : cameraList)
                {
                    if(!strcmp(buffer, c->name.c_str()))
                    {
                        find = true;
                        break;
                    }
                }

                if(!find)
                {
                    ofCamera* camera = new ofCamera();
                    //ofEasyCam* camera = new ofEasyCam();
                    a3CameraData* data = new a3CameraData(camera, buffer);
                    cameraList.push_back(data);
                }
                else
                {
                    a3Log::warning("Camera named:%s already exsited\n", buffer);
                }

                ImGui::CloseCurrentPopup(); 
            }
            ImGui::SameLine();
            if(ImGui::Button(generateLabel("Cancel", "Camera").c_str(), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::End();
    }
}

//--------------------------------------------------------------
void ofApp::aboutWindow()
{
    if(!openAboutWindow) return;

    ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ofVec2f(400, 300), ImGuiSetCond_FirstUseEver);
    if(ImGui::Begin("About", &openAboutWindow))
    {
        ImGui::ImageButton((ImTextureID) (uintptr_t) logoButtonID, ImVec2(200, 200));
        ImGui::SameLine();
        ImGui::Text("AtmosEditor is developed by GBB in 2017.");
        ImGui::SameLine();
        ImGui::Text("And I guess no one would notice this passage.");
    }

    ImGui::End();
}

//--------------------------------------------------------------
ofCamera* ofApp::getActiveCamera()
{
    if(freeCamPreview)
        return &freeCam;
    else
    {
        if(activeCameraIndex >= 0 && activeCameraIndex < cameraList.size())
            return cameraList[activeCameraIndex]->camera;
        else
            return &freeCam;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

a3CameraData::a3CameraData(ofCamera * cam, string name)
    :camera(cam), name(name)
{
    active = false;
    fov = 40.0f;

    distance = 100;

    origin.set(0, 0, 0);
    up.set(0, 1, 0);
    lookAt.set(0, 0, -1);

    dimension.set(1920, 1080);

    cam->setAspectRatio(dimension[0] / dimension[1]);
    cam->setPosition(origin);
    cam->lookAt(lookAt, up);
    cam->setFov(fov);
}

a3CameraData::~a3CameraData()
{
    A3_SAFE_DELETE(camera);
}

void a3CameraData::draw()
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

a3ModelData::~a3ModelData()
{
    A3_SAFE_DELETE(model);
}

void a3ModelData::draw()
{
    ofPushStyle();
    model->drawWireframe();
    ofPopStyle();
}

a3ShapeData::a3ShapeData()
{ 
    type = SPHERE; 

    radius = 10.0f;

    position.set(0, 0, 0);
    normal.set(0, 1, 0);

    name = "Shape";

    sphere = NULL;
    plane = NULL;
}

a3ShapeData::~a3ShapeData()
{
    A3_SAFE_DELETE(sphere);
    A3_SAFE_DELETE(plane);
}

void a3ShapeData::draw()
{
    //ofPushStyle();
    switch(type)
    {
    case SPHERE:
        //ofSetColor(180, 180, 50);
        sphere->drawWireframe();
        break;
    case DISK:
        break;
    case PLANE:
        plane->drawWireframe();
        break;
    case TRIANGLE:
        break;
    case INFINITE_PLANE:
        break;
    default:
        a3Log::warning("Shape Type not allowed\n");
        break;
    }
    //ofPopStyle();
}
