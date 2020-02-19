#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <set>
#include <cmath>

using namespace std;

#ifndef ourmax
#define ourmax(a,b) (((a) > (b)) ? (a) : (b))
#endif

class Vertex
{
public:
	Vertex(int Veindex =0, float vx=0, float vy=0, float vz=0):Vindex(Veindex)
	{
		x = vx;
		y = vy;
		z = vz;
	}
	~Vertex(){}
	 double e = 1E-5;//顶点冗余去除判断精度
	//一、判断是否相同元素
	//bool operator < (const Vertex& v) const
	//{
	//	//按顶点坐标进行去重
	//	if (abs(x - v.x) < e && abs(y - v.y) < e && abs(z - v.z) < e)
	//		return false;
	//	else//按顶点索引进行排序
	//	{
	//		if (Vindex != v.Vindex)
	//		{
	//			return Vindex < v.Vindex;//升序
	//			//return Vindex > v.Vindex;//降序
	//		}
	//		else
	//			return false;
	//	}
	//}

	//二、进行set容器元素排序
	bool operator < (const Vertex& ve) const
	{
		//按照z,y,x进行排序,顶点为逆时针进行
		if (z < ve.z)
			return true;
		else if (z == ve.z)
		{
			if (y < ve.y)
				return true;
			else if (y == ve.y)
			{
				if (x < ve.x)
					return true;
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
		
	friend class  LessVertX;
	friend class  LessVertY;
	friend class  LessVertZ;
	friend class STLModel;
	friend class LessVertIndex;
private:
	float x;
	float y;
	float z;
	int Vindex;//顶点的索引号
};

class LessVertX
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.x < v2.x;//比较顶点的X值
	}
};

class LessVertY
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.y < v2.y;//比较顶点的Y值
	}
};

class LessVertZ
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.z < v2.z;//比较顶点的Z值
	}
};

class LessVertIndex
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.Vindex < v2.Vindex;//比较顶点编号
	}
};

//边数据结构
class Edge
{
public:
	Edge(int eindex=0, int Vertexindex1=0, int Vertexindex2=0, int neiborFacet1=0, int neiborFacet2=0):Eindex(eindex)
	{		
		Verindex0 = Vertexindex1;
		Verindex1 = Vertexindex2;
		neiFacet0 = neiborFacet1;
		neiFacet1 = neiborFacet2;
	}
	~Edge(){}
	//一、判断边在set容器中是否相同
	//bool operator < (const Edge& e) const
	//{
	//	//按边的顶点序号相同进行去重
	//	if ((Verindex0 == e.Verindex0 && Verindex1 == e.Verindex1) ||
	//		(Verindex1 == e.Verindex0 && Verindex0 == e.Verindex1))
	//		return false;
	//	else  //按边的序号进行排序
	//		if (Eindex != e.Eindex)
	//		{
	//			return Eindex < e.Eindex;//升序
	//		  //return Vindex > v.Vindex;//降序
	//		}
	//		else
	//			return false;
	//}

	//二、先比较顶点
	bool operator < (const Edge& ed) const
	{
		if (Verindex0 < ed.Verindex0)
			return true;
		else if (Verindex0 == ed.Verindex0 )
		{
			if (Verindex1 < ed.Verindex1)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	friend class LessEdgeIndex;
	friend class STLModel;

private:
	int Verindex0;
	int Verindex1;
	int Eindex;//边的索引号
	int neiFacet0;//邻接面片的索引号
	int neiFacet1;//创建三条边的所在面片
};

class LessEdgeIndex
{
public:
	bool operator()(const Edge& e1, const Edge& e2)
	{
		return e1.Eindex < e2.Eindex;
	}
};

//面片数据结构
class Facet
{
public:
	//默认排序
	bool operator <(const Facet& f)
	{
		return Findex < f.Findex;//升序
		//return f.Findex < Findex;//降序
	}

	//判断去重面片
	bool operator == (const Facet& f)
	{
		return ((zcoor[0] == f.zcoor[0]) && (zcoor[1] == f.zcoor[1]) && (zcoor[2] == f.zcoor[2]));
	}

	//2020-2-1 当z坐标相等时，无法用坐标值判断顶点的索引，尝试直接返回索引
	//获取面片顶点Z坐标数组的索引 0，1，2
	inline int zmaxIdx()
	{
		if (zcoor[0] >= zcoor[1] && zcoor[0] >= zcoor[2])
			return 0;
		else if (zcoor[1] >= zcoor[0] && zcoor[1] >= zcoor[2])
			return 1;
		else
			return 2;
	}

	inline int zminIdx()
	{
		if (zcoor[0] <= zcoor[1] && zcoor[0] <= zcoor[2])
			return 0;
		else if (zcoor[1] <= zcoor[0] && zcoor[1] <= zcoor[2])
			return 1;
		else
			return 2;
	}
	//2020-2-5,当zcoor[i]有重复时，会被判断为相同的索引号
	inline int zmidIdx()
	{
		if (0 != zmaxIdx() && 0 != zminIdx())
			return 0;
		else if (1 != zmaxIdx() && 1 != zminIdx())
			return 1;
		else
			return 2;
	}

	//获取面片顶点z值对应的坐标值
	inline float getFZmax()
	{			
		return zcoor[this->zmaxIdx()];
	}
	inline float getFZmid()
	{
		return zcoor[this->zmidIdx()];
	}
	inline float getFZmin()
	{
		return zcoor[this->zminIdx()];
	}
	//获取面片顶点z值对应的顶点索引
	inline int getFZmaxVid()
	{
		return VertexIndex[this->zmaxIdx()];
	}
	inline int getFZmidVid()
	{
		return VertexIndex[this->zmidIdx()];
	}
	inline int getFZminVid()
	{
		return VertexIndex[this->zminIdx()];
	}

	friend class STLModel;
	friend class LessFacetZcoor;
	
private:
	int VertexIndex[3];//三个点的索引
	float zcoor[3];//面片的三个z坐标

	int EdgeIndex[3];//所属的边索引
	int FacetIndex[3];//该面片相邻的三个面片索引
	float Normal[3];//面片法向量的坐标
	int Findex;//面片的索引号
	bool insectFlag;//面片相切处理标志
};

//按面片三个顶点Z坐标大小对三角面片进行排序
class LessFacetZcoor
{
public:
	bool operator()( Facet& f1, Facet& f2)
	{
		if (f1.getFZmin() != f2.getFZmin())
			return f1.getFZmin() < f2.getFZmin();
		else if (f1.getFZmax() != f2.getFZmax())
			return f1.getFZmax() < f2.getFZmax();
		else
			return false;
	}
};

//三角面片与切片交点类
class insectPoint
{
public:
	insectPoint(int Vindex = 0,float _x = 0, float _y = 0, float _z = 0,int number = 0):edgeIndex(0)
	{
		x = _x;
		y = _y;
		z = _z;
		pointIndex = Vindex;
	}
	friend class STLModel;
private:
	float x, y, z; //每一层切片上交点的坐标x，y,z
	int pointIndex;
	int edgeIndex; //公共邻接边编号
};

//切片层类
class Layer
{
public:
	Layer(int iLayer=1, float heightz=0):layerIndex(iLayer)//初始化构造函数，默认第一层是最底层，z坐标为0
	{
		layerIndex = iLayer;
		zHeight = heightz;
	}
	~Layer(){}

	//默认排序，按层编号升序排序
	bool operator < (class Layer& layer)
	{
		return layerIndex < layer.layerIndex;
	}

	friend class STLModel;
private:
	int layerIndex;//切片层编号
	float zHeight;//层高
};