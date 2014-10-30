#pragma once

struct GLFWwindow;

// base class that each project can inherit from
class Application
{
public:

	Application() : m_running(false) {}
	virtual ~Application() {}

	bool	create(const char* a_name, int a_width, int a_height, int a_argc, char* a_argv[]);

	void	run();

	void	quit()	{ m_running = false; }

	inline GLFWwindow * GetWindow()
	{
		return m_window;
	}

protected:

	virtual bool	onCreate(int a_argc, char* a_argv[]) = 0;
	virtual void	onUpdate(float a_deltaTime) = 0;
	virtual void	onDraw() = 0;
	virtual void	onDestroy() = 0;
	
	GLFWwindow*		m_window;
	bool			m_running;
};