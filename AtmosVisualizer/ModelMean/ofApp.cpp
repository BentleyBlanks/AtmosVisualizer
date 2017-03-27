#include "ofApp.h"
#include "lineIntersect.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <fstream>

#include "a3Log.h"
#include "a3LogHTML.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(50, 50, 50, 0);
    ofEnableDepthTest();

    // models
    //loadModels();
    prefix = "hello ";
    saveFileName = "points.txt";

    drawPolylines = false;
    drawMean = false;
    //loadModel("BJ037.obj");
    //loadModel("cd005.obj");
    //loadModel("cs015.obj");
    //loadModel("xa001.obj");

    printf("%f\n", ofHexToFloat("4060000000000000"));

    cam = new ofEasyCam();

    gui.setup();

    rayCount = 360;
    modelNum = 4;
    zInterval = 5;

    //triangle.set(ofVec3f(10, 0, 0), ofVec3f(200, 500, 0), ofVec3f(-500, 0, 0), ofVec3f(200, -500, 0));
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::loadModels()
{
    //a3Log::debug("Model Num: %d\n", modelNum);

    //for(int i = 1; i <= modelNum; i++)
    //{
    //    string prefix = "face (";
    //    prefix += ofToString(i);
    //    prefix += ").obj";

    //    Shape* shape = new Shape();
    //    shape->load(prefix.c_str());

    //    shapes.push_back(shape);

    //    a3Log::info("Loading Models: %8.2f \r", (float)i / modelNum * 100);
    //}
}

//--------------------------------------------------------------
std::vector<ofVec3f>* ofApp::updateMean(const std::vector<ofVec3f>* mean, const std::vector<ofVec3f>* ps2, float z, float weight)
{
    const int rayLength = 100000;
    std::vector<ofVec3f>* newMean = new std::vector<ofVec3f>();

    //rays.clear();
    //test.clear();

    float angle = 0.0f;
    while(angle <= 360.0f)
    {
        // 射线
        ofVec2f q1(0, 0), q2(cos(ofDegToRad(angle)) * rayLength, sin(ofDegToRad(angle)) * rayLength);

        ofVec2f result1, result2;

        bool intersect1 = false, intersect2 = false;

        for(int i = 0; i < mean->size()-1; i++)
        {
            ofVec2f p1((*mean)[i].x, (*mean)[i].y), p2((*mean)[i+1].x, (*mean)[i+1].y);

            //if(abs(p1.x) < 0.1 && abs(p1.y) < 0.1)
            //    continue;

            //if(abs(p2.x) < 0.1 && abs(p2.y) < 0.1)
            //    continue;

            if(segmentsIntersect(result1, p1, p2, q2, q1))
            {
                intersect1 = true;
                break;
            }
        }

        for(int i = 0; i < ps2->size()-1; i++)
        {
            ofVec2f p1((*ps2)[i].x, (*ps2)[i].y), p2((*ps2)[i + 1].x, (*ps2)[i + 1].y);
            //if(abs(p1.x) < 0.1 && abs(p1.y) < 0.1)
            //    continue;

            //if(abs(p2.x) < 0.1 && abs(p2.y) < 0.1)
            //    continue;

            if(segmentsIntersect(result2, p1, p2, q2, q1))
            {
                intersect2 = true;
                break;
            }
        }

        if(intersect1 && intersect2)
        {
            //test.push_back(result1);
            //test.push_back(result2);

            ofVec2f middle = result1 * weight + result2 * (1.0f - weight);

            newMean->push_back(ofVec3f(middle.x, middle.y, z));
        }

        //if(!(intersect1 && intersect2))
        //{
        //    rays.push_back(q2);
        //}

        angle += 360.0f / rayCount;
    }

    return newMean;
}

//--------------------------------------------------------------
void ofApp::draw(){

    cam->begin();
    ofPushStyle();

    ofDrawAxis(400);

    ofSetColor(255, 255, 255);
    //if(drawPolylines)
    //{
    //    for(auto p : polylines)
    //    {
    //        p->draw();
    //    }
    //}

    //if(drawMean)
    //{
    //    ofSetColor(255, 0, 0);
    //    meanPoly.draw();
    //    vector<ofVec3f> pss = meanPoly.getVertices();
    //    for(auto p : pss)
    //    {
    //        ofDrawSphere(p, 0.5);
    //    }
    //}

    if(drawFace)
    {
        ofSetColor(0, 255, 0);
        for(auto m : meanFace)
            m.draw();
    }

    //ofSetColor(0, 0, 255);
    //for(auto v : test)
    //    ofDrawSphere(v, 0.5);

    //ofSetColor(0, 0, 0);
    //for(auto l : rays)
    //    ofDrawLine(ofVec3f::zero(), l);

    ofPopStyle();
    cam->end();

    gui.begin();

    //if(ImGui::DragFloat("Z", &z, 1, -100, 100))
    //{
    //    polylines.clear();
    //    for(int i = 0; i < shapes.size(); i++)
    //    {
    //        polylines.push_back(shapes[i]->getPolylineWithZ(z));
    //    }
    //}

    ImGui::DragFloat("z Interval", &zInterval, 1, 1, 50);
    ImGui::DragInt("Ray Count", &rayCount, 720, 1, 3600);
    ImGui::DragInt("Model Count", &modelNum, 1, 1, 300);

    if(ImGui::Button("Set Title"))
    {
        ofSetWindowTitle("Whosyourdadyy?");
    }

    if(ImGui::Button("Toggle Fullscreen"))
    {
        ofToggleFullscreen();
    }
    //ImGui::Checkbox("Draw Polyline", &drawPolylines);

    //ImGui::Checkbox("Draw Mean", &drawMean);
    char modelPrefix[20];
    strcpy(modelPrefix, prefix.c_str());
    if(ImGui::InputText("Model Prefix", modelPrefix, 20))
    {
        prefix.clear();
        prefix = modelPrefix;
    }

    char fileName[40];
    strcpy(fileName, saveFileName.c_str());
    if(ImGui::InputText("Output File Name", fileName, 40))
    {
        saveFileName.clear();
        saveFileName = fileName;
    }

    ImGui::Checkbox("Draw Face", &drawFace);

    //if(ImGui::Button("Update Polylines"))
    //{
    //    polylines.clear();
    //    for(int i = 0; i < shapes.size(); i++)
    //    {
    //        polylines.push_back(shapes[i]->getPolylineWithZ(z));
    //    }
    //}

    if(ImGui::Button("Calculating"))
    {
        if(modelNum > 0)
        {
            a3Log::print("\n");

            mean.clear();
            a3Log::debug("Model Num: %d\n", modelNum);

            for(int i = 1; i <= modelNum; i++)
            {
                a3Log::info("Model Num: %d     Calculating: %8.2f    \n", i, (float) i / modelNum * 100);

                // load model
                string path(prefix);
                path += "(";
                path += ofToString(i);
                path += ").obj";

                // Shape
                Shape* shape = new Shape();
                if(!shape->load(path.c_str()))
                {
                    a3Log::error("模型:%s 加载失败\n", path.c_str());
                    continue;
                }
                else
                    a3Log::success("已加载模型:%s \n", path.c_str());

                float start = -120, zStart = start, zEnd = 120;

                // 第一次导入模型初始化mean
                if(mean.empty())
                {
                    // 从0开始
                    mean.resize((unsigned int) ((zEnd - zStart) / zInterval) + 1);
                    for(auto m : mean)
                        m = NULL;
                }

                a3Log::info("start: %f, end: %f\n", start, zEnd);

                int j = 0;
                // 从zStart开始切割
                while(zStart <= zEnd)
                {
                    a3Log::info("z: %f \r", zStart);

                    // 第一个形状上的交点
                    std::vector<ofVec3f>* points = shape->getPointsWithZ(zStart);

                    if(mean[j] == NULL)
                    {
                        // 空mean直接为第一个形状截面点
                        mean[j] = points;
                    }
                    else
                    {
                        std::vector<ofVec3f>* temp = updateMean(mean[j], points, zStart, (float) (i - 1) / (float) i);

                        // 释放上一次平均结果
                        delete mean[j];
                        mean[j] = temp;

                        // 释放当前形状位置截面数据
                        delete points;
                    }

                    zStart += zInterval;
                    j++;
                }

                // 可重入
                delete shape;
                shape = NULL;
            }

            // Visualize
            for(auto m : mean)
            {
                if(!m->empty())
                {
                    ofPolyline poly;
                    poly.clear();
                    poly.addVertices(*m);
                    meanFace.push_back(poly);
                }
            }

            a3Log::success("平均计算结束\n");
        }
        else
            a3Log::warning("导入模型数量不得少于0\n");
    }


    if(ImGui::Button("Save"))
    {
        // 开头Points 按最底下层层向上
        if(saveFileName.empty())
        {
            a3Log::warning("输出文件名不能为空\n");
        }
        else
        {
            a3Log::print("\n");
            ofstream out(saveFileName.c_str());
            if(out.is_open())
            {
                out << "Points" << endl;
                // 平均线
                int meanSize = mean.size();
                for(int i = 0; i < mean.size(); i++)
                {
                    a3Log::info("Saving: %8.2f\%    \r", (float)i / meanSize * 100.0f);

                    if(!mean[i]->empty())
                    {
                        // 遍历线中点
                        for(auto point : *mean[i])
                        {
                            out << ofToString(point.x, 5) << " " << ofToString(point.y, 5) << " " << ofToString(point.z, 5) << endl;
                        }
                    }
                }

                a3Log::success("文件: %s 保存成功\n", saveFileName.c_str());
                out.close();
            }
            else
                a3Log::warning("生成文件错误\n");
        }
    }

    //if(ImGui::Button("Calculate"))
    //{
    //    if(shapes.size() > 0)
    //    {
    //        // 第一个形状上的交点
    //        std::vector<ofVec3f>* last = shapes[0]->getPointsWithZ(z);
    //        for(int i = 1; i < shapes.size(); i++)
    //        {
    //            std::vector<ofVec3f>* temp = NULL;
    //            std::vector<ofVec3f>* next = shapes[i]->getPointsWithZ(z);
    //            temp = updateMean(last, next, z);

    //            // 释放上一次结果
    //            delete last;
    //            delete next;
    //            last = temp;
    //        }

    //        if(!last->empty())
    //        {
    //            meanPoly.clear();
    //            meanPoly.addVertices(*last);
    //        }

    //        delete last;
    //        last = NULL;
    //    }
    //}

    //if(ImGui::Button("Calculate Face"))
    //{
    //    if(shapes.size() > 0)
    //    {
    //        float zStart = -120;
    //        while(zStart <= 120)
    //        {
    //            ofPolyline poly;
    //            // 第一个形状上的交点
    //            std::vector<ofVec3f>* last = shapes[0]->getPointsWithZ(zStart);
    //            for(int i = 1; i < shapes.size(); i++)
    //            {
    //                std::vector<ofVec3f>* temp = NULL;
    //                std::vector<ofVec3f>* next = shapes[i]->getPointsWithZ(zStart);
    //                temp = updateMean(last, next, zStart);


    //                // 释放上一次结果
    //                delete last;
    //                delete next;
    //                last = temp;
    //            }

    //            if(!last->empty())
    //            {
    //                poly.clear();
    //                poly.addVertices(*last);
    //                meanFace.push_back(poly);
    //            }

    //            delete last;
    //            last = NULL;

    //            zStart += zInterval;
    //        }
    //    }
    //}



    gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
