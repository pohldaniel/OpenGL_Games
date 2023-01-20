#include "VectorMath.h"

class Trackball
{
    public:
        Trackball(float radius, unsigned int width, unsigned int height);
        void MouseDown(int x, int y);
        void MouseUp(int x, int y);
        void MouseMove(int x, int y);
        void ReturnHome();
        VectorMath::Matrix3 GetRotation() const;
        void Update(unsigned int elapsedMicroseconds);
    private:
        VectorMath::Vector3 MapToSphere(int x, int y);
        VectorMath::Vector3 m_startPos;
        VectorMath::Vector3 m_currentPos;
        VectorMath::Vector3 m_previousPos;
        VectorMath::Vector3 m_axis;
        VectorMath::Quat m_quat;
        bool m_active;
        float m_radius;
        float m_radiansPerSecond;
        unsigned int m_currentTime;
        unsigned int m_previousTime;

		unsigned int m_width;
		unsigned int m_height;

        struct {
            bool Active;
            VectorMath::Quat DepartureQuat;
            unsigned int ElapsedMicroseconds;
        } m_voyageHome;

        struct {
            bool Active;
            VectorMath::Vector3 Axis;
            float RadiansPerSecond;
        } m_inertia;
};
