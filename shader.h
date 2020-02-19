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
	/*创建构造函数*/
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		/*1. 从文件路径中获取顶点/片段着色器*/
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;

		// 保证ifstream对象可以抛出异常：
		/*failbit和badbit均是用来检测流的状态(flags)是否正常
		failbit是逻辑错误，badbit是读写错误，其返回的是true或false*/
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			// 读取文件的缓冲内容到数据流中
			stringstream vShaderStream, fShaderStream;
			/*缓冲区类streambuf,供输入输出流对象使用，每个标准C++输入输出流对象
			都包含一个指向streambuf的指针，通过调用成员函数rdbuf()获取指针，
			直接访问底层streambuf对象，进行数据的直接读写，跳过上层的格式化输入输出操作
			对于文件流、字符串流类，都可以使用
			*/
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//关闭文件
			vShaderFile.close();
			fShaderFile.close();
			// 转换数据流到string
			vertexCode = vShaderStream.str();//stringstream类的成员函数str()返回一个临时的string对象
			fragmentCode = fShaderStream.str();//str("")如果是空字符串，就会清空缓冲，如果是非空字符串就是重新赋值
		}
		catch (ifstream::failure e)
		{
			cout << "ERROR::SHADER::File not succesfully read" << endl;
		}
		/*将字符串转换到字符数组*/
		const GLchar* vShaderCode = vertexCode.c_str();//string类的成员函数c_str()是将字符串全部输出
		const GLchar* fShaderCode = fragmentCode.c_str();

		/*编译着色器*/
		GLuint vertexShader, fragmentShader;
		
		//顶点着色器
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		checkShaderErrors(vertexShader, "VERTEX");

		//片段着色器
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkShaderErrors(fragmentShader, "FRAGMENT");

		//着色器程序对象
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		checkShaderErrors(shaderProgram, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	/*启用着色器*/
	void use()
	{
		glUseProgram(shaderProgram);
	}

	//应用uniform函数
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
		/*第三个参数指是否需要交换矩阵的行与列，OpenGL主要使用列主序布局，GLM默认是列主序*/
		/*第四个参数是真正的矩阵，可以以矩阵的地址传入*/
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
	//使用函数检查着色器的编译和链接状态
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
