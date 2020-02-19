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
	bool ReadSTLFile(const char* filename);//判断是哪一种类型
	bool ReadASCII_stl(const char* filename);
	bool ReadBinary_stl(const char* filename);	
	int Getfacetnum();//返回面片数量
	void GetVertexPosition(float *verPositon);//将vector向量的数据转化为一维数组的数据
	void GetLayerData(float* layerData); //将单层轮廓交点数据转化为一维数组
	void GetLayerVertexPosition(float *layerverPositon, int& layerindex);//显示单一层的相交面片
	void GetSlicingPlane(float *layerverPositon, int& layerindex);//显示单一层的切片平面
	void GetLayerFL(float *fl, int& layerindex);

	set<Vertex> SetVertex;//顶点集合
	set<Edge> SetEdge;//边的集合

	vector<Vertex> vecVertex;//点容器
	vector<Edge> vecEdge;//边容器
	vector<Facet> vecFacet;//面容器		
	//2019-11-29: 修改存储数据结构为vector,切片相关容器
	STLModel():insectFacetData(layerNumber) {};
	vector<set<int>> insectFacetData;//存储所有层相交面片
	vector<vector<int>> perlayerFacetLoop;//存储单层相交面片环,包含单层多个环
	vector<vector<vector<int>>> insectFacetLoop;//存储所有层的相交面片环
	vector<vector<int>> perlayerInsectEdge;//存储单层相交公共邻接边,包含单层多个环
	vector<vector<vector<int>>> insectEdgeData;//存储所有层的相交公共邻接边
	vector<vector<insectPoint>> perlayerInsectPoint;//存储单层切片轮廓点,包含单层多个环的交点集
	vector<vector<vector<insectPoint>>> insectPointData;//存储所有层的轮廓交点信息

	//切片数据
public:
	vector<Layer> vecLayer;//存储切片层信息
	void slice(int mode);
	void showinsectPoints();

	//计算某一层切片的被切三角面片，2种格式，由层厚计算层数，由层数计算层厚
	bool insectFacets(int mode); //mode为1以分层层数计算层厚，mode为2以分层层厚计算层数
	bool getInsectFacetsWithThickness(const int& layernum);
	bool getInsectFacetsWithHeight(const int& layernum);
	int obtainFacetLoopDirect(int facetidx, int i, int& edgenumber);
	bool obtainFacetLoop(const int& layernum);
	vector<float> maxCusp;
	void getCuspHeightofLayer();
	float getVE();


	//展示每层的相交面片
	inline void displayInsectFacet()
	{
		for (size_t i = 0; i < insectFacetData.size(); i++)
		{
			set<int>::iterator ite = insectFacetData[i].begin();
			cout << "第" << i + 1 << "层的相交面片是:";
			for (ite; ite != insectFacetData[i].end(); ite++)
			{
				cout<< *ite << "->";
			}
			cout <<"NULL" <<endl;
		}
	}	
	//展示每层的相交面片环面片
	inline void displayFacetLoop()
	{
		for (size_t i = 0; i < insectFacetLoop.size(); i++)
		{
			cout << "第" << i + 1 << "层的第";
			for (size_t j = 0; j < insectFacetLoop[i].size(); j++)
			{
				cout << j + 1 << "个面片环的相交面片是";
				for (size_t k = 0; k < insectFacetLoop[i][j].size(); k++)
				{
					cout <<insectFacetLoop[i][j][k] << "->";
				}
				cout << "NULL" << endl;
			}
		}
	}
	
	//获得2相邻面片的公共邻接边
	inline int getAdjedge(const int& f1, const int& f2)
	{
		int e = 0;
		if (vecFacet[f1].FacetIndex[0] != f2 && vecFacet[f1].FacetIndex[1] != f2 && vecFacet[f1].FacetIndex[2] != f2)
		{
			cout << "面片" << f1 << "和面片" << f2 << "不是相邻面片" << endl;
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

	//获取每层的最大残余高度
	inline void showMaxcusp()
	{
		for (size_t i = 0; i < maxCusp.size(); i++)
		{
			//cout << "第" << i + 1 << "层的最大参与高度是：";
			cout << maxCusp[i] << endl;
		}
	}

private:
	Facet mfacet;
	int facetNum;//面片数
	int layerNumber = 5000;////以层厚计算的切片层数
	float layerThickness;//切片层厚
public:
	//void drawSlice();
	//void drawAABBbox();//绘制AABB凸包围盒
	//void drawOOBBbox();//绘制OOBB凸包围盒
	//void drawSlicePlane();//绘制切片平面

public:
	//存储每个顶点的邻接面片集合
	vector<vector<int>*> AdjacentFacesPerVertex;
	//存储每个顶点的邻接顶点集合
	vector<vector<int>*> AdjacentVerticesPerVertex;

	//计算每一个顶点的邻接顶点
	void CaculateAdjacentVerticesPerVertex();
	//计算每一个点的邻接面片
	void CaculateAdjacentFacesPerVertex();
	
public:
	//模型极值点信息
	float GetXstroke();//包围盒X尺寸
	float GetYstroke();//包围盒Y尺寸
	float GetZstroke();//包围盒Z尺寸
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
	float Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;//模型最大顶点坐标值
	float Xac, Yac, Zac;//模型中心坐标
	float Xstroke, Ystroke, Zstroke;//分层行程


};
