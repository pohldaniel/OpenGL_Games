#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#define	EPSILON  1e-6f

class TrackBall {

public:

	enum Button {
		ENoButton = 0x0,
		ELeftButton = 0x1,
		EMiddleButton = 0x2,
		ERightButton = 0x4
	};

	enum Modifier {
		ENoModifier = 0x0,
		EShiftModifier = 0x1,
		EControlModifier = 0x2,
		EAltModifier = 0x3
	};

	TrackBall() : _width(0), _height(0), _tbActivateButton(ELeftButton), _dActivateButton(ERightButton), _pActivateButton(EMiddleButton),
		_tbActivateModifiers(ENoModifier), _dActivateModifiers(ENoModifier), _pActivateModifiers(ENoModifier), _tbActive(false), _dActive(false), _pActive(false) {
		_r = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		_incr = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		_pan = glm::vec3(0.0f, 0.0f, 0.0f);
		_dolly = glm::vec3(0.0f, 0.0f, 0.0f);
		_centroid = glm::vec3(0.0f, 0.0f, 0.0f);
		_tbScale = 1.0f;
		_dScale = 0.01f;
		_pScale = 0.01f;
	}

	~TrackBall() {}

	void reset() {
		_r = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		_incr = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		_pan = glm::vec3(0.0f, 0.0f, 0.0f);
		_dolly = glm::vec3(0.0f, 0.0f, 0.0f);
		_centroid = glm::vec3(0.0f, 0.0f, 0.0f);
		_tbScale = 1.0f;
		_dScale = 0.01f;
		_pScale = 0.01f;
		_tbActive = false;
		_dActive = false;
		_pActive = false;

	}

	//
	//  reshape
	//
	//    This function should be called, whenever the region containing the control
	//  has been resized. Typically, this is a window resize event.
	////////////////////////////////////////////////////////////////////////////
	void reshape(int width, int height) { _width = width; _height = height; }

	//
	//
	//////////////////////////////////////////////////////////////////
	void mouse(Button button, Modifier mod, bool depressed, int x, int y) {

		if (button == _tbActivateButton && depressed && (_tbActivateModifiers == mod)) {

			_tbActive = true;
			_x = _x0 = x;
			_y = _y0 = y;
			_dx = _dy = 0;

		}
		else if (button == _tbActivateButton && !depressed) {
			if (_dx == 0 && _dy == 0)
				update();
			_tbActive = false;
			_dx = _dy = 0;
		}

		if (button == _dActivateButton && depressed && (_dActivateModifiers == mod)) {

			_dActive = true;
			_x = _x0 = x;
			_y = _y0 = y;
			_dx = _dy = 0;

		}
		else if (button == _dActivateButton && !depressed) {
			if (_dx == 0 && _dy == 0)
				update();
			_dActive = false;
			_dx = _dy = 0;
		}

		if (button == _pActivateButton && depressed && (_pActivateModifiers == mod)) {

			_pActive = true;
			_x = _x0 = x;
			_y = _y0 = y;
			_dx = _dy = 0;

		}
		else if (button == _pActivateButton && !depressed) {
			if (_dx == 0 && _dy == 0)
				update();
			_pActive = false;
			_dx = _dy = 0;
		}
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void motion(int x, int y) {
		if (_tbActive || _dActive || _pActive) {
			_dx = x - _x;   _dy = y - _y;
			_x = x;   _y = y;
			update();
		}
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void idle() {
		//simply increment the rotation
		_r = _incr * _r;
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void updateTrackball() {
		float min = _width < _height ? static_cast<float>(_width) : static_cast<float>(_height);
		min *= 0.5f;
		glm::vec3 offset(static_cast<float>(_width) / 2.0f, static_cast<float>(_height) / 2.0f, 0.0f);
		glm::vec3 a(static_cast<float>(_x - _dx), static_cast<float>(_y + _dy), 0.0f);
		glm::vec3 b(static_cast<float>(_x), static_cast<float>(_y), 0.0f);

		a -= offset;
		b -= offset;
		a /= min;
		b /= min;

		a[2] = pow(2.0f, 0.5f * glm::length(a));
		a = glm::normalize(a);
		b[2] = pow(2.0f, 0.5f * glm::length(b));
		b = glm::normalize(b);
		glm::vec3 axis = glm::cross(a, b);
		
		if (glm::length2(axis) < EPSILON * EPSILON) {
			_incr = glm::quat(1.0f, 0.0f, 0.0, 0.0f);
		}else {
			axis = glm::normalize(axis);
			float rad = acos(glm::dot(a, b));
			_incr = glm::angleAxis(rad * _tbScale, axis);
		}
		_r = _incr * _r;
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void updatePan() {
		glm::vec3 v(static_cast<float>(_dx), static_cast<float>(_dy), 0.0f);
		_pan += v * _pScale;
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void updateDolly() {
		glm::vec3 v(0.0f, 0.0f, static_cast<float>(_dy));
		_dolly -= v * _dScale;
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void update() {
		if (_tbActive)
			updateTrackball();
		if (_dActive)
			updateDolly();
		if (_pActive)
			updatePan();
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	const glm::mat4 getTransform() const {		
		glm::mat4 transform = glm::translate(glm::mat4(1.0), _dolly);
		transform = glm::translate(transform, _pan);
		transform = glm::translate(transform, _centroid);
		transform *= glm::mat4_cast(_r);
		transform = glm::translate(transform, -_centroid);
		return transform;
	}

	const glm::mat4 getTransform(const glm::vec3& axis, float degree) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0), _dolly);
		transform = glm::translate(transform, _pan);
		transform = glm::translate(transform, _centroid);
		transform *= glm::mat4_cast(_r);
		transform *= glm::rotate(glm::mat4(1.0f), degree, axis);
		transform = glm::translate(transform, -_centroid);
		return transform;
	}

	//
	//  setTrackballActivate
	//
	//    Set the mouse button and optional modifiers that activate
	//  the trackball.
	//////////////////////////////////////////////////////////////////
	void setTrackballActivate(Button b, Modifier m = ENoModifier) {
		_tbActivateButton = b;
		_tbActivateModifiers = m;
	}

	//
	//  setTrackballScale
	//
	//    Set the speed for the trackball.
	//////////////////////////////////////////////////////////////////
	void setTrackballScale(float scale) {
		_tbScale = scale;
	}

	//
	//  setDollyActivate
	//
	//    Set the mouse button and optional modifiers that activate
	//  the dolly operation.
	//////////////////////////////////////////////////////////////////
	void setDollyActivate(Button b, Modifier m = ENoModifier) {
		_dActivateButton = b;
		_dActivateModifiers = m;
	}

	//
	//  setDollyScale
	//
	//    Set the speed for the dolly operation.
	//////////////////////////////////////////////////////////////////
	void setDollyScale(float scale) {
		_dScale = scale;
	}

	//
	//  setDollyPosition
	//
	//    Set the Dolly to a specified distance.
	//////////////////////////////////////////////////////////////////
	void setDollyPosition(float pos) {
		_dolly[2] = pos;
	}

	void setDollyPosition(float posx, float posy, float posz) {
		_dolly[0] = posx;
		_dolly[1] = posy;
		_dolly[2] = posz;
	}

	void setDollyPosition(const glm::vec3& position) {
		_dolly = position;
	}

	//
	//  setPanActivate
	//
	//    Set the mouse button and optional modifiers that activate
	//  the pan operation.
	//////////////////////////////////////////////////////////////////
	void setPanActivate(Button b, Modifier m = ENoModifier) {
		_pActivateButton = b;
		_pActivateModifiers = m;
	}

	//
	//  setPanScale
	//
	//    Set the speed for panning.
	//////////////////////////////////////////////////////////////////
	void setPanScale(float scale) {
		_pScale = scale;
	}

	//
	//  setCenterOfRotation
	//
	//    Set the point around which the trackball will rotate.
	//////////////////////////////////////////////////////////////////
	void setCenterOfRotation(const glm::vec3& c) {
		_centroid = c;
	}

	// get the rotation quaternion
	glm::quat& getRotation() { return _r; }

	// get the rotation increment
	glm::quat& getIncrement() { return _incr; }

protected:

	int _width, _height;
	int _x, _y;
	int _x0, _y0;
	int _dx, _dy;

	// UI commands that this trackball responds to (defaults to left mouse button with no modifier key)
	Button _tbActivateButton, _dActivateButton, _pActivateButton;
	Modifier _tbActivateModifiers, _dActivateModifiers, _pActivateModifiers;

	// Variable used to determine if the manipulator is presently tracking the mouse
	bool _tbActive;
	bool _dActive;
	bool _pActive;

	float _tbScale; //trackball scale
	float _dScale;  //dolly scale
	float _pScale;  //pan scale
	

	glm::quat _incr;
	glm::quat _r;
	glm::vec3 _pan;
	glm::vec3 _dolly;
	glm::vec3 _centroid;

};