#ifndef SHADER_H
#define SHADER_H

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader
{
public:
	GLuint shaderProgram;
	/*�������캯��*/
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		/*1. ���ļ�·���л�ȡ����/Ƭ����ɫ��*/
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;

		// ��֤ifstream��������׳��쳣��
		/*failbit��badbit���������������״̬(flags)�Ƿ�����
		failbit���߼�����badbit�Ƕ�д�����䷵�ص���true��false*/
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			// ��ȡ�ļ��Ļ������ݵ���������
			stringstream vShaderStream, fShaderStream;
			/*��������streambuf,���������������ʹ�ã�ÿ����׼C++�������������
			������һ��ָ��streambuf��ָ�룬ͨ�����ó�Ա����rdbuf()��ȡָ�룬
			ֱ�ӷ��ʵײ�streambuf���󣬽������ݵ�ֱ�Ӷ�д�������ϲ�ĸ�ʽ�������������
			�����ļ������ַ������࣬������ʹ��
			*/
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//�ر��ļ�
			vShaderFile.close();
			fShaderFile.close();
			// ת����������string
			vertexCode = vShaderStream.str();//stringstream��ĳ�Ա����str()����һ����ʱ��string����
			fragmentCode = fShaderStream.str();//str("")����ǿ��ַ������ͻ���ջ��壬����Ƿǿ��ַ����������¸�ֵ
		}
		catch (ifstream::failure e)
		{
			cout << "ERROR::SHADER::File not succesfully read" << endl;
		}
		/*���ַ���ת�����ַ�����*/
		const GLchar* vShaderCode = vertexCode.c_str();//string��ĳ�Ա����c_str()�ǽ��ַ���ȫ�����
		const GLchar* fShaderCode = fragmentCode.c_str();

		/*������ɫ��*/
		GLuint vertexShader, fragmentShader;
		
		//������ɫ��
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		checkShaderErrors(vertexShader, "VERTEX");

		//Ƭ����ɫ��
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkShaderErrors(fragmentShader, "FRAGMENT");

		//��ɫ���������
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		checkShaderErrors(shaderProgram, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	/*������ɫ��*/
	void use()
	{
		glUseProgram(shaderProgram);
	}

	//Ӧ��uniform����
	// ------------------------------------------------------------------------
	void setBool(const string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	 void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }
	void setVec2(const std::string &name, float x,float y) const
	{
		glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x,y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }
	void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
		/*����������ָ�Ƿ���Ҫ��������������У�OpenGL��Ҫʹ�������򲼾֣�GLMĬ����������*/
		/*���ĸ������������ľ��󣬿����Ծ���ĵ�ַ����*/
        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
	// ------------------------------------------------------------------------
private:
	//ʹ�ú��������ɫ���ı��������״̬
	void checkShaderErrors(GLuint shader, string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				cout << "ERROR::Failed to Compile" << type << "\n" << infoLog
					<<"\n--------------------------"<< endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				cout << "ERROR::Failed to Link" << type << "\n" << infoLog
					<< "\n--------------------------" << endl;
			}
		}
	}
};

#endif
