#include "STLModel.h"
using namespace std;

//将STL模型的三角面片向量数据转化为一维数组数据
void STLModel::GetSlicingPlane(float *layerPlane, int& layerindex)
{
	//cout << "当前层的高度是：" << vecLayer[layerindex].zHeight << endl;
	layerPlane[0] = GetXmin();
	layerPlane[1] = GetYmin();
	layerPlane[2] = vecLayer[layerindex].zHeight;
	layerPlane[3] = GetXmax();
	layerPlane[4] = GetYmin();
	layerPlane[5] = vecLayer[layerindex].zHeight;
	layerPlane[6] = GetXmax();
	layerPlane[7] = GetYmax();
	layerPlane[8] = vecLayer[layerindex].zHeight;
	layerPlane[9] = GetXmin();
	layerPlane[10] = GetYmax();
	layerPlane[11] = vecLayer[layerindex].zHeight;	
}