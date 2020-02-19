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
	 double e = 1E-5;//��������ȥ���жϾ���
	//һ���ж��Ƿ���ͬԪ��
	//bool operator < (const Vertex& v) const
	//{
	//	//�������������ȥ��
	//	if (abs(x - v.x) < e && abs(y - v.y) < e && abs(z - v.z) < e)
	//		return false;
	//	else//������������������
	//	{
	//		if (Vindex != v.Vindex)
	//		{
	//			return Vindex < v.Vindex;//����
	//			//return Vindex > v.Vindex;//����
	//		}
	//		else
	//			return false;
	//	}
	//}

	//��������set����Ԫ������
	bool operator < (const Vertex& ve) const
	{
		//����z,y,x��������,����Ϊ��ʱ�����
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
	int Vindex;//�����������
};

class LessVertX
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.x < v2.x;//�Ƚ϶����Xֵ
	}
};

class LessVertY
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.y < v2.y;//�Ƚ϶����Yֵ
	}
};

class LessVertZ
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.z < v2.z;//�Ƚ϶����Zֵ
	}
};

class LessVertIndex
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)
	{
		return v1.Vindex < v2.Vindex;//�Ƚ϶�����
	}
};

//�����ݽṹ
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
	//һ���жϱ���set�������Ƿ���ͬ
	//bool operator < (const Edge& e) const
	//{
	//	//���ߵĶ��������ͬ����ȥ��
	//	if ((Verindex0 == e.Verindex0 && Verindex1 == e.Verindex1) ||
	//		(Verindex1 == e.Verindex0 && Verindex0 == e.Verindex1))
	//		return false;
	//	else  //���ߵ���Ž�������
	//		if (Eindex != e.Eindex)
	//		{
	//			return Eindex < e.Eindex;//����
	//		  //return Vindex > v.Vindex;//����
	//		}
	//		else
	//			return false;
	//}

	//�����ȱȽ϶���
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
	int Eindex;//�ߵ�������
	int neiFacet0;//�ڽ���Ƭ��������
	int neiFacet1;//���������ߵ�������Ƭ
};

class LessEdgeIndex
{
public:
	bool operator()(const Edge& e1, const Edge& e2)
	{
		return e1.Eindex < e2.Eindex;
	}
};

//��Ƭ���ݽṹ
class Facet
{
public:
	//Ĭ������
	bool operator <(const Facet& f)
	{
		return Findex < f.Findex;//����
		//return f.Findex < Findex;//����
	}

	//�ж�ȥ����Ƭ
	bool operator == (const Facet& f)
	{
		return ((zcoor[0] == f.zcoor[0]) && (zcoor[1] == f.zcoor[1]) && (zcoor[2] == f.zcoor[2]));
	}

	//2020-2-1 ��z�������ʱ���޷�������ֵ�ж϶��������������ֱ�ӷ�������
	//��ȡ��Ƭ����Z������������� 0��1��2
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
	//2020-2-5,��zcoor[i]���ظ�ʱ���ᱻ�ж�Ϊ��ͬ��������
	inline int zmidIdx()
	{
		if (0 != zmaxIdx() && 0 != zminIdx())
			return 0;
		else if (1 != zmaxIdx() && 1 != zminIdx())
			return 1;
		else
			return 2;
	}

	//��ȡ��Ƭ����zֵ��Ӧ������ֵ
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
	//��ȡ��Ƭ����zֵ��Ӧ�Ķ�������
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
	int VertexIndex[3];//�����������
	float zcoor[3];//��Ƭ������z����

	int EdgeIndex[3];//�����ı�����
	int FacetIndex[3];//����Ƭ���ڵ�������Ƭ����
	float Normal[3];//��Ƭ������������
	int Findex;//��Ƭ��������
	bool insectFlag;//��Ƭ���д����־
};

//����Ƭ��������Z�����С��������Ƭ��������
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

//������Ƭ����Ƭ������
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
	float x, y, z; //ÿһ����Ƭ�Ͻ��������x��y,z
	int pointIndex;
	int edgeIndex; //�����ڽӱ߱��
};

//��Ƭ����
class Layer
{
public:
	Layer(int iLayer=1, float heightz=0):layerIndex(iLayer)//��ʼ�����캯����Ĭ�ϵ�һ������ײ㣬z����Ϊ0
	{
		layerIndex = iLayer;
		zHeight = heightz;
	}
	~Layer(){}

	//Ĭ�����򣬰�������������
	bool operator < (class Layer& layer)
	{
		return layerIndex < layer.layerIndex;
	}

	friend class STLModel;
private:
	int layerIndex;//��Ƭ����
	float zHeight;//���
};