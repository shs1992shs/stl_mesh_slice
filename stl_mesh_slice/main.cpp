#include "STLModel.h"
#include <ctime>
using namespace std;

const unsigned int SCR_WIDTH = 1000, SCR_HEIGHT = 1000;
static float Maxstroke;
STLModel sm;
//Z����Ϊ������-------------------------------------------------------------------------
//����һ����ʼ�����λ��,��ʱZ�����ϣ�x��ָ����Ļ���
glm::vec3 Position = glm::vec3(100.0f, 0.0f, 0.0f);
CameraO camera(Position);

 /*��ʼ����һ֡�����λ�ã�����Ϊ��Ļ����*/
static double lastX = (double)(SCR_WIDTH / 2);
static double lastY = (double)(SCR_HEIGHT / 2);

bool firstMouse = true;//�����Ƿ��ǵ�һ�λ�ȡ�������
/*ʱ��*/
float deltaTime = 0.0f;//��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f;//��ǰ֡
unsigned int VBOs[3];
unsigned int VAOs[3];
unsigned int EBO;
//��������
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//------------------------------------------------------------
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (xpos > 0 && xpos < SCR_WIDTH && ypos < SCR_HEIGHT  && ypos>0)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);/*�����ǰ��*/
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);/*���������*/
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);/*���������*/
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);/*���������*/
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			camera.Position = glm::vec3(2 * Maxstroke, 0.0f, 0.0f);
			camera.ProcessKeyboard(F, deltaTime);/*�ص��������ʼλ��*/
		}			
	}		
}

//��������ӿڵ�ģ�ͳ�����ת�仯
void cursor_pos_leftButton_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS && xpos > 0 && xpos < SCR_WIDTH  && ypos < SCR_HEIGHT  && ypos>0)
	{
		if (firstMouse)// ���bool������ʼʱ���趨Ϊtrue��
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		/*ע���������෴�ģ���Ϊ��ȡ������y�����ǴӶ������ײ��������ӵģ�
		�����϶����ʱ��yֵ����
		����OpenGL���y�����������෴��*/
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;		
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
		return;
}

//�м������ӿڵ�ģ�ͳ���ƽ�Ʊ仯
void cursor_pos_middleButton_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (state == GLFW_PRESS && xpos > 0 && xpos < SCR_WIDTH  && ypos < SCR_HEIGHT  && ypos>0)
	{
		if (firstMouse)// ���bool������ʼʱ���趨Ϊtrue��
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		/*ע���������෴�ģ���Ϊ��ȡ������y�����ǴӶ������ײ��������ӵģ�
		�����϶����ʱ��yֵ����
		����OpenGL���y�����������෴��*/
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		//2019-12-21�������ֻ�Ǹı�view����Ҫ��ʵ��ģ���ƶ�����ʹ��ģ�;���
		camera.ProcessMouseScrollMovement(xoffset, yoffset);
	}
	else
		return;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwSetCursorPosCallback(window, cursor_pos_leftButton_callback);
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		glfwSetCursorPosCallback(window, cursor_pos_middleButton_callback);
	}
	else
		return;
}

//��������Ӧ�¼�
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);//��ȡ��ǰ���λ��
	if (xpos > 0 && xpos < SCR_WIDTH  && ypos > 0 && ypos < SCR_HEIGHT)//��һ�ӿ�
		camera.ProcessMouseScroll(yoffset);			
	else
		return;
}

//�ӿ���Ӧ����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void vertexInit(STLModel &sm)
{
	int layerindex = 27;//Ҫ��ʾ�Ĳ��

	//��ʾ��һ����Ƭƽ��
	float *vertPosition = new float[12];
	sm.GetSlicingPlane(vertPosition, layerindex);//����vector���ݵ�һά���飬�Ա�OpenGL����	

	unsigned int indices[] =
	{
		0,1,2,//��һ��������
		0,2,3//�ڶ���������
	};

	glGenVertexArrays(3, VAOs);
	glGenBuffers(3, VBOs);

	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertPosition, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	delete[] vertPosition;
	//-----------------------------------------------------------------------
	//��ʾSTL��Ƭ
	//int positionSize = sm.vecFacet.size() * 9;
	//float *vertPosition1 = new float[positionSize];
	//sm.GetVertexPosition(vertPosition1);//����vector���ݵ�һά���飬�Ա�OpenGL����		
	
	//��ʾ��һ���ཻ��Ƭ	
	int positionSize = sm.insectFacetData[layerindex].size() * 9;
	float *vertPosition1 = new float[positionSize];
	sm.GetLayerVertexPosition(vertPosition1, layerindex);//����vector���ݵ�һά���飬�Ա�OpenGL����	
	
	//��ʾ��һ���ཻ��Ƭ��-------------------------------------------
	//int positionSize = 0;
	//for (int i = 0; i < sm.insectFacetLoop[layerindex].size(); i++)
	////for (int i = 0; i < 1; i++)
	//{
	//	positionSize += (int)sm.insectFacetLoop[layerindex][i].size();
	//}	
	//positionSize *= 9;
	//float *vertPosition1 = new float[positionSize];
	//sm.GetLayerFL(vertPosition1, layerindex);//����vector���ݵ�һά���飬�Ա�OpenGL����	

	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);	
	glBufferData(GL_ARRAY_BUFFER, positionSize * sizeof(float), vertPosition1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	delete[] vertPosition1;

	//��ʾ�Ȳ����Ƭ����------------------------------------------------
	int positionSize2 = 0;
	for (unsigned long i = 0; i < sm.insectPointData.size(); i++)
	{
		for (unsigned long j = 0; j < sm.insectPointData[i].size(); j++)
		{
			positionSize2 += sm.insectPointData[i][j].size();
		}
	}
	positionSize2 = positionSize2 * 3;
	float *vertPosition2 = new float[positionSize2];
	sm.GetLayerData(vertPosition2);
	glBindVertexArray(VAOs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);	
	glBufferData(GL_ARRAY_BUFFER, positionSize2 * sizeof(float), vertPosition2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);	
	delete[] vertPosition2;
}


void drawSlicingPlane(STLModel &sm, Shader& shader)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//������ɫ
	glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);/*��ɫ*/
	//glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);/*��ɫ*/
	//glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);/*��ɫ*/
	shader.setVec3("sPlaneColor", color);

	//ʹ������ƶ�------�۲����----------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();//����glm::lookAt()����
	shader.setMat4("view", view);
	//���ֿ���ͶӰ����--------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 2000.0f);
	shader.setMat4("projection", projection);
	//ģ�;���---------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-sm.GetXac(), -sm.GetYac(), -sm.GetZac()));//��ģ�����ĵ��ƶ�����������(0.0f,0.0f,0.0f)	
	shader.setMat4("model", model);

	glBindVertexArray(VAOs[0]);
	//����STLģ�ͣ���ͼģʽ���㡢�ߡ���
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(0.1f);	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void drawlayerFacets(STLModel &sm, Shader& shader)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//������ɫ
	//glm::vec3 ourColor = glm::vec3(0.0f, 1.0f, 0.0f);/*��ɫ*/
	glm::vec3 ourColor = glm::vec3(1.0f, 0.0f, 0.0f);/*��ɫ*/
	//glm::vec3 ourColor = glm::vec3(0.0f, 0.0f, 0.0f);/*��ɫ*/
	shader.setVec3("ourColor", ourColor);

	//ʹ������ƶ�------�۲����----------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();//����glm::lookAt()����
	shader.setMat4("view", view);
	//���ֿ���ͶӰ����--------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 2000.0f);
	shader.setMat4("projection", projection);
	//ģ�;���---------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-sm.GetXac(), -sm.GetYac(), -sm.GetZac()));//��ģ�����ĵ��ƶ�����������(0.0f,0.0f,0.0f)	-sm.GetZac()
	shader.setMat4("model", model);

	glBindVertexArray(VAOs[1]);
	//����STLģ�ͣ���ͼģʽ���㡢�ߡ���
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLineWidth(0.1f);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sm.vecFacet.size());	
	glBindVertexArray(0);
}

//����ģ�͵Ȳ����Ƭ����-------------------------------
void drawequalllayers(STLModel &sm, Shader& shader)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//������ɫ
	//glm::vec3 ourColor = glm::vec3(0.0f, 1.0f, 0.0f);/*��ɫ*/
	glm::vec3 ourColor = glm::vec3(1.0f, 0.0f, 0.0f);/*��ɫ*/
	shader.setVec3("ourColor", ourColor);

	//ʹ������ƶ�------�۲����----------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();//����glm::lookAt()����
	shader.setMat4("view", view);
	//���ֿ���ͶӰ����--------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 2000.0f);
	shader.setMat4("projection", projection);
	//ģ�;���---------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-sm.GetXac(), -sm.GetYac(), -sm.GetZac()));//��ģ�����ĵ��ƶ�����������(0.0f,0.0f,0.0f)	-sm.GetZac()
	shader.setMat4("model", model);

	glBindVertexArray(VAOs[2]);
	//��ʼ���ƣ���ͼģʽ���㡢��
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int count = 0;
	for (unsigned long i = 0; i<sm.insectPointData.size(); i++)
	{
		for (unsigned long j = 0; j<sm.insectPointData[i].size(); j++)
		{
			glDrawArrays(GL_LINE_LOOP, count, sm.insectPointData[i][j].size());
			count += sm.insectPointData[i][j].size();
		}
	}
	glBindVertexArray(0);
}

int main()
{
	//��ʼ��ȡSTL
	string stlfilename = "";//����".stl"����
	cout << "Please input the STL file name." << endl;
	cin >> stlfilename;
	char* p = &stlfilename[0];
	if (!cin)
	{
		cout << "�����������������" << endl;
		cin.clear();// ���������־λ
		cin.sync();// ��ռ��̻�����
	}
	clock_t start, finish, start1, finish1;
	start = clock();//��ʼ��ȡ		
	sm.ReadSTLFile(p);
	finish = clock();//���н���
	
	double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;//��ʱ��
	cout << stlfilename << "ģ�͵Ķ�ȡʱ��Ϊ��" << totaltime << "(s)" << endl;
	cout << stlfilename << "ģ��������Ƭ��Ϊ��" << sm.Getfacetnum() << endl;
	cout << stlfilename << "ģ�͵Ķ�����Ϊ:" << sm.vecVertex.size() << endl;
	cout << stlfilename << "ģ�͵ı���Ϊ:" << sm.vecEdge.size() << endl;
	cout << stlfilename << "ģ�͵�Xmin,Xmax,Ymin,Ymax,Zmin,ZmaxΪ��"
		<< sm.GetXmin() << " " << sm.GetXmax() << " " << sm.GetYmin() << " " << sm.GetYmax()
		<< " " << sm.GetZmin() << " " << sm.GetZmax() << endl;
	cout << stlfilename << "ģ�͵��г�Xstroke,Ystroke,ZstrokeΪ��" << sm.GetXstroke() << " "
		<< sm.GetYstroke() << " " << sm.GetZstroke() << endl;
	cout << stlfilename << "ģ�͵�͹��Χ������Xac,Yac,ZacΪ:"
		<< sm.GetXac() << " " << sm.GetYac() << " " << sm.GetZac() << endl;
		
	start1 = clock();//��ʼ��ȡ	
	sm.slice(1);
	finish1 = clock();//���н���
	double totaltime1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;//��ʱ��
	cout << stlfilename << "ģ�͵���Ƭʱ��Ϊ��" << totaltime1 << "(s)" << endl;
	//sm.showinsectPoints();

	Maxstroke = ourmax(ourmax(sm.GetXstroke(), sm.GetYstroke()), sm.GetZstroke());
	camera.Position = glm::vec3(2 * Maxstroke, 0.0f, 0.0f);//���������λ��*/
	//camera.MouseSensitivity = 0.1f;
	camera.MovementSpeed = 20.0f;
	//OpenGL����
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ReadSTL", NULL, NULL);
	if (!window)
	{
		cout << "Error::Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);/*��Ļ���ţ�������Ӧ�ӿ�*/

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//���ع�꣬����׽��
	glfwSetMouseButtonCallback(window, mouse_button_callback);/*��Ӧ��갴���¼�*/
	glfwSetScrollCallback(window, scroll_callback);/*��Ӧ�������¼�*/

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Error::Failed to initialize GLAD" << endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);/*������Ȳ���*/							
	glEnable(GL_LINE_SMOOTH);/*���������*/
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	glEnable(GL_BLEND);//������ϣ�����͸��
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);/*���û��ģʽ*/

	Shader shader1("ReadSTL.vs.txt", "ReadSTL.fs.txt");
	Shader shader2("ReadSTL.vs.txt", "slicingPlane.fs.txt");
	vertexInit(sm);

	while (!glfwWindowShouldClose(window))
	{
		//����ǰһ֡���һ֮֡���ʱ���------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//---------------------------------------------
		processInput(window);

		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader1.use();		
	    //drawlayerFacets(sm,shader1);
		drawequalllayers(sm, shader1);

		shader2.use();
		//drawSlicingPlane(sm,shader2);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(3, VAOs);
	glDeleteBuffers(3,VBOs);
	glfwTerminate();
	return 0;
}