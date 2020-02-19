#pragma once
#include "STLModel.h"

//�жϲ���������ʽ�ķֲ㷽ʽ��1��������2�����
bool STLModel::insectFacets(int mode)
{
	//�Էֲ����������
	if (mode == 1)
	{
		//�ֶ��������
		cout << "������ֲ����" << endl;
		cin >> layerNumber;
		if (!cin)
		{
			cout << "�����������������" << endl;
			cin.clear();// ���������־λ
			cin.sync();// ��ռ��̻�����
		}

		if (layerNumber <= 1)
		{
			cout << "�ֲ����Ӧ����1" << endl;
			return false;
		}
		else
		{
			layerThickness = this->GetZstroke() / layerNumber;//�ֲ���		
			for (size_t i = 0; i < layerNumber; i++)
			{
				Layer la;
				la.layerIndex = i + 1;
				la.zHeight =this->GetZmin()+ i*layerThickness;//��ģ��û��ת�����굽ԭ��,����͸߶ȿ�ʼ�ǵ�һ��
				vecLayer.push_back(la);
			}
		}
	}
	//�Էֲ���������
	else if (mode == 2)
	{
		//�ֶ�������
		cout << "������ֲ���" << endl;
		cin >> layerThickness;
		if (!cin)
		{
			cout << "�����������������" << endl;
			cin.clear();// ���������־λ
			cin.sync();// ��ռ��̻�����
		}

		if (layerThickness <= 0.0)
		{
			cout << "�ֲ���Ӧ����0" << endl;
			return false;
		}
		else if (layerThickness >= GetZstroke())
		{
			cout << "�ֲ���ӦС��ģ�����߶�" << endl;
			return false;
		}
		layerNumber = (int)ceil(GetZstroke() / layerThickness);//ceilȡ�����ڲ�������С����
		cout << "�ֲ����Ϊ��"<< layerNumber << endl;
		for (size_t i = 0; i < layerNumber; i++)
		{
			Layer la;
			la.layerIndex = i + 1;
			la.zHeight =this->GetZmin()+ i*layerThickness;//��ģ��û��ת�����굽ԭ��
			vecLayer.push_back(la);
		}
	}	
	getInsectFacetsWithThickness(layerNumber);
	//getInsectFacetsWithHeight(layerNumber);
	obtainFacetLoop(layerNumber);//������Ƭ��
	//cout << "��Ƭƽ����ཻ��Ƭ������Ƭ�����ǣ�" << endl;
	//displayFacetLoop();	
	return true;
}

//����Ƭ�߶ȼ������ཻ��Ƭ�������Բ��̶�
bool STLModel::getInsectFacetsWithHeight(const int& layernum)
{
	insectFacetData.resize(layernum);
	std::sort(vecFacet.begin(), vecFacet.end(), LessFacetZcoor());	/*����Ƭ�����z����ֵ��������*/
	
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

//����Ȳ��ֲ�������Ƭƽ����ཻ��Ƭ���̶����
bool STLModel::getInsectFacetsWithThickness(const int& layernum)
{
	insectFacetData.resize(layernum);
	set<int> perlayerfacet;
	//����ÿһ����Ƭƽ����ཻ������Ƭ
	float p = 0, q = 0;
	int k = 0, m = 0;
	for (size_t i = 0; i < vecFacet.size(); i++)
	{			
		if (vecFacet[i].getFZmin() == vecFacet[i].getFZmax())
			continue;//�����Ƭ�����Сz������ȣ������Ƭ������,ֱ���ж���һ����Ƭ			
		//��������Ƭ�ཻ����Ƭƽ����ŵķ�Χ.��Ƭ��͵�����Ƭ�ཻ���룬��ߵ��ཻ������
		p = (vecFacet[i].getFZmin() - this->GetZmin()) / layerThickness;//��Χ�±�
		q = (vecFacet[i].getFZmax() - this->GetZmin()) / layerThickness;//��Χ�ϱ�
		if(ceil(p)==ceil(q))//�����Ƭ����������Ƭƽ��֮�䣬�����Ƭ������,ֱ���ж���һ����Ƭ	
			continue;
		//��������������������Ƭ�ཻ����Ƭƽ������������֮�䣬j���ڣ�p,q����������
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

//Ѱ����Ƭ������
int STLModel::obtainFacetLoopDirect(int facetindex, int i, int& edgenumber)
{
	/*2019-10-18��������Ƭ��������ˮƽ���߶�֮���ʸ����Ϊ�ж����������ӵ�һ����Ƭ����һ�ཻ�߽���P1���򹫹��ڽӱߵĽ���P2
	���zֵ����Сzֵ���ڱ�һ���ཻ��Ƭƽ��*/

	int nextfindex = 0;//�ݹ������Ƭ������	
	Edge e1, e2, e3;
	set<Edge>::iterator iteedge1, iteedge2, iteedge3;
	float p1[3]; float p2[3];//�洢�������������
	float direction[3];//����һ��ȷ����Ƭ��������������洢������ֵ	

	int mn = vecFacet[facetindex].getFZminVid();//��һ����Ƭ����С��������
	int md = vecFacet[facetindex].getFZmidVid();//��һ����Ƭ���м䶥������
	int mx = vecFacet[facetindex].getFZmaxVid();//��һ����Ƭ����󶥵�����		

	//�����Сzֵ�������ڱ�2
	if (mx > mn) //����������������
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
		cout << "��ǰ��Ƭ����Ƭƽ�治�ཻ" << endl;
		return false;
	}
	else if (vecLayer[i].zHeight < vecFacet[facetindex].getFZmid())//��zmid����Zi,���zmin��zmid�ཻ���ҵ��ཻ�Ĺ�����1
	{
		if (md > mn)	//����������������
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
		//��ȡ������1����Ƭƽ��Ľ���P1,�Լ������Сzֵ���ڱ�2����Ƭƽ��Ľ���P2��2019-10-24����������жϣ�����Ƭƽ��߶ȸ���͵��غ�ʱ
		if (vecLayer[i].zHeight == vecFacet[facetindex].getFZmin())
		{
			//P1������,������1�����꣬����zmid����
			p1[0] = vecVertex[md].x;
			p1[1] = vecVertex[md].y;
			//p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
			p1[2] = vecVertex[md].z;//�޸�Ϊzmid���������
			// P2������, ��2�����꣬�޸�vecLayer[i].zHeight == vecVertex[m].z
			p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
			p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
			p2[2] = vecVertex[md].z;//�޸�Ϊzmid�����z����			
		}
		else {
			//P1������,������1�ϵ�����
			p1[0] = (vecVertex[mn].x - vecVertex[md].x)*(vecLayer[i].zHeight - vecVertex[md].z) / (vecVertex[mn].z - vecVertex[md].z) + vecVertex[md].x;
			p1[1] = (vecVertex[mn].y - vecVertex[md].y)*(vecLayer[i].zHeight - vecVertex[md].z) / (vecVertex[mn].z - vecVertex[md].z) + vecVertex[md].y;
			p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�				
			//P2������,��2������
			p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
			p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
			p2[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
		}
	}
	else if (vecLayer[i].zHeight >= vecFacet[facetindex].getFZmid() && vecLayer[i].zHeight < vecFacet[facetindex].getFZmax())//��zmidС��Zi,���zmax��zmid�ཻ���ҵ���
	{
		if (md > mx)	//����������������
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
		//��ȡ������1����Ƭƽ��Ľ���P1,�Լ������Сzֵ���ڱ�2����Ƭƽ��Ľ���P2
		if (vecLayer[i].zHeight == vecFacet[facetindex].getFZmid())//2020-2-16 �޸ķ����жϷ�����������С�м�����ڱ߶�Ӧ���ڽ���Ƭ����ͬһ��Ƭƽ����
		{
			//P1������,������1�����꣬����zmid����
			p1[0] = vecVertex[md].x;
			p1[1] = vecVertex[md].y;
			//p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
			p1[2] = vecVertex[md].z;//�޸�Ϊzmid���������
			// P2������, ��2�����꣬�޸�vecLayer[i].zHeight == vecVertex[m].z
			p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
			p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecVertex[md].z - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
			p2[2] = vecVertex[md].z;//�޸�Ϊzmid�����z����	
		}
		else {
			if (vecFacet[facetindex].getFZmin() == vecFacet[facetindex].getFZmid())//zmid��zmin���
			{
				//P1������,������1�����꣬����zmid����
				p1[0] = vecVertex[md].x;
				p1[1] = vecVertex[md].y;
				//p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
				p1[2] = vecVertex[md].z;//�޸�Ϊzmid���������

				//P2������,��zmin/zmax�ϵ����꣬�߶ȵ���zmid=zmin=zheight
				p2[0] = vecVertex[mn].x;
				p2[1] = vecVertex[mn].y;
				//p2[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
				p2[2] = vecVertex[mn].z;//�޸�Ϊzmin���������
			}
			else {
				//P1������,������1������
				p1[0] = (vecVertex[md].x - vecVertex[mx].x)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[md].z - vecVertex[mx].z) + vecVertex[mx].x;
				p1[1] = (vecVertex[md].y - vecVertex[mx].y)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[md].z - vecVertex[mx].z) + vecVertex[mx].y;
				p1[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�
				//P2������,������2������
				p2[0] = (vecVertex[mn].x - vecVertex[mx].x)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].x;
				p2[1] = (vecVertex[mn].y - vecVertex[mx].y)*(vecLayer[i].zHeight - vecVertex[mx].z) / (vecVertex[mn].z - vecVertex[mx].z) + vecVertex[mx].y;
				p2[2] = vecLayer[i].zHeight;//������Ƭƽ��߶�	 
			}
		}		
	}
	else if (vecLayer[i].zHeight >= vecFacet[facetindex].getFZmax())
	{
		cout << "��ǰ��Ƭ����Ƭƽ�治�ཻ" << endl;
		return false;
	}
	//��P2ָ��P1����ʼ��Ƭ�ķ�����n��direction���
	direction[0] = p1[0] - p2[0];
	direction[1] = p1[1] - p2[1];
	direction[2] = p1[2] - p2[2];

	float direcZ = vecFacet[facetindex].Normal[0] * direction[1] - vecFacet[facetindex].Normal[1] * direction[0];
	if (direcZ > 0)
	{
		//�ڶ�����Ƭ�ڵ�һ����Ƭ���ұߣ���1�ڽ���Ƭ������ʱ�룬������
		if (iteedge1 != SetEdge.end())
		{
			if ((*iteedge1).neiFacet0 != facetindex)
			{
				nextfindex = (*iteedge1).neiFacet0;//���µݹ������Ƭ����					
			}
			else {
				nextfindex = (*iteedge1).neiFacet1;//���µݹ������Ƭ����					
			}
			edgenumber = (*iteedge1).Eindex;
		}
		else
			cout << "δ�ҵ��ñߣ���˶ԱߵĶ�������" << endl;
	}
	else if (direcZ < 0)
	{
		//�ڶ�����Ƭ�ڵ�һ����Ƭ����ߣ�������2�������С���㣬�ڽ���Ƭ������һ�ڽ���ƬΪ��������Ƭ,�����߲�������Ƭ				
		if (iteedge2 != SetEdge.end())
		{
			if ((*iteedge2).neiFacet0 != facetindex)
			{
				nextfindex = (*iteedge2).neiFacet0;//���µݹ������Ƭ����					
			}
			else {
				nextfindex = (*iteedge2).neiFacet1;//���µݹ������Ƭ����					
			}
			edgenumber = (*iteedge2).Eindex;
		}
		else
			cout << "δ�ҵ��ñߣ���˶ԱߵĶ�������" << endl;
	}
	return  nextfindex;//������Ƭ����ʼ��Ƭ����һ��Ƭ������ 
}

bool STLModel::obtainFacetLoop(const int& layernum)
{
	vector<int> perLoopInsectEdge;//�洢���㵥������Ƭ�ཻ�����ڽӱ�
	vector<int> perFacetLoop;//�洢���㵥��������Ƭ
	insectFacetLoop.reserve(layernum);
	/*2019-12-1������getInsectFacetsWithHeight()��ȡ�ཻ��Ƭʱ����Ƭ������������vecFacet�����Ų�һ��,��Ҫ�ٴ���������*/
	//std::sort(vecFacet.begin(), vecFacet.end());
	/*------------------------------------------------------------------------------*/
	int nextfacet = 0;//�ݹ������Ƭ������
	for (size_t i = 0; i < insectFacetData.size(); i++)
	{
		//cout << "��ǰ���ǵڣ�" << i << "��" << endl;
		set<int> setfacet(insectFacetData[i]);//��������

		int edgenum = 0;//�����ڽӱ�����	
		int firstFacet = *insectFacetData[i].begin();//��¼��ʼ��Ƭ		
		perFacetLoop.push_back(firstFacet);	//��Ƭ����ʼ��Ƭ		
		nextfacet = obtainFacetLoopDirect(firstFacet, i, edgenum);//�����Ƭ����ʼ��Ƭ����һ��Ƭ	  
		perFacetLoop.push_back(nextfacet);//��Ƭ���ڶ���Ƭ����
		perLoopInsectEdge.push_back(edgenum);//�����ʼ��Ƭ��ڶ�����Ƭ֮��Ĺ����ڽӱ�		
		setfacet.erase(firstFacet);//��¼һ����ɾ��һ��
		setfacet.erase(nextfacet);

		while (setfacet.size() != 0)//�Եڶ�����Ƭ��������ʼ�ݹ��������	
		{
			Edge e;
			set<Edge>::iterator iteedge;
			//�жϵ�ǰ��Ƭ�������ڽ���Ƭ�Ƿ��ڵ�ǰ��Ƭƽ����ཻ��Ƭ�� 
			vector<int> layeradjf;//���ͬһ��������Ƭ������
			vector<int> adjfacet;//���������Ƭ������
			for (int j = 0; j < 3; j++)
			{
				int k = vecFacet[nextfacet].FacetIndex[j];//������Ƭ������				
				auto ite1 = insectFacetData[i].find(k);
				if (ite1 != insectFacetData[i].end())
					layeradjf.push_back(k);
				set<int>::iterator ite2 = setfacet.find(k);//�ҵ���˵����δ������Ƭ��,����Ч�ʸ���
				if (ite1 != insectFacetData[i].end() && ite2 != setfacet.end())
					adjfacet.push_back(k);//��Ŵ���ͬһ��Ƭƽ��ʣ���ڽ���Ƭ������Ϊ1�������2����0����ʾ�ɻ�				
			}
			//���ж�ͬһ�㻹�м���������Ƭ
			if (adjfacet.size() == 1)
			{
				if (layeradjf.size() == 3)
				{
					int nf = obtainFacetLoopDirect(nextfacet, i, edgenum);
					if (nf == firstFacet)//�ж��Ƿ�ص���ʼ��Ƭ
					{
						//���ҵ���Ƭ�����һ����Ƭ����ʼ��Ƭ�Ĺ�����					
						edgenum = getAdjedge(nextfacet, firstFacet);
						perLoopInsectEdge.push_back(edgenum);
						perlayerInsectEdge.push_back(perLoopInsectEdge);//����߻�	
						perLoopInsectEdge.clear();

						perlayerFacetLoop.push_back(perFacetLoop);//������Ƭ��	
						perFacetLoop.resize(0);

						//��������Ƭ��--------------------------------------------					     
						firstFacet = *setfacet.begin();
						//cout << "����Ƭ���ĵ�һ����Ƭ�ǣ�" << firstFacet <<endl;
						perFacetLoop.push_back(firstFacet);
						nextfacet = obtainFacetLoopDirect(firstFacet, i, edgenum);
						perFacetLoop.push_back(nextfacet);//����Ƭ���ڶ���Ƭ����
						perLoopInsectEdge.push_back(edgenum);

						setfacet.erase(firstFacet);//��¼һ����ɾ��һ��
						setfacet.erase(nextfacet);
					}
					else  //û�лص���ʼ��Ƭ�Ļ����������뵱ǰ��Ƭ��
					{
						edgenum = getAdjedge(nextfacet, adjfacet[0]);
						nextfacet = adjfacet[0];
						perFacetLoop.push_back(nextfacet);
						perLoopInsectEdge.push_back(edgenum);//��¼�����ڽӱ�
						setfacet.erase(nextfacet);
					}
				}
				else {
					edgenum = getAdjedge(nextfacet, adjfacet[0]);
					nextfacet = adjfacet[0];
					perFacetLoop.push_back(nextfacet);
					perLoopInsectEdge.push_back(edgenum);//��¼�����ڽӱ�
					setfacet.erase(nextfacet);
				}
			}
			else if (adjfacet.size() == 2)
			{
				int mn = vecFacet[nextfacet].getFZminVid();//��С��������
				int md = vecFacet[nextfacet].getFZmidVid();//�м䶥������				
				int mx = vecFacet[nextfacet].getFZmaxVid();//��󶥵�����
				if (vecLayer[i].zHeight < vecFacet[nextfacet].getFZmid())
				{
					if (md > mn)	//��������������ߵ����
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
					//��������������ߵ����
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
				/*��������С�������ڱ�û�б��У�����һ�����е�һ���Ǹñ�*/
				vector<int>::iterator ite3 = find(perLoopInsectEdge.begin(), perLoopInsectEdge.end(), (*iteedge).Eindex);
				if (ite3 != perLoopInsectEdge.end())
				{
					//��������������ߵ����
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
				//ѡ��������ߵ��ڽ���Ƭ
				if (iteedge != SetEdge.end())
				{
					if ((*iteedge).neiFacet0 == nextfacet)
					{
						nextfacet = (*iteedge).neiFacet1;//���µݹ������Ƭ����
					}
					else {
						nextfacet = (*iteedge).neiFacet0;//���µݹ������Ƭ����
					}
				}
				else
					cout << "û���ҵ��ñ�" << endl;
				perFacetLoop.push_back(nextfacet);
				perLoopInsectEdge.push_back((*iteedge).Eindex);//��¼�����ڽӱ�		
				setfacet.erase(nextfacet);
			}
			//����һȦ������һ����Ƭ��������һ��Ƭ��ʼ
			else if (adjfacet.size() == 0)
			{
				//���ҵ���Ƭ�����һ����Ƭ����ʼ��Ƭ�Ĺ�����					
				edgenum = getAdjedge(nextfacet, firstFacet);
				perLoopInsectEdge.push_back(edgenum);
				perlayerInsectEdge.push_back(perLoopInsectEdge);//����߻�	
				perLoopInsectEdge.clear();

				perlayerFacetLoop.push_back(perFacetLoop);//������Ƭ��	
				perFacetLoop.resize(0);

				//��������Ƭ��--------------------------------------------					     
				firstFacet = *setfacet.begin();
				//cout << "����Ƭ���ĵ�һ����Ƭ�ǣ�" << firstFacet <<endl;
				perFacetLoop.push_back(firstFacet);
				nextfacet = obtainFacetLoopDirect(firstFacet, i, edgenum);
				perFacetLoop.push_back(nextfacet);//����Ƭ���ڶ���Ƭ����
				perLoopInsectEdge.push_back(edgenum);
				setfacet.erase(firstFacet);//��¼һ����ɾ��һ��
				setfacet.erase(nextfacet);

			}
		};
		edgenum = getAdjedge(nextfacet, firstFacet);
		perLoopInsectEdge.push_back(edgenum);
		perlayerInsectEdge.push_back(perLoopInsectEdge);//����߻�
		perLoopInsectEdge.clear();

		perlayerFacetLoop.push_back(perFacetLoop);//������Ƭ��	
		perFacetLoop.resize(0);

		insectEdgeData.push_back(perlayerInsectEdge);//����ÿһ�����Ƭ�����ཻ�����ڽӱ�
		perlayerInsectEdge.clear();//��յ�ǰ���ཻ�����ߵ��������Ԫ��		
		insectFacetLoop.push_back(perlayerFacetLoop);
		perlayerFacetLoop.clear();
		setfacet.clear();
	}
	return true;
}