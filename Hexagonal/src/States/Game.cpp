#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include "_Game.h"
#include "CreatePrefabStrategies.h"
#include"tracer.h"

#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH  128

// Simulated frame buffer
char Screen[SCREEN_HEIGHT][SCREEN_WIDTH];

typedef struct
{
	long x, y;
	unsigned char color;
} Point2D;


// min X and max X for every horizontal line within the triangle
long ContourX[SCREEN_HEIGHT][2];

#define ABS(x) ((x >= 0) ? x : -x)

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2), static_cast<float>(Application::Width / 2), -static_cast<float>(Application::Height / 2), static_cast<float>(Application::Height / 2), -1.0f, 1.0f);

	m_camera.lookAt(Vector3f(-800.0f, -1500.0f, 0.0f), Vector3f(-800.0f, -1500.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));


	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(400.0f);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);

	TileSet::Get().init(1024u, 1024u);
	loadTileSet("res/tilesetFrames2.bimg");
	loadMap("res/EvilTown2.emap");

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);

	m_atlas = TileSet::Get().getAtlas();
	//Spritesheet::Safe("test", m_atlas);

	pixels = new uint32_t[128 * 128];

	rast;
	rast.SetFrameBuffer(pixels, 128, 128);

	color1 = Color(1.0f, 0.0f, 0.0f);
	color2 = Color(0.0f, 1.0f, 0.0f);
	color3 = Color(0.0f, 0.0f, 1.0f);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 1.0f * (1.0f / m_zoomFactor), 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f * (1.0f / m_zoomFactor), 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f * (1.0f / m_zoomFactor), 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f * (1.0f / m_zoomFactor), 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
}

void Game::render() {
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("quad_array");
	shader->use();
	
	const TextureRect& rect = TileSet::Get().getTextureRects()[11];
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * Matrix4f::Scale(rect.width, rect.height, 0.0f));
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Spritesheet::Bind(m_atlas);

	Globals::shapeManager.get("quad").drawRaw();

	Spritesheet::Unbind();
	shader->unuse();
	glDisable(GL_BLEND);*/

	//m_mainRT.bindWrite();
	glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.494f, 0.686f, 0.796f, 1.0f}.data());
	glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);
	//m_background.draw();
	culling();
	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());

	Spritesheet::Bind(m_atlas);
	for (auto cell : m_useCulling ? m_visibleCells : m_cells) {
		//if(!cell.selected)
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), cell.posY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), cell.rect.width * m_zoomFactor, cell.rect.height * m_zoomFactor), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), cell.selected ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) :  Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);
	}

	Batchrenderer::Get().drawBufferRaw();
	Spritesheet::Unbind();
	shader->unuse();
	glDisable(GL_BLEND);

	if(m_drawCullingRect)
		drawCullingRect();

	drawMouseRect();
	//m_mainRT.unbindWrite();

	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	m_mainRT.bindColorTexture();
	m_zoomableQuad.drawRaw();
	m_mainRT.unbindColorTexture();
	shader->unuse();*/

	if (m_drawUi)
		renderUi();
}

float diagonal_distance(Vector2f p0, Vector2f p1) {
	float dx = p1[0] - p0[0], dy = p1[1] - p0[1];
	return std::max(fabsf(dx), fabsf(dy));
}

Vector2f round_point(Vector2f p) {
	return Vector2f(round(p[0]), round(p[1]));
}

float lerp(float start, float end, float t) {
	return start * (1.0 - t) + t * end;
}

Vector2f lerp_point(Vector2f p0, Vector2f p1, float t) {
	return Vector2f(lerp(p0[0], p1[0], t), lerp(p0[1], p1[1], t));
}

void supercover_line(Vector2f p0, Vector2f p1, std::vector<Vector2f>& points) {
	float dx = p1[0] - p0[0], dy = p1[1] - p0[1];
	float nx = fabs(dx), ny = fabs(dy);
	int sign_x = dx > 0 ? 1 : -1, sign_y = dy > 0 ? 1 : -1;


	Vector2f p = Vector2f(p0[0], p0[1]);
	points.push_back(p);

	for (int ix = 0, iy = 0; ix < nx || iy < ny;) {
		int decision = (1 + 2 * ix) * ny - (1 + 2 * iy) * nx;
		if (decision == 0) {
			// next step is diagonal
			p[0] += sign_x ;
			p[1] += sign_y;
			ix = ix + 1;
			iy = iy + 1;
		}
		else if (decision < 0) {
			// next step is horizontal
			p[0] += sign_x;
			ix = ix + 1;;
		}
		else {
			// next step is vertical
			p[1] += sign_y;
			iy = iy + 1;
		}
		points.push_back(p);
	}
	//return points;
}

void bsline(int x, int y, int x2, int y2, std::vector<std::array<int, 2>>& points) {
	int dx, dy, p;
	dx = x2 - x;
	dy = y2 - y;
	p = 2 * (dy)-(dx);
	while (x <= x2)
	{
		if (p < 0)
		{
			x = x + 1;
			y = y;
			p = p + 2 * (dy);
		}
		else
		{
			x = x + 1;
			y = y + 1;
			p = p + 2 * (dy - dx);
		}
		points.push_back({x, y});
		//putpixel(x, y, RED);
		//delay(10);
	}
}

// Scans a side of a triangle setting min X and max X in ContourX[][]
// (using the Bresenham's line drawing algorithm).
void ScanLine(long x1, long y1, long x2, long y2)
{
	long sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

	sx = x2 - x1;
	sy = y2 - y1;

	if (sx > 0) dx1 = 1;
	else if (sx < 0) dx1 = -1;
	else dx1 = 0;

	if (sy > 0) dy1 = 1;
	else if (sy < 0) dy1 = -1;
	else dy1 = 0;

	m = ABS(sx);
	n = ABS(sy);
	dx2 = dx1;
	dy2 = 0;

	if (m < n)
	{
		m = ABS(sy);
		n = ABS(sx);
		dx2 = 0;
		dy2 = dy1;
	}

	x = x1; y = y1;
	cnt = m + 1;
	k = n / 2;

	while (cnt--)
	{
		if ((y >= 0) && (y < SCREEN_HEIGHT))
		{
			if (x < ContourX[y][0]) ContourX[y][0] = x;
			if (x > ContourX[y][1]) ContourX[y][1] = x;
		}

		k += n;
		if (k < m)
		{
			x += dx2;
			y += dy2;
		}
		else
		{
			k -= m;
			x += dx1;
			y += dy1;
		}
	}
}

void DrawTriangle(Point2D p0, Point2D p1, Point2D p2, std::vector<std::array<int, 2>> _points)
{
	int y;

	for (y = 0; y < SCREEN_HEIGHT; y++)
	{
		ContourX[y][0] = LONG_MAX; // min X
		ContourX[y][1] = LONG_MIN; // max X
	}

	ScanLine(p0.x, p0.y, p1.x, p1.y);
	ScanLine(p1.x, p1.y, p2.x, p2.y);
	ScanLine(p2.x, p2.y, p0.x, p0.y);

	for (y = 0; y < SCREEN_HEIGHT; y++)
	{
		if (ContourX[y][1] >= ContourX[y][0])
		{
			long x = ContourX[y][0];
			long len = 1 + ContourX[y][1] - ContourX[y][0];

			// Can draw a horizontal line instead of individual pixels here
			while (len--)
			{
				//SetPixel(x++, y, p0.color);
				_points.push_back({ x++, y });
			}
		}
	}
}

template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	//template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

typedef Vec2<int>   Vec2i;

/*void line(Vec2i p0, Vec2i p1, std::vector<std::array<int, 2>>& points) {
	bool steep = false;
	if (std::abs(p0.x - p1.x)<std::abs(p0.y - p1.y)) {
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x>p1.x) {
		std::swap(p0, p1);
	}

	for (int x = p0.x; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y*(1. - t) + p1.y*t;
		if (steep) {
			points.push_back({ y, x });
		}
		else {
			points.push_back({ y, x });
		}
	}
}*/



void linev5(int x1, int y1, int x2, int y2, std::vector<std::array<int, 2>>& points) {
	int dx = x2 - x1,
		dy = y2 - y1;

	if (dx || dy) {
		if (abs(dx) >= abs(dy)) {
			float y = y1 + 0.5;
			float dly = float(dy) / float(dx);
			if (dx > 0)
				for (int x = x1; x <= x2; x++) {
					//s.Plot(x, unsigned(floor(y)), colour);
					points.push_back({ int(x), int(floor(y)) });
					y += dly;
				}
			else
				for (int x = x1; x >= int(x2); x--) {
					//s.Plot(x, unsigned(floor(y)), colour);
					points.push_back({ int(x), int(floor(y)) });
					y -= dly;
				}
		}
		else {
			float x = x1 + 0.5;
			float dlx = float(dx) / float(dy);
			if (dy > 0)
				for (int y = y1; y <= y2; y++) {
					//s.Plot(unsigned(floor(x)), y, colour);
					points.push_back({ int(floor(x)), int(y) });
					x += dlx;
				}
			else
				for (int y = y1; y >= int(y2); y--) {
					//s.Plot(unsigned(floor(x)), y, colour);
					points.push_back({ int(floor(x)), int(y) });
					x -= dlx;
				}
		}
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, std::vector<std::array<int, 2>>& points) {
	linev5(t0.x, t0.y, t1.x, t1.y, points);
	linev5(t1.x, t1.y, t2.x, t2.y, points);
	linev5(t2.x, t2.y, t0.x, t0.y, points);
}

void _troj_line(int *pl, int *pr, int x0, int y0, int x1, int y1)
{
	int *pp;
	int x, y, kx, ky, dx, dy, k, m, p;
	// DDA variables (d)abs delta,(k)step direction
	kx = 0; dx = x1 - x0; if (dx>0) kx = +1;  if (dx<0) { kx = -1; dx = -dx; }
	ky = 0; dy = y1 - y0; if (dy>0) ky = +1;  if (dy<0) { ky = -1; dy = -dy; }
	// target buffer according to ky direction
	if (ky>0) pp = pl; else pp = pr;
	// integer DDA line start point
	x = x0; y = y0;
	// fix endpoints just to be sure (wrong division constants by +/-1 can cause that last point is missing)
	pp[y1] = x1; pp[y0] = x0;
	if (dx >= dy) // x axis is major
	{
		k = dy + dy;
		m = (dy - dx); m += m;
		p = m;
		for (;;)
		{
			pp[y] = x;
			if (x == x1) break;
			x += kx;
			if (p>0) { y += ky; p += m; }
			else p += k;
		}
	}
	else {       // y axis is major
		k = dx + dx;
		m = (dx - dy); m += m;
		p = m;
		for (;;)
		{
			pp[y] = x;
			if (y == y1) break;
			y += ky;
			if (p>0) { x += kx; p += m; }
			else p += k;
		}
	}
}

void troj(int x0, int y0, int x1, int y1, int x2, int y2, std::vector<std::array<int, 2>>& points)
{

	int *pl, *pr;        // left/right buffers
	pl = new int[128];
	pr = new int[128];
	int x, y, yy0, yy1, xx0, xx1;
	// boundary line coordinates to buffers
	_troj_line(pl, pr, x0, y0, x1, y1);
	_troj_line(pl, pr, x1, y1, x2, y2);
	_troj_line(pl, pr, x2, y2, x0, y0);
	// y range
	yy0 = y0; if (yy0>y1) yy0 = y1; if (yy0>y2) yy0 = y2;
	yy1 = y0; if (yy1<y1) yy1 = y1; if (yy1<y2) yy1 = y2;
	// fill with horizontal lines
	for (y = yy0; y <= yy1; y++)
	{
		if (pl[y]<pr[y]) { xx0 = pl[y]; xx1 = pr[y]; }
		else { xx1 = pl[y]; xx0 = pr[y]; }
		for (x = xx0; x <= xx1; x++)
			points.push_back({y, x});
			//pyx[y][x] = col;
	}
	delete[] pl;
	delete[] pr;
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void fillFlatSideTriangleInt(Vec2i v1, Vec2i v2, Vec2i v3, std::vector<std::array<int, 2>>& points) {
	Vec2i vTmp1 = Vec2i(v1.x, v1.y);
	Vec2i vTmp2 = Vec2i(v1.x, v1.y);

	boolean changed1 = false;
	boolean changed2 = false;

	int dx1 = abs(v2.x - v1.x);
	int dy1 = abs(v2.y - v1.y);

	int dx2 = abs(v3.x - v1.x);
	int dy2 = abs(v3.y - v1.y);

	if (dx1 == 0.0f) {	
		return;
	}

	if (dy1 == 0.0f) {
		return;
	}

	if (dx2 == 0.0f) {
		return;
	}

	if (dy2 == 0.0f) {
		return;
	}

	int signx1 = sgn(v2.x - v1.x);
	int signx2 = sgn(v3.x - v1.x);

	int signy1 = sgn(v2.y - v1.y);
	int signy2 = sgn(v3.y - v1.y);

	if (dy1 > dx1)
	{   // swap values
		int tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}

	if (dy2 > dx2)
	{   // swap values
		int tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}

	int e1 = 2 * dy1 - dx1;
	int e2 = 2 * dy2 - dx2;

	for (int i = 0; i <= dx1; i++)
	{
		linev5(vTmp1.x, vTmp1.y, vTmp2.x, vTmp2.y, points);

		while (e1 >= 0)
		{
			if (changed1)
				vTmp1.x += signx1;
			else
				vTmp1.y += signy1;
			e1 = e1 - 2 * dx1;
		}

		if (changed1)
			vTmp1.y += signy1;
		else
			vTmp1.x += signx1;

		e1 = e1 + 2 * dy1;

		/* here we rendered the next point on line 1 so follow now line 2
		* until we are on the same y-value as line 1.
		*/
		while (vTmp2.y != vTmp1.y)
		{
			while (e2 >= 0)
			{
				if (changed2)
					vTmp2.x += signx2;
				else
					vTmp2.y += signy2;
				e2 = e2 - 2 * dx2;
			}

			if (changed2)
				vTmp2.y += signy2;
			else
				vTmp2.x += signx2;

			e2 = e2 + 2 * dy2;
		}
	}
}

void rasterTriangle(std::vector<std::array<int, 2>>& initial, std::vector<std::array<int, 2>>& points) {
	std::sort(initial.begin(), initial.end(), [](const std::array<int, 2> &a, const std::array<int, 2> &b)
	{
		return a[1] > b[1];
	});


	if (initial[1][1] == initial[2][2])
	{
		fillFlatSideTriangleInt({ initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, { initial[2][0],initial[2][1] }, points);
	}
	else if (initial[0][1] == initial[1][1])
	{

		fillFlatSideTriangleInt({ initial[2][0],initial[2][1] }, { initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, points);
	}
	else
	{
		Vec2i vTmp = Vec2i((int)(initial[0][0] + ((float)(initial[1][1] - initial[0][1]) / (float)(initial[2][1] - initial[0][1])) * (initial[2][0] - initial[0][0])), initial[1][1]);

		fillFlatSideTriangleInt({ initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, vTmp, points);
		fillFlatSideTriangleInt({ initial[2][0],initial[2][1] }, { initial[1][0],initial[1][1] }, vTmp, points);
	}
}


void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
	

	if (m_mouseDown) {
		unselect();
		float offsetX = m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float offsetY = m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_curMouseX = mouseViewX;
		m_curMouseY = mouseViewY;

		float left = std::min(m_mouseX, mouseViewX);
		float right = std::max(m_mouseX, mouseViewX);
		float bottom = std::min(m_mouseY, mouseViewY);
		float top = std::max(m_mouseY, mouseViewY);

		std::array<Vector2f, 4> corners;

		corners[0] = Vector2f(left, bottom);
		corners[1] = Vector2f(left, top);
		corners[2] = Vector2f(right, top);
		corners[3] = Vector2f(right, bottom);

		/*corners[0] = Vector2f(m_mouseX, mouseViewX);
		corners[1] = Vector2f(m_mouseX, mouseViewY);
		corners[2] = Vector2f(m_mouseY, mouseViewY);
		corners[3] = Vector2f(m_mouseY, mouseViewX);

		isometricToCol(corners[0][0], corners[0][1], m_colMax, cellHeight * m_zoomFactor);
		isometricToRow(corners[1][0], corners[1][1], m_rowMin, cellWidth  * m_zoomFactor);
		isometricToCol(corners[2][0], corners[2][1], m_colMin, cellHeight * m_zoomFactor);
		isometricToRow(corners[3][0], corners[3][1], m_rowMax, cellWidth  * m_zoomFactor);
		
		std::cout << corners[0][0] << "  " << corners[0][1] << std::endl;
		std::cout << corners[1][0] << "  " << corners[1][1] << std::endl;
		std::cout << corners[2][0] << "  " << corners[2][1] << std::endl;
		std::cout << corners[3][0] << "  " << corners[3][1] << std::endl;

		rast.Clear();
		rast.DrawTriangle(color1, corners[0][0] / 32.0f, corners[0][1] / 32.0f, color2, corners[1][0] / 32.0f, corners[1][1] / 32.0f, color3, corners[2][0] / 32.0f, corners[2][1] / 32.0f);

		for (int col = 0; col < 128; col++) {
			for (int row = 0; row < 128; row++) {
				if (pixels[col * 128 + row] != 0) {

					int _row, _col;
					isometricToCartesian(rast.positions[col * 128 + row][0] * row + offsetX, rast.positions[col * 128 + row][1] * col + offsetY, _row, _col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

					for (int j = 0; j < m_layer.size(); j++) {
						m_selectedCells.push_back(m_cells[m_layer[j][_col][_row].second]);
						m_selectedCells.back().get().selected = true;


					}
				}

			}
		}


		corners[0] = Vector2f(left - (right - left) * 0.5f, bottom + (top - bottom) * 0.5f);
		corners[1] = Vector2f(left + (right - left) * 0.5f, top + (top - bottom) * 0.5f);
		corners[2] = Vector2f(right + (right - left) * 0.5f, bottom + (top - bottom) * 0.5f);
		corners[3] = Vector2f(left + (right - left) * 0.5f, bottom - (top - bottom) * 0.5f);*/

		std::vector<Vector2f> points;
		points.clear();

		std::vector<std::array<int, 2>> _points;
		_points.clear();

		int _startX, _startY;
		isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, _startX, _startY, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

		
		int _endX, _endY;
		isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, _endX, _endY, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
		//bsline(_startX, _startY, _endX, _endY, _points);
		//rast.Clear();
		//rast.DrawLine(_startX, _startY, _endX, _endY, _points);
		//line({ _startX, _startY }, { _endX, _endY }, _points);
		//linev5(_startX, _startY, _endX, _endY, _points);

		

		int _point01, _point02;
		isometricToCartesian(corners[0][0] + offsetX, corners[0][1] + offsetY, _point01, _point02, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

		int _point11, _point12;
		isometricToCartesian(corners[1][0] + offsetX, corners[1][1] + offsetY, _point11, _point12, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

		int _point21, _point22;
		isometricToCartesian(corners[2][0] + offsetX, corners[2][1] + offsetY, _point21, _point22, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

		int _point31, _point32;
		isometricToCartesian(corners[3][0] + offsetX, corners[3][1] + offsetY, _point31, _point32, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
		

		std::vector<std::array<int, 2>> initial;
		initial.push_back({ _point01, _point02 });
		initial.push_back({ _point11, _point12 });
		initial.push_back({ _point31, _point32 });

		rasterTriangle(initial, _points);

		std::vector<std::array<int, 2>> initial2;
		initial2.push_back({ _point11, _point12 });
		initial2.push_back({ _point21, _point22 });
		initial2.push_back({ _point31, _point32 });

		rasterTriangle(initial2, _points);

		/*std::sort(initial.begin(), initial.end(), [](const std::array<int, 2> &a, const std::array<int, 2> &b)
		{
			return a[1] > b[1];
		});


		if (initial[1][1] == initial[2][2])
		{
			fillFlatSideTriangleInt({ initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, { initial[2][0],initial[2][1] }, _points);
		}
		else if (initial[0][1] == initial[1][1])
		{

			fillFlatSideTriangleInt({ initial[2][0],initial[2][1] }, { initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, _points);
		}
		else
		{
			Vec2i vTmp = Vec2i((int)(initial[0][0] + ((float)(initial[1][1] - initial[0][1]) / (float)(initial[2][1] - initial[0][1])) * (initial[2][0] - initial[0][0])), initial[1][1]);
			
			fillFlatSideTriangleInt({ initial[0][0],initial[0][1] }, { initial[1][0],initial[1][1] }, vTmp, _points);
			fillFlatSideTriangleInt({ initial[2][0],initial[2][1] }, { initial[1][0],initial[1][1] }, vTmp, _points);
		}*/

		
		//std::cout << _point11 << "  " << _point12 << std::endl;
		//std::cout << _point21 << "  " << _point22 << std::endl;
		//std::cout << _point31 << "  " << _point32 << std::endl;

		//triangle({ _point11, _point12 }, { _point31, _point32 }, { _point21, _point22 }, _points);
		//triangle2({ unsigned(_point11), unsigned(_point12) }, { unsigned(_point31), unsigned(_point32) }, { unsigned(_point21), unsigned(_point22) }, _points);

		//troj(_point11, _point12, _point21, _point22, _point31, _point32, _points);

		//rast.Clear();
		//rast.DrawTriangle(initial[0][0],initial[0][1], initial[1][0],initial[1][1], initial[2][0],initial[2][1], _points);
		//DrawTriangle({ _point11, _point12 }, { _point21, _point22 }, { _point31, _point32 }, _points);
		for (auto point : _points) {
			//int row, col;
			//isometricToCartesian(point[0] + offsetX, point[1] + offsetY, row, col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
			for (int j = 0; j < m_layer.size(); j++) {
				if (isValid(point[0], point[1]) && m_layer[j][point[1]][point[0]].first != -1) {
					m_selectedCells.push_back(m_cells[m_layer[j][point[1]][point[0]].second]);
					m_selectedCells.back().get().selected = true;
				}
			}
		}

		//rast.DrawTriangle(left, bottom, left, top, right, bottom, points);
		//rast.DrawTriangle(left, top, right, top, right, bottom, points);

		/*float N = diagonal_distance(Vector2f(m_mouseX, m_mouseY), Vector2f(mouseViewX, mouseViewY));
		for (int step = 0; step <= N; step++) {
			float t = N == 0 ? 0.0 : step / N;
			points.push_back(round_point(lerp_point(Vector2f(m_mouseX, m_mouseY), Vector2f(mouseViewX, mouseViewY), t)));
		}*/
	
		/*for (auto point : points) {
			int row, col;
			isometricToCartesian(point[0] + offsetX, point[1] + offsetY, row, col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
			for (int j = 0; j < m_layer.size(); j++) {
				if (isValid(row, col) && m_layer[j][col][row].first != -1) {
					m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
					m_selectedCells.back().get().selected = true;
				}

			}
		}*/
		//std::cout << "Size: " << _points.size() << std::endl;
		
		//std::cout << corners[0][0] << "  " << corners[0][1] << std::endl;
		//std::cout << corners[1][0] << "  " << corners[1][1] << std::endl;
		//std::cout << corners[2][0] << "  " << corners[2][1] << std::endl;
		//std::cout << corners[3][0] << "  " << corners[3][1] << std::endl;


		/*isometricToCol(corners[0][0] + offsetX, corners[0][1] + offsetY, m_colMax, cellHeight * m_zoomFactor);
		isometricToRow(corners[1][0] + offsetX, corners[1][1] + offsetY, m_rowMin, cellWidth  * m_zoomFactor);
		isometricToCol(corners[2][0] + offsetX, corners[2][1] + offsetY, m_colMin, cellHeight * m_zoomFactor);
		isometricToRow(corners[3][0] + offsetX, corners[3][1] + offsetY, m_rowMax, cellWidth  * m_zoomFactor);

		int row, col;
		isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, row, col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if (isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				m_selectedCells.back().get().selected = true;
			}

		}

		isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row, col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if (isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				m_selectedCells.back().get().selected = true;
			}

		}
		isometricToCartesian(corners[2][0] + offsetX, corners[2][1] + offsetY, row, col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if (isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				m_selectedCells.back().get().selected = true;
			}

		}
		isometricToCartesian(corners[3][0] + offsetX, corners[3][1] + offsetY, row, col, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if (isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				m_selectedCells.back().get().selected = true;
			}

		}*/

		

		/*for (int j = 0; j < m_layer.size(); j++) {
			for (int col = m_colMin; col < m_colMax; col++) {
				for (int row = m_rowMin; row < m_rowMax; row++) {
					if (isValid(row, col) && m_layer[j][col][row].first != -1) {
						m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
						m_selectedCells.back().get().selected = true;
					}

				}
			}
		}*/

	}
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);

		unselect();
		//clip
		//float mouseNDCX = (2.0f * event.x) / static_cast<float>(Application::Width) - 1.0f;
		//float mouseNDCY = 1.0f - (2.0f * event.y) / static_cast<float>(Application::Height);

		//view
		//float mouseViewX = 0.5f * (m_right * (mouseNDCX + 1.0f) + m_left   * (1.0f - mouseNDCX));
		//float mouseViewY = 0.5f * (m_top   * (mouseNDCY + 1.0f) + m_bottom * (1.0f - mouseNDCY));
		
		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		//world with zoom
		float mouseWorldX = mouseViewX + m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float mouseWorldY = mouseViewY + m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		/*int row, col;
		isometricToCartesian(mouseWorldX, mouseWorldY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if(isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				m_selectedCells.back().get().selected = true;
			}
		}*/

		m_mouseDown = true;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;

	}else if (event.button == 2u) {
		unselect();
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);

		float offsetX = m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float offsetY = m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		/*int left = static_cast<int>(floor(std::min(m_mouseX, mouseViewX) / 32.0f) * 32.0f);
		int right = static_cast<int>(ceil(std::max(m_mouseX, mouseViewX) / 32.0f) * 32.0f);
		int bottom = static_cast<int>(ceil(std::min(m_mouseY, mouseViewY) / 16.0f) * 16.0f);
		int top = static_cast<int>(floor(std::max(m_mouseY, mouseViewY) / 16.0f) * 16.0f);

		for (int x = left; x < right; x = x + 32) {
			for (int y = bottom; y < top; y = y + 16) {
				int row, col;
				isometricToCartesian(x + offsetX, y + offsetY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
				for (int j = 0; j < m_layer.size(); j++) {
					if(isValid(row, col) && m_layer[j][col][row].first != -1) {
						m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
						m_selectedCells.back().get().selected = true;
					}
				}

			}
		}*/

		/*float left = std::min(m_mouseX, mouseViewX);
		float right = std::max(m_mouseX, mouseViewX);
		float bottom = std::min(m_mouseY, mouseViewY);
		float top = std::max(m_mouseY, mouseViewY);

		std::array<Vector2f, 4> corners;
		corners[0] = Vector2f(left - (right - left) * 0.5f, bottom + (top - bottom) * 0.5f);
		corners[1] = Vector2f(left + (right - left) * 0.5f, top + (top - bottom) * 0.5f);
		corners[2] = Vector2f(right + (right - left) * 0.5f, bottom + (top - bottom) * 0.5f);
		corners[3] = Vector2f(left + (right - left) * 0.5f, bottom - (top - bottom) * 0.5f);

		isometricToCol(corners[3][0] + offsetX , corners[3][1] + offsetY, m_colMax, cellHeight * m_zoomFactor);
		isometricToRow(corners[1][0] + offsetX , corners[1][1] + offsetY   , m_rowMin, cellWidth  * m_zoomFactor);
		isometricToCol(corners[1][0] + offsetX, corners[1][1] + offsetY   , m_colMin, cellHeight * m_zoomFactor);
		isometricToRow(corners[3][0] + offsetX, corners[3][1] + offsetY, m_rowMax, cellWidth  * m_zoomFactor);

		for (int j = 0; j < m_layer.size(); j++) {
			for (int col = m_colMin; col < m_colMax; col++) {
				for (int row = m_rowMin; row < m_rowMax; row++) {
					if (isValid(row, col) && m_layer[j][col][row].first != -1) {
						m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
						m_selectedCells.back().get().selected = true;
					}

				}
			}
		}*/

		m_mouseDown = false;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;
		unselect();

	}else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoomFactor = m_zoomFactor - 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}

	if (event.direction == 0u) {
		m_zoomFactor = m_zoomFactor + 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_mainRT.resize(Application::Width, Application::Height);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);
}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Game::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Use Culling", &m_useCulling);
	ImGui::Checkbox("Draw Culling Rect", &m_drawCullingRect);
	ImGui::SliderFloat("Screen Border", &m_screeBorder, 0.0f, 450.0f);
	ImGui::SliderFloat("Focus Point X", &m_focusPointX, -1600.0f, 1600.0f);
	ImGui::SliderFloat("Focus Point Y", &m_focusPointY, -1000.0f, 1000.0f);
	ImGui::SliderFloat("Enlarge", &m_enlargeBorder, 0.0f, 600.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::unselect() {
	std::for_each(m_selectedCells.begin(), m_selectedCells.end(), [](std::reference_wrapper<Cell> const& cell) {
		cell.get().selected = false;
	});

	m_selectedCells.clear();

}

void Game::loadTileSet(std::string name) {
	std::ifstream readTileSet(name);

	char resourceFilename[MAX_ESTRING_LENGTH];
	while (!readTileSet.eof()) {
		readTileSet >> resourceFilename;

		std::ifstream readImageDef(resourceFilename);
		char textureFilepath[MAX_ESTRING_LENGTH];
		memset(textureFilepath, 0, sizeof(textureFilepath));
		readImageDef.getline(textureFilepath, sizeof(textureFilepath), '\n');

		int imageWidth, imageHeight;
		unsigned char* bytes = Texture::LoadFromFile(textureFilepath, imageWidth, imageHeight, false);

		int accessInt = 0, numFrames = 0;
		readImageDef >> accessInt >> numFrames;

		unsigned int posX = 0, posY = 0, width = 0, height = 0;

		if (numFrames == 0) {
			posX = posY = 0;
			width = imageWidth;
			height = imageHeight;

			m_textureRects.push_back({ static_cast<float>(posX) / static_cast<float>(imageWidth),
                                       static_cast<float>(posY) / static_cast<float>(imageHeight),
                                       static_cast<float>(width) / static_cast<float>(imageWidth),
                                       static_cast<float>(height) / static_cast<float>(imageHeight),
                                       width,
                                       height,
                                       0u });
		}else {

			while (!readImageDef.eof()) {
				readImageDef >> posX >> posY >> width >> height;

				m_textureRects.push_back({ static_cast<float>(posX) / static_cast<float>(imageWidth),
                                           static_cast<float>(posY) / static_cast<float>(imageHeight),
                                           static_cast<float>(width) / static_cast<float>(imageWidth),
                                           static_cast<float>(height) / static_cast<float>(imageHeight),
                                           width,
                                           height,
                                           0u });

				readImageDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		readImageDef.close();

		TileSet::Get().addTexture(bytes, imageWidth, imageHeight, m_textureRects, true);
		free(bytes);
		m_textureRects.clear();
		m_textureRects.shrink_to_fit();
	}
	readTileSet.close();
}

void Game::SkipFileKey(std::ifstream & read) {
	read.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	while (read.peek() == ' ' || read.peek() == '\t')
		read.ignore();
}


void Game::loadMap(std::string name) {
	std::ifstream read(name);
	char buffer[MAX_ESTRING_LENGTH];
	memset(buffer, 0, sizeof(buffer));

	numColumns = 0;
	numRows = 0;
	cellWidth = 0;
	cellHeight = 0;
	numLayers = 0;
	int row = 0;
	int column = 0;

	while (read.peek() == '#')
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	for (int i = 0; i < 6; ++i) {
		SkipFileKey(read);
		switch (i) {
		case 0: read >> numColumns; break;
		case 1: read >> numRows;	break;
		case 2: read >> cellWidth;	break;
		case 3: read >> cellHeight; break;
		case 4: read >> numLayers;	break;
		case 5: read.getline(buffer, sizeof(buffer), '\n'); break;		// tileSet filename
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line (BUGFIX: and the # begin layer def comment line)
	}

	m_layer.resize(numLayers);

	Uint32 layer = 0;
	read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "Layers {"
	read.ignore(1, '\n');													// ignore the '\n' past '{'

	while (read.peek() != '}') {
		row = 0;
		column = 0;
		size_t tallestRenderBlock = 0;
		read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "layer_# {"
		read.ignore(1, '\n');													// ignore the '\n' past '{'
		m_layer[layer] = new std::pair<int, unsigned int>*[numColumns];
		for (int i = 0; i < numColumns; ++i)
			m_layer[layer][i] = new std::pair<int, unsigned int>[numRows];
																				// read one layer
		while (read.peek() != '}') {
			int tileType = INVALID_ID;
			read >> m_layer[layer][column][row].first;

			m_layer[layer][column][row].first--;

			if (m_layer[layer][column][row].first != -1) {

				const TextureRect& rect = TileSet::Get().getTextureRects()[m_layer[layer][column][row].first];
				float cartX = static_cast<float>(row);
				float cartY = static_cast<float>(column);
				cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
				m_cells.push_back({ rect, cartX, -cartY , false});
				m_layer[layer][column][row].second = m_cells.size() - 1;
			}

			if (read.peek() == '\n') {
				read.ignore(1, '\n');
				row = 0;
				column++;
			}else if (read.peek() == ',') {
				read.ignore(1, ',');
				row++;
				if (row >= numRows) {
					row = 0;
					column++;
				}
			}
			
			
		}
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		++layer;
		
	}
	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
void Game::cartesianToIsometric(float & x, float & y, float cellWidth, float cellHeight) {
	float cartX = x * cellWidth;
	float cartY = y * cellHeight;
	x = (cartX - cartY);
	y = (cartX + cartY) * 0.5f;
}

//iso -> cart
//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
//cartX = (isoX  + cartY * CELL_HEIGHT) / CELL_WIDTH
//cartY = (-2.0 * isoY  - cartX * CELL_WIDTH) / CELL_HEIGHT
//
//cartX = (isoX - 2.0 * isoY - cartX * CELL_WIDTH) / CELL_WIDTH
//cartX =  (0.5f * isoX - isoY) /CELL_WIDTH 
//
//cartY = (-2.0 * isoY  - isoX  - cartY * CELL_HEIGHT) / CELL_HEIGHT
//cartY =-(0.5f * isoX + isoY) / CELL_HEIGHT
void Game::isometricToCartesian(float& x, float& y, float cellWidth, float cellHeight) {
	float isoX = x;
	float isoY = y;

	x =  (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;
}

void Game::isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;

	row = static_cast<int>(std::roundf(x));
	col = static_cast<int>(std::roundf(y )) + 1;
}

void Game::isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight, int min, int max) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;

	row = static_cast<int>(std::roundf(x));
	col = static_cast<int>(std::roundf(y)) + 1;

	row = Math::Clamp(row, min, max);
	col = Math::Clamp(col, min, max);
}

void Game::isometricToRow(float x, float y, int& row,  float cellWidth) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	row = static_cast<int>(std::roundf(x));
}

void Game::isometricToRow(float x, float y, int& row, float cellWidth, int min, int max) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	row = static_cast<int>(std::roundf(x));

	row = Math::Clamp(row, min, max);
}

void Game::isometricToCol(float x, float y, int& col, float cellHeight) {
	float isoX = x;
	float isoY = y;

	y = -(0.5f * isoX + isoY) / cellHeight;
	col = static_cast<int>(std::roundf(y)) + 1;
}

void Game::isometricToCol(float x, float y, int& col, float cellHeight, int min, int max) {
	float isoX = x;
	float isoY = y;

	y = -(0.5f * isoX + isoY) / cellHeight;
	col = static_cast<int>(std::roundf(y)) + 1;

	col = Math::Clamp(col, min, max);
}

bool Game::isValid(const int row, const int column) const {
	return (row >= 0 && row < 128 && column >= 0 && column < 128);
}

void Game::culling() {

	Vector3f m_position = m_camera.getPosition();

	corners[0] = Vector2f(m_left  + m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	corners[1] = Vector2f(m_left  + m_screeBorder, m_top - m_screeBorder)    + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	corners[2] = Vector2f(m_right - m_screeBorder, m_top - m_screeBorder)    + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	corners[3] = Vector2f(m_right - m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);

	int rowMin, rowMax, colMin, colMax;
	isometricToCol(corners[0][0] - m_enlargeBorder, corners[0][1] - m_enlargeBorder, colMax, cellHeight * m_zoomFactor, 0, 128);
	isometricToRow(corners[3][0] + m_enlargeBorder, corners[3][1] - m_enlargeBorder, rowMax, cellWidth  * m_zoomFactor, 0, 128);
	isometricToRow(corners[1][0], corners[1][1], rowMin, cellWidth  * m_zoomFactor, 0, 128);
	isometricToCol(corners[2][0], corners[2][1], colMin, cellHeight * m_zoomFactor, 0, 128);
 
	m_visibleCells.clear();
	

	for (int j = 0; j < m_layer.size(); j++) {
		for (int y = colMin; y < colMax; y++) {
			for (int x = rowMin; x < rowMax; x++) {
				if (m_layer[j][y][x].first != -1) {					
					m_visibleCells.push_back(m_cells[m_layer[j][y][x].second]);
				}

			}
		}
	}
}

void Game::drawCullingRect() {
	Vector3f m_position = m_camera.getPosition();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_focusPointX - m_zoomFactor * (m_position[0] + m_focusPointX), m_focusPointY - m_zoomFactor * (m_position[1] + m_focusPointY), 0.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(corners[0][0], corners[0][1], 0.0f);
	glVertex3f(corners[1][0], corners[1][1], 0.0f);
	glVertex3f(corners[2][0], corners[2][1], 0.0f);
	glVertex3f(corners[3][0], corners[3][1], 0.0f);

	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void  Game::drawMouseRect() {
	if (!m_mouseDown) return;

	float offsetX = m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
	float offsetY = m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	

	/*float left = std::min(m_mouseX, m_curMouseX);
	float bottom = std::min(m_mouseY, m_curMouseY);
	cartesianToIsometric(left, bottom, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
	glVertex3f(left, bottom, 0.0f);

	left = std::min(m_mouseX, m_curMouseX);
	float top = std::max(m_mouseY, m_curMouseY);
	cartesianToIsometric(left, top, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
	glVertex3f(left, top , 0.0f);

	float right = std::max(m_mouseX, m_curMouseX);
	top = std::max(m_mouseY, m_curMouseY);
	cartesianToIsometric(right, top, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
	glVertex3f(right, top , 0.0f);

	right = std::max(m_mouseX, m_curMouseX);
	bottom = std::min(m_mouseY, m_curMouseY);
	cartesianToIsometric(right, bottom, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);
	glVertex3f(right, bottom , 0.0f);*/

	/*float left = std::min(m_mouseX, m_curMouseX);
	float bottom = std::min(m_mouseY, m_curMouseY);


	float _left = std::min(m_mouseX, m_curMouseX);
	float _bottom = std::min(m_mouseY, m_curMouseY);
	float top = std::max(m_mouseY, m_curMouseY);
	float right = std::max(m_mouseX, m_curMouseX);
	std::cout << "Left: " << left << " Bottom: " << bottom << std::endl;
	isometricToCartesian(_left, _bottom, right - left, top - bottom);
	std::cout << "_Left: " << _left << " _Bottom: " << _bottom << std::endl;

	glVertex3f(_left, -_bottom , 0.0f);
	glVertex3f(left,  top, 0.0f);
	glVertex3f(right, top, 0.0f);
	glVertex3f(right, bottom, 0.0f);*/

	float left = std::min(m_mouseX, m_curMouseX);
	float bottom = std::min(m_mouseY, m_curMouseY);
	float top = std::max(m_mouseY, m_curMouseY);
	float right = std::max(m_mouseX, m_curMouseX);

	float _left = left - (right - left) * 0.5f;
	float _bottom = bottom - (top - bottom) * 0.5f;
	float _top = top + (top - bottom) * 0.5f;
	float _right = right + (right - left) * 0.5f;

	std::array<Vector2f, 4> corners;
	corners[0] = Vector2f(left, bottom);
	corners[1] = Vector2f(left, top);
	corners[2] = Vector2f(right, top);
	corners[3] = Vector2f(right, bottom);


	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(left, bottom, 0.0f);
	glVertex3f(left, top, 0.0f);
	glVertex3f(right, top, 0.0f);
	glVertex3f(right, bottom, 0.0f);
	glEnd();
	

	/*corners[0] = Vector2f(left - (right - left) * 0.5f, bottom + (top - bottom) * 0.5f);
	corners[1] = Vector2f(left + (right - left) * 0.5f, top + (top - bottom) * 0.5f);
	corners[2] = Vector2f(right + (right - left) * 0.5f, bottom + (top - bottom) * 0.5f);
	corners[3] = Vector2f(left + (right - left) * 0.5f, bottom - (top - bottom) * 0.5f);


	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(corners[0][0], corners[0][1], 0.0f);
	glVertex3f(corners[1][0], corners[1][1], 0.0f);
	glVertex3f(corners[2][0], corners[2][1], 0.0f);
	glVertex3f(corners[3][0], corners[3][1], 0.0f);
	glEnd();*/

	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}