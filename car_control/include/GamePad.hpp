#pragma once
#ifndef __GAMEPAD_HPP__
# define __GAMEPAD_HPP__

#include <iostream>
#include <mutex>
#include <boost/python.hpp>

#define GAMEPAD_MODULE_NAME "gamepads"
#define GAMEPAD_CLASS_NAME "ShanWanGamepad"

namespace py = boost::python;

typedef struct
{
	double x;
	double y;
	double z;
} Vector3d;

typedef struct
{
	Vector3d	analog_stick_left;
	Vector3d	analog_stick_right;
	bool		button_x;
	bool		button_y;
	bool		button_a;
	bool		button_b;
	bool		button_select;
	bool		button_start;
	bool		button_home;
	bool		button_l1;
	bool		button_l2;
	bool		button_r1;
	bool		button_r2;
} Input;

class GamePad
{
private:
	py::object			pModule;
	py::object			pClass;
	py::object			pInstance;
	static GamePad* 	_instance;
	static std::mutex 	_mutex;
	Input				convert(const py::object&);
	bool				hasAttr(const py::object&, const char*);	
protected:
	GamePad();
	~GamePad();
public:
	GamePad(const GamePad&) = delete;
	GamePad& operator=(const GamePad&) = delete;
	static GamePad*	getInstance();
	bool 			deleteInstance();
	Input 			readInput();
};

#endif