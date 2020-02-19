#include "STLModel.h"
using namespace std;

//��STLģ�͵�������Ƭ��������ת��Ϊһά��������
void STLModel::GetSlicingPlane(float *layerPlane, int& layerindex)
{
	//cout << "��ǰ��ĸ߶��ǣ�" << vecLayer[layerindex].zHeight << endl;
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