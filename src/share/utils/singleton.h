#pragma once

/*
* Singleton Template
* CRTP Design Pattern( inherited this class)
* Local static variable(thread safety initialize)
*/

template<typename T>
class Singleton
{
public:
	T& instance() {
		static T _instance;
		return _instance;
	}

	~Singleton() = default;
	Singleton(Singleton const&) = delete;
	Singleton& operator=(Singleton const&) = delete;
protected:
	Singleton() = default;
};
