#ifndef __TrackBallH__
#define __TrackBallH__

#include "Vector.h"

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
		_tbActivateModifiers(ENoModifier), _dActivateModifiers(ENoModifier), _pActivateModifiers(ENoModifier), _tbActive(false), _dActive(false), _pActive(false)  {
		_r = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		_incr = Quaternion(0.0f, 0.0f, 0.0f, 1.0f); //no rotation
		_tbScale = 1.0f;
		_dScale = 0.01f;
		_pScale = 0.01f;
	}

	~TrackBall() {}

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

		} else if (button == _tbActivateButton && !depressed) {
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

		} else if (button == _dActivateButton && !depressed) {
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

		} else if (button == _pActivateButton && !depressed) {
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
			_dx = x - _x;   _dy = _y - y;
			_x = x;   _y = y;
			update();
		}
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void idle() {
		//simply increment the rotation
		_r = _incr*_r;
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void updateTrackball() {
		float min = _width < _height ? _width : _height;
		min *= 0.5f;
		Vector3f offset(_width / 2.f, _height / 2.f, 0);
		Vector3f a(_x - _dx, _height - (_y + _dy), 0);
		Vector3f b(_x, _height - _y, 0);
		a -= offset;
		b -= offset;
		a /= min;
		b /= min;

		a[2] = pow(2.0f, 0.5f * a.length());
		Vector3f::Normalize(a);
		b[2] = pow(2.0f, 0.5f * b.length());
		Vector3f::Normalize(b);
		Vector3f axis = Vector3f::Cross(a, b);
		Vector3f::Normalize(axis);

		float rad = acos(Vector3f::Dot(a, b));

		//original glh version had an invert flag and a parent frame, do we need one?		
		_incr.set(axis, rad * _180_ON_PI * _tbScale);
		_incr.conjugate();

		_r = _incr*_r;	
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void updatePan() {
		Vector3f v(_dx, _dy, 0);

		_pan += v * _pScale;
	}

	//
	//
	//////////////////////////////////////////////////////////////////
	void updateDolly() {
		Vector3f v(0, 0, _dy);

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
	Matrix4f getTransform() {
		Matrix4f trans_, invTrans_, rotation_;
		Matrix4f pan_, dolly_;
		
		rotation_ = _r.toMatrix4f();
		pan_.translate(_pan[0], _pan[1], _pan[2]);
		dolly_.translate(_dolly[0], _dolly[1], _dolly[2]);		
		trans_.translate(-_centroid[0], -_centroid[1], -_centroid[2]);
		invTrans_.translate(_centroid[0], _centroid[1], _centroid[2]);

		return pan_ * dolly_ * trans_ * rotation_ * invTrans_;
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
	void setCenterOfRotation(const Vector3f& c) {
		_centroid = c;
	}

	// get the rotation quaternion
	Quaternion &getRotation() { return _r; }

	// get the rotation increment
	Quaternion &getIncrement() { return _incr; }

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

	Quaternion _r;
	Vector3f _pan;
	Vector3f _dolly;

	float _tbScale; //trackball scale
	float _dScale;  //dolly scale
	float _pScale;   //pan scale
	Quaternion _incr;
	Vector3f _centroid;
};

#endif // __cubeH__