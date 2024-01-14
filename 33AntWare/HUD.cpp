#include <engine/Texture.h>
#include <HUD.h>

using namespace aw;
using namespace glm;

Hud::Hud(){
    char buffer[128];
    loadTexture("res/textures/crosshair.png", crosshair);
    loadTexture("res/textures/backSlash.png", backSlash);
    loadTexture("res/textures/plus.png", plus);
    loadTexture("res/textures/ammo.png", ammo);
    loadTexture("res/textures/loading.png", loading);
    loadTexture("res/textures/lose.png", lose);
    loadTexture("res/textures/win.png", win);
    loadTexture("res/textures/hurt.png", hurt);

    for (unsigned i = 0; i < 10; ++i){
        sprintf(buffer, "res/textures/%d.png", i);
        loadTexture(buffer, digits[i]);
    }
    createQuadVAO();
}

Hud &Hud::instance(){
    static Hud hud;
    return hud;
}

void Hud::draw(){

    glUseProgram(shaderProgram);
    glDisable(GL_DEPTH_TEST);
    // Draw here
    if (status == ONGOING){
        drawQuad(crosshair, glm::vec2(0.1f, 0.0f), {1, 1});
        drawHP();
        drawAmmo();
        if (isHurting)
            drawQuad(hurt, glm::vec2(0.0f, 0.0f), {16.0f, 16.0f});
    }else if (status == WIN){
        drawQuad(win, glm::vec2(0.0f, 0.0f), {16.0f, 16.0f});
    }else if (status == LOSE){
        drawQuad(lose, glm::vec2(0.0f, 0.0f), {16.0f, 16.0f});
    }else if (status == LOADING){
        drawQuad(loading, glm::vec2(0.0f, 0.0f), {16.0f, 16.0f});
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
}

void Hud::loadTexture(const char *path, GLuint &tex){

	int imgHeight;
	int imgWidth;
	const unsigned char* imgData = Texture::LoadFromFile(path, imgWidth, imgHeight, true);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(glGetError() == 0);
}

void Hud::drawQuad(GLuint texture, glm::vec2 pos, glm::vec2 size, glm::vec3 color){

    size /= 2.0f;
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform3f(uniformsLocations[COLOR_VEC], color.r, color.g, color.b);

    mat4 model = translate(mat4(1.0f), vec3{pos, 0.01f}) * scale(mat4(1.0f), vec3{size, 0});

    glUniformMatrix4fv(uniformsLocations[MODEL_MAT], 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}

void Hud::drawAmmo(){

    float inHandAmmoY = -3.9f, totalAmmoY = -4.0f;
    float inHandAmmoSize = 0.8f, totalAmmoSize = 0.5f;
    float inHandDiff = 0.6 * inHandAmmoSize;
    float totalDiff = 0.6 * totalAmmoSize;
    float slashX = 6.0f;
    float inHand3X = slashX - inHandDiff, inHand2X = inHand3X - inHandDiff, inHand1X = inHand2X - inHandDiff;
    float total1X = slashX + totalDiff, total2X = total1X + totalDiff, total3X = total2X + totalDiff;
    float bulletDiff = totalDiff - 0.18f, bulletX = total3X + totalDiff + 0.1f, bulletY = totalAmmoY - 0.1f, bulletSize = 0.4f;

    if (totalAmmo < 0)
        totalAmmo = 0;
    if (inHandAmmo < 0)
        inHandAmmo = 0;

    if (totalAmmo >= 100){
        drawQuad(ammo, {bulletX, bulletY}, {bulletSize, bulletSize});

        drawQuad(digits[totalAmmo / 100], {total1X, totalAmmoY}, {totalAmmoSize, totalAmmoSize});
        drawQuad(digits[(totalAmmo / 10) % 10], {total2X, totalAmmoY}, {totalAmmoSize, totalAmmoSize});
        drawQuad(digits[totalAmmo % 10], {total3X, totalAmmoY}, {totalAmmoSize, totalAmmoSize});

        drawQuad(backSlash, {slashX, totalAmmoY}, {totalAmmoSize, totalAmmoSize});

        if (inHandAmmo >= 100)
        {
            drawQuad(digits[inHandAmmo / 100], {inHand1X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
        }
        if (inHandAmmo >= 10)
        {
            drawQuad(digits[(inHandAmmo / 10) % 10], {inHand2X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
        }
        drawQuad(digits[inHandAmmo % 10], {inHand3X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});

    }else if (totalAmmo >= 10){
        slashX += totalDiff;
        inHand3X = slashX - inHandDiff, inHand2X = inHand3X - inHandDiff, inHand1X = inHand2X - inHandDiff;
        total1X = slashX + totalDiff, total2X = total1X + totalDiff, total3X = total2X + totalDiff;
        bulletDiff = totalDiff - 0.18f, bulletX = total2X + totalDiff + 0.1f, bulletY = totalAmmoY - 0.1f, bulletSize = 0.4f;

        drawQuad(ammo, {bulletX, bulletY}, {bulletSize, bulletSize});

        drawQuad(digits[(totalAmmo / 10) % 10], {total1X, totalAmmoY}, {totalAmmoSize, totalAmmoSize});
        drawQuad(digits[totalAmmo % 10], {total2X, totalAmmoY}, {totalAmmoSize, totalAmmoSize});

        drawQuad(backSlash, {slashX, totalAmmoY}, {totalAmmoSize, totalAmmoSize});

        if (inHandAmmo >= 100)
        {
            drawQuad(digits[inHandAmmo / 100], {inHand1X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
        }
        if (inHandAmmo >= 10)
        {
            drawQuad(digits[(inHandAmmo / 10) % 10], {inHand2X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
        }
        drawQuad(digits[inHandAmmo % 10], {inHand3X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});

    }else{

        slashX += totalDiff * 2;
        inHand3X = slashX - inHandDiff, inHand2X = inHand3X - inHandDiff, inHand1X = inHand2X - inHandDiff;
        total1X = slashX + totalDiff, total2X = total1X + totalDiff, total3X = total2X + totalDiff;
        bulletDiff = totalDiff - 0.18f, bulletX = total1X + totalDiff + 0.1f, bulletY = totalAmmoY - 0.1f, bulletSize = 0.4f;

        drawQuad(ammo, {bulletX, bulletY}, {bulletSize, bulletSize});

        drawQuad(digits[totalAmmo % 10], {total1X, totalAmmoY}, {totalAmmoSize, totalAmmoSize});

        drawQuad(backSlash, {slashX, totalAmmoY}, {totalAmmoSize, totalAmmoSize});

        if (inHandAmmo >= 100)
        {
            drawQuad(digits[inHandAmmo / 100], {inHand1X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
        }
        if (inHandAmmo >= 10)
        {
            drawQuad(digits[(inHandAmmo / 10) % 10], {inHand2X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
        }
        drawQuad(digits[inHandAmmo % 10], {inHand3X, inHandAmmoY}, {inHandAmmoSize, inHandAmmoSize});
    }
}

void Hud::drawHP(){

    float hpY = -3.9f, hpSize = 0.9f;
    float hpDiff = 0.6f * hpSize;
    float plusX = -7.0f, plusY = -3.75f, plusSize = 1.8f;
    float hp1X = plusX + hpDiff + 0.1f, hp2X = hp1X + hpDiff, hp3X = hp2X + hpDiff;

    if (hp < 0)
        hp = 0;

    drawQuad(plus, {plusX, plusY}, {plusSize, plusSize});
    if (hp >= 100){
        drawQuad(digits[hp / 100], {hp1X, hpY}, {hpSize, hpSize});
    }

    if (hp >= 10){
        drawQuad(digits[(hp / 10) % 10], {hp2X, hpY}, {hpSize, hpSize});
    }

    drawQuad(digits[hp % 10], {hp3X, hpY}, {hpSize, hpSize});
}

void Hud::drawHurtEffect(){

    glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-16, 9);
    glVertex2f(16, 9);
    glVertex2f(16, -9);
    glVertex2f(-16, -9);
    glEnd();
    glColor3f(1, 1, 1);
}

void Hud::setHP(unsigned hp){
    this->hp = hp;
}

void Hud::setInHandAmmo(unsigned ammo){
    this->inHandAmmo = ammo;
}

void Hud::setTotalAmmo(unsigned ammo){
    this->totalAmmo = ammo;
}

void Hud::setStatus(Status status){
    this->status = status;
}

void Hud::setIsHurting(bool isHurting){
    this->isHurting = isHurting;
}

void Hud::setShaderProgram(GLuint program){
    this->shaderProgram = program;
    glUseProgram(program);
    uniformsLocations[MODEL_MAT] = glGetUniformLocation(program, "model");
    uniformsLocations[COLOR_VEC] = glGetUniformLocation(program, "color");
    assert(glGetError() == 0);
}

GLuint Hud::getShaderProgram(){
    return shaderProgram;
}

void Hud::createQuadVAO(){
    GLfloat vertcies[] = {
        -1, -1,
        -1, 1,
        1, -1,
        1, 1};
    GLubyte indices[] = {
        1, 0, 2,
        3, 1, 2};
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    GLuint buffers[2];
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertcies, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    assert(glGetError() == 0);
}