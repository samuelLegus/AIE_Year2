#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>

const static float STEP_SCALE = 0.f;
const static int MARGIN = 10;

Camera::Camera(int a_windowWidth, int a_windowHeight, GLFWwindow * a_window)
{
	m_windowWidth = a_windowWidth;
	m_windowHeight = a_windowHeight;
	m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	m_target = glm::vec3(0.0f, 0.0f, 1.0f);
	m_target = glm::normalize(m_target);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_window = a_window;

	Init();
}

Camera::Camera(int a_windowWidth, int a_windowHeight, const glm::vec3& a_pos, const glm::vec3& a_target, const glm::vec3& a_up, GLFWwindow * a_window)
{
	m_windowWidth = a_windowWidth;
	m_windowHeight = a_windowHeight;
	m_pos = a_pos;

	m_target = a_target;
	m_target = glm::normalize(m_target);

	m_up = a_up;
	m_up = glm::normalize(m_up);

	m_window = a_window;

	Init();
}

void Camera::Init()
{
	
	glm::vec3 HTarget(m_target.x, 0.0, m_target.z);
	HTarget = glm::normalize(HTarget);

	if (HTarget.z >= 0.0f)
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = 360.0f - glm::degrees(asin(HTarget.z));
		}
		else
		{
			m_AngleH = 180.0f + glm::degrees(asin(HTarget.z));
		}
	}
	else
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = glm::degrees(asin(-HTarget.z));
		}
		else
		{
			m_AngleH = 90.0f + glm::degrees(asin(-HTarget.z));
		}
	}

	m_AngleV = -glm::degrees(asin(m_target.y));

	m_OnUpperEdge = false;
	m_OnLowerEdge = false;
	m_OnLeftEdge = false;
	m_OnRightEdge = false;
	m_mousePos.x = m_windowWidth / 2;
	m_mousePos.y = m_windowHeight / 2;

	glfwSetCursorPos(m_window, m_mousePos.x, m_mousePos.y);
}

bool Camera::OnKeyboard(int a_key)
{
	bool Ret = false;

	switch (a_key)
	{
		case GLFW_KEY_UP:
		{
			m_pos += (m_target * STEP_SCALE);
			Ret = true;
		}
		break;

		case GLFW_KEY_DOWN:
		{
			m_pos -= (m_target * STEP_SCALE);
			Ret = true;
		}
		break;

		case GLFW_KEY_LEFT:
		{
			glm::vec3 Left = glm::cross(m_target, m_up);
			Left = glm::normalize(Left);
			Left *= STEP_SCALE;
			m_pos += Left;
			Ret = true;
		}
		break;

		case GLFW_KEY_RIGHT:
		{
			glm::vec3 Right = glm::cross(m_target, m_up);
			Right = glm::normalize(Right);
			Right *= STEP_SCALE;
			m_pos += Right;
			Ret = true;
		}
		break;
	}

	return Ret;
}

void Camera::OnMouse(int a_x, int a_y)
{
	const int DeltaX = x - m_mousePos.x;
	const int DeltaY = y - m_mousePos.y;

	m_mousePos.x = x;
	m_mousePos.y = y;

	m_AngleH += (float)DeltaX / 20.0f;
	m_AngleV += (float)DeltaY / 20.0f;

	if (DeltaX == 0) {
		if (x <= MARGIN) {
			//    m_AngleH -= 1.0f;
			m_OnLeftEdge = true;
		}
		else if (x >= (m_windowWidth - MARGIN)) {
			//    m_AngleH += 1.0f;
			m_OnRightEdge = true;
		}
	}
	else {
		m_OnLeftEdge = false;
		m_OnRightEdge = false;
	}

	if (DeltaY == 0) {
		if (y <= MARGIN) {
			m_OnUpperEdge = true;
		}
		else if (y >= (m_windowHeight - MARGIN)) {
			m_OnLowerEdge = true;
		}
	}
	else {
		m_OnUpperEdge = false;
		m_OnLowerEdge = false;
	}

	Update();
}

void Camera::OnRender()
{
	bool ShouldUpdate = false;

	if (m_OnLeftEdge) {
		m_AngleH -= 0.1f;
		ShouldUpdate = true;
	}
	else if (m_OnRightEdge) {
		m_AngleH += 0.1f;
		ShouldUpdate = true;
	}

	if (m_OnUpperEdge) {
		if (m_AngleV > -90.0f) {
			m_AngleV -= 0.1f;
			ShouldUpdate = true;
		}
	}
	else if (m_OnLowerEdge) {
		if (m_AngleV < 90.0f) {
			m_AngleV += 0.1f;
			ShouldUpdate = true;
		}
	}

	if (ShouldUpdate) {
		Update();
	}
}

void Camera::Update()
{
	const glm::vec3 Vaxis(0.0f, 1.0f, 0.0f);

	// Rotate the view vector by the horizontal angle around the vertical axis
	glm::vec3 View(1.0f, 0.0f, 0.0f);
	View = 

	// Rotate the view vector by the vertical angle around the horizontal axis
	Vector3f Haxis = Vaxis.Cross(View);
	Haxis.Normalize();
	View.Rotate(m_AngleV, Haxis);

	m_target = View;
	m_target.Normalize();

	m_up = m_target.Cross(Haxis);
	m_up.Normalize();
}