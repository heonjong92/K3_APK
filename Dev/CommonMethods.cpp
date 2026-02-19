#include "stdafx.h"
#include "CommonMethods.h"

void CommonMethods::splitString(std::vector<string> &v, size_t index, const string &src, const string &separate)
{
    std::vector<std::string>res;
    size_t pos = src.find(separate);
    if (std::string::npos == pos)
    {
        v.push_back(src);
        return;
    }
    v.push_back(src.substr(index, pos));
    // splitString(v, pos + separate.length(), src.substr(pos + separate.size(), src.size() - pos - separate.size()), separate);
	// substr()하면 src 파라미터의 내용이 잘리므로 index는 0이어야 함, 241217 황대은J
	splitString(v, 0, src.substr(pos + separate.size(), src.size() - pos - separate.size()), separate); 
}

void CommonMethods::AnalysisIpInfos(const char *pIps, vector<string> &localIpList, vector<string> &cameraIpList)
{
    if (pIps!=nullptr)
    {
        vector<string> vAdapters;
        string s = pIps;
        splitString(vAdapters, 0, s, "###");
        std::cout<<"ips:"<<s<<std::endl;
        for (string Adapter : vAdapters)
        {
            vector<string> tmp;
            splitString(tmp, 0, Adapter, "@@@");
            vector<string> vSplitLocalAndCameras;
            splitString(vSplitLocalAndCameras, 0, tmp[0], ";");
            if (vSplitLocalAndCameras.size() == 2)
            {
                localIpList.push_back(vSplitLocalAndCameras[1]);
                vector<string> Cameras;
                splitString(Cameras, 0, vSplitLocalAndCameras[0], "(");
                cameraIpList.push_back(Cameras[0]);
            }
        }
    }
}

unsigned char CommonMethods::Rescale(float value, float max, float min)
{
    if (value < min)
    {
        return 0;
    }
    else if (value > max)
    {
        return 255;
    }
    else if (abs(max - min) < 0.00001)
    {
        // 同一个平面
        return 0;
    }
    return (unsigned char)(((value - min) / (max - min)) * 255);
}

void CommonMethods::DepthData2Gary(float *pDepth, unsigned char *pGray, int iDataSize)
{
    float zMax = pDepth[0];
    float zMin = pDepth[0];
    for (int j = 0; j < iDataSize; j++)
    {
        if (pDepth[j] > 0)
        {
            if (pDepth[j] < zMin)
            {
                zMin = pDepth[j];
            }
            if(pDepth[j]>zMax)
            {
                zMax = pDepth[j];
            }
        }
    }
    for (int i = 0; i < iDataSize; i++)
    {
        pGray[i] = Rescale(pDepth[i], zMax, zMin);
    }
}
