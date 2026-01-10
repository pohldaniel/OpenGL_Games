#pragma once
#include <list>
#include <vector>
#include <memory>
#include <functional>
#include "../Vector.h"
#include "../Sprite.h"
#include "../utils/StringHash.h"

#define SURFACE_VERTEX      "#version 410 core                                                            \n \
                                                                                                          \n \
                            layout(location = 0) in vec3 i_position;                                      \n \
                            layout(location = 1) in vec2 i_texCoord;                                      \n \
                                                                                                          \n \
                            flat out int v_layer;                                                         \n \
                            out vec2 v_texCoord;                                                          \n \
                            out vec4 v_color;                                                             \n \
                                                                                                          \n \
                            uniform bool u_flip = false;                                                  \n \
                            uniform mat4 u_transform = mat4(1.0);                                         \n \
                            uniform vec4 u_texRect = vec4(0.0, 0.0, 1.0, 1.0);                            \n \
                            uniform vec4 u_color = vec4(1.0);                                             \n \
                            uniform int u_layer = 0;                                                      \n \
                                                                                                          \n \
                            void main() {                                                                 \n \
                                gl_Position = u_transform * vec4(i_position, 1.0);                        \n \
                                v_layer = u_layer;                                                        \n \
                                                                                                          \n \
                                v_texCoord.y =  u_flip ? 1.0 - i_texCoord.y : i_texCoord.y;               \n \
                                v_texCoord.x = i_texCoord.x * (u_texRect.z - u_texRect.x) + u_texRect.x;  \n \
                                v_texCoord.y = i_texCoord.y * (u_texRect.w - u_texRect.y) + u_texRect.y;  \n \
                                v_color = u_color;                                                        \n \
                            }"


#define SURFACE_FRGAMENT    "#version 410 core                                                                           \n \
                                                                                                                         \n \
                             flat in int v_layer;                                                                        \n \
                             in vec2 v_texCoord;                                                                         \n \
                             in vec4 v_color;                                                                            \n \
                                                                                                                         \n \
                             out vec4 outColor;                                                                          \n \
                                                                                                                         \n \
                             uniform float u_radius = 5;                                                                 \n \
                             uniform uint u_edge = 0;                                                                    \n \
                             uniform vec2 u_dimensions;                                                                  \n \
                                                                                                                         \n \
                             float calcDistance(vec2 uv) {                                                               \n \
                                 vec2 pos = (abs(uv - 0.5) + 0.5);                                                       \n \
                                 vec2 delta = max(pos * u_dimensions - u_dimensions + pos * u_radius, 0.0);              \n \
                                 return length(delta);                                                                   \n \
                             }                                                                                           \n \
                                                                                                                         \n \
                             void main() {                                                                               \n \
                                 float dist = calcDistance(v_texCoord);                                                  \n \
                                                                                                                         \n \
                                 //all                                                                                   \n \
                                 if (u_edge == 0 && dist > u_radius) {                                                   \n \
                                     discard;                                                                            \n \
                                 //bottom left                                                                           \n \
                                 }else if (u_edge == 1 && dist > u_radius && v_texCoord.x < 0.5 && v_texCoord.y < 0.5) { \n \
                                     discard;                                                                            \n \
	                            //top left                                                                               \n \
                                } else if (u_edge == 2 && dist > u_radius && v_texCoord.x < 0.5 && v_texCoord.y > 0.5) { \n \
	                                discard;                                                                             \n \
	                            //top right                                                                              \n \
                                }else if (u_edge == 3 && dist > u_radius && v_texCoord.x > 0.5 && v_texCoord.y > 0.5) {  \n \
                                    discard;                                                                             \n \
	                            //bottom right                                                                           \n \
                                }else if (u_edge == 4 && dist > u_radius && v_texCoord.x > 0.5 && v_texCoord.y < 0.5) {  \n \
                                    discard;                                                                             \n \
	                            //right                                                                                  \n \
                                }else if (u_edge == 5 && dist > u_radius && v_texCoord.x > 0.5) {                        \n \
                                    discard;                                                                             \n \
                                //left                                                                                   \n \
                                }else if (u_edge == 6 && dist > u_radius && v_texCoord.x < 0.5) {                        \n \
                                    discard;                                                                             \n \
                                //top                                                                                    \n \
                                }else if (u_edge == 7 && dist > u_radius && v_texCoord.y > 0.5) {                        \n \
                                    discard;                                                                             \n \
	                            //bottom                                                                                 \n \
                                }else if (u_edge == 8 && dist > u_radius && v_texCoord.y < 0.5) {                        \n \
                                    discard;                                                                             \n \
                                }                                                                                        \n \
                                                                                                                         \n \
                                outColor = v_color;                                                                      \n \
                             }"

namespace ui
{
	enum Edge {
		ALL,
		BOTTOM_LEFT,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_RIGHT,
		EDGE_RIGHT,
		EDGE_LEFT,
		TOP,
		BOTTOM,
		EDGE_NONE
	};

	class Node {

	public:

		Node();
		Node(const Node& rhs);
		Node(Node&& rhs);
		virtual ~Node();

		std::list<std::shared_ptr<Node>>& getChildren() const;
		void eraseSelf();
		void eraseChild(Node* child);
		void eraseChild(const int index);
		void eraseAllChildren(size_t offset = 0);
		template <class T> void eraseChildren() const;
		Node* addChild(Node* node);
		template <class T> T* addChild();
		template <class T, class U> T* addChild(const U& ref);
		template <class T, class U> T* addChild(U& ref);
		template <class T, class U, class V> T* addChild(const U& ref1, const V& ref2);
		template <class T, class U, class V> T* addChild(U& ref1, V& ref2);
		template <class T, class U, class V, class W> T* addChild(const U& ref1, const V& ref2, const W& ref3);
		template <class T, class U, class V, class W> T* addChild(U& ref1, V& ref2, const W& ref3);
		Node* getParent() const;
		void setParent(Node* node);
		const int getIndex() const;
		void setName(const std::string& name);
		void setIndex(const int index);
		template <class T> T* findChild(std::string name, bool recursive = true) const;
		template <class T> T* findChild(StringHash nameHash, bool recursive = true) const;
		template <class T> T* findChild(const int index, bool recursive = true) const;
		size_t countNodes();

	protected:

		mutable std::list<std::shared_ptr<Node>> m_children;
		Node* m_parent;
		StringHash m_nameHash;
		int m_index;
	};

	template <class T> T* Node::addChild() {
		m_children.emplace_back(std::shared_ptr<Node>(new T()));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T, class U> T* Node::addChild(const U& ref) {
		m_children.emplace_back(std::unique_ptr<T>(new T(ref)));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T, class U> T* Node::addChild(U& ref) {
		m_children.emplace_back(std::unique_ptr<T>(new T(ref)));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T, class U, class V> T* Node::addChild(const U& ref1, const V& ref2) {
		m_children.emplace_back(std::unique_ptr<T>(new T(ref1, ref2)));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T, class U, class V> T* Node::addChild(U& ref1, V& ref2) {
		m_children.emplace_back(std::unique_ptr<T>(new T(ref1, ref2)));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T, class U, class V, class W> T* Node::addChild(const U& ref1, const V& ref2, const W& ref3) {
		m_children.emplace_back(std::unique_ptr<T>(new T(ref1, ref2, ref3)));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T, class U, class V, class W> T* Node::addChild(U& ref1, V& ref2, const W& ref3) {
		m_children.emplace_back(std::unique_ptr<T>(new T(ref1, ref2, ref3)));
		m_children.back()->m_parent = this;
		return static_cast<T*>(m_children.back().get());
	}

	template <class T> T* Node::findChild(std::string name, bool recursive) const {
		return findChild<T>(StringHash(name), recursive);
	}

	template <class T> T* Node::findChild(StringHash nameHash, bool recursive) const {
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Node* child = (*it).get();
			if (!child) {
				continue;
			}

			if (child->m_nameHash == nameHash && dynamic_cast<T*>(child) != nullptr)
				return dynamic_cast<T*>(child);
			else if (recursive && child->m_children.size()) {
				Node* result = child->findChild<T>(nameHash, recursive);
				if (result)
					return dynamic_cast<T*>(result);
			}
		}
		return nullptr;
	}

	template <class T> T* Node::findChild(const int index, bool recursive) const {
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Node* child = (*it).get();
			if (!child) {
				continue;
			}

			if (child->m_index == index && dynamic_cast<T*>(child) != nullptr)
				return dynamic_cast<T*>(child);
			else if (recursive && child->m_children.size()) {
				Node* result = child->findChild<T>(index, recursive);
				if (result)
					return dynamic_cast<T*>(result);
			}
		}
		return nullptr;
	}

	template <class T> void Node::eraseChildren() const {

		for (auto it = m_children.begin(); it != m_children.end();) {
			Node* child = (*it).get();
			if (child && dynamic_cast<T*>(child)) {
				it = m_children.erase(it);
			}
			else ++it;
		}
	}

	class Widget : public Node, public Sprite {

	public:

		Widget();
		Widget(const Widget& rhs);
		Widget(Widget&& rhs);
		virtual ~Widget();

		virtual void draw();

		void setScale(const float sx, const float sy) override;
		void setScale(const Vector2f& scale) override;
		void setScale(const float s) override;

		void setScaleAbsolute(const float sx, const float sy);
		void setScaleAbsolute(const Vector2f& scale);
		void setScaleAbsolute(const float s);

		void setPosition(const float x, const float y) override;
		void setPosition(const Vector2f& position) override;

		void setOrigin(const float x, const float y) override;
		void setOrigin(const Vector2f& origin);

		void setOrientation(const float degrees) override;

		void translate(const Vector2f& trans) override;
		void translate(const float dx, const float dy) override;

		void translateRelative(const Vector2f& trans) override;
		void translateRelative(const float dx, const float dy) override;

		void scale(const Vector2f& scale) override;
		void scale(const float sx, const float sy) override;
		void scale(const float s) override;

		void scaleAbsolute(const Vector2f& scale);
		void scaleAbsolute(const float sx, const float sy);
		void scaleAbsolute(const float s);

		void rotate(const float degrees) override;

		const Matrix4f& getWorldTransformation() const;
		const Vector2f& getWorldPosition(bool update = true) const;
		const Vector2f& getWorldScale(bool update = true) const;
		const float getWorldOrientation(bool update = true) const;
		void updateWorldTransformation() const;

		const Matrix4f getWorldTransformationWithTranslation(const Vector2f& trans) const;
		const Matrix4f getWorldTransformationWithTranslation(float dx, float dy) const;

		const Matrix4f getWorldTransformationWithScale(float sx, float sy, bool relative = true) const;
		const Matrix4f getWorldTransformationWithScale(const Vector2f& scale, bool relative = true) const;
		const Matrix4f getWorldTransformationWithScale(const float s, bool relative = true) const;

		const Matrix4f getWorldTransformationWithScaleAndTranslation(float sx, float sy, float dx, float dy, bool relative = true) const;
		const Matrix4f getWorldTransformationWithScaleAndTranslation(const Vector2f& scale, const Vector2f& trans, bool relative = true) const;
		void setDrawFunction(std::function<void()> fun);

		static void Init();
		static Shader* GetShader();
		static void UnuseShader();

	protected:

		void OnTransformChanged();
		void drawTree();
		std::function<void()> m_draw;

		static std::unique_ptr<Shader> SurfaceShader;

	private:

		virtual void drawDefault() = 0;

		mutable Matrix4f m_modelMatrix;
		mutable bool m_isDirty;

		static Vector2f WorldPosition;
		static Vector2f WorldScale;
		static float WorldOrientation;		
	};

}