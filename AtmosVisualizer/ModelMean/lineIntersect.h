#pragma once
#include "ofMain.h"

double mult3(ofVec2f a, ofVec2f b, ofVec2f c)
{
    return (a.x - c.x)*(b.y - c.y) - (b.x - c.x)*(a.y - c.y);
}

//aa, bbΪһ���߶����˵� cc, ddΪ��һ���߶ε����˵� �ཻ����true, ���ཻ����false  
bool lineIntersect(ofVec2f aa, ofVec2f bb, ofVec2f cc, ofVec2f dd)
{
    if(max(aa.x, bb.x)<min(cc.x, dd.x))
    {
        return false;
    }
    if(max(aa.y, bb.y)<min(cc.y, dd.y))
    {
        return false;
    }
    if(max(cc.x, dd.x)<min(aa.x, bb.x))
    {
        return false;
    }
    if(max(cc.y, dd.y)<min(aa.y, bb.y))
    {
        return false;
    }
    if(mult3(cc, bb, aa)*mult3(bb, dd, aa)<0)
    {
        return false;
    }
    if(mult3(aa, dd, cc)*mult3(dd, bb, cc)<0)
    {
        return false;
    }
    return true;
}

// �߶��ཻ���Բ��󽻵�
bool segmentsIntersect(ofVec2f& result, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d)
{

    // ������abc �����2��  
    float area_abc = (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);

    // ������abd �����2��  
    float area_abd = (a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x);

    // ���������ͬ���������߶�ͬ��,���ཻ (�Ե����߶��ϵ����,�����������ཻ����);  
    if(area_abc * area_abd >= 0)
    {
        return false;
    }

    // ������cda �����2��  
    float area_cda = (c.x - a.x) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);
    // ������cdb �����2��  
    // ע��: ������һ��С�Ż�.����Ҫ���ù�ʽ�������,����ͨ����֪����������Ӽ��ó�.  
    float area_cdb = area_cda + area_abc - area_abd;
    if(area_cda * area_cdb >= 0)
    {
        return false;
    }

    //���㽻������  
    float t = area_cda / (area_abd - area_abc);
    float dx = t*(b.x - a.x),
        dy = t*(b.y - a.y);

    result.x = a.x + dx;
    result.y = a.y + dy;

    return true;
}
