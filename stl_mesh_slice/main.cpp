#include "STLModel.h"
#include <ctime>
using namespace std;

const unsigned int SCR_WIDTH = 1000, SCR_HEIGHT = 1000;
static float Maxstroke;
STLModel sm;
//Z轴作为上向量-------------------------------------------------------------------------
//定义一个初始摄像机位置,此时Z轴向上，x轴指向屏幕外侧
glm::vec3 Position = glm::vec3(100.0f, 0.0f, 0.0f);
CameraO camera(Position);

 /*初始的上一帧的鼠标位置，设置为屏幕中心*/
static double lastX = (double)(SCR_WIDTH / 2);
static double lastY = (double)(SCR_HEIGHT / 2);

bool firstMouse = true;//检验是否是第一次获取鼠标输入
/*时间*/
float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//当前帧
unsigned int VBOs[3];
unsigned int VAOs[3];
unsigned int EBO;
//键盘输入
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
			camera.ProcessKeyboard(FORWARD, deltaTime);/*摄像机前移*/
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);/*摄像机后移*/
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);/*摄像机左移*/
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);/*摄像机右移*/
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			camera.Position = glm::vec3(2 * Maxstroke, 0.0f, 0.0f);
			camera.ProcessKeyboard(F, deltaTime);/*回到摄像机初始位置*/
		}			
	}		
}

//左键控制视口的模型场景旋转变化
void cursor_pos_leftButton_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS && xpos > 0 && xpos < SCR_WIDTH  && ypos < SCR_HEIGHT  && ypos>0)
	{
		if (firstMouse)// 这个bool变量初始时是设定为true的
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		/*注意这里是相反的，因为获取的鼠标的y坐标是从顶部往底部依次增加的，
		往下拖动鼠标时，y值增加
		这与OpenGL里的y轴正方向是相反的*/
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;		
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
		return;
}

//中键控制视口的模型场景平移变化
void cursor_pos_middleButton_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (state == GLFW_PRESS && xpos > 0 && xpos < SCR_WIDTH  && ypos < SCR_HEIGHT  && ypos>0)
	{
		if (firstMouse)// 这个bool变量初始时是设定为true的
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		/*注意这里是相反的，因为获取的鼠标的y坐标是从顶部往底部依次增加的，
		往下拖动鼠标时，y值增加
		这与OpenGL里的y轴正方向是相反的*/
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		//2019-12-21，摄像机只是改变view矩阵，要想实现模型移动，得使用模型矩阵
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

//鼠标滚轮响应事件
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);//获取当前鼠标位置
	if (xpos > 0 && xpos < SCR_WIDTH  && ypos > 0 && ypos < SCR_HEIGHT)//第一视口
		camera.ProcessMouseScroll(yoffset);			
	else
		return;
}

//视口响应函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void vertexInit(STLModel &sm)
{
	int layerindex = 27;//要显示的层号

	//显示单一层切片平面
	float *vertPosition = new float[12];
	sm.GetSlicingPlane(vertPosition, layerindex);//拷贝vector数据到一维数组，以便OpenGL调用	

	unsigned int indices[] =
	{
		0,1,2,//第一个三角形
		0,2,3//第二个三角形
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
	//显示STL面片
	//int positionSize = sm.vecFacet.size() * 9;
	//float *vertPosition1 = new float[positionSize];
	//sm.GetVertexPosition(vertPosition1);//拷贝vector数据到一维数组，以便OpenGL调用		
	
	//显示单一层相交面片	
	int positionSize = sm.insectFacetData[layerindex].size() * 9;
	float *vertPosition1 = new float[positionSize];
	sm.GetLayerVertexPosition(vertPosition1, layerindex);//拷贝vector数据到一维数组，以便OpenGL调用	
	
	//显示单一层相交面片环-------------------------------------------
	//int positionSize = 0;
	//for (int i = 0; i < sm.insectFacetLoop[layerindex].size(); i++)
	////for (int i = 0; i < 1; i++)
	//{
	//	positionSize += (int)sm.insectFacetLoop[layerindex][i].size();
	//}	
	//positionSize *= 9;
	//float *vertPosition1 = new float[positionSize];
	//sm.GetLayerFL(vertPosition1, layerindex);//拷贝vector数据到一维数组，以便OpenGL调用	

	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);	
	glBufferData(GL_ARRAY_BUFFER, positionSize * sizeof(float), vertPosition1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	delete[] vertPosition1;

	//显示等层厚切片轮廓------------------------------------------------
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

	//设置颜色
	glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);/*绿色*/
	//glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);/*红色*/
	//glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);/*黑色*/
	shader.setVec3("sPlaneColor", color);

	//使摄像机移动------观察矩阵----------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();//创建glm::lookAt()函数
	shader.setMat4("view", view);
	//滚轮控制投影矩阵--------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 2000.0f);
	shader.setMat4("projection", projection);
	//模型矩阵---------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-sm.GetXac(), -sm.GetYac(), -sm.GetZac()));//将模型中心点移动到世界坐标(0.0f,0.0f,0.0f)	
	shader.setMat4("model", model);

	glBindVertexArray(VAOs[0]);
	//绘制STL模型，绘图模式：点、线、面
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

	//设置颜色
	//glm::vec3 ourColor = glm::vec3(0.0f, 1.0f, 0.0f);/*绿色*/
	glm::vec3 ourColor = glm::vec3(1.0f, 0.0f, 0.0f);/*红色*/
	//glm::vec3 ourColor = glm::vec3(0.0f, 0.0f, 0.0f);/*黑色*/
	shader.setVec3("ourColor", ourColor);

	//使摄像机移动------观察矩阵----------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();//创建glm::lookAt()函数
	shader.setMat4("view", view);
	//滚轮控制投影矩阵--------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 2000.0f);
	shader.setMat4("projection", projection);
	//模型矩阵---------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-sm.GetXac(), -sm.GetYac(), -sm.GetZac()));//将模型中心点移动到世界坐标(0.0f,0.0f,0.0f)	-sm.GetZac()
	shader.setMat4("model", model);

	glBindVertexArray(VAOs[1]);
	//绘制STL模型，绘图模式：点、线、面
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLineWidth(0.1f);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sm.vecFacet.size());	
	glBindVertexArray(0);
}

//绘制模型等层厚切片轮廓-------------------------------
void drawequalllayers(STLModel &sm, Shader& shader)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//设置颜色
	//glm::vec3 ourColor = glm::vec3(0.0f, 1.0f, 0.0f);/*绿色*/
	glm::vec3 ourColor = glm::vec3(1.0f, 0.0f, 0.0f);/*红色*/
	shader.setVec3("ourColor", ourColor);

	//使摄像机移动------观察矩阵----------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();//创建glm::lookAt()函数
	shader.setMat4("view", view);
	//滚轮控制投影矩阵--------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 2000.0f);
	shader.setMat4("projection", projection);
	//模型矩阵---------------------------------------------------
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-sm.GetXac(), -sm.GetYac(), -sm.GetZac()));//将模型中心点移动到世界坐标(0.0f,0.0f,0.0f)	-sm.GetZac()
	shader.setMat4("model", model);

	glBindVertexArray(VAOs[2]);
	//开始绘制，绘图模式：点、线
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
	//开始读取STL
	string stlfilename = "";//输入".stl"带上
	cout << "Please input the STL file name." << endl;
	cin >> stlfilename;
	char* p = &stlfilename[0];
	if (!cin)
	{
		cout << "输入错误，请重新输入" << endl;
		cin.clear();// 清理输入标志位
		cin.sync();// 清空键盘缓冲区
	}
	clock_t start, finish, start1, finish1;
	start = clock();//开始读取		
	sm.ReadSTLFile(p);
	finish = clock();//运行结束
	
	double totaltime = (double)(finish - start) / CLOCKS_PER_SEC;//总时间
	cout << stlfilename << "模型的读取时间为：" << totaltime << "(s)" << endl;
	cout << stlfilename << "模型三角面片数为：" << sm.Getfacetnum() << endl;
	cout << stlfilename << "模型的顶点数为:" << sm.vecVertex.size() << endl;
	cout << stlfilename << "模型的边数为:" << sm.vecEdge.size() << endl;
	cout << stlfilename << "模型的Xmin,Xmax,Ymin,Ymax,Zmin,Zmax为："
		<< sm.GetXmin() << " " << sm.GetXmax() << " " << sm.GetYmin() << " " << sm.GetYmax()
		<< " " << sm.GetZmin() << " " << sm.GetZmax() << endl;
	cout << stlfilename << "模型的行程Xstroke,Ystroke,Zstroke为：" << sm.GetXstroke() << " "
		<< sm.GetYstroke() << " " << sm.GetZstroke() << endl;
	cout << stlfilename << "模型的凸包围盒中心Xac,Yac,Zac为:"
		<< sm.GetXac() << " " << sm.GetYac() << " " << sm.GetZac() << endl;
		
	start1 = clock();//开始读取	
	sm.slice(1);
	finish1 = clock();//运行结束
	double totaltime1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;//总时间
	cout << stlfilename << "模型的切片时间为：" << totaltime1 << "(s)" << endl;
	//sm.showinsectPoints();

	Maxstroke = ourmax(ourmax(sm.GetXstroke(), sm.GetYstroke()), sm.GetZstroke());
	camera.Position = glm::vec3(2 * Maxstroke, 0.0f, 0.0f);//更新摄像机位置*/
	//camera.MouseSensitivity = 0.1f;
	camera.MovementSpeed = 20.0f;
	//OpenGL窗口
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);/*屏幕缩放，重新适应视口*/

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//隐藏光标，并捕捉它
	glfwSetMouseButtonCallback(window, mouse_button_callback);/*响应鼠标按键事件*/
	glfwSetScrollCallback(window, scroll_callback);/*响应鼠标滚轮事件*/

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Error::Failed to initialize GLAD" << endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);/*开启深度测试*/							
	glEnable(GL_LINE_SMOOTH);/*开启抗锯齿*/
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	glEnable(GL_BLEND);//开启混合，设置透明
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);/*设置混合模式*/

	Shader shader1("ReadSTL.vs.txt", "ReadSTL.fs.txt");
	Shader shader2("ReadSTL.vs.txt", "slicingPlane.fs.txt");
	vertexInit(sm);

	while (!glfwWindowShouldClose(window))
	{
		//计算前一帧与后一帧之间的时间差------------
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