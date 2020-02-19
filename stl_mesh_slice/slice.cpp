#pragma once
#include "STLModel.h"

//����Ƭƽ��߶���ȡ��������	
void STLModel::slice(int mode)
{
	insectFacets(mode);//ѡ�����ַ�ʽ��Ƭ�ֲ㣬1�Էֲ����������2���Էֲ���������
	vector<insectPoint> perLoopInsectp;//�����������Ľ���
	insectPointData.reserve(layerNumber);
	for (size_t i = 0; i < insectEdgeData.size(); i++)
	{
		int point_Index = 0;
		perlayerInsectPoint.clear();
		perlayerInsectPoint.reserve(insectEdgeData[i].size());
		for (size_t j = 0; j < insectEdgeData[i].size(); j++)
		{
			for (size_t k = 0; k < insectEdgeData[i][j].size(); k++)
			{
				insectPoint p;			
				int m = vecEdge[insectEdgeData[i][j][k]].Verindex0;
				int n = vecEdge[insectEdgeData[i][j][k]].Verindex1;
				if (vecVertex[m].z == vecLayer[i].zHeight)
				{
					p.x = vecVertex[m].x;
					p.y = vecVertex[m].y;
					//p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
					p.z = vecVertex[m].z;//�޸�Ϊ���������
				}
				else if (vecVertex[n].z == vecLayer[i].zHeight)
				{
					p.x = vecVertex[n].x;
					p.y = vecVertex[n].y;
					//p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
					p.z = vecVertex[n].z;//�޸�Ϊ���������
				}
				else {
					p.x = (vecVertex[m].x - vecVertex[n].x)*(vecLayer[i].zHeight - vecVertex[n].z) / (vecVertex[m].z - vecVertex[n].z) + vecVertex[n].x;
					p.y = (vecVertex[m].y - vecVertex[n].y)*(vecLayer[i].zHeight - vecVertex[n].z) / (vecVertex[m].z - vecVertex[n].z) + vecVertex[n].y;
					p.z = vecLayer[i].zHeight;//������Ƭƽ��߶�
				}
				p.edgeIndex = vecEdge[insectEdgeData[i][j][k]].Eindex;
				p.pointIndex = point_Index;				
				point_Index++;
				perLoopInsectp.push_back(p);
			}
			perlayerInsectPoint.push_back(perLoopInsectp);
			perLoopInsectp.clear();
		}
		insectPointData.push_back(perlayerInsectPoint);
	}	
}

void STLModel::showinsectPoints()
{
	for (size_t i = 0; i < insectPointData.size(); i++)
	{
		for (size_t j = 0; j < insectPointData[i].size(); j++)
		{
			for (size_t k = 0; k < insectPointData[i][j].size(); k++)
			{
				cout << "��" << i + 1 << "��ĵ�" << j + 1 << "����Ƭ���ĵ�" << k + 1 << "����ı���ǣ�" << insectPointData[i][j][k].pointIndex 
					<< "�����ǣ�"<<insectPointData[i][j][k].x << " " << insectPointData[i][j][k].y << " " << insectPointData[i][j][k].z << endl;
			}
		}
	}
}

//��STLģ�͵�������Ƭ��������ת��Ϊһά��������
void STLModel::GetVertexPosition(float *verPositon)
{
	for (int i = 0; i <(int)vecFacet.size(); i++)
	{
		verPositon[i * 9] = vecVertex[vecFacet[i].VertexIndex[0]].x;
		verPositon[i * 9 + 1] = vecVertex[vecFacet[i].VertexIndex[0]].y;
		verPositon[i * 9 + 2] = vecVertex[vecFacet[i].VertexIndex[0]].z;
		verPositon[i * 9 + 3] = vecVertex[vecFacet[i].VertexIndex[1]].x;
		verPositon[i * 9 + 4] = vecVertex[vecFacet[i].VertexIndex[1]].y;
		verPositon[i * 9 + 5] = vecVertex[vecFacet[i].VertexIndex[1]].z;
		verPositon[i * 9 + 6] = vecVertex[vecFacet[i].VertexIndex[2]].x;
		verPositon[i * 9 + 7] = vecVertex[vecFacet[i].VertexIndex[2]].y;
		verPositon[i * 9 + 8] = vecVertex[vecFacet[i].VertexIndex[2]].z;
	}
}

//��STLģ�͵�������Ƭ��������ת��Ϊһά��������
void STLModel::GetLayerVertexPosition(float *layerverPositon, int& layerindex)
{
	int i = 0;
	set<int>::iterator ite = insectFacetData[layerindex].begin();
	for (ite; ite != insectFacetData[layerindex].end(); ite++)
	{
		layerverPositon[i * 9] = vecVertex[vecFacet[*ite].VertexIndex[0]].x;
		layerverPositon[i * 9 + 1] = vecVertex[vecFacet[*ite].VertexIndex[0]].y;
		layerverPositon[i * 9 + 2] = vecVertex[vecFacet[*ite].VertexIndex[0]].z;
		layerverPositon[i * 9 + 3] = vecVertex[vecFacet[*ite].VertexIndex[1]].x;
		layerverPositon[i * 9 + 4] = vecVertex[vecFacet[*ite].VertexIndex[1]].y;
		layerverPositon[i * 9 + 5] = vecVertex[vecFacet[*ite].VertexIndex[1]].z;
		layerverPositon[i * 9 + 6] = vecVertex[vecFacet[*ite].VertexIndex[2]].x;
		layerverPositon[i * 9 + 7] = vecVertex[vecFacet[*ite].VertexIndex[2]].y;
		layerverPositon[i * 9 + 8] = vecVertex[vecFacet[*ite].VertexIndex[2]].z;
		i++;
	}
}

void STLModel::GetLayerFL(float *fl, int& layerindex)
{
	int cnt = 0;
	for (int i = 0; i < insectFacetLoop[layerindex].size(); i++)
	//for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < insectFacetLoop[layerindex][i].size(); j++)
		{
			fl[cnt + 0] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[0]].x;
			fl[cnt + 1] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[0]].y;
			fl[cnt + 2] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[0]].z;
			fl[cnt + 3] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[1]].x;
			fl[cnt + 4] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[1]].y;
			fl[cnt + 5] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[1]].z;
			fl[cnt + 6] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[2]].x;
			fl[cnt + 7] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[2]].y;
			fl[cnt + 8] = vecVertex[vecFacet[insectFacetLoop[layerindex][i][j]].VertexIndex[2]].z;
			cnt += 9;
		}
	}
}

//����Ƭ����������ת��Ϊһά������ʽ
void STLModel::GetLayerData(float* layerData)
{
	int count = 0;
	for (unsigned long i = 0; i < insectPointData.size(); i++)
	{
		for (unsigned long j = 0; j < insectPointData[i].size(); j++)
		{
			for (unsigned long k = 0; k < insectPointData[i][j].size(); k++)
			{
				layerData[count + 0] = insectPointData[i][j][k].x;
				layerData[count + 1] = insectPointData[i][j][k].y;
				layerData[count + 2] = insectPointData[i][j][k].z;
				count += 3;
			}
		}
	}
}
