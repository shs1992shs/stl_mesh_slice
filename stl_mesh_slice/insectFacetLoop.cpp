#pragma once
#include "STLModel.h"

//判断采用哪种形式的分层方式，1按层数，2按层厚
bool STLModel::insectFacets(int mode)
{
	//以分层层数计算层厚
	if (mode == 1)
	{
		//手动输入层数
		cout << "请输入分层层数" << endl;
		cin >> layerNumber;
		if (!cin)
		{
			cout << "输入错误，请重新输入" << endl;
			cin.clear();// 清理输入标志位
			cin.sync();// 清空键盘缓冲区
		}

		if (layerNumber <= 1)
		{
			cout << "分层层数应大于1" << endl;
			return false;
		}
		else
		{
			layerThickness = this->GetZstroke() / layerNumber;//分层层厚		
			for (size_t i = 0; i < layerNumber; i++)
			{
				Layer la;
				la.layerIndex = i + 1;
				la.zHeight =this->GetZmin()+ i*layerThickness;//若模型没有转换坐标到原点,从最低高度开始是第一层
				vecLayer.push_back(la);
			}
		}
	}
	//以分层层厚计算层数
	else if (mode == 2)
	{
		//手动输入层厚
		cout << "请输入分层厚度" << endl;
		cin >> layerThickness;
		if (!cin)
		{
			cout << "输入错误，请重新输入" << endl;
			cin.clear();// 清理输入标志位
			cin.sync();// 清空键盘缓冲区
		}

		if (layerThickness <= 0.0)
		{
			cout << "分层厚度应大于0" << endl;
			return false;
		}
		else if (layerThickness >= GetZstroke())
		{
			cout << "分层厚度应小于模型最大高度" << endl;
			return false;
		}
		layerNumber = (int)ceil(GetZstroke() / layerThickness);//ceil取整大于参数的最小整数
		cout << "分层层数为："<< layerNumber << endl;
		for (size_t i = 0; i < layerNumber; i++)
		{
			Layer la;
			la.layerIndex = i + 1;
			la.zHeight =this->GetZmin()+ i*layerThickness;//若模型没有转换坐标到原点
			vecLayer.push_back(la);
		}
	}	
	getInsectFacetsWithThickness(layerNumber);
	//getInsectFacetsWithHeight(layerNumber);
	obtainFacetLoop(layerNumber);//创建面片环
	//cout << "切片平面的相交面片环的面片索引是：" << endl;
	//displayFacetLoop();	
	return true;
}

//以切片高度计算其相交面片，层厚可以不固定
bool STLModel::getInsectFacetsWithHeight(const int& layernum)
{
	insectFacetData.resize(layernum);
	std::sort(vecFacet.begin(), vecFacet.end(), LessFacetZcoor());	/*按面片顶点的z坐标值进行排序*/
	
	for (size_t i = 0; i < vecFacet.size(); i++)
	{
		for (int j = 0; j < layernum; j++)
		{
			if (vecLayer[j].zHeight < vecFacet[i].getFZmin())
				continue;
			else if (vecFacet[i].getFZmin() <= vecLayer[j].zHeight && vecFacet[i].getFZmax() > vecLayer[j].zHeight)
			{
				insectFacetData[j].insert(vecFacet[i].Findex);
			}
			else if (vecLayer[j].zHeight >= vecFacet[i].getFZmax())
				break;
		}	
	}
	//displayInsectFacet();
	return true;
}

//计算等层厚分层所有切片平面的相交面片，固定层厚
bool STLModel::getInsectFacetsWithThickness(const int& layernum)
{
	insectFacetData.resize(layernum);
	set<int> perlayerfacet;
	//计算每一层切片平面的相交三角面片
	float p = 0, q = 0;
	int k = 0, m = 0;
	for (size_t i = 0; i < vecFacet.size(); i++)
	{			
		if (vecFacet[i].getFZmin() == vecFacet[i].getFZmax())
			continue;//如果面片最大最小z坐标相等，则该面片不计入,直接判断下一个面片			
		//计算与面片相交的切片平面序号的范围.面片最低点与切片相交计入，最高点相交不计入
		p = (vecFacet[i].getFZmin() - this->GetZmin()) / layerThickness;//范围下标
		q = (vecFacet[i].getFZmax() - this->GetZmin()) / layerThickness;//范围上标
		if(ceil(p)==ceil(q))//如果面片介于相邻切片平面之间，则该面片不计入,直接判断下一个面片	
			continue;
		//如果不是整数，则与该面片相交的切片平面的序号在两者之间，j属于（p,q），开区间
		k = (int)ceil(p);
		m = (int)ceil(q);
		for (int j = k; j < m ; j++) 
		{
			insectFacetData[j].insert(vecFacet[i].Findex);
		}			
	}
	//displayInsectFacet();
	return true;
}

//寻找面片环方向
int STLModel::obtainFacetLoopDirect(int facetindex, int i, int& edgenumber)
{
	/*2019-10-18：采用面片法向量与水平交线段之间的矢量积为判断条件，即从第一个面片的另一相交边交点P1走向公共邻接边的交点P2
	最大z值与最小z值所在边一定相交切片平面*/

	int nextfindex = 0;//递归遍历面片的索引	
	Edge e1, e2, e3;
	set<Edge>::iterator iteedge1, iteedge2, iteedge3;
	float p1[3]; float p2[3];//存储两个交点的坐标
	float direction[3];//定义一个确定面片环方向的向量，存储其坐标值	

	int mn = vecFacet[facetindex].getFZminVid();//第一个面片的最小顶点索引
	int md = vecFacet[facetindex].getFZmidVid();//第一个面片的中间顶点索引
	int mx = vecFacet[facetindex].getFZmaxVid();//第一个面片的最大顶点索引		

	//最大最小z值顶点所在边2
	if (mx > mn) //顶点索引大的做起点
	{
		e2.Verindex0 = mx;
		e2.Verindex1 = mn;
		iteedge2 = SetEdge.find(e2);
	}
	else {
		e2.Verindex0 = mn;
		e2.Verindex1 = mx;
		iteedge2 = SetEdge.find(e2);
	}
	
    if (vecLayer[i].zHeight < vecFacet[facetindex].getFZmin())
	{
		cout << "当前面片与切片平面不相交" << endl;
		return false;
	}
	else if (vecLayer[i].zHeight < vecFacet[facetindex].getFZmid())//若zmid大于Zi,与边zmin与zmid相交，找到相交的公共边1
	{
		if (md > mn)	//顶点索引大的做起点
		{
			e1.Verindex0 = md;
			e1.Verindex1 = mn;
			iteedge1 = SetEdge.find(e1);
		}
		else {
			e1.Verindex0 = mn;
			e1.Verindex1 = md;
			iteedge1 = SetEdge.find(e1);
		}
		//求取公共边1与切片平面的交点P1,以及最大最小z值所在边2与切片平面的交点P2。2019-10-24，增加情况判断，当切片平面高度跟最低点重合时
		if (vecLayer[i].zHeight == vecFacet[facetindex].getFZmin())
		{
			//P1点坐标,公共边1上坐标，等于zmid坐标
			p1[0] = vecVertex[md].x;
			p1[1] = vecVertex[md].y;
			//p1[2] = vecLayer[i].zHeight;//等于切片平面高度
			p1[2] = vecVertex[md].z;//修改为zmid顶点的坐标
			// P2点坐标, 边2上坐标，修改vecLayer[i].zHeight == vecVertex[m].z
			p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
			p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
			p2[2] = vecVertex[md].z;//修改为zmid顶点的z坐标			
		}
		else {
			//P1点坐标,公共边1上的坐标
			p1[0] = (vecVertex[mn].x - vecVertex[md].x)*(vecLayer[i].zHeight - vecVertex[md].z) / (vecVertex[mn].z - vecVertex[md].z) + vecVertex[md].x;
			p1[1] = (vecVertex[mn].y - vecVertex[md].y)*(vecLayer[i].zHeight - vecVertex[md].z) / (vecVertex[mn].z - vecVertex[md].z) + vecVertex[md].y;
			p1[2] = vecLayer[i].zHeight;//等于切片平面高度				
			//P2点坐标,边2上坐标
			p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
			p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
			p2[2] = vecLayer[i].zHeight;//等于切片平面高度
		}
	}
	else if (vecLayer[i].zHeight >= vecFacet[facetindex].getFZmid() && vecLayer[i].zHeight < vecFacet[facetindex].getFZmax())//若zmid小于Zi,与边zmax与zmid相交，找到它
	{
		if (md > mx)	//顶点索引大的做起点
		{
			e1.Verindex0 = md;
			e1.Verindex1 = mx;
			iteedge1 = SetEdge.find(e1);
		}
		else {
			e1.Verindex0 = mx;
			e1.Verindex1 = md;
			iteedge1 = SetEdge.find(e1);
		}
		//求取公共边1与切片平面的交点P1,以及最大最小z值所在边2与切片平面的交点P2
		if (vecLayer[i].zHeight == vecFacet[facetindex].getFZmid())//2020-2-16 修改方向判断方法，避免最小中间点所在边对应的邻接面片不在同一切片平面内
		{
			//P1点坐标,公共边1上坐标，等于zmid坐标
			p1[0] = vecVertex[md].x;
			p1[1] = vecVertex[md].y;
			//p1[2] = vecLayer[i].zHeight;//等于切片平面高度
			p1[2] = vecVertex[md].z;//修改为zmid顶点的坐标
			// P2点坐标, 边2上坐标，修改vecLayer[i].zHeight == vecVertex[m].z
			p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
			p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
			p2[2] = vecVertex[md].z;//修改为zmid顶点的z坐标	
		}
		else {
			if (vecFacet[facetindex].getFZmin() == vecFacet[facetindex].getFZmid())//zmid与zmin相等
			{
				//P1点坐标,公共边1上坐标，等于zmid坐标
				p1[0] = vecVertex[md].x;
				p1[1] = vecVertex[md].y;
				//p1[2] = vecLayer[i].zHeight;//等于切片平面高度
				p1[2] = vecVertex[md].z;//修改为zmid顶点的坐标

				//P2点坐标,边zmin/zmax上的坐标，高度等于zmid=zmin=zheight
				p2[0] = vecVertex[mn].x;
				p2[1] = vecVertex[mn].y;
				//p2[2] = vecLayer[i].zHeight;//等于切片平面高度
				p2[2] = vecVertex[mn].z;//修改为zmin顶点的坐标
			}
			else {
				//P1点坐标,公共边1上坐标
				p1[0] = (vecVertex[md].x - vecVertex[mx].x)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[md].z - vecVertex[mx].z) + vecVertex[mx].x;
				p1[1] = (vecVertex[md].y - vecVertex[mx].y)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[md].z - vecVertex[mx].z) + vecVertex[mx].y;
				p1[2] = vecLayer[i].zHeight;//等于切片平面高度
				//P2点坐标,公共边2上坐标
				p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
				p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
				p2[2] = vecLayer[i].zHeight;//等于切片平面高度	 
			}
		}		
	}
	else if (vecLayer[i].zHeight >= vecFacet[facetindex].getFZmax())
	{
		cout << "当前面片与切片平面不相交" << endl;
		return false;
	}
	//从P2指向P1，起始面片的法向量n与direction叉积
	direction[0] = p1[0] - p2[0];
	direction[1] = p1[1] - p2[1];
	direction[2] = p1[2] - p2[2];

	float direcZ = vecFacet[facetindex].Normal[0] * direction[1] - vecFacet[facetindex].Normal[1] * direction[0];
	if (direcZ > 0)
	{
		//第二个面片在第一个面片的右边（边1邻接面片），逆时针，正方向
		if (iteedge1 != SetEdge.end())
		{
			if ((*iteedge1).neiFacet0 != facetindex)
			{
				nextfindex = (*iteedge1).neiFacet0;//更新递归遍历面片索引					
			}
			else {
				nextfindex = (*iteedge1).neiFacet1;//更新递归遍历面片索引					
			}
			edgenumber = (*iteedge1).Eindex;
		}
		else
			cout << "未找到该边，请核对边的顶点索引" << endl;
	}
	else if (direcZ < 0)
	{
		//第二个面片在第一个面片的左边（公共边2，最大最小顶点，邻接面片），另一邻接面片为正方向面片,创建边查找其面片				
		if (iteedge2 != SetEdge.end())
		{
			if ((*iteedge2).neiFacet0 != facetindex)
			{
				nextfindex = (*iteedge2).neiFacet0;//更新递归遍历面片索引					
			}
			else {
				nextfindex = (*iteedge2).neiFacet1;//更新递归遍历面片索引					
			}
			edgenumber = (*iteedge2).Eindex;
		}
		else
			cout << "未找到该边，请核对边的顶点索引" << endl;
	}
	return  nextfindex;//返回面片环初始面片的下一面片的索引 
}

bool STLModel::obtainFacetLoop(const int& layernum)
{
	vector<int> perLoopInsectEdge;//存储单层单个环切片相交公共邻接边
	vector<int> perFacetLoop;//存储单层单个环的面片
	insectFacetLoop.reserve(layernum);
	/*2019-12-1：采用getInsectFacetsWithHeight()求取相交面片时，面片的索引号与在vecFacet里的序号不一致,需要再次重新排序*/
	//std::sort(vecFacet.begin(), vecFacet.end());
	/*------------------------------------------------------------------------------*/
	int nextfacet = 0;//递归遍历面片的索引
	for (size_t i = 0; i < insectFacetData.size(); i++)
	{
		//cout << "当前层是第：" << i << "层" << endl;
		set<int> setfacet(insectFacetData[i]);//拷贝构造

		int edgenum = 0;//公共邻接边索引	
		int firstFacet = *insectFacetData[i].begin();//记录起始面片		
		perFacetLoop.push_back(firstFacet);	//面片环初始面片		
		nextfacet = obtainFacetLoopDirect(firstFacet, i, edgenum);//获得面片环初始面片的下一面片	  
		perFacetLoop.push_back(nextfacet);//面片环第二面片存入
		perLoopInsectEdge.push_back(edgenum);//存入初始面片与第二个面片之间的公共邻接边		
		setfacet.erase(firstFacet);//记录一个则删掉一个
		setfacet.erase(nextfacet);

		while (setfacet.size() != 0)//以第二个面片的索引开始递归遍历查找	
		{
			Edge e;
			set<Edge>::iterator iteedge;
			//判断当前面片的三个邻接面片是否在当前切片平面的相交面片内 
			vector<int> layeradjf;//存放同一层相邻面片的索引
			vector<int> adjfacet;//存放相邻面片的索引
			for (int j = 0; j < 3; j++)
			{
				int k = vecFacet[nextfacet].FacetIndex[j];//相邻面片的索引				
				auto ite1 = insectFacetData[i].find(k);
				if (ite1 != insectFacetData[i].end())
					layeradjf.push_back(k);
				set<int>::iterator ite2 = setfacet.find(k);//找到，说明还未存入面片环,查找效率更高
				if (ite1 != insectFacetData[i].end() && ite2 != setfacet.end())
					adjfacet.push_back(k);//存放处于同一切片平面剩余邻接面片，最少为1个，最多2个，0个表示成环				
			}
			//先判断同一层还有几个相邻面片
			if (adjfacet.size() == 1)
			{
				if (layeradjf.size() == 3)
				{
					int nf = obtainFacetLoopDirect(nextfacet, i, edgenum);
					if (nf == firstFacet)//判断是否回到初始面片
					{
						//先找到面片环最后一个面片与起始面片的公共边					
						edgenum = getAdjedge(nextfacet, firstFacet);
						perLoopInsectEdge.push_back(edgenum);
						perlayerInsectEdge.push_back(perLoopInsectEdge);//存入边环	
						perLoopInsectEdge.clear();

						perlayerFacetLoop.push_back(perFacetLoop);//存入面片环	
						perFacetLoop.resize(0);

						//创建新面片环--------------------------------------------					     
						firstFacet = *setfacet.begin();
						//cout << "新面片环的第一个面片是：" << firstFacet <<endl;
						perFacetLoop.push_back(firstFacet);
						nextfacet = obtainFacetLoopDirect(firstFacet, i, edgenum);
						perFacetLoop.push_back(nextfacet);//新面片环第二面片存入
						perLoopInsectEdge.push_back(edgenum);

						setfacet.erase(firstFacet);//记录一个则删掉一个
						setfacet.erase(nextfacet);
					}
					else  //没有回到初始面片的话，继续存入当前面片环
					{
						edgenum = getAdjedge(nextfacet, adjfacet[0]);
						nextfacet = adjfacet[0];
						perFacetLoop.push_back(nextfacet);
						perLoopInsectEdge.push_back(edgenum);//记录公共邻接边
						setfacet.erase(nextfacet);
					}
				}
				else {
					edgenum = getAdjedge(nextfacet, adjfacet[0]);
					nextfacet = adjfacet[0];
					perFacetLoop.push_back(nextfacet);
					perLoopInsectEdge.push_back(edgenum);//记录公共邻接边
					setfacet.erase(nextfacet);
				}
			}
			else if (adjfacet.size() == 2)
			{
				int mn = vecFacet[nextfacet].getFZminVid();//最小顶点索引
				int md = vecFacet[nextfacet].getFZmidVid();//中间顶点索引				
				int mx = vecFacet[nextfacet].getFZmaxVid();//最大顶点索引
				if (vecLayer[i].zHeight < vecFacet[nextfacet].getFZmid())
				{
					if (md > mn)	//顶点索引大的做边的起点
					{
						e.Verindex0 = md;
						e.Verindex1 = mn;
						iteedge = SetEdge.find(e);
					}
					else {
						e.Verindex0 = mn;
						e.Verindex1 = md;
						iteedge = SetEdge.find(e);
					}
				}
				else if (vecLayer[i].zHeight >= vecFacet[nextfacet].getFZmid())
				{
					//顶点索引大的做边的起点
					if (md > mx)
					{
						e.Verindex0 = md;
						e.Verindex1 = mx;
						iteedge = SetEdge.find(e);
					}
					else {
						e.Verindex0 = mx;
						e.Verindex1 = md;
						iteedge = SetEdge.find(e);
					}
				}
				/*如果最大最小顶点所在边没有被切，则下一个被切的一定是该边*/
				vector<int>::iterator ite3 = find(perLoopInsectEdge.begin(), perLoopInsectEdge.end(), (*iteedge).Eindex);
				if (ite3 != perLoopInsectEdge.end())
				{
					//顶点索引大的做边的起点
					if (mn > mx)
					{
						e.Verindex0 = mn;
						e.Verindex1 = mx;
						iteedge = SetEdge.find(e);
					}
					else {
						e.Verindex0 = mx;
						e.Verindex1 = mn;
						iteedge = SetEdge.find(e);
					}
				}
				//选择与这个边的邻接面片
				if (iteedge != SetEdge.end())
				{
					if ((*iteedge).neiFacet0 == nextfacet)
					{
						nextfacet = (*iteedge).neiFacet1;//更新递归遍历面片索引
					}
					else {
						nextfacet = (*iteedge).neiFacet0;//更新递归遍历面片索引
					}
				}
				else
					cout << "没有找到该边" << endl;
				perFacetLoop.push_back(nextfacet);
				perLoopInsectEdge.push_back((*iteedge).Eindex);//记录公共邻接边		
				setfacet.erase(nextfacet);
			}
			//找满一圈，构建一个面片环，从下一面片开始
			else if (adjfacet.size() == 0)
			{
				//先找到面片环最后一个面片与起始面片的公共边					
				edgenum = getAdjedge(nextfacet, firstFacet);
				perLoopInsectEdge.push_back(edgenum);
				perlayerInsectEdge.push_back(perLoopInsectEdge);//存入边环	
				perLoopInsectEdge.clear();

				perlayerFacetLoop.push_back(perFacetLoop);//存入面片环	
				perFacetLoop.resize(0);

				//创建新面片环--------------------------------------------					     
				firstFacet = *setfacet.begin();
				//cout << "新面片环的第一个面片是：" << firstFacet <<endl;
				perFacetLoop.push_back(firstFacet);
				nextfacet = obtainFacetLoopDirect(firstFacet, i, edgenum);
				perFacetLoop.push_back(nextfacet);//新面片环第二面片存入
				perLoopInsectEdge.push_back(edgenum);
				setfacet.erase(firstFacet);//记录一个则删掉一个
				setfacet.erase(nextfacet);

			}
		};
		edgenum = getAdjedge(nextfacet, firstFacet);
		perLoopInsectEdge.push_back(edgenum);
		perlayerInsectEdge.push_back(perLoopInsectEdge);//存入边环
		perLoopInsectEdge.clear();

		perlayerFacetLoop.push_back(perFacetLoop);//存入面片环	
		perFacetLoop.resize(0);

		insectEdgeData.push_back(perlayerInsectEdge);//存入每一层的面片环的相交公共邻接边
		perlayerInsectEdge.clear();//清空当前层相交公共边的容器里的元素		
		insectFacetLoop.push_back(perlayerFacetLoop);
		perlayerFacetLoop.clear();
		setfacet.clear();
	}
	return true;
}