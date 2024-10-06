#pragma once

class Application
{
public: 
	Application() {}
	virtual void Initialize() {}
	virtual void Update(float DeltaTime) {}
	virtual void Render(float AspectRatio) {}
	virtual void Shutdown() {}

private:
	Application(const Application&);
	Application& operator=(const Application&);
};

