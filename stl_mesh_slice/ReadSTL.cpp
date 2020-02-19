#pragma once
#include "STLModel.h"
using namespace std;

bool STLModel::ReadSTLFile(const char* filename)
{
	//打开文件filename
	ifstream in(filename, ios::in);
	if (in.is_open())
	{
		cout << "输入成功，正在加载" << "" << filename << "文件" << endl;
	}
	else
		return false;
	string str1, str2;//获取头两行
	getline(in, str1);
	getline(in, str2);
	in.close();

	int no_f = 0;      //注意'\0'是字符串结束符，' '代表一个显示为空白的字符
	while (str2[no_f] == ' ')//判断第二行是否为空字符
	{
		no_f++;
	}
	ios::sync_with_stdio(false);//取消iostream的输入输出缓存，节省时间
	if (str1[0] == 's'&& str2[no_f] == 'f')//判断是哪种格式的文件
	{
		cout << "加载的STL文件是ASCII格式,正在读取..." << endl;
		ReadASCII_stl(filename);//读取ASCII格式STL
	}
	else
	{
		cout << "加载的STL文件是Binary格式，正在读取..." << endl;
		ReadBinary_stl(filename);//读取二进制STL
	}
	ios::sync_with_stdio(true);
	return true;
}
//2019-11-27，修改获取坐标值，有些坐标值是0，但是显示却不是0，浮点数很小，1e-7;采用数据截断，转换为整型

//ASCII文件以逐行方式进行读取
bool STLModel::ReadASCII_stl(const char* filename)
{
	facetNum = 0;//三角面片计数
	float x, y, z;//记录顶点的坐标值
	int i = 0, j = 0, cnt = 0;
	int V_index = 0;//初始化点索引
	int E_index = 0;//初始化边索引
	int F_index = 0;//初始化面索引

	char a[100];
	char str[100];//读取每一行，记录到字符数组a中
	int nline = 0;//记录读取的有效行数
	ifstream in(filename, ios::in);//读取文件
	if (!in)
		return false;
	do
	{
		i = 0;
		cnt = 0;
		in.getline(a, 100, '\n');
		while (a[i] != '\0')
		{
			//非小写字母、大写字母和空字符，即是数字
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
		//l(字母L)f表示double，f表示float
		if (sscanf_s(str, "%f%f%f", &x, &y, &z) == 3)
		{
			nline++;//记录读取的行数
			if (nline % 4 == 1)
			{
				mfacet.Normal[0] = x;
				mfacet.Normal[1] = y;
				mfacet.Normal[2] = z;
			}
			else
			{
				int k = nline % 4 - 2;//读取点计数
				if (k < 0)//4的倍数，此时k为-2
					k = k + 4;
				Vertex v;
				v.x = ((int)(x * 1000+0.5)) *0.001f;
				v.y = ((int)(y * 1000+0.5)) *0.001f;
				v.z = ((int)(z * 1000+0.5)) *0.001f;

				set<Vertex>::iterator iteV;//顶点集合的迭代器
				iteV = SetVertex.find(v);
				if (iteV != SetVertex.end())//非空
				{
					mfacet.VertexIndex[k] = iteV->Vindex;
					mfacet.zcoor[k] = iteV->z;
				}
				else
				{
					v.Vindex = V_index;//使用变量赋值访问更快
					//v.Vindex = SetVertex.size();
					mfacet.zcoor[k] = v.z;
					mfacet.VertexIndex[k] = v.Vindex;
					V_index++;
					SetVertex.insert(v);
				}
			}
			if (nline % 4 == 0 && nline != 0)//三个点读取完毕
			{
				//创建面片的三条边
				Edge edge1, edge2, edge3;
				//第一条边
				if (mfacet.VertexIndex[0] > mfacet.VertexIndex[1]) //相反边
				{
					edge1.Verindex0 = mfacet.VertexIndex[0];
					edge1.Verindex1 = mfacet.VertexIndex[1];
				}
				else {
					edge1.Verindex0 = mfacet.VertexIndex[1];
					edge1.Verindex1 = mfacet.VertexIndex[0];
				}
				//edge1.neiFacet1 = F_index;//以边的第二个元素表示为边的所在面片
				
				//第二条边
				if ( mfacet.VertexIndex[0] > mfacet.VertexIndex[2]) //相反边
				{
					edge2.Verindex0 = mfacet.VertexIndex[0];
					edge2.Verindex1 = mfacet.VertexIndex[2];
				}
				else {
					edge2.Verindex0 = mfacet.VertexIndex[2];
					edge2.Verindex1 = mfacet.VertexIndex[0];
				}

				//第三条边
				if (mfacet.VertexIndex[1] > mfacet.VertexIndex[2]) //相反边
				{
					edge3.Verindex0 = mfacet.VertexIndex[1];
					edge3.Verindex1 = mfacet.VertexIndex[2];					
				}
				else {
					edge3.Verindex0 = mfacet.VertexIndex[2];
					edge3.Verindex1 = mfacet.VertexIndex[1];
				}

				set<Edge>::iterator iteEdge1;//边集合的迭代器
				set<Edge>::iterator iteEdge2;
				set<Edge>::iterator iteEdge3;

				iteEdge1 = SetEdge.find(edge1);
				if (iteEdge1 != SetEdge.end())
				{
					//创建边的2个邻接面片,注意边是相同的，顶点顺序一样
					edge1.neiFacet0 = iteEdge1->neiFacet0;
					edge1.neiFacet1 = F_index;
					edge1.Eindex = iteEdge1->Eindex;

					SetEdge.erase(iteEdge1);
					SetEdge.insert(edge1);					

				}
				else {
					edge1.neiFacet0= F_index;
					edge1.Eindex = E_index;//使用变量赋值访问更快
					//edge1.Eindex = SetEdge.size();
									
					SetEdge.insert(edge1);
					E_index++;
				}

				iteEdge2 = SetEdge.find(edge2);
				if (iteEdge2 != SetEdge.end())
				{
					//创建边的2个邻接面片,注意边是相同的，顶点顺序一样
					edge2.neiFacet0 = iteEdge2->neiFacet0;
					edge2.neiFacet1 = F_index;
					edge2.Eindex = iteEdge2->Eindex;

					SetEdge.erase(iteEdge2);
					SetEdge.insert(edge2);
				}
				else {
					edge2.neiFacet0 = F_index;
					edge2.Eindex = E_index;//使用变量赋值访问更快
					//edge1.Eindex = SetEdge.size();
					SetEdge.insert(edge2);
					E_index++;
				}

				iteEdge3 = SetEdge.find(edge3);
				if (iteEdge3 != SetEdge.end())
				{
					//创建边的2个邻接面片,注意边是相同的，顶点顺序一样
					edge3.neiFacet0 = iteEdge3->neiFacet0;
					edge3.neiFacet1 = F_index;
					edge3.Eindex = iteEdge3->Eindex;

					SetEdge.erase(iteEdge3);
					SetEdge.insert(edge3);
				}
				else {
					edge3.neiFacet0 = F_index;
					edge3.Eindex = E_index;//使用变量赋值访问更快
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

	//复制到vector容器中
	vecVertex.resize(SetVertex.size());//申请空间
	vecEdge.resize(SetEdge.size());//申请空间
	std::copy(SetVertex.begin(), SetVertex.end(), vecVertex.begin());
	std::copy(SetEdge.begin(), SetEdge.end(), vecEdge.begin());
	//按索引值进行排序
	//std::sort(vecVertex.begin(), vecVertex.end());//默认排序
	std::sort(vecVertex.begin(), vecVertex.end(), LessVertIndex());
	//std::sort(vecEdge.begin(), vecEdge.end());//默认排序
	std::sort(vecEdge.begin(), vecEdge.end(), LessEdgeIndex());


	//求取顶点坐标最大最小值
	Xmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Xmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Ymin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Ymax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Zmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;
	Zmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;

	//构建面片的三个邻接面片
	vector<Facet>::iterator itefacet;
	for (itefacet = vecFacet.begin(); itefacet != vecFacet.end(); itefacet++)
	{
		for (int i = 0; i <= 2; i++)
		{
			//增加判断
			if (vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0 == (*itefacet).Findex)
				(*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet1;
			else
			    (*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0;
		}
	}
	return true;
}

//读取二进制STL文件
bool STLModel::ReadBinary_stl(const char* filename)
{
	int i, j;
	int V_index = 0;//初始化点索引
	int E_index = 0;//初始化边索引
	int F_index = 0;//初始化面索引
	char name[80];//头80个字节，文件名
	ifstream in(filename, ios::in | ios::binary);
	if (!in)
		return false;

	in.read(name, 80);//读取头文件
	in.read((char*)&facetNum, sizeof(int));//读取面片数
	if (facetNum == 0)
		return false;

	//开始循环读取各个面片的信息
	for (i = 0; i < facetNum; i++)
	{
		//首先读取法向量
		//输入到fnorms容器中
		float coorxyz[12];//存储12个数据，为3个法向量坐标，和3*3个顶点坐标
		in.read((char*)&coorxyz, sizeof(float) * 12);//12个float类型字节量 sizeof(double)字节数为8
		mfacet.Normal[0] = coorxyz[0];
		mfacet.Normal[1] = coorxyz[1];
		mfacet.Normal[2] = coorxyz[2];
		for (j = 1; j < 4; j++)
		{
			int k = j - 1;//面片顶点数组计数
			Vertex v;
			v.x = ((int)(coorxyz[j * 3] * 1000 + 0.5)) *0.001f;
			v.y = ((int)(coorxyz[j * 3+1] * 1000 + 0.5)) *0.001f;
			v.z = ((int)(coorxyz[j * 3+2] * 1000 + 0.5)) *0.001f;

			set<Vertex>::iterator iteV;//顶点集合的迭代器
			iteV = SetVertex.find(v);
			if (iteV != SetVertex.end())//非空
			{
				mfacet.VertexIndex[k] = iteV->Vindex;
				mfacet.zcoor[k] = iteV->z;
			}
			else
			{
				v.Vindex = V_index;//使用变量赋值访问更快
				//v.Vindex = SetVertex.size();
				mfacet.zcoor[k] = v.z;

				mfacet.VertexIndex[k] = v.Vindex;
				V_index++;
				SetVertex.insert(v);
			}
		}
		//最后两个字节的属性信息，无用
		in.read((char*)&coorxyz, 2);

		//创建面片的三条边
		Edge edge1, edge2, edge3;
		//第一条边
		if (mfacet.VertexIndex[0] > mfacet.VertexIndex[1]) //相反边
		{
			edge1.Verindex0 = mfacet.VertexIndex[0];
			edge1.Verindex1 = mfacet.VertexIndex[1];
		}
		else {
			edge1.Verindex0 = mfacet.VertexIndex[1];
			edge1.Verindex1 = mfacet.VertexIndex[0];
		}
		//edge1.neiFacet1 = F_index;//以边的第二个元素表示为边的所在面片

		//第二条边
		if (mfacet.VertexIndex[0] > mfacet.VertexIndex[2]) //相反边
		{
			edge2.Verindex0 = mfacet.VertexIndex[0];
			edge2.Verindex1 = mfacet.VertexIndex[2];
		}
		else {
			edge2.Verindex0 = mfacet.VertexIndex[2];
			edge2.Verindex1 = mfacet.VertexIndex[0];
		}

		//第三条边
		if (mfacet.VertexIndex[1] > mfacet.VertexIndex[2]) //相反边
		{
			edge3.Verindex0 = mfacet.VertexIndex[1];
			edge3.Verindex1 = mfacet.VertexIndex[2];
		}
		else {
			edge3.Verindex0 = mfacet.VertexIndex[2];
			edge3.Verindex1 = mfacet.VertexIndex[1];
		}

		set<Edge>::iterator iteEdge1;//边集合的迭代器
		set<Edge>::iterator iteEdge2;
		set<Edge>::iterator iteEdge3;

		iteEdge1 = SetEdge.find(edge1);
		if (iteEdge1 != SetEdge.end())
		{
			//创建边的2个邻接面片,注意边是相同的，顶点顺序一样
			edge1.neiFacet0 = iteEdge1->neiFacet0;
			edge1.neiFacet1 = F_index;
			edge1.Eindex = iteEdge1->Eindex;

			SetEdge.erase(iteEdge1);
			SetEdge.insert(edge1);

		}
		else {
			edge1.neiFacet0 = F_index;
			edge1.Eindex = E_index;//使用变量赋值访问更快
			//edge1.Eindex = SetEdge.size();

			SetEdge.insert(edge1);
			E_index++;
		}

		iteEdge2 = SetEdge.find(edge2);
		if (iteEdge2 != SetEdge.end())
		{
			//创建边的2个邻接面片,注意边是相同的，顶点顺序一样
			edge2.neiFacet0 = iteEdge2->neiFacet0;
			edge2.neiFacet1 = F_index;
			edge2.Eindex = iteEdge2->Eindex;

			SetEdge.erase(iteEdge2);
			SetEdge.insert(edge2);
		}
		else {
			edge2.neiFacet0 = F_index;
			edge2.Eindex = E_index;//使用变量赋值访问更快
								   //edge1.Eindex = SetEdge.size();
			SetEdge.insert(edge2);
			E_index++;
		}

		iteEdge3 = SetEdge.find(edge3);
		if (iteEdge3 != SetEdge.end())
		{
			//创建边的2个邻接面片,注意边是相同的，顶点顺序一样
			edge3.neiFacet0 = iteEdge3->neiFacet0;
			edge3.neiFacet1 = F_index;
			edge3.Eindex = iteEdge3->Eindex;

			SetEdge.erase(iteEdge3);
			SetEdge.insert(edge3);
		}
		else {
			edge3.neiFacet0 = F_index;
			edge3.Eindex = E_index;//使用变量赋值访问更快
			 //edge1.Eindex = SetEdge.size();
			SetEdge.insert(edge3);
			E_index++;
		}
		
		mfacet.EdgeIndex[0] = edge1.Eindex;
		mfacet.EdgeIndex[1] = edge2.Eindex;
		mfacet.EdgeIndex[2] = edge3.Eindex;
		mfacet.Findex = F_index;//变量赋值访问更快
		//mfacet.Findex = vecFacet.size();
		vecFacet.push_back(mfacet);	
        F_index++;
	}
	in.close();

	//复制到vector容器中
	vecVertex.resize(SetVertex.size());//申请空间
	vecEdge.resize(SetEdge.size());//申请空间
	std::copy(SetVertex.begin(), SetVertex.end(), vecVertex.begin());
	std::copy(SetEdge.begin(), SetEdge.end(), vecEdge.begin());
	//按索引值进行排序
	//std::sort(vecVertex.begin(), vecVertex.end());//默认排序
	std::sort(vecVertex.begin(), vecVertex.end(), LessVertIndex());
	//std::sort(vecEdge.begin(), vecEdge.end());//默认排序
	std::sort(vecEdge.begin(), vecEdge.end(), LessEdgeIndex());

	//求取顶点坐标最大最小值
	Xmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Xmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertX())).x;
	Ymin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Ymax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertY())).y;
	Zmin = (*min_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;
	Zmax = (*max_element(vecVertex.begin(), vecVertex.end(), LessVertZ())).z;

	//构建面片的三个邻接面片
	vector<Facet>::iterator itefacet;
	for (itefacet = vecFacet.begin(); itefacet != vecFacet.end(); itefacet++)
	{
		for (int i = 0; i < 3; i++)
		{
			//增加判断
			if (vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0 == (*itefacet).Findex)
				(*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet1;
			else
				(*itefacet).FacetIndex[i] = vecEdge[(*itefacet).EdgeIndex[i]].neiFacet0;
		}
	}
	return true;
}

//获取三角面片数 
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
