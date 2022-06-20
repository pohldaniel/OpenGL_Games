// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tom� Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tom� Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David �abata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include <glm/gtx/transform2.hpp>

class Camera2 {

	public:

		Camera2(void);
		void Reset();
		
		void Move(float x, float y, float z);		
		void Aim(float vertical_angle, float horizontal_angle);
		
		glm::mat4 GetMatrix();	

		
		void Follow(glm::mat4 & targetMat, glm::vec3 targetVelocity, const float gameTime);
        void ResetFollow() { currentSettleTime = 0.0f; }

		//void toggleObserve();

		inline glm::vec3 getEye()
		{
			return eye;
		}
		
		inline glm::vec3 getTarget()
		{
			return target;
		}

	private:
		glm::vec3 eye;			// pozice kamery
		glm::vec3 target;		// smer pohledu
		glm::vec3 up;			// up vektor kamery
		float angle_horiz;
		float angle_vert;
		
		float currentSettleTime; // pomocna promenna pro "intro" follow kamery

		/*
		bool isObserving;
		Vector3f observeCenter;
		float observeRadius;
		float observeSpeed;
		CTimer observeTimer;
		*/	

};

#endif
