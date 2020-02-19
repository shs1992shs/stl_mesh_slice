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
const float YAW         = -90.0f; //ƫ��������Ϊ0�Ļ���ʼ��cameraFront��ָ���Ҳ�(1,0,0),����ʼ��cameraFront��ָ��z�ĸ�����
const float PITCH       =  0.0f;  //����������Ϊ0����������
const float SPEED       =  2.5f;  //������ƶ��ٶ�
const float SENSITIVITY =  0.1f;  //�����ƶ�������ֵ
const float ZOOM        =  45.0f; //ͶӰ��Ұֵ 

const float YAWZ = 180.0f;//������ΪZ��ʱ�ĳ�ʼƫ���ǣ���ʱcameraFront��ָ��x��ĸ�����(0,0,0)
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
    float MovementSpeed;//���̿���������ƶ��ٶȵ��ڲ���
    float MouseSensitivity;//�����ƶ�������ֵ
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();//�ᴴ��ǰ��������������������
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();//�ᴴ��ǰ��������������������
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
            Position -= Right * velocity;//��������ʱ�����Զ�����������������������Ҫ���ı�
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
        Front = glm::normalize(front);//��ʼֵΪ��0��0��-1��
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

//2018-10-1���¶���camera�࣬��Z��Ϊ��������Y��Ϊ������
//���¶���һ����z��Ϊ���������������
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
	float MovementSpeed;//������ƶ��ٶȵ��ڲ���
	float MouseSensitivity;//�����ƶ�������ֵ
	float Zoom;

	float cameraPositonLenght;//�����λ������ģ��
	// Constructor with vectors
	CameraZ(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float yaw = YAWZ, float pitch = PITCH) : Front(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();//�ᴴ��ǰ��������������������
	}
	// Constructor with scalar values
	CameraZ(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();//�ᴴ��ǰ��������������������
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
			Position -= Right * velocity;//��������ʱ�����Զ�����������������������Ҫ���ı�
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
	
	//2018-10-10,���Ӻ�����갴����������ԭ����ת
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
		//�����λ������ģ��
		//cameraPositonLenght = sqrt(pow(Position.x, 2) + pow(Position.y, 2) + pow(Position.z, 2));
		//--------------------------------------------------------------------------------------
		Position.x = cameraPositonLenght*cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Position.y = cameraPositonLenght*sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Position.z = cameraPositonLenght*sin(glm::radians(Pitch));

	}
	//�����������׼��������ԭ��
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
		front.z = sin(glm::radians(Pitch));//Z��Ϊ������
		Front = glm::normalize(front);//��ʼֵΪ��-1.0��0.0��0.0��
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

//2019-12-17���¶���һ��camera�࣬��Z��Ϊ��������Y��Ϊ��������������ϵԭ��Ϊ�ӽ�����
class CameraO
{
public:
	// Camera Attributes
	glm::vec3 Position;//�����λ��
	glm::vec3 Target;//������۲�ĵ㣬һ����Ϊ��������
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;	
	glm::vec3 vecPosition = glm::vec3(1.0f,0.0f,0.0f);/*���������ǰ���ƶ�*/
	// Euler Angles
	float Yaw;//ƫ����
	float Pitch ;//������
	// Camera options
	float MovementSpeed;//������ƶ��ٶȵ��ڲ���
	float MouseSensitivity;//�����ƶ�������ֵ
	float cameraPositonLenght;//�����λ�þ��룬������������	
	// Constructor with vectors
	CameraO(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f)) : Target(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY),Yaw(YAWZ),Pitch(PITCH)
	{
		Position =position;
		WorldUp = up;
		updateCameraVectors();//�ᴴ�����������ϵ��ǰ��������������������
	}
	// Constructor with scalar values
	CameraO(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Target(glm::vec3(0.0f,0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();//�ᴴ�����������ϵ��ǰ��������������������
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix	�����������׼��������ԭ��
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
			updateCameraVectors();//�ᴴ��ǰ��������������������	
		}
		if (direction == RIGHT)
		{
			Yaw += velocity;
			updateCameraVectors();//�ᴴ��ǰ��������������������
		}
		if (direction == F)
		{	
	        Yaw = YAWZ;
		    Pitch = PITCH;
		    updateCameraVectors();//�ᴴ��ǰ��������������������	
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
	   cameraPositonLenght = sqrt(pow(Position.x, 2) + pow(Position.y, 2) + pow(Position.z, 2));/*�����λ�ó���*/
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
		target.z = sin(glm::radians(Pitch));//Z��Ϊ������
		Target = glm::normalize(target);//��ʼֵΪ��0.0��0.0��0.0��
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Target, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Target));
	}
};
#endif