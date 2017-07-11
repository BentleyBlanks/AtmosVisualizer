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

//--------------------------------------------------------------
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

//--------------------------------------------------------------
string generateLabel(string name, string anonymous)
{
    string label = name;
    label += "##";
    label += anonymous;
    return label;
}

//--------------------------------------------------------------
void reallocateFbo(ofFbo* fbo, int width, int height, int numSamples = 0)
{
    if(!fbo) return;

    if(width <= 0 || height <= 0) return;

    if(!fbo->isAllocated() ||
       fbo->getWidth() != width ||
       fbo->getHeight() != height)
    {
        // no need to clear
        ofDisableArbTex();
        fbo->allocate(width, height, GL_RGBA, numSamples);
        ofEnableArbTex();
    }
}

//--------------------------------------------------------------
inline ofVec3f float3ToVec3(const float* f3)
{
    // 不做安全检查
    return ofVec3f(f3[0], f3[1], f3[2]);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableDepthTest();
    ofSetFrameRate(60);
    ShowWindow(ofGetWin32Window(), SW_MAXIMIZE);

    freeCamPreview = true;
    // 左手系
    freeCam.setVFlip(true);
    freeCam.lookAt(ofVec3f(0, 0, 1), ofVec3f(0, 1, 0));

    activeCameraIndex = -1;

    // preview 
    ground = new Graph3D(650, 650, 30, 30);
    //previewShader.load("./preview.vert", "./preview.frag");

    ipcS2C.init(L"Who's Your Daddy S2C", true, msgMaxNum / 2, S2CMsgSize);
    ipcC2S.init(L"Who's Your Daddy C2S", true, msgMaxNum, C2SMsgSize);

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
    primitiveSetType = 1;
    visualizeRays = false;

    guiSetup();
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

                a3Ray* temp = new a3Ray(float3ToVec3(msg->origin), float3ToVec3(msg->origin) + float3ToVec3(msg->direction) * 200);
                rayLists.push_back(temp);

                break;
            }
            case A3_C2S_MSG_LIGHTPATH:
            {
                //const a3C2SLightPathMessage* msg = (const a3C2SLightPathMessage*) pMsg;

                //a3Ray* temp = new a3Ray(float3ToVec3(msg->origin), float3ToVec3(msg->direction) * 200);
                //rayLists.push_back(temp);
                break;
            }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{    
    //--!could stop update when isn't previewing
    realtimePreview();

    guiDraw();
}

//--------------------------------------------------------------
void ofApp::realtimePreview()
{
    ofFbo* fbo = getPreviewFbo();
    if(!fbo) return;

    fbo->begin();
    ofClear(45, 45, 57);
    ofEnableAntiAliasing();

    // -----------------------------------cam begin-----------------------------------
    getActiveCamera()->begin(ofRectangle(0, 0, fbo->getWidth(), fbo->getHeight()));
    //getActiveCamera()->begin(ofRectangle(0, 0, 500, 500));

    //ofPushStyle();
    //ofPushMatrix();

    // turn to left-haned coordinate
    //previewShader.begin();

    // 左手坐标系
    float axisLength = 550;
    a3DrawAxis(axisLength);
    //ofDrawAxis(axisLength);

    ofPushStyle();
    ofSetColor(200, 200, 200, 200);
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

    if(visualizeRays)
    {
        for(auto r : rayLists)
            r->draw();
    }
    //previewShader.end();

    //ofPopMatrix();
    //ofPopStyle();

    // -----------------------------------cam end-----------------------------------
    getActiveCamera()->end();

    fbo->end();
}

//--------------------------------------------------------------
void ofApp::guiSetup()
{
    gui.setup();

    // imgui style
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowRounding = 0.0f;
    style->FrameRounding = 0.0f;
    style->GrabRounding = 0.0f;
    style->ScrollbarRounding = 0.0f;

    // left
    rendererDock.initialize("Renderer", true, ImVec2(), [this](ImVec2 area) { rendererWindow(); });

    cameraDock.initialize("Camera", true, ImVec2(), [this](ImVec2 area) { cameraWindow(); });

    modelDock.initialize("Model", true, ImVec2(), [this](ImVec2 area) { modelWindow(); });

    // middle
    sceneDock.initialize("Scene", true, ImVec2(), [this](ImVec2 area)
    {
        freeCamPreview = true;

        ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        ImVec2 winPos = ImGui::GetWindowPos();
        freeCam.setMouseTriggerRegion(ofRectangle(winPos.x, winPos.y, contentRegion.x, contentRegion.y));
        // reallocate
        reallocateFbo(&sceneFbo, contentRegion.x, contentRegion.y);

        // realtime rendering preview
        ImGui::Image((ImTextureID) (uintptr_t) sceneFbo.getTexture().getTextureData().textureID, ImVec2(sceneFbo.getWidth(), sceneFbo.getHeight()));
    });

    previewDock.initialize("Preview", true, ImVec2(), [this](ImVec2 area)
    {
        // active camera preview 
        freeCamPreview = false;

        ofVec2f d(previewFbo.getWidth(), previewFbo.getHeight());
        if(activeCameraIndex >= 0 && activeCameraIndex < cameraList.size())
            d = cameraList[activeCameraIndex]->dimension;
        else return;

        // reallocate
        reallocateFbo(&previewFbo, d.x, d.y);

        // realtime rendering preview
        ImGui::Image((ImTextureID) (uintptr_t) previewFbo.getTexture().getTextureData().textureID, ImVec2(previewFbo.getWidth(), previewFbo.getHeight()));
    });

    offlineResultDock.initialize("Result", true, ImVec2(), [this](ImVec2 area)
    {
        // ipc renderer image
        ofVec2f d(ipcFbo.getWidth(), ipcFbo.getHeight());
        if(activeCameraIndex >= 0 && activeCameraIndex < cameraList.size())
            d = cameraList[activeCameraIndex]->dimension;
        else return;

        // reallocate
        reallocateFbo(&ipcFbo, d.x, d.y);

        // realtime rendering preview
        ImGui::Image((ImTextureID) (uintptr_t) ipcFbo.getTexture().getTextureData().textureID, ImVec2(ipcFbo.getWidth(), ipcFbo.getHeight()));
    });

    // bottom
    materialDock.initialize("Material", true, ImVec2(), [this](ImVec2 area) { materialWindow(); });

    textureDock.initialize("Texture", true, ImVec2(), [this](ImVec2 area) { textureWindow(); });

    // right
    shapeDock.initialize("Shape", true, ImVec2(), [this](ImVec2 area) { shapeWindow(); });

    lightDock.initialize("Light", true, ImVec2(), [this](ImVec2 area) { lightWindow(); });

    int width = ofGetWidth(), height = ofGetHeight();

    dockspace.dock(&rendererDock, ImGuiDock::DockSlot::Left, 0.25 * width, true);
    dockspace.dockWith(&sceneDock, &rendererDock, ImGuiDock::DockSlot::Right, 0.7 * width, true);
    dockspace.dockWith(&previewDock, &sceneDock, ImGuiDock::DockSlot::Tab, 0, false);
    dockspace.dockWith(&offlineResultDock, &sceneDock, ImGuiDock::DockSlot::Tab, 0, false);

    dockspace.dockWith(&cameraDock, &rendererDock, ImGuiDock::DockSlot::Bottom, 0.6 * height, true);
    dockspace.dockWith(&modelDock, &cameraDock, ImGuiDock::DockSlot::Tab, 0, false);

    dockspace.dock(&shapeDock, ImGuiDock::DockSlot::Right, 0.15 * width, true);
    dockspace.dockWith(&lightDock, &shapeDock, ImGuiDock::DockSlot::Tab, 0, false);

    dockspace.dockWith(&materialDock, &shapeDock, ImGuiDock::DockSlot::Bottom, 0.5 * height, false);
    dockspace.dockWith(&textureDock, &materialDock, ImGuiDock::DockSlot::Tab, 0, true);
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
                    startRenderingProvess(true);

                if(ImGui::MenuItem(generateLabel("Release", "IPC").c_str(), "F5"))
                    startRenderingProvess(false);

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

        // 替代View Window 为腾出空间给材质编辑Window
        if(ImGui::BeginMenu("View"))
        {
            ImGui::Separator();
            if(ImGui::TreeNode("Preview Camera"))
            {
                ofCamera* activeCamera = getActiveCamera();
                ofVec3f up = activeCamera->getUpDir();
                ofVec3f lookAt = activeCamera->getLookAtDir();
                ImGui::Text("Look At: (%.2f, %.2f, %.2f)", lookAt.x, lookAt.y, lookAt.z);
                ImGui::Text("Up: (%.2f, %2f, %.2f)", up.x, up.y, up.z);
                ImGui::Text("Aspect Ratio: %.2f", activeCamera->getAspectRatio());

                ImGui::TreePop();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("About"))
        {
            ImGui::Text("AtmosEditor is developed by GBB in 2017.");
            if(ImGui::ImageButton((ImTextureID) (uintptr_t) logoButtonID, ImVec2(200, 200)))
                a3Log::debug("And I guess no one would notice this passage.\n");

            ImGui::EndMenu();
        }

        ImGui::Text("FPS: %d", (int) ofGetFrameRate());

        ImGui::EndMainMenuBar();
    }

    static bool openAllUI = true;
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;

    // the window contains all docks
    // --!hard coded, can't get it
    int menuHeight = 22;
    int width = ofGetWidth(), height = ofGetHeight();
    ImGui::SetNextWindowPos(ImVec2(0.0f, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(width, height - menuHeight));

    ImGui::Begin("All UI Widgets", &openAllUI, ImVec2(), 1.0f, window_flags);
    dockspace.updateAndDraw(ImGui::GetContentRegionAvail());
    ImGui::End();

    gui.end();
}

//--------------------------------------------------------------
void ofApp::debugDraw()
{
    // -----------------------------------mouse pos-----------------------------------
    string mousePos = "[x:";
    mousePos += ofToString(ofGetMouseX());
    mousePos += ", y:";
    mousePos += ofToString(ofGetMouseY());
    mousePos += "]";
    ofDrawBitmapString(mousePos.c_str(), ofGetMouseX(), ofGetMouseY());

    ofDrawRectangle(freeCam.getMouseTriggerRegion());
}

//--------------------------------------------------------------
void ofApp::startRenderingProvess(bool isDebug)
{
    if(activeCameraIndex >= 0 && activeCameraIndex < cameraList.size())
    {
        a3Log::info("Start IPC Debug Rendering\n");

        // 清空上次可视化光线数据
        for(auto r : rayLists)
            A3_SAFE_DELETE(r);
        rayLists.clear();

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
    if(activeCameraIndex < 0 || activeCameraIndex >= cameraList.size())
    {
        a3Log::warning("IPC Render needs a camera to be actived\n");
        return;
    }

    a3EditorCameraData* data = cameraList[activeCameraIndex];

    // init the grid preview fbo
    ofDisableArbTex();
    ipcFbo.allocate(data->dimension.x, data->dimension.y, GL_RGBA);
    ofEnableArbTex();

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
    for(vector<a3EditorModelData*>::iterator it = modelList.begin(); it != modelList.end();)
    {
        bool continued = true;
        a3EditorModelData* m = *it;

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
            loader->setScaleNormalization(false);

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
}

//--------------------------------------------------------------
void ofApp::shapeWindow()
{
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
            data->type = (a3ShapeType) item;
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
}

//--------------------------------------------------------------
void ofApp::lightWindow()
{
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
}

//--------------------------------------------------------------
void ofApp::cameraWindow()
{
    if(ImGui::TreeNode("Free Camera"))
    {
        if(ImGui::Checkbox("Lock Mouse Input", &lockMouseInput))
        {
            if(lockMouseInput)
                freeCam.disableMouseInput();
            else
                freeCam.enableMouseInput();
        }

        if(ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
            freeCam.reset();

        ImGui::TreePop();
    }

    for(vector<a3EditorCameraData*>::iterator it = cameraList.begin(); it != cameraList.end();)
    {
        a3EditorCameraData* c = *it;
        bool continued = true;

        if(ImGui::TreeNode(c->name.c_str()))
        {
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
                // 左手系
                camera->setVFlip(true);
                camera->lookAt(ofVec3f(0, 0, 1), ofVec3f(0, 1, 0));

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
}

//--------------------------------------------------------------
void ofApp::rendererWindow()
{
    ImGui::Combo(generateLabel("Integrator", "Renderer").c_str(), &integratorType, "Path Tracing\0Direct Lighting\0");
    ImGui::Combo(generateLabel("PrimitiveSet", "Renderer").c_str(), &primitiveSetType, "Exhaustive\0BVH\0");

    ImGui::DragInt2("Grid Level", gridLevel, 1, 1, 500);
    ImGui::DragInt("Spp", &spp, 1, 1, 10000);
    ImGui::Checkbox("Enable Gamma Correction", &enableGammaCorrection);
    ImGui::Checkbox("Enable Tone Mapping", &enableToneMapping);

    ImGui::Checkbox("Visualise Rays", &visualizeRays);

    if(integratorType == A3_INTEGRATOR_PATH)
    {
        ImGui::DragInt("Max Depth", &maxDepth);
        ImGui::DragInt("RR Depth", &russianRouletteDepth);
    }

    IMGUI_STYLE_BEGIN(2);
    if(ImGui::Button(generateLabel("Select Save Image Path", "Renderer").c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
    {
        ofFileDialogResult result = ofSystemSaveDialog("1.png", "");
        if(result.bSuccess)
            saveImagePath = result.filePath;
    }
    IMGUI_STYLE_END();
}

//--------------------------------------------------------------
void ofApp::materialWindow()
{
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
}

//--------------------------------------------------------------
void ofApp::textureWindow()
{
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

ofFbo * ofApp::getPreviewFbo()
{
    if(freeCamPreview)
    {
        if(sceneFbo.isAllocated())
            return &sceneFbo;
        else
            return NULL;
    }
    else
    {
        if(previewFbo.isAllocated())
            return &previewFbo;
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
        startRenderingProvess(true);
    else if(key == OF_KEY_F5)
        startRenderingProvess(false);
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
