#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
 {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	F
};

// Default camera values
const float YAW         = -90.0f; //偏航角设置为0的话初始的cameraFront就指向右侧(1,0,0),而初始的cameraFront需指向z的负半轴
const float PITCH       =  0.0f;  //俯仰角设置为0，正视物体
const float SPEED       =  2.5f;  //摄像机移动速度
const float SENSITIVITY =  0.1f;  //鼠标的移动灵敏度值
const float ZOOM        =  45.0f; //投影视野值 

const float YAWZ = 180.0f;//上向量为Z轴时的初始偏航角，此时cameraFront就指向x轴的负半轴(0,0,0)
// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;//键盘控制摄像机移动速度调节参数
    float MouseSensitivity;//鼠标的移动灵敏度值
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();//会创建前向量，右向量和上向量
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();//会创建前向量，右向量和上向量
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;//创建对象时，会自动创建右向量，右向量不需要被改变
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);//初始值为（0，0，-1）
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

//2018-10-1重新定义camera类，上Z轴为上向量，Y轴为右向量
//重新定义一个以z轴为上向量的摄像机类
class CameraZ
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;//摄像机移动速度调节参数
	float MouseSensitivity;//鼠标的移动灵敏度值
	float Zoom;

	float cameraPositonLenght;//摄像机位置向量模长
	// Constructor with vectors
	CameraZ(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float yaw = YAWZ, float pitch = PITCH) : Front(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();//会创建前向量，右向量和上向量
	}
	// Constructor with scalar values
	CameraZ(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();//会创建前向量，右向量和上向量
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;//创建对象时，会自动创建右向量，右向量不需要被改变
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}
	
	//2018-10-10,增加函数鼠标按键控制物体原地旋转
	void ProcessMouseButtonMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		//摄像机位置向量模长
		//cameraPositonLenght = sqrt(pow(Position.x, 2) + pow(Position.y, 2) + pow(Position.z, 2));
		//--------------------------------------------------------------------------------------
		Position.x = cameraPositonLenght*cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Position.y = cameraPositonLenght*sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Position.z = cameraPositonLenght*sin(glm::radians(Pitch));

	}
	//保持照相机对准世界坐标原点
	glm::mat4 GetViewMatrixStatic()
	{
		return glm::lookAt(Position, glm::vec3(0.0f,0.0f,0.0f), Up);
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.z = sin(glm::radians(Pitch));//Z轴为上向量
		Front = glm::normalize(front);//初始值为（-1.0，0.0，0.0）
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

//2019-12-17重新定义一个camera类，上Z轴为上向量，Y轴为右向量，以坐标系原点为视角中心
class CameraO
{
public:
	// Camera Attributes
	glm::vec3 Position;//摄像机位置
	glm::vec3 Target;//摄像机观察的点，一般设为世界中心
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;	
	glm::vec3 vecPosition = glm::vec3(1.0f,0.0f,0.0f);/*计算摄像机前后移动*/
	// Euler Angles
	float Yaw;//偏航角
	float Pitch ;//俯仰角
	// Camera options
	float MovementSpeed;//摄像机移动速度调节参数
	float MouseSensitivity;//鼠标的移动灵敏度值
	float cameraPositonLenght;//摄像机位置距离，距离世界中心	
	// Constructor with vectors
	CameraO(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f)) : Target(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY),Yaw(YAWZ),Pitch(PITCH)
	{
		Position =position;
		WorldUp = up;
		updateCameraVectors();//会创建摄像机坐标系下前向量，右向量和上向量
	}
	// Constructor with scalar values
	CameraO(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Target(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();//会创建摄像机坐标系下前向量，右向量和上向量
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix	保持照相机对准世界坐标原点
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Target, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += vecPosition * velocity;
		if (direction == BACKWARD)
			Position -= vecPosition * velocity;
		if (direction == LEFT)
		{
			Yaw -= velocity;
			updateCameraVectors();//会创建前向量，右向量和上向量	
		}
		if (direction == RIGHT)
		{
			Yaw += velocity;
			updateCameraVectors();//会创建前向量，右向量和上向量
		}
		if (direction == F)
		{	
	        Yaw = YAWZ;
		    Pitch = PITCH;
		    updateCameraVectors();//会创建前向量，右向量和上向量	
		}
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;
		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		// Update Target, Right and Up Vectors using the updated Euler angles			
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	 void ProcessMouseScroll(float yoffset)
	{
		 Position += vecPosition*yoffset*MovementSpeed;
		 updateCameraVectors();
    }
	
	// Processes input received from a mouse scroll-wheel press event. Expects the offset value in both the x and y direction.
	 void ProcessMouseScrollMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		 xoffset *= MouseSensitivity;
		 yoffset *= MouseSensitivity;
		 
		Position.y += xoffset;
		Position.z += yoffset;
		// Update Target, Right and Up Vectors using the updated Euler angles			
		updateCameraVectors();
    }
	
	//obtain the camera position lenght
    float getLenght()
	{
	   cameraPositonLenght = sqrt(pow(Position.x, 2) + pow(Position.y, 2) + pow(Position.z, 2));/*摄像机位置长度*/
	   return cameraPositonLenght;
	}
private:
	// Calculates the viewpoint vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Target vector
		float d = getLenght();
		Position.x = d*cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Position.y = d*sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Position.z = d*sin(glm::radians(Pitch));
		glm::vec3 target;
		target.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		target.y = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		target.z = sin(glm::radians(Pitch));//Z轴为上向量
		Target = glm::normalize(target);//初始值为（0.0，0.0，0.0）
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Target, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Target));
	}
};
#endif