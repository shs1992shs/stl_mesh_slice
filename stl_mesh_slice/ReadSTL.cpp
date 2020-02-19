#pragma once
#include "STLModel.h"
using namespace std;

bool STLModel::ReadSTLFile(const char* filename)
{
	//���ļ�filename
	ifstream in(filename, ios::in);
	if (in.is_open())
	{
		cout << "����ɹ������ڼ���" << "" << filename << "�ļ�" << endl;
	}
	else
		return false;
	string str1, str2;//��ȡͷ����
	getline(in, str1);
	getline(in, str2);
	in.close();

	int no_f = 0;      //ע��'\0'���ַ�����������' '����һ����ʾΪ�հ׵��ַ�
	while (str2[no_f] == ' ')//�жϵڶ����Ƿ�Ϊ���ַ�
	{
		no_f++;
	}
	ios::sync_with_stdio(false);//ȡ��iostream������������棬��ʡʱ��
	if (str1[0] == 's'&& str2[no_f] == 'f')//�ж������ָ�ʽ���ļ�
	{
		cout << "���ص�STL�ļ���ASCII��ʽ,���ڶ�ȡ..." << endl;
		ReadASCII_stl(filename);//��ȡASCII��ʽSTL
	}
	else
	{
		cout << "���ص�STL�ļ���Binary��ʽ�����ڶ�ȡ..." << endl;
		ReadBinary_stl(filename);//��ȡ������STL
	}
	ios::sync_with_stdio(true);
	return true;
}
//2019-11-27���޸Ļ�ȡ����ֵ����Щ����ֵ��0��������ʾȴ����0����������С��1e-7;�������ݽضϣ�ת��Ϊ����

//ASCII�ļ������з�ʽ���ж�ȡ
bool STLModel::ReadASCII_stl(const char* filename)
{
	facetNum = 0;//������Ƭ����
	float x, y, z;//��¼���������ֵ
	int i = 0, j = 0, cnt = 0;
	int V_index = 0;//��ʼ��������
	int E_index = 0;//��ʼ��������
	int F_index = 0;//��ʼ��������

	char a[100];
	char str[100];//��ȡÿһ�У���¼���ַ�����a��
	int nline = 0;//��¼��ȡ����Ч����
	ifstream in(filename, ios::in);//��ȡ�ļ�
	if (!in)
		return false;
	do
	{
		i = 0;
		cnt = 0;
		in.getline(a, 100, '\n');
		while (a[i] != '\0')
		{
			//��Сд��ĸ����д��ĸ�Ϳ��ַ�����������
			if (!islower((int)a[i]) && !isupper((int)a[i]) && a[i] != ' ')
				break;
			cnt++;
			i++;
		}
		while (a[cnt] != '\0')
		{
			str[j] = a[cnt];
			cnt++;
			j++;
		}
		str[j] = '\0';
		j = 0;
		//l(��ĸL)f��ʾdouble��f��ʾfloat
		if (sscanf_s(str, "%f%f%f", &x, &y, &z) == 3)
		{
			nline++;//��¼��ȡ������
			if (nline % 4 == 1)
			{
				mfacet.Normal[0] = x;
				mfacet.Normal[1] = y;
				mfacet.Normal[2] = z;
			}
			else
			{
				int k = nline % 4 - 2;//��ȡ�����
				if (k < 0)//4�ı�������ʱkΪ-2
					k = k + 4;
				Vertex v;
				v.x = ((int)(x * 1000+0.5)) *0.001f;
				v.y = ((int)(y * 1000+0.5)) *0.001f;
				v.z = ((int)(z * 1000+0.5)) *0.001f;

				set<Vertex>::iterator iteV;//���㼯�ϵĵ�����
				iteV = SetVertex.find(v);
				if (iteV != SetVertex.end())//�ǿ�
				{
					mfacet.VertexIndex[k] = iteV->Vindex;
					mfacet.zcoor[k] = iteV->z;
				}
				else
				{
					v.Vindex = V_index;//ʹ�ñ�����ֵ���ʸ���
					//v.Vindex = SetVertex.size();
					mfacet.zcoor[k] = v.z;
					mfacet.VertexIndex[k] = v.Vindex;
					V_index++;
					SetVertex.insert(v);
				}
			}
			if (nline % 4 == 0 && nline != 0)//�������ȡ���
			{
				//������Ƭ��������
				Edge edge1, edge2, edge3;
				//��һ����
				if (mfacet.VertexIndex[0] > mfacet.VertexIndex[1]) //�෴��
				{
					edge1.Verindex0 = mfacet.VertexIndex[0];
					edge1.Verindex1 = mfacet.VertexIndex[1];
				}
				else {
					edge1.Verindex0 = mfacet.VertexIndex[1];
					edge1.Verindex1 = mfacet.VertexIndex[0];
				}
				//edge1.neiFacet1 = F_index;//�Աߵĵڶ���Ԫ�ر�ʾΪ�ߵ�������Ƭ
				
				//�ڶ�����
				if ( mfacet.VertexIndex[0] > mfacet.VertexIndex[2]) //�෴��
				{
					edge2.Verindex0 = mfacet.VertexIndex[0];
					edge2.Verindex1 = mfacet.VertexIndex[2];
				}
				else {
					edge2.Verindex0 = mfacet.VertexIndex[2];
					edge2.Verindex1 = mfacet.VertexIndex[0];
				}

				//��������
				if (mfacet.VertexIndex[1] > mfacet.VertexIndex[2]) //�෴��
				{
					edge3.Verindex0 = mfacet.VertexIndex[1];
					edge3.Verindex1 = mfacet.VertexIndex[2];					
				}
				else {
					edge3.Verindex0 = mfacet.VertexIndex[2];
					edge3.Verindex1 = mfacet.VertexIndex[1];
				}

				set<Edge>::iterator iteEdge1;//�߼��ϵĵ�����
				set<Edge>::iterator iteEdge2;
				set<Edge>::iterator iteEdge3;

				iteEdge1 = SetEdge.find(edge1);
				if (iteEdge1 != SetEdge.end())
				{
					//�����ߵ�2���ڽ���Ƭ,ע�������ͬ�ģ�����˳��һ��
					edge1.neiFacet0 = iteEdge1->neiFacet0;
					edge1.neiFacet1 = F_index;
					edge1.Eindex = iteEdge1->Eindex;

					SetEdge.erase(iteEdge1);
					SetEdge.insert(edge1);					

				}
				else {
					edge1.neiFacet0= F_index;
					edge1.Eindex = E_index;//ʹ�ñ�����ֵ���ʸ���
					//edge1.Eindex = SetEdge.size();
									
					SetEdge.insert(edge1);
					E_index++;
				}

				iteEdge2 = SetEdge.find(edge2);
				if (iteEdge2 != SetEdge.end())
				{
					//�����ߵ�2���ڽ���Ƭ,ע�������ͬ�ģ�����˳��һ��
					edge2.neiFacet0 = iteEdge2->neiFacet0;
					edge2.neiFacet1 = F_index;
					edge2.Eindex = iteEdge2->Eindex;

					SetEdge.erase(iteEdge2);
					SetEdge.insert(edge2);
				}
				else {
					edge2.neiFacet0 = F_index;
					edge2.Eindex = E_index;//ʹ�ñ�����ֵ���ʸ���
					//edge1.Eindex = SetEdge.size();
					SetEdge.insert(edge2);
					E_index++;
				}

				iteEdge3 = SetEdge.find(edge3);
				if (iteEdge3 != SetEdge.end())
				{
					//�����ߵ�2���ڽ���Ƭ,ע�������ͬ�ģ�����˳��һ��
					edge3.neiFacet0 = iteEdge3->neiFacet0;
					edge3.neiFacet1 = F_index;
					edge3.Eindex = iteEdge3->Eindex;

					SetEdge.erase(iteEdge3);
					SetEdge.insert(edge3);
				}
				else {
					edge3.neiFacet0 = F_index;
					edge3.Eindex = E_index;//ʹ�ñ�����ֵ���ʸ���
					//edge1.Eindex = SetEdge.size();
					SetEdge.insert(edge3);
					E_index++;
				}

				mfacet.EdgeIndex[0] = edge1.Eindex;
				mfacet.EdgeIndex[1] = edge2.Eindex;
				mfacet.EdgeIndex[2] = edge3.Eindex;
				mfacet.Findex = F_index;
				//mfacet.Findex = vecFacet.size();
				vecFacet.push_back(mfacet);
				F_index++;		
			}
		}
	} while (!in.eof());
	facetNum = (int)vecFacet.size();
	in.close();

	//���Ƶ�vector������
	vecVertex.resize(SetVertex.size());//����ռ�
	vecEdge.resize(SetEdge.size());//����ռ�
	std::copy(SetVertex.begin(), SetVertex.end(), vecVertex.begin());
	std::copy(SetEdge.begin(), SetEdge.end(), vecEdge.begin());
	//������ֵ��������
	//std::sort(vecVertex.begin(), vecVertex.end());//Ĭ������
	std::sort(vecVertex.begin(), vecVertex.end(), LessVertIndex());
	//std::sort(vecEdge.begin(), vecEdge.end());//Ĭ������
	std::sort(vecEdge.begin(), vecEdge.end(), LessEdgeIndex());


	//��ȡ�������������Сֵ
	Xmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Xmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Ymin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Ymax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Zmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;
	Zmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;

	//������Ƭ�������ڽ���Ƭ
	vector<Facet>::iterator itefacet;
	for (itefacet = vecFacet.begin(); itefacet != vecFacet.end(); itefacet++)
	{
		for (int i = 0; i <= 2; i++)
		{
			//�����ж�
			if (vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0 == (*itefacet).Findex)
				(*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet1;
			else
			    (*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0;
		}
	}
	return true;
}

//��ȡ������STL�ļ�
bool STLModel::ReadBinary_stl(const char* filename)
{
	int i, j;
	int V_index = 0;//��ʼ��������
	int E_index = 0;//��ʼ��������
	int F_index = 0;//��ʼ��������
	char name[80];//ͷ80���ֽڣ��ļ���
	ifstream in(filename, ios::in | ios::binary);
	if (!in)
		return false;

	in.read(name, 80);//��ȡͷ�ļ�
	in.read((char*)&facetNum, sizeof(int));//��ȡ��Ƭ��
	if (facetNum == 0)
		return false;

	//��ʼѭ����ȡ������Ƭ����Ϣ
	for (i = 0; i < facetNum; i++)
	{
		//���ȶ�ȡ������
		//���뵽fnorms������
		float coorxyz[12];//�洢12�����ݣ�Ϊ3�����������꣬��3*3����������
		in.read((char*)&coorxyz, sizeof(float) * 12);//12��float�����ֽ��� sizeof(double)�ֽ���Ϊ8
		mfacet.Normal[0] = coorxyz[0];
		mfacet.Normal[1] = coorxyz[1];
		mfacet.Normal[2] = coorxyz[2];
		for (j = 1; j < 4; j++)
		{
			int k = j - 1;//��Ƭ�����������
			Vertex v;
			v.x = ((int)(coorxyz[j * 3] * 1000 + 0.5)) *0.001f;
			v.y = ((int)(coorxyz[j * 3+1] * 1000 + 0.5)) *0.001f;
			v.z = ((int)(coorxyz[j * 3+2] * 1000 + 0.5)) *0.001f;

			set<Vertex>::iterator iteV;//���㼯�ϵĵ�����
			iteV = SetVertex.find(v);
			if (iteV != SetVertex.end())//�ǿ�
			{
				mfacet.VertexIndex[k] = iteV->Vindex;
				mfacet.zcoor[k] = iteV->z;
			}
			else
			{
				v.Vindex = V_index;//ʹ�ñ�����ֵ���ʸ���
				//v.Vindex = SetVertex.size();
				mfacet.zcoor[k] = v.z;

				mfacet.VertexIndex[k] = v.Vindex;
				V_index++;
				SetVertex.insert(v);
			}
		}
		//��������ֽڵ�������Ϣ������
		in.read((char*)&coorxyz, 2);

		//������Ƭ��������
		Edge edge1, edge2, edge3;
		//��һ����
		if (mfacet.VertexIndex[0] > mfacet.VertexIndex[1]) //�෴��
		{
			edge1.Verindex0 = mfacet.VertexIndex[0];
			edge1.Verindex1 = mfacet.VertexIndex[1];
		}
		else {
			edge1.Verindex0 = mfacet.VertexIndex[1];
			edge1.Verindex1 = mfacet.VertexIndex[0];
		}
		//edge1.neiFacet1 = F_index;//�Աߵĵڶ���Ԫ�ر�ʾΪ�ߵ�������Ƭ

		//�ڶ�����
		if (mfacet.VertexIndex[0] > mfacet.VertexIndex[2]) //�෴��
		{
			edge2.Verindex0 = mfacet.VertexIndex[0];
			edge2.Verindex1 = mfacet.VertexIndex[2];
		}
		else {
			edge2.Verindex0 = mfacet.VertexIndex[2];
			edge2.Verindex1 = mfacet.VertexIndex[0];
		}

		//��������
		if (mfacet.VertexIndex[1] > mfacet.VertexIndex[2]) //�෴��
		{
			edge3.Verindex0 = mfacet.VertexIndex[1];
			edge3.Verindex1 = mfacet.VertexIndex[2];
		}
		else {
			edge3.Verindex0 = mfacet.VertexIndex[2];
			edge3.Verindex1 = mfacet.VertexIndex[1];
		}

		set<Edge>::iterator iteEdge1;//�߼��ϵĵ�����
		set<Edge>::iterator iteEdge2;
		set<Edge>::iterator iteEdge3;

		iteEdge1 = SetEdge.find(edge1);
		if (iteEdge1 != SetEdge.end())
		{
			//�����ߵ�2���ڽ���Ƭ,ע�������ͬ�ģ�����˳��һ��
			edge1.neiFacet0 = iteEdge1->neiFacet0;
			edge1.neiFacet1 = F_index;
			edge1.Eindex = iteEdge1->Eindex;

			SetEdge.erase(iteEdge1);
			SetEdge.insert(edge1);

		}
		else {
			edge1.neiFacet0 = F_index;
			edge1.Eindex = E_index;//ʹ�ñ�����ֵ���ʸ���
			//edge1.Eindex = SetEdge.size();

			SetEdge.insert(edge1);
			E_index++;
		}

		iteEdge2 = SetEdge.find(edge2);
		if (iteEdge2 != SetEdge.end())
		{
			//�����ߵ�2���ڽ���Ƭ,ע�������ͬ�ģ�����˳��һ��
			edge2.neiFacet0 = iteEdge2->neiFacet0;
			edge2.neiFacet1 = F_index;
			edge2.Eindex = iteEdge2->Eindex;

			SetEdge.erase(iteEdge2);
			SetEdge.insert(edge2);
		}
		else {
			edge2.neiFacet0 = F_index;
			edge2.Eindex = E_index;//ʹ�ñ�����ֵ���ʸ���
								   //edge1.Eindex = SetEdge.size();
			SetEdge.insert(edge2);
			E_index++;
		}

		iteEdge3 = SetEdge.find(edge3);
		if (iteEdge3 != SetEdge.end())
		{
			//�����ߵ�2���ڽ���Ƭ,ע�������ͬ�ģ�����˳��һ��
			edge3.neiFacet0 = iteEdge3->neiFacet0;
			edge3.neiFacet1 = F_index;
			edge3.Eindex = iteEdge3->Eindex;

			SetEdge.erase(iteEdge3);
			SetEdge.insert(edge3);
		}
		else {
			edge3.neiFacet0 = F_index;
			edge3.Eindex = E_index;//ʹ�ñ�����ֵ���ʸ���
			 //edge1.Eindex = SetEdge.size();
			SetEdge.insert(edge3);
			E_index++;
		}
		
		mfacet.EdgeIndex[0] = edge1.Eindex;
		mfacet.EdgeIndex[1] = edge2.Eindex;
		mfacet.EdgeIndex[2] = edge3.Eindex;
		mfacet.Findex = F_index;//������ֵ���ʸ���
		//mfacet.Findex = vecFacet.size();
		vecFacet.push_back(mfacet);	
        F_index++;
	}
	in.close();

	//���Ƶ�vector������
	vecVertex.resize(SetVertex.size());//����ռ�
	vecEdge.resize(SetEdge.size());//����ռ�
	std::copy(SetVertex.begin(), SetVertex.end(), vecVertex.begin());
	std::copy(SetEdge.begin(), SetEdge.end(), vecEdge.begin());
	//������ֵ��������
	//std::sort(vecVertex.begin(), vecVertex.end());//Ĭ������
	std::sort(vecVertex.begin(), vecVertex.end(), LessVertIndex());
	//std::sort(vecEdge.begin(), vecEdge.end());//Ĭ������
	std::sort(vecEdge.begin(), vecEdge.end(), LessEdgeIndex());

	//��ȡ�������������Сֵ
	Xmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Xmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Ymin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Ymax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Zmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;
	Zmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;

	//������Ƭ�������ڽ���Ƭ
	vector<Facet>::iterator itefacet;
	for (itefacet = vecFacet.begin(); itefacet != vecFacet.end(); itefacet++)
	{
		for (int i = 0; i < 3; i++)
		{
			//�����ж�
			if (vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0 == (*itefacet).Findex)
				(*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet1;
			else
				(*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0;
		}
	}
	return true;
}

//��ȡ������Ƭ�� 
int STLModel::Getfacetnum()
{
	return facetNum;
}

float STLModel::GetXstroke()
{
	Xstroke = GetXmax() - GetXmin();
	return Xstroke;
}
float STLModel::GetYstroke()
{
	Ystroke = GetYmax() - GetYmin();
	return Ystroke;
}
float STLModel::GetZstroke()
{
	Zstroke = GetZmax() - GetZmin();
	return Zstroke;
}
float STLModel::GetXac()
{
	Xac = (GetXmax() + GetXmin()) / 2;
	return Xac;
}
float STLModel::GetYac()
{
	Yac = (GetYmax() + GetYmin()) / 2;
	return Yac;
}
float STLModel::GetZac()
{
	Zac = (GetZmax() + GetZmin()) / 2;
	return Zac;
}
float STLModel::GetXmax()
{
	return Xmax;
}
float STLModel::GetYmax()
{
	return Ymax;
}
float STLModel::GetZmax()
{
	return Zmax;
}
float STLModel::GetXmin()
{
	return Xmin;
}
float STLModel::GetYmin()
{
	return Ymin;
}
float STLModel::GetZmin()
{
	return Zmin;
}
