// For conditions of distribution and use, see copyright notice in License.txt

#include "FrustumTu.h"
#include <iostream>

inline Vector3 ClipEdgeZ(const Vector3& v0, const Vector3& v1, float clipZ)
{
    return Vector3(
        v1.x + (v0.x - v1.x) * ((clipZ - v1.z) / (v0.z - v1.z)),
        v1.y + (v0.y - v1.y) * ((clipZ - v1.z) / (v0.z - v1.z)),
        clipZ
    );
}

void ProjectAndMergeEdge(Vector3 v0, Vector3 v1, Rect& rect, const Matrix4& projection)
{
    // Check if both vertices behind near plane
    if (v0.z < M_EPSILON && v1.z < M_EPSILON)
        return;
    
    // Check if need to clip one of the vertices
    if (v1.z < M_EPSILON)
        v1 = ClipEdgeZ(v1, v0, M_EPSILON);
    else if (v0.z < M_EPSILON)
        v0 = ClipEdgeZ(v0, v1, M_EPSILON);
    
    // Project, perspective divide and merge
    Vector3 tV0(projection * v0);
    Vector3 tV1(projection * v1);
    rect.Merge(Vector2(tV0.x, tV0.y));
    rect.Merge(Vector2(tV1.x, tV1.y));
}

// SAT test code inspired by https://github.com/juj/MathGeoLib/

void SATData::Calculate(const FrustumTu& frustum)
{
    // Add box normals (constant)
    size_t idx = 0;
    axes[idx++] = Vector3::RIGHT;
    axes[idx++] = Vector3::UP;
    axes[idx++] = Vector3::FORWARD;

    // Add frustum normals. Disregard the near plane as it only points the other way from the far plane
    for (size_t i = 1; i < NUM_FRUSTUM_PLANES; ++i)
        axes[idx++] = frustum.planes[i].normal;

    // Finally add cross product axes
    Vector3 frustumEdges[6] = {
        frustum.vertices[0] - frustum.vertices[2],
        frustum.vertices[0] - frustum.vertices[1],
        frustum.vertices[4] - frustum.vertices[0],
        frustum.vertices[5] - frustum.vertices[1],
        frustum.vertices[6] - frustum.vertices[2],
        frustum.vertices[7] - frustum.vertices[3]
    };

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 6; ++j)
            axes[idx++] = axes[i].CrossProduct(frustumEdges[j]);
    }

    // Now precalculate the projections of the frustum on each axis
    for (size_t i = 0; i < NUM_SAT_AXES; ++i)
        frustumProj[i] = frustum.Projected(axes[i]);
}

FrustumTu::FrustumTu()
{
    for (size_t i = 0; i < NUM_FRUSTUM_VERTICES; ++i)
        vertices[i] = Vector3::ZERO;

    UpdatePlanes();
}

FrustumTu::FrustumTu(const FrustumTu& frustum)
{
    *this = frustum;
}

FrustumTu& FrustumTu::operator = (const FrustumTu& rhs)
{
    for (size_t i = 0; i < NUM_FRUSTUM_PLANES; ++i)
        planes[i] = rhs.planes[i];
    for (size_t i = 0; i < NUM_FRUSTUM_VERTICES; ++i)
        vertices[i] = rhs.vertices[i];
    
    return *this;
}

void FrustumTu::Define(float fov, float aspectRatio, float zoom, float nearZ, float farZ, const Matrix4& invView, const Matrix4& perspective, const Matrix4& view, const Matrix4f& _perspective, const Matrix4f& _view)
{
    nearZ = Max(nearZ, 0.0f);
    farZ = Max(farZ, nearZ);
    float halfViewSize = tanf(fov * M_DEGTORAD_2) / zoom;
    Vector3 near, far;
    
    near.z = nearZ;
    near.y = near.z * halfViewSize;
    near.x = near.y * aspectRatio;
    far.z = farZ;
    far.y = far.z * halfViewSize;
    far.x = far.y * aspectRatio;
    
    Define(near, far, invView, perspective, view, _perspective, _view);
}

void FrustumTu::Define(const Vector3& near, const Vector3& far, const Matrix4& invView, const Matrix4& perspective, const Matrix4& view, const Matrix4f& _perspective, const Matrix4f& _view)
{
	std::cout << "Define: " << std::endl;
	/*view.Inverse().print();
	invView.print();

	std::cout << "Define: " << std::endl;
	Matrix4 tmp = invView.Transpose();

    vertices[0] = tmp * near;
    vertices[1] = tmp * Vector3(near.x, -near.y, near.z);
    vertices[2] = tmp * Vector3(-near.x, -near.y, near.z);
    vertices[3] = tmp * Vector3(-near.x, near.y, near.z);
    vertices[4] = tmp * far;
    vertices[5] = tmp * Vector3(far.x, -far.y, far.z);
    vertices[6] = tmp * Vector3(-far.x, -far.y, far.z);
    vertices[7] = tmp * Vector3(-far.x, far.y, far.z);

	std::cout << vertices[0].x << "  " << vertices[0].y << "  " << vertices[0].z << std::endl;*/

	vertices[0] = invView ^ Vector3(near.x, near.y, near.z);
	vertices[1] = invView ^ Vector3(near.x, -near.y, near.z);
	vertices[2] = invView ^ Vector3(-near.x, -near.y, near.z);
	vertices[3] = invView ^ Vector3(-near.x, near.y, near.z);
	vertices[4] = invView ^ Vector3(far.x, far.y, far.z);
	vertices[5] = invView ^ Vector3(far.x, -far.y, far.z);
	vertices[6] = invView ^ Vector3(-far.x, -far.y, far.z);
	vertices[7] = invView ^ Vector3(-far.x, far.y, far.z);

	/*std::cout << vertices[0].x << "  " << vertices[0].y << "  " << vertices[0].z << std::endl;
	std::cout << vertices[1].x << "  " << vertices[1].y << "  " << vertices[1].z << std::endl;
	std::cout << vertices[2].x << "  " << vertices[2].y << "  " << vertices[2].z << std::endl;
	std::cout << vertices[3].x << "  " << vertices[3].y << "  " << vertices[3].z << std::endl;
	std::cout << vertices[4].x << "  " << vertices[4].y << "  " << vertices[4].z << std::endl;
	std::cout << vertices[5].x << "  " << vertices[5].y << "  " << vertices[5].z << std::endl;
	std::cout << vertices[6].x << "  " << vertices[6].y << "  " << vertices[6].z << std::endl;
	std::cout << vertices[7].x << "  " << vertices[7].y << "  " << vertices[7].z << std::endl;*/
	//UpdatePlanes();
	//perspective.print();
	//_perspective.print();
	//view.print();
	//_view.print();

	Matrix4 vp = view * perspective ;

	/*Matrix4f vp2 = _perspective * _view;
	vp.print();
	vp2.print();*/

	planes[PLANE_NEAR].Define(Vector4(vp.m03 + vp.m02, vp.m13 + vp.m12, vp.m23 + vp.m22, vp.m33 + vp.m32));
	planes[PLANE_FAR].Define(Vector4(vp.m03 - vp.m02, vp.m13 - vp.m12, vp.m23 - vp.m22, vp.m33 - vp.m32));

	planes[PLANE_LEFT].Define(Vector4(vp.m03 + vp.m00, vp.m13 + vp.m10, vp.m23 + vp.m20, vp.m33 + vp.m30));
	planes[PLANE_RIGHT].Define(Vector4(vp.m03 - vp.m00, vp.m13 - vp.m10, vp.m23 - vp.m20, vp.m33 - vp.m30));

	planes[PLANE_DOWN].Define(Vector4(vp.m03 + vp.m01, vp.m13 + vp.m11, vp.m23 + vp.m21, vp.m33 + vp.m31));
	planes[PLANE_UP].Define(Vector4(vp.m03 - vp.m01, vp.m13 - vp.m11, vp.m23 - vp.m21, vp.m33 - vp.m31));

	/*float _near = perspective.m32 / (perspective.m22 - 1);
	float heightNear = (2.0f / perspective.m11) * _near;
	float widthNear = (heightNear *  perspective.m11) / perspective.m00;

	float _far = perspective.m32 / (perspective.m22 + 1);
	float heightFar = (2.0f / perspective.m11) * _far;
	float widthFar = (heightFar  * perspective.m11) / perspective.m00;

	const Vector3& right = Vector3(view.m00, view.m10, view.m20);
	const Vector3& up = Vector3(view.m01, view.m11, view.m21);
	const Vector3& viewDirection = Vector3(-view.m02, -view.m12, -view.m22);
	const Vector3& pos = Vector3(-(view.m30 * view.m00 + view.m31 * view.m01 + view.m32 * view.m02),
		-(view.m30 * view.m10 + view.m31 * view.m11 + view.m32 * view.m12),
		-(view.m30 * view.m20 + view.m31 * view.m21 + view.m32 * view.m22));

	Vector3 centerNear = pos - viewDirection * _near;
	Vector3 centerFar = pos - viewDirection * _far;

	Vector3 nearBottomLeft = centerNear - up * (heightNear * 0.5f) - right * (widthNear * 0.5f);
	Vector3 nearTopLeft = centerNear + up * (heightNear * 0.5f) - right * (widthNear * 0.5f);
	Vector3 nearTopRight = centerNear + up * (heightNear * 0.5f) + right * (widthNear * 0.5f);
	Vector3 nearBottomRight = centerNear - up * (heightNear * 0.5f) + right * (widthNear * 0.5f);

	Vector3 farBottomLeft = centerFar - up * (heightFar * 0.5f) - right * (widthFar * 0.5f);
	Vector3 farTopLeft = centerFar + up * (heightFar * 0.5f) - right * (widthFar * 0.5f);
	Vector3 farTopRight = centerFar + up * (heightFar * 0.5f) + right * (widthFar * 0.5f);
	Vector3 farBottomRight = centerFar - up * (heightFar * 0.5f) + right * (widthFar * 0.5f);


	vertices[0] = nearBottomLeft;
	vertices[1] = nearTopLeft;
	vertices[2] = nearTopRight;
	vertices[3] = nearBottomRight;
	vertices[4] = farBottomLeft;
	vertices[5] = farTopLeft;
	vertices[6] = farTopRight;
	vertices[7] = farBottomRight;

	std::cout << "wwwwwwwwwwwwwwwwwww" << std::endl;

	std::cout << vertices[0].x << "  " << vertices[0].y << "  " << vertices[0].z << std::endl;
	std::cout << vertices[1].x << "  " << vertices[1].y << "  " << vertices[1].z << std::endl;
	std::cout << vertices[2].x << "  " << vertices[2].y << "  " << vertices[2].z << std::endl;

	std::cout << vertices[3].x << "  " << vertices[3].y << "  " << vertices[3].z << std::endl;

	std::cout << vertices[4].x << "  " << vertices[4].y << "  " << vertices[4].z << std::endl;
	std::cout << vertices[5].x << "  " << vertices[5].y << "  " << vertices[5].z << std::endl;
	std::cout << vertices[6].x << "  " << vertices[6].y << "  " << vertices[6].z << std::endl;
	std::cout << vertices[7].x << "  " << vertices[7].y << "  " << vertices[7].z << std::endl;*/

	//UpdatePlanes();*/
}

void FrustumTu::Define(const BoundingBox& box, const Matrix3x4& transform)
{
    vertices[0] = transform * Vector3(box.max.x, box.max.y, box.min.z);
    vertices[1] = transform * Vector3(box.max.x, box.min.y, box.min.z);
    vertices[2] = transform * Vector3(box.min.x, box.min.y, box.min.z);
    vertices[3] = transform * Vector3(box.min.x, box.max.y, box.min.z);
    vertices[4] = transform * Vector3(box.max.x, box.max.y, box.max.z);
    vertices[5] = transform * Vector3(box.max.x, box.min.y, box.max.z);
    vertices[6] = transform * Vector3(box.min.x, box.min.y, box.max.z);
    vertices[7] = transform * Vector3(box.min.x, box.max.y, box.max.z);
    
    UpdatePlanes();
}

void FrustumTu::DefineOrtho(float orthoSize, float aspectRatio, float zoom, float nearZ, float farZ, const Matrix3x4& transform)
{
    nearZ = Max(nearZ, 0.0f);
    farZ = Max(farZ, nearZ);
    float halfViewSize = orthoSize * 0.5f / zoom;
    Vector3 near, far;
    
    near.z = nearZ;
    far.z = farZ;
    far.y = near.y = halfViewSize;
    far.x = near.x = near.y * aspectRatio;
    
    Define(near, far, transform);
}

void FrustumTu::Transform(const Matrix3& transform)
{
    for (size_t i = 0; i < NUM_FRUSTUM_VERTICES; ++i)
        vertices[i] = transform * vertices[i];
    
    UpdatePlanes();
}

void FrustumTu::Transform(const Matrix3x4& transform)
{
    for (size_t i = 0; i < NUM_FRUSTUM_VERTICES; ++i)
        vertices[i] = transform * vertices[i];
    
    UpdatePlanes();
}

FrustumTu FrustumTu::Transformed(const Matrix3& transform) const
{
	FrustumTu transformed;
    for (size_t i = 0; i < NUM_FRUSTUM_VERTICES; ++i)
        transformed.vertices[i] = transform * vertices[i];
    
    transformed.UpdatePlanes();
    return transformed;
}

FrustumTu FrustumTu::Transformed(const Matrix3x4& transform) const
{
	FrustumTu transformed;
    for (size_t i = 0; i < NUM_FRUSTUM_VERTICES; ++i)
        transformed.vertices[i] = transform * vertices[i];
    
    transformed.UpdatePlanes();
    return transformed;
}

Rect FrustumTu::Projected(const Matrix4& projection) const
{
    Rect rect;
    
    ProjectAndMergeEdge(vertices[0], vertices[4], rect, projection);
    ProjectAndMergeEdge(vertices[1], vertices[5], rect, projection);
    ProjectAndMergeEdge(vertices[2], vertices[6], rect, projection);
    ProjectAndMergeEdge(vertices[3], vertices[7], rect, projection);
    ProjectAndMergeEdge(vertices[4], vertices[5], rect, projection);
    ProjectAndMergeEdge(vertices[5], vertices[6], rect, projection);
    ProjectAndMergeEdge(vertices[6], vertices[7], rect, projection);
    ProjectAndMergeEdge(vertices[7], vertices[4], rect, projection);
    
    return rect;
}

std::pair<float, float> FrustumTu::Projected(const Vector3& axis) const
{
    std::pair<float, float> ret;
    ret.first = ret.second = axis.DotProduct(vertices[0]);

    for (size_t i = 1; i < NUM_FRUSTUM_VERTICES; ++i)
    {
        float proj = axis.DotProduct(vertices[i]);
        ret.first = Min(proj, ret.first);
        ret.second = Max(proj, ret.second);
    }

    return ret;
}

void FrustumTu::UpdatePlanes()
{
    planes[PLANE_NEAR].Define(vertices[2], vertices[1], vertices[0]);
    planes[PLANE_LEFT].Define(vertices[3], vertices[7], vertices[6]);
    planes[PLANE_RIGHT].Define(vertices[1], vertices[5], vertices[4]);
    planes[PLANE_UP].Define(vertices[0], vertices[4], vertices[7]);
    planes[PLANE_DOWN].Define(vertices[6], vertices[5], vertices[1]);
    planes[PLANE_FAR].Define(vertices[5], vertices[6], vertices[7]);

    // Check if we ended up with inverted planes (reflected transform) and flip in that case
    if (planes[PLANE_NEAR].Distance(vertices[5]) < 0.0f)
    {
        for (size_t i = 0; i < NUM_FRUSTUM_PLANES; ++i)
        {
            planes[i].normal = -planes[i].normal;
            planes[i].d = -planes[i].d;
        }
    }
}
