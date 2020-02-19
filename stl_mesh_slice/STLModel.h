#pragma once
#include "Model.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <learnopengl\shader.h>
#include <learnopengl\camera.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class STLModel
{
public:
	bool ReadSTLFile(const char* filename);//�ж�����һ������
	bool ReadASCII_stl(const char* filename);
	bool ReadBinary_stl(const char* filename);	
	int Getfacetnum();//������Ƭ����
	void GetVertexPosition(float *verPositon);//��vector����������ת��Ϊһά���������
	void GetLayerData(float* layerData); //������������������ת��Ϊһά����
	void GetLayerVertexPosition(float *layerverPositon, int& layerindex);//��ʾ��һ����ཻ��Ƭ
	void GetSlicingPlane(float *layerverPositon, int& layerindex);//��ʾ��һ�����Ƭƽ��
	void GetLayerFL(float *fl, int& layerindex);

	set<Vertex> SetVertex;//���㼯��
	set<Edge> SetEdge;//�ߵļ���

	vector<Vertex> vecVertex;//������
	vector<Edge> vecEdge;//������
	vector<Facet> vecFacet;//������		
	//2019-11-29: �޸Ĵ洢���ݽṹΪvector,��Ƭ�������
	STLModel():insectFacetData(layerNumber) {};
	vector<set<int>> insectFacetData;//�洢���в��ཻ��Ƭ
	vector<vector<int>> perlayerFacetLoop;//�洢�����ཻ��Ƭ��,������������
	vector<vector<vector<int>>> insectFacetLoop;//�洢���в���ཻ��Ƭ��
	vector<vector<int>> perlayerInsectEdge;//�洢�����ཻ�����ڽӱ�,������������
	vector<vector<vector<int>>> insectEdgeData;//�洢���в���ཻ�����ڽӱ�
	vector<vector<insectPoint>> perlayerInsectPoint;//�洢������Ƭ������,�������������Ľ��㼯
	vector<vector<vector<insectPoint>>> insectPointData;//�洢���в������������Ϣ

	//��Ƭ����
public:
	vector<Layer> vecLayer;//�洢��Ƭ����Ϣ
	void slice(int mode);
	void showinsectPoints();

	//����ĳһ����Ƭ�ı���������Ƭ��2�ָ�ʽ���ɲ�����������ɲ���������
	bool insectFacets(int mode); //modeΪ1�Էֲ����������modeΪ2�Էֲ���������
	bool getInsectFacetsWithThickness(const int& layernum);
	bool getInsectFacetsWithHeight(const int& layernum);
	int obtainFacetLoopDirect(int facetidx, int i, int& edgenumber);
	bool obtainFacetLoop(const int& layernum);
	vector<float> maxCusp;
	void getCuspHeightofLayer();
	float getVE();


	//չʾÿ����ཻ��Ƭ
	inline void displayInsectFacet()
	{
		for (size_t i = 0; i < insectFacetData.size(); i++)
		{
			set<int>::iterator ite = insectFacetData[i].begin();
			cout << "��" << i + 1 << "����ཻ��Ƭ��:";
			for (ite; ite != insectFacetData[i].end(); ite++)
			{
				cout<< *ite << "->";
			}
			cout <<"NULL" <<endl;
		}
	}	
	//չʾÿ����ཻ��Ƭ����Ƭ
	inline void displayFacetLoop()
	{
		for (size_t i = 0; i < insectFacetLoop.size(); i++)
		{
			cout << "��" << i + 1 << "��ĵ�";
			for (size_t j = 0; j < insectFacetLoop[i].size(); j++)
			{
				cout << j + 1 << "����Ƭ�����ཻ��Ƭ��";
				for (size_t k = 0; k < insectFacetLoop[i][j].size(); k++)
				{
					cout <<insectFacetLoop[i][j][k] << "->";
				}
				cout << "NULL" << endl;
			}
		}
	}
	
	//���2������Ƭ�Ĺ����ڽӱ�
	inline int getAdjedge(const int& f1, const int& f2)
	{
		int e = 0;
		if (vecFacet[f1].FacetIndex[0] != f2 && vecFacet[f1].FacetIndex[1] != f2 && vecFacet[f1].FacetIndex[2] != f2)
		{
			cout << "��Ƭ" << f1 << "����Ƭ" << f2 << "����������Ƭ" << endl;
			return -1;
		}
	    for (size_t i = 0; i < 3; i++)
		{					
			if (vecFacet[f1].FacetIndex[i] == f2)
			{
				e = vecFacet[f1].EdgeIndex[i];
				break;
			}
		}		
		return e;
	}

	//��ȡÿ���������߶�
	inline void showMaxcusp()
	{
		for (size_t i = 0; i < maxCusp.size(); i++)
		{
			//cout << "��" << i + 1 << "���������߶��ǣ�";
			cout << maxCusp[i] << endl;
		}
	}

private:
	Facet mfacet;
	int facetNum;//��Ƭ��
	int layerNumber = 5000;////�Բ��������Ƭ����
	float layerThickness;//��Ƭ���
public:
	//void drawSlice();
	//void drawAABBbox();//����AABB͹��Χ��
	//void drawOOBBbox();//����OOBB͹��Χ��
	//void drawSlicePlane();//������Ƭƽ��

public:
	//�洢ÿ��������ڽ���Ƭ����
	vector<vector<int>*> AdjacentFacesPerVertex;
	//�洢ÿ��������ڽӶ��㼯��
	vector<vector<int>*> AdjacentVerticesPerVertex;

	//����ÿһ��������ڽӶ���
	void CaculateAdjacentVerticesPerVertex();
	//����ÿһ������ڽ���Ƭ
	void CaculateAdjacentFacesPerVertex();
	
public:
	//ģ�ͼ�ֵ����Ϣ
	float GetXstroke();//��Χ��X�ߴ�
	float GetYstroke();//��Χ��Y�ߴ�
	float GetZstroke();//��Χ��Z�ߴ�
	float GetXac();
	float GetYac();
	float GetZac();
	float GetXmax();
	float GetYmax();
	float GetZmax();
	float GetXmin();
	float GetYmin();
	float GetZmin();
private:
	float Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;//ģ����󶥵�����ֵ
	float Xac, Yac, Zac;//ģ����������
	float Xstroke, Ystroke, Zstroke;//�ֲ��г�


};
