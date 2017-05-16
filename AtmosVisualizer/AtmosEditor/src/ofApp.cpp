#include "ofApp.h"
#include "log/a3Log.h"
#include "log/a3Settings.h"
#include "messageQueue/a3Message.h"
#include "messageQueue/a3MessageQueueIPC.h"
#include "a3EditorUtils.h"
#include <ShellAPI.h>

const int msgMaxNum = 100;
const int S2CMsgSize = sizeof(a3S2CInitMessage);
const int C2SMsgSize = sizeof(a3C2SGridBufferMessage);

#define A3_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static int styleID = 0;
#define IMGUI_STYLE_BEGIN(style)    static int tempStyleID = styleID++;\
                                    ImGui::PushID(tempStyleID);\
                                    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(style / 7.0f, 0.6f, 0.6f));\
                                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(style / 7.0f, 0.7f, 0.7f));\
                                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(style / 7.0f, 0.8f, 0.8f));

#define IMGUI_STYLE_END()           ImGui::PopStyleColor(3); \
                                    ImGui::PopID();

#define IMGUI_BUTTON_DELETE_BEGIN(name)     {IMGUI_STYLE_BEGIN(0);\
                                            if(ImGui::Button(generateLabel("Delete", name).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0))){

#define IMGUI_BUTTON_DELETE_END()           }IMGUI_STYLE_END();}

#define IMGUI_POPUP_SELECT_TEXTURE(domainName, destIndex)           if(destIndex != -1)\
                                                                        ImGui::TextWrapped("Texture Name: %s", textureList[destIndex]->name.c_str());\
                                                                    if(ImGui::Button(generateLabel("Choose Texture", domainName).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))\
                                                                        ImGui::OpenPopup("Select Texture In Library");\
                                                                    if(ImGui::BeginPopupModal("Select Texture In Library", NULL, ImGuiWindowFlags_AlwaysAutoResize)){\
                                                                        static int selectedIndex = -1; \
                                                                        ImGui::ListBoxHeader(generateLabel("Select Texture", domainName).c_str());\
                                                                        for(int i = 0; i < textureList.size(); i++){\
                                                                            if(ImGui::Selectable(textureList[i]->name.c_str(), selectedIndex == i ? true : false)){\
                                                                               destIndex = i;\
                                                                               selectedIndex = i; \
                                                                            }\
                                                                        }\
                                                                        ImGui::ListBoxFooter();\
                                                                        if(ImGui::Button(generateLabel("OK", "Select Texture").c_str(), ImVec2(120, 0))) { \
                                                                            if(selectedIndex != -1) ImGui::CloseCurrentPopup(); }\
                                                                        ImGui::SameLine();\
                                                                        if(ImGui::Button(generateLabel("Cancel", "Select Texture").c_str(), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }\
                                                                        ImGui::EndPopup();\
                                                                    }

#define IMGUI_POPUP_SELECT_MATERIAL(domainName, destIndex)          if(destIndex != -1)\
                                                                        ImGui::TextWrapped("Material Name: %s", materialList[destIndex]->name.c_str());\
                                                                    if(ImGui::Button(generateLabel("Choose Material", domainName).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))\
                                                                        ImGui::OpenPopup("Select Material In Library");\
                                                                    if(ImGui::BeginPopupModal("Select Material In Library", NULL, ImGuiWindowFlags_AlwaysAutoResize)){\
                                                                        static int selectedIndex = -1; \
                                                                        ImGui::ListBoxHeader(generateLabel("Select Material", domainName).c_str());\
                                                                        for(int i = 0; i < materialList.size(); i++){\
                                                                            if(ImGui::Selectable(materialList[i]->name.c_str(), selectedIndex == i ? true : false)){\
                                                                               destIndex = i;\
                                                                               selectedIndex = i; \
                                                                            }\
                                                                        }\
                                                                        ImGui::ListBoxFooter();\
                                                                        if(ImGui::Button(generateLabel("OK", "Select Material").c_str(), ImVec2(120, 0))) { \
                                                                            if(selectedIndex != -1) ImGui::CloseCurrentPopup(); }\
                                                                        ImGui::SameLine();\
                                                                        if(ImGui::Button(generateLabel("Cancel", "Select Material").c_str(), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }\
                                                                        ImGui::EndPopup();\
                                                                    }

static wstring atmosDebugEXEPath = L"C:\\Bingo\\Program\\Renderer\\Atmos\\build\\Atmos.vs2015\\Atmos\\x64\\Debug\\AtmosTestd.x64.exe";

static wstring atmosReleaseEXEPath = L"C:\\Bingo\\Program\\Renderer\\Atmos\\build\\Atmos.vs2015\\Atmos\\x64\\Release\\AtmosTest.x64.exe";

string getWindowToggleName(string windowName, bool windowOpened)
{
    string name;

    if(windowOpened)
        name = "Close ";
    else
        name = "Open";

    name += windowName;
    name += " Window";

    return name;
}

string generateLabel(string name, string anonymous)
{
    string label = name;
    label += "##";
    label += anonymous;
    return label;
}

string getPreviewModeName(a3PreviewType type)
{
    switch(type)
    {
    case A3_PREVIEW_REALTIME:
        return "RealTime Preview";
    case A3_PREVIEW_LOCAL:
        return "Local Preview";
    case A3_PREVIEW_IPC:
        return "IPC Preview";
    default:
        return "Unknown Type";
    }
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableDepthTest();
    ofSetFrameRate(60);

    gui.setup();

    freeCamPreview = true;
    freeCam.lookAt(ofVec3f(0, 0, 1), ofVec3f(0, -1, 0));

    activeCameraIndex = -1;

    // preview 
    ground = new Graph3D(650, 650, 30, 30);

    ipcS2C.init(L"Who's Your Daddy S2C", true, msgMaxNum / 2, S2CMsgSize);
    ipcC2S.init(L"Who's Your Daddy C2S", true, msgMaxNum, C2SMsgSize);
    fullScreenIPCPreview = false;

    // window
    openCameraWindow = true;
    openShapeWindow = true;
    openLightWindow = true;
    openModelWindow = true;
    openRendererWindow = true;
    openMaterialWindow = true;
    openTextureWindow = true;
    //openViewWindow = true;
    //openAboutWindow = false;

    // about window
    ofDisableArbTex();
    logo.loadImage("./logo.png");
    ofEnableArbTex();
    //logoButtonID = gui.loadImage(logo);
    logoButtonID = logo.getTexture().getTextureData().textureID;

    // renderer
    spp = 16;
    maxDepth = -1;
    russianRouletteDepth = 4;
    enableGammaCorrection = true;
    enableToneMapping = false;
    gridLevel[0] = 16;
    gridLevel[1] = 16;

    ShowWindow(ofGetWin32Window(), SW_MAXIMIZE);
}

//--------------------------------------------------------------
void ofApp::update()
{
    // update the message queue
    updateMQ();
}

//--------------------------------------------------------------
void ofApp::updateMQ()
{
    // check the message queue
    if(!ipcC2S.isEmpty())
    {
        char msg_buffer[C2SMsgSize];
        memset(msg_buffer, 0, C2SMsgSize);

        a3MessageEntryHead* pMsg = (a3MessageEntryHead*) msg_buffer;
        while(ipcC2S.dequeue(*pMsg))
        {
            switch(pMsg->type)
            {
            case A3_C2S_MSG_GRIDIMAGE:
            {
                const a3C2SGridBufferMessage* msg = (const a3C2SGridBufferMessage*) pMsg;

                a3Log::success("Message Grid[%d] Buffer Recieved\n", msg->currentIndex);

                ofFloatPixels pixels;
                pixels.setFromPixels(msg->buffer, msg->gridWidth, msg->gridHeight, OF_IMAGE_COLOR);
                ofFloatImage img(pixels);

                ipcFbo.begin(); 
                img.draw(msg->gridX, msg->gridY);
                ipcFbo.end();
                break;
            }
            case A3_C2S_MSG_LIGHTPATH:
                break;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    switch(previewType)
    {
    case A3_PREVIEW_REALTIME:
        realtimePreview();
        break;
    case A3_PREVIEW_LOCAL:
        localPreview();
        break;
    case A3_PREVIEW_IPC:
        ipcPreview();
        break;
    }

    guiDraw();
}

//--------------------------------------------------------------
void ofApp::realtimePreview()
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

    for(auto l : lightList)
        l->draw();

    // -----------------------------------cam end-----------------------------------
    getActiveCamera()->end();
}

//--------------------------------------------------------------
void ofApp::localPreview()
{
    //ofBackground(85, 65, 65);
    //ofDrawBitmapString("Local Preview", ofPoint(ofGetWindowWidth() / 2.0f, ofGetWindowHeight() / 2.0f));
}

//--------------------------------------------------------------
void ofApp::ipcPreview()
{
    //ofBackground(65, 85, 65);
    //ofDrawBitmapString("IPC Preview", ofPoint(ofGetWindowWidth() / 2.0f, ofGetWindowHeight() / 2.0f));
    if(!fullScreenIPCPreview)
        ipcFbo.draw(ofGetWidth() / 2 - ipcFbo.getWidth() / 2, ofGetHeight() / 2 - ipcFbo.getHeight() / 2);
    else
        ipcFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
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

            if(ImGui::MenuItem("Save As", NULL)) { /*--! in the future*/ }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Build"))
        {
            if(ImGui::MenuItem("Start Local", NULL)) { /*--! in the future*/ }

            if(ImGui::BeginMenu("Start IPC"))
            {
                if(ImGui::MenuItem(generateLabel("Debug", "IPC").c_str(), "F4"))
                    renderProvess(true);

                if(ImGui::MenuItem(generateLabel("Release", "IPC").c_str(), "F5"))
                    renderProvess(false);

                if(ImGui::MenuItem(generateLabel("Select Atmos Debug EXE...", "IPC").c_str()))
                {
                    ofFileDialogResult result = ofSystemLoadDialog("Select Atmos Debug exe");
                    if(result.bSuccess)
                    {
                        atmosDebugEXEPath = a3S2WS(result.filePath);
                        a3Log::debug("Atmos exe path: %s\n", result.filePath.c_str());
                    }
                }

                if(ImGui::MenuItem(generateLabel("Select Atmos Release EXE...", "IPC").c_str()))
                {
                    ofFileDialogResult result = ofSystemLoadDialog("Select Atmos release exe");
                    if(result.bSuccess)
                    {
                        atmosReleaseEXEPath = a3S2WS(result.filePath);
                        a3Log::debug("Atmos exe path: %s\n", result.filePath.c_str());
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Window"))
        {
            if(ImGui::MenuItem(getWindowToggleName("Shape", openShapeWindow).c_str(), NULL)) { openShapeWindow = !openShapeWindow; }

            if(ImGui::MenuItem(getWindowToggleName("Light", openLightWindow).c_str(), NULL)) { openLightWindow = !openLightWindow; }

            if(ImGui::MenuItem(getWindowToggleName("Model", openModelWindow).c_str(), NULL)) { openModelWindow = !openModelWindow; }

            //if(ImGui::MenuItem(getWindowToggleName("View", openViewWindow).c_str(), NULL)) { openViewWindow = !openViewWindow; }

            if(ImGui::MenuItem(getWindowToggleName("Camera", openCameraWindow).c_str(), NULL)) { openCameraWindow = !openCameraWindow; }

            if(ImGui::MenuItem(getWindowToggleName("Renderer", openRendererWindow).c_str(), NULL)) { openRendererWindow = !openRendererWindow; }

            if(ImGui::MenuItem(getWindowToggleName("Material", openMaterialWindow).c_str(), NULL)) { openMaterialWindow = !openMaterialWindow; }

            if(ImGui::MenuItem(getWindowToggleName("Texture", openTextureWindow).c_str(), NULL)) { openTextureWindow = !openTextureWindow; }
            
            ImGui::EndMenu();
        }

        // 替代View Window 为腾出空间给材质编辑Window
        if(ImGui::BeginMenu("View"))
        {
            ImGui::Text("Preview Mode: %s", getPreviewModeName(previewType).c_str());
            ImGui::Text("FPS:%.2f", ofGetFrameRate());
            ImGui::Checkbox("Full Screen IPC Preview", &fullScreenIPCPreview);

            ImGui::Separator();
            if(ImGui::TreeNode("Preview Camera"))
            {
                if(freeCamPreview)
                    ImGui::Text("Mode: Free Camera");
                else
                    ImGui::Text("Mode: User Defined Camera");

                ofCamera* activeCamera = getActiveCamera();
                ofVec3f up = activeCamera->getUpDir();
                ofVec3f lookAt = activeCamera->getLookAtDir();
                ImGui::Text("Look At: (%.2f, %.2f, %.2f)", lookAt.x, lookAt.y, lookAt.z);
                ImGui::Text("Up: (%.2f, %2f, %.2f)", up.x, up.y, up.z);
                ImGui::Text("Aspect Ratio: %.2f", activeCamera->getAspectRatio());

                ImGui::TreePop();
            }

            ImGui::Separator();
            if(ImGui::Button("RealTime Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                previewType = A3_PREVIEW_REALTIME;

            if(ImGui::Button("Local Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                previewType = A3_PREVIEW_LOCAL;

            if(ImGui::Button("IPC Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                previewType = A3_PREVIEW_IPC;

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("About"))
        {
            ImGui::Text("AtmosEditor is developed by GBB in 2017.");
            if(ImGui::ImageButton((ImTextureID) (uintptr_t) logoButtonID, ImVec2(200, 200)))
                a3Log::debug("And I guess no one would notice this passage.\n");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // -----------------------------------windows-----------------------------------
    modelWindow();
    cameraWindow();
    lightWindow();
    shapeWindow();
    rendererWindow();
    materialWindow();
    textureWindow();

    //viewWindow();
    //aboutWindow();

    gui.end();
}

//--------------------------------------------------------------
void ofApp::renderProvess(bool isDebug)
{
    if(activeCameraIndex >= 0 && activeCameraIndex < cameraList.size())
    {
        a3Log::info("Start IPC Debug Rendering\n");

        SHELLEXECUTEINFO shell = {sizeof(shell)};
        shell.fMask = SEE_MASK_FLAG_DDEWAIT;
        shell.lpVerb = L"open";
        if(isDebug)
            shell.lpFile = atmosDebugEXEPath.c_str();
        else
            shell.lpFile = atmosReleaseEXEPath.c_str();
        shell.nShow = SW_SHOWNORMAL;
        BOOL ret = ShellExecuteEx(&shell);
        if(ret == TRUE)
            sendInitMessage();
        else
            a3Log::warning("Atmos core exe can't open, pls select an usable path.\n");
    }
    else
        a3Log::warning("IPC Render needs a camera to be actived\n");
}

//--------------------------------------------------------------
void ofApp::sendInitMessage()
{
    a3EditorCameraData* data = cameraList[activeCameraIndex];

    if(ipcFbo.isAllocated())
        ipcFbo.clear();

    // init the grid preview fbo
    ipcFbo.allocate(data->dimension.x, data->dimension.y, GL_RGBA);
    ipcFbo.begin();
    ofClear(255, 255, 255);
    ipcFbo.end();

    // send init msg
    a3S2CInitMessage* msg = new a3S2CInitMessage();

    // renderer
    strcpy(msg->imagePath, saveImagePath.c_str());
    msg->imageWidth = data->dimension.x;
    msg->imageHeight = data->dimension.y;
    msg->levelX = gridLevel[0];
    msg->levelY = gridLevel[1];

    msg->enableToneMapping = enableToneMapping;
    msg->enableGammaCorrection = enableGammaCorrection;

    msg->integratorType = (a3IntegratorType) integratorType;
    msg->primitiveSetType = (a3PrimitiveSetType) primitiveSetType;

    msg->spp = spp;
    msg->maxDepth = maxDepth;
    msg->russianRouletteDepth = russianRouletteDepth;

    // camera(operator =)
    msg->camera = *data;

    // models
    msg->modelListLength = modelList.size();
    for(int i = 0; i < modelList.size(); i++)
    {
        // init material
        int index = modelList[i]->materialIndex;
        if(index != -1)
            modelList[i]->materialData = (a3MaterialData) *materialList[index];

        // operator =
        msg->modelList[i] = *modelList[i];
    }

    // shapes
    msg->shapeListLength = shapeList.size();
    for(int i = 0; i < shapeList.size(); i++)
    {
        // init material
        int index = shapeList[i]->materialIndex;
        if(index != -1)
            shapeList[i]->materialData = (a3MaterialData) *materialList[index];

        // operator =
        msg->shapeList[i] = *shapeList[i];
    }

    // lights
    msg->lightListLength = lightList.size();
    for(int i = 0; i < lightList.size(); i++)
    {
        // init texture
        int index = lightList[i]->textureIndex;
        if(index != -1)
        {
            // image texture for image based lighting
            if(lightList[i]->type == A3_LIGHT_INFINITE_AREA)
                strcpy(lightList[i]->imagePath, textureList[i]->imagePath);
        }

        // operator =
        msg->lightList[i] = *lightList[i];
    }

    if(!ipcS2C.isFull())
        ipcS2C.enqueue(*msg);
    else
        a3Log::warning("Full Message Queue\n");

    delete msg;
}

//--------------------------------------------------------------
void ofApp::modelWindow()
{
    if(openModelWindow)
    {
        ImGui::Begin("Models");

        for(vector<a3EditorModelData*>::iterator it = modelList.begin(); it != modelList.end();)
        {
            bool continued = true;
            a3EditorModelData* m= *it;

            if(ImGui::TreeNode((*it)->name.c_str()))
            {
                // select material
                IMGUI_POPUP_SELECT_MATERIAL(m->name, m->materialIndex)

                IMGUI_BUTTON_DELETE_BEGIN("Models" + m->name)
                    a3EditorModelData* temp = *it;

                    it = modelList.erase(it);
                    continued = false;

                    A3_SAFE_DELETE(temp);
                IMGUI_BUTTON_DELETE_END()

                ImGui::TreePop();
            }

            if(continued) it++;
        }

        if(ImGui::Button(generateLabel("+", "Model").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
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
                        a3EditorModelData* data = new a3EditorModelData(loader, result.filePath, result.fileName);
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

        for(vector<a3EditorShapeData*>::iterator it = shapeList.begin(); it != shapeList.end();)
        {
            a3EditorShapeData* s = *it;
            bool continued = true;

            if(ImGui::TreeNode(s->name.c_str()))
            {
                ImGui::TextWrapped("Shape Type: %s", a3TypeToString(s->type).c_str());

                switch(s->type)
                {
                case A3_SHAPE_SPHERE:
                {
                    if(ImGui::DragFloat(generateLabel("Radius", s->name).c_str(), &s->radius))
                        s->sphere->setRadius(s->radius);
                    
                    if(ImGui::DragFloat3(generateLabel("Position", s->name).c_str(), &s->position[0]))
                        s->sphere->setPosition(a3Float3ToVec3(s->position));

                    break;
                }
                case A3_SHAPE_DISK:
                    break;
                case A3_SHAPE_PLANE:
                    break;
                case A3_SHAPE_TRIANGLE:
                    break;
                case A3_SHAPE_INFINITE_PLANE:
                    break;
                }

                // select material
                IMGUI_POPUP_SELECT_MATERIAL(s->name, s->materialIndex)

                IMGUI_BUTTON_DELETE_BEGIN("Shape" + s->name)
                    it = shapeList.erase(it);
                    continued = false;
                    A3_SAFE_DELETE(s);
                IMGUI_BUTTON_DELETE_END()

                ImGui::TreePop();
            }

            if(continued) it++;
        }

        if(ImGui::Button(generateLabel("+", "Shape").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
            ImGui::OpenPopup("Add Shape");

        if(ImGui::BeginPopupModal("Add Shape", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static int item = 0;
            ImGui::Combo(generateLabel("Type", "Shape").c_str(), &item, "Sphere\0Disk\0Plane\0Triangle\0InfinityPlane\0");

            if(ImGui::Button(generateLabel("Ok", "Shape").c_str(), ImVec2(120, 0))) 
            {
                a3EditorShapeData* data = new a3EditorShapeData();
                data->type = (a3ShapeType)item;
                data->name = "Shape" + ofToString(shapeList.size());
                switch(data->type)
                {
                case A3_SHAPE_SPHERE:
                    data->sphere = new ofSpherePrimitive();
                    break;
                case A3_SHAPE_DISK:
                    break;
                case A3_SHAPE_PLANE:
                    data->plane = new ofPlanePrimitive();
                    break;
                case A3_SHAPE_TRIANGLE:
                    break;
                case A3_SHAPE_INFINITE_PLANE:
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

        for(vector<a3EditorLightData*>::iterator it = lightList.begin(); it != lightList.end();)
        {
            bool continued = true;

            a3EditorLightData* l = *it;
            if(ImGui::TreeNode(l->name.c_str()))
            {
                ImGui::TextWrapped("Light Type: %s", a3TypeToString(l->type).c_str());

                switch(l->type)
                {
                case A3_LIGHT_POINT:
                {
                    ImGui::DragFloat3(generateLabel("Emission", l->name).c_str(), l->emission);
                    ImGui::DragFloat3(generateLabel("Position", l->name).c_str(), l->position);

                    break;
                }
                case A3_LIGHT_AREA:
                {
                    ImGui::DragFloat3(generateLabel("Emission", l->name).c_str(), l->emission);
                    break;
                }
                case A3_LIGHT_INFINITE_AREA:
                {
                    //--! For now only support image texture
                    // select a texture
                    IMGUI_POPUP_SELECT_TEXTURE(l->name, l->textureIndex)
                    break;
                }
                case A3_LIGHT_SPOT:
                {
                    // all light needs emmission settings
                    ImGui::DragFloat3(generateLabel("Emission", l->name).c_str(), l->emission);

                    if(ImGui::DragFloat3(generateLabel("Position", l->name).c_str(), l->position))
                        l->updatePosition();

                    ImGui::DragFloat3(generateLabel("Direction", l->name).c_str(), l->direction);

                    ImGui::DragFloat(generateLabel("Cone Angle", l->name).c_str(), &l->coneAngle);

                    ImGui::DragFloat(generateLabel("Falloff Start", l->name).c_str(), &l->falloffStart);

                    // display
                    ImGui::DragFloat(generateLabel("Cone Height", l->name).c_str(), &l->coneHeight);

                    break;
                }
                default:
                    break;
                }

                IMGUI_BUTTON_DELETE_BEGIN("Light" + l->name)
                    it = lightList.erase(it);
                    continued = false;
                    A3_SAFE_DELETE(l);
                IMGUI_BUTTON_DELETE_END()

                ImGui::TreePop();
            }

            if(continued) it++;
        }

        if(ImGui::Button(generateLabel("+", "Light").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
            ImGui::OpenPopup("Add Light");
        if(ImGui::BeginPopupModal("Add Light", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static int item = 0;
            ImGui::Combo(generateLabel("Type", "Light").c_str(), &item, "Point Light\0Area Light\0Infinite Area Light\0Spot Light\0");

            if(ImGui::Button(generateLabel("Ok", "Light").c_str(), ImVec2(120, 0)))
            {
                a3EditorLightData* data = new a3EditorLightData();
                data->type = (a3LightType) item;
                data->name = "Light" + ofToString(lightList.size());
                
                lightList.push_back(data);

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
//void ofApp::viewWindow()
//{
//    if(openViewWindow)
//    {
//        ImGui::Begin("Views");
//
//        ImGui::TextWrapped("Preview Mode: %s", getPreviewModeName(previewType).c_str());
//        ImGui::TextWrapped("FPS:%.2f", ofGetFrameRate());
//        ImGui::Checkbox("Full Screen IPC Preview", &fullScreenIPCPreview);
//
//        ImGui::Separator();
//        if(ImGui::TreeNode("Preview Camera"))
//        {
//            if(freeCamPreview)
//                ImGui::TextWrapped("Mode: Free Camera");
//            else
//                ImGui::TextWrapped("Mode: User Defined Camera");
//
//            ofCamera* activeCamera = getActiveCamera();
//            ofVec3f up = activeCamera->getUpDir();
//            ofVec3f lookAt = activeCamera->getLookAtDir();
//            ImGui::TextWrapped("Look At: (%.2f, %.2f, %.2f)", lookAt.x, lookAt.y, lookAt.z);
//            ImGui::TextWrapped("Up: (%.2f, %2f, %.2f)", up.x, up.y, up.z);
//            ImGui::TextWrapped("Aspect Ratio: %.2f", activeCamera->getAspectRatio());
//
//            ImGui::TreePop();
//        }
//
//        ImGui::Separator();
//        if(ImGui::Button("RealTime Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
//            previewType = A3_PREVIEW_REALTIME;
//        //ImGui::SameLine();
//
//        if(ImGui::Button("Local Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
//            previewType = A3_PREVIEW_LOCAL;
//        //ImGui::SameLine();
//
//        if(ImGui::Button("IPC Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
//            previewType = A3_PREVIEW_IPC;
//        
//        ImGui::End();
//    }
//}

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

        for(vector<a3EditorCameraData*>::iterator it = cameraList.begin(); it != cameraList.end();)
        {
            a3EditorCameraData* c = *it;
            bool continued = true;

            if(ImGui::TreeNode(c->name.c_str()))
            {
                //if(ImGui::Checkbox(generateLabel("Lock Mouse Input", it->name).c_str(), &it->lockMouseInput))
                //{
                //    if(it->lockMouseInput)
                //        it->camera->disableMouseInput();
                //    else
                //        it->camera->enableMouseInput();
                //}

                // active camera to preview
                if(ImGui::Checkbox(generateLabel("Active", c->name).c_str(), &c->active))
                {
                    // 被激活摄像机索引
                    activeCameraIndex = it - cameraList.begin();
                }

                if(ImGui::DragFloat("Width", &c->dimension[0], 1.0f, 1.0f, 1920.0f))
                {
                    c->camera->setAspectRatio(c->dimension[0] / c->dimension[1]);
                }

                if(ImGui::DragFloat("Height", &c->dimension[1], 1.0f, 1.0f, 1080.0f))
                {
                    c->camera->setAspectRatio(c->dimension[0] / c->dimension[1]);
                }

                // camera orcation
                //c->lookAt = c->camera->getLookAtDir();
                if(ImGui::DragFloat3(generateLabel("Look At", c->name).c_str(), &c->lookat[0]))
                {
                    c->camera->lookAt(a3Float3ToVec3(c->lookat), a3Float3ToVec3((c->up)));
                }

                //c->up = c->camera->getUpDir();
                if(ImGui::DragFloat3(generateLabel("Up", c->name).c_str(), &c->up[0]))
                {
                    c->camera->lookAt(a3Float3ToVec3(c->lookat), a3Float3ToVec3(c->up));
                }

                a3Float3Set(c->origin, c->camera->getPosition());
                if(ImGui::DragFloat3(generateLabel("Origin", c->name).c_str(), &c->origin[0]))
                {
                    c->camera->setPosition(a3Float3ToVec3(c->origin));
                }

                // fov
                c->fov = c->camera->getFov();
                if(ImGui::DragFloat(generateLabel("FOV", c->name).c_str(), &c->fov, 1.0f, 1.0f, 180.0f))
                {
                    c->camera->setFov(c->fov);
                }

                // 视锥体距离
                ImGui::DragFloat(generateLabel("Distance", c->name).c_str(), &c->distance);

                IMGUI_BUTTON_DELETE_BEGIN("Camera" + c->name)
                    a3EditorCameraData* temp = *it;

                    it = cameraList.erase(it);
                    if(cameraList.size() <= 0)
                        activeCameraIndex = -1;
                    continued = false;

                    A3_SAFE_DELETE(temp);
                IMGUI_BUTTON_DELETE_END()

                ImGui::TreePop();
            }

            if(continued)
                it++;
        }

        ImGui::Separator();

        if(ImGui::Button("Change Camera To Preview", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
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

        if(ImGui::Button(generateLabel("+", "Camera").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
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
                    a3EditorCameraData* data = new a3EditorCameraData(camera, buffer);
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
void ofApp::rendererWindow()
{
    if(!openRendererWindow) return;

    ImGui::Begin("Renderer");

    ImGui::Combo("Integrator Type", &integratorType, "Path Tracing\0Direct Lighting\0");
    ImGui::Combo("PrimitiveSet Type", &primitiveSetType, "Exhaustive\0BVH\0");

    ImGui::DragInt2("Grid Level", gridLevel, 1, 1, 500);
    ImGui::DragInt("Spp", &spp, 1, 1, 10000);
    ImGui::Checkbox("Enable Gamma Correction", &enableGammaCorrection);
    ImGui::Checkbox("Enable Tone Mapping", &enableToneMapping);

    if(integratorType == A3_INTEGRATOR_PATH)
    {
        ImGui::DragInt("Max Depth", &maxDepth);
        ImGui::DragInt("RussianRoulette Depth", &russianRouletteDepth);
    }

    IMGUI_STYLE_BEGIN(2);
    if(ImGui::Button(generateLabel("Select Save Image Path", "Renderer").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
    {
        ofFileDialogResult result = ofSystemSaveDialog("1.png", "");
        if(result.bSuccess)
            saveImagePath = result.filePath;
    }
    IMGUI_STYLE_END();

    ImGui::End();
}

//--------------------------------------------------------------
void ofApp::materialWindow()
{
    if(!openMaterialWindow) return;

    ImGui::Begin("Material");

    for(vector<a3EditorMaterialData*>::iterator it = materialList.begin(); it != materialList.end();)
    {
        a3EditorMaterialData* m = *it;
        bool continued = true;

        if(ImGui::TreeNode(m->name.c_str()))
        {
            ImGui::TextWrapped("Material Type: %s", a3TypeToString(m->type).c_str());
            ImGui::DragFloat3(generateLabel("R", "Material").c_str(), m->R, 0.1f, 0.0f, 1.0f);

            if(m->textureIndex != -1)
                ImGui::TextWrapped("Texture Name: %s", textureList[m->textureIndex]->name.c_str());

            // select a texture
            IMGUI_POPUP_SELECT_TEXTURE(m->name, m->textureIndex)

            IMGUI_BUTTON_DELETE_BEGIN("Material" + m->name)
                a3EditorMaterialData* temp = m;

                it = materialList.erase(it);
                continued = false;

                A3_SAFE_DELETE(temp);
            IMGUI_BUTTON_DELETE_END()

            ImGui::TreePop();
        }

        if(continued)
            it++;
    }

    // add matertial
    if(ImGui::Button(generateLabel("+", "Material").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        ImGui::OpenPopup("Add Material");
    if(ImGui::BeginPopupModal("Add Material", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static int item = 0;
        ImGui::Combo(generateLabel("Type", "Material").c_str(), &item, "Lambertian\0Mirror\0Glass\0");

        if(ImGui::Button(generateLabel("OK", "Material").c_str(), ImVec2(120, 0)))
        {
            a3EditorMaterialData* data = new a3EditorMaterialData();
            data->name = "Material";
            data->name += ofToString(materialList.size());
            data->type = (a3MaterialType) item;
            materialList.push_back(data);

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button(generateLabel("Cancel", "Texture").c_str(), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::End();
}

//--------------------------------------------------------------
void ofApp::textureWindow()
{
    if(!openTextureWindow) return;

    ImGui::Begin("Texture");

    for(vector<a3EditorTextureData*>::iterator it = textureList.begin(); it != textureList.end();)
    {
        a3EditorTextureData* t = *it;
        bool continued = true;

        if(ImGui::TreeNode(t->name.c_str()))
        {
            ImGui::TextWrapped("Texture Type: %s", a3TypeToString(t->type).c_str());

            switch(t->type)
            {
            case A3_TEXTURE_IMAGE:
            {
                // load a image texture
                IMGUI_STYLE_BEGIN(2);
                if(ImGui::Button(generateLabel("Load Image", t->name).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
                {
                    ofFileDialogResult result = ofSystemLoadDialog("Load Image");
                    if(result.bSuccess)
                    {
                        strcpy(t->imagePath, result.filePath.c_str());
                        // OF纹理render target是GL_TEXTURE_RECTANGLE_ARB。ImGui需要的是GL_TEXTURE_2D
                        ofDisableArbTex();
                        t->image.load(result.filePath);
                        ofEnableArbTex();
                    }
                }
                IMGUI_STYLE_END();

                // visualize
                if(t->image.isAllocated())
                {
                    string pathContent = "Path: ";
                    pathContent += t->imagePath;
                    ImGui::TextWrapped(pathContent.c_str());
                    float ratio = t->image.getWidth() / t->image.getHeight();
                    float w = ImGui::GetContentRegionAvailWidth();
                    GLuint a = t->image.getTexture().getTextureData().textureID;
                    t->image.update();
                    ImGui::Image((ImTextureID) (uintptr_t) a, ImVec2(w, w / ratio));
                }
                break;
            }
            case A3_TEXTURE_CONSTANT:
            {
                ImGui::DragFloat3(generateLabel("value", t->name).c_str(), t->value);
                break;
            }
            case A3_TEXTURE_CHECKBOARD:
            {
                ImGui::DragFloat(generateLabel("t1", t->name).c_str(), &t->t1);
                ImGui::DragFloat(generateLabel("t2", t->name).c_str(), &t->t2);

                ImGui::DragFloat(generateLabel("level", t->name).c_str(), &t->level);
                break;
            }
            }

            // delete
            IMGUI_BUTTON_DELETE_BEGIN("Texture" + t->name)
                a3EditorTextureData* temp = t;

                it = textureList.erase(it);
                continued = false;

                A3_SAFE_DELETE(temp);
            IMGUI_BUTTON_DELETE_END()

            ImGui::TreePop();
        }

        if(continued)
            it++;
    }

    if(ImGui::Button(generateLabel("+", "Texture").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        ImGui::OpenPopup("Add Texture");
    if(ImGui::BeginPopupModal("Add Texture", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static int item = 0;
        ImGui::Combo(generateLabel("Type", "Texture").c_str(), &item, "Image\0Constant\0Checkboard\0");

        if(ImGui::Button(generateLabel("OK", "Texture").c_str(), ImVec2(120, 0)))
        {
            a3EditorTextureData* data = new a3EditorTextureData();
            data->name = "Texture";
            data->name += ofToString(textureList.size());
            // 第一个类型为NULL
            data->type = (a3TextureType) (item + 1);
            textureList.push_back(data);

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button(generateLabel("Cancel", "Texture").c_str(), ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::End();
}

//--------------------------------------------------------------
//void ofApp::aboutWindow()
//{
//    if(!openAboutWindow) return;
//
//    ImGuiWindowFlags window_flags = 0;
//    ImGui::SetNextWindowSize(ofVec2f(400, 300), ImGuiSetCond_FirstUseEver);
//    if(ImGui::Begin("About", &openAboutWindow))
//    {
//        ImGui::ImageButton((ImTextureID) (uintptr_t) logoButtonID, ImVec2(200, 200));
//        ImGui::SameLine();
//        ImGui::Text("AtmosEditor is developed by GBB in 2017.");
//        ImGui::SameLine();
//        ImGui::Text("And I guess no one would notice this passage.");
//    }
//
//    ImGui::End();
//}

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

a3EditorCameraData* ofApp::getActiveCameraData()
{
    if(freeCamPreview)
        return NULL;
    else
    {
        if(activeCameraIndex >= 0 && activeCameraIndex < cameraList.size())
            return cameraList[activeCameraIndex];
        else
            return NULL;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if(key == OF_KEY_F4)
        renderProvess(true);
    else if(key == OF_KEY_F5)
        renderProvess(false);
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
