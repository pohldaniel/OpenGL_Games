#include "Drawing.hpp"

void Drawing::drawText(GPU_Target* target, const char* string,
    TTF_Font* font, int x, int y,
    uint8_t fR, uint8_t fG, uint8_t fB, int align) {
    drawText(target, string, font, x, y, fR, fG, fB, true, align);
}

void Drawing::drawText(GPU_Target* target, const char* string,
    TTF_Font* font, int x, int y,
    uint8_t fR, uint8_t fG, uint8_t fB, bool shadow, int align) {

    SDL_Color foregroundColor = {fR, fG, fB};

    if(shadow) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, {0, 0, 0});
        GPU_Image* Message = GPU_CopyImageFromSurface(textSurface);
        GPU_SetImageFilter(Message, GPU_FILTER_NEAREST);

        //SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
        GPU_Rect Message_rect;
        Message_rect.x = x + 1 - align * textSurface->w / 2.0f;
        Message_rect.y = y + 1;
        Message_rect.w = textSurface->w;
        Message_rect.h = textSurface->h;

        GPU_BlitRect(Message, NULL, target, &Message_rect);

        SDL_FreeSurface(textSurface);
        GPU_FreeImage(Message);
    }

    {

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, foregroundColor);

        GPU_Image* Message = GPU_CopyImageFromSurface(textSurface);
        GPU_SetImageFilter(Message, GPU_FILTER_NEAREST);

        //SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
        GPU_Rect Message_rect;
        Message_rect.x = x - align * textSurface->w / 2.0f;
        Message_rect.y = y;
        Message_rect.w = textSurface->w;
        Message_rect.h = textSurface->h;

        GPU_BlitRect(Message, NULL, target, &Message_rect);

        SDL_FreeSurface(textSurface);
        GPU_FreeImage(Message);
    }

    //TTF_CloseFont(font);
}

void Drawing::drawTextBG(GPU_Target* target, const char* string,
    TTF_Font* font, int x, int y,
    uint8_t fR, uint8_t fG, uint8_t fB, SDL_Color bgCol, int align) {
    drawTextBG(target, string, font, x, y, fR, fG, fB, bgCol, true, align);
}

void Drawing::drawTextBG(GPU_Target* target, const char* string,
    TTF_Font* font, int x, int y,
    uint8_t fR, uint8_t fG, uint8_t fB, SDL_Color bgCol, bool shadow, int align) {


    SDL_Color foregroundColor = {fR, fG, fB};


    SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, string, foregroundColor);

    {
        GPU_Rect Message_rect;
        Message_rect.x = x + 1 - align * textSurface2->w / 2.0f - 3;
        Message_rect.y = y + 1;
        Message_rect.w = textSurface2->w + 5;
        Message_rect.h = textSurface2->h;

        GPU_RectangleFilled2(target, Message_rect, bgCol);
    }

    if(shadow) {

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, {0, 0, 0});

        GPU_Image* Message = GPU_CopyImageFromSurface(textSurface);
        GPU_SetImageFilter(Message, GPU_FILTER_NEAREST);

        //SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
        GPU_Rect Message_rect;
        Message_rect.x = x + 1 - align * textSurface->w / 2.0f;
        Message_rect.y = y + 1;
        Message_rect.w = textSurface->w;
        Message_rect.h = textSurface->h;

        GPU_BlitRect(Message, NULL, target, &Message_rect);

        SDL_FreeSurface(textSurface);
        GPU_FreeImage(Message);
    }

    {
        
        GPU_Image* Message = GPU_CopyImageFromSurface(textSurface2);
        GPU_SetImageFilter(Message, GPU_FILTER_NEAREST);

        //SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
        GPU_Rect Message_rect;
        Message_rect.x = x - align * textSurface2->w / 2.0f;
        Message_rect.y = y;
        Message_rect.w = textSurface2->w;
        Message_rect.h = textSurface2->h;

        GPU_BlitRect(Message, NULL, target, &Message_rect);

        SDL_FreeSurface(textSurface2);
        GPU_FreeImage(Message);
    }

    //TTF_CloseFont(font);
}

DrawTextParams Drawing::drawTextParams(GPU_Target* renderer, const char* string,
    TTF_Font* font, int x, int y,
    uint8_t fR, uint8_t fG, uint8_t fB, int align) {
    return drawTextParams(renderer, string, font, x, y, fR, fG, fB, true, align);
}

DrawTextParams Drawing::drawTextParams(GPU_Target* renderer, const char* string,
    TTF_Font* font, int x, int y,
    uint8_t fR, uint8_t fG, uint8_t fB, bool shadow, int align) {

    GPU_Image* t1 = nullptr;

    if(shadow) {
        SDL_Surface* sf = TTF_RenderText_Solid(font, string, {0, 0, 0});

        t1 = GPU_CopyImageFromSurface(sf);
        GPU_SetImageFilter(t1, GPU_FILTER_NEAREST);

        SDL_FreeSurface(sf);
    }

    SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, string, {fR, fG, fB});

    GPU_Image* t2 = GPU_CopyImageFromSurface(textSurface2);
    GPU_SetImageFilter(t2, GPU_FILTER_NEAREST);

    int w = textSurface2->w;
    int h = textSurface2->h;

    SDL_FreeSurface(textSurface2);

    return {t1, t2, w, h};
    //TTF_CloseFont(font);
}

void Drawing::drawText(GPU_Target* target, DrawTextParams pm, int x, int y, int align) {
    drawText(target, pm, x, y, true, align);
}


void Drawing::drawText(GPU_Target* target, DrawTextParams pm, int x, int y, bool shadow, int align) {

    if(shadow) {

        GPU_Blit(pm.t1, NULL, target, x + 1 - align * pm.w / 2.0f + pm.w / 2.0f, y + 1 + pm.h / 2.0f);


        //SDL_FreeSurface(textSurface);
        //SDL_DestroyTexture(Message);
    }

    {

        GPU_Blit(pm.t2, NULL, target, x - align * pm.w / 2.0f + pm.w / 2.0f, y + pm.h / 2.0f);


        //SDL_FreeSurface(textSurface);
        //SDL_DestroyTexture(Message);
    }

    //TTF_CloseFont(font);
}

b2Vec2 Drawing::rotate_point(float cx, float cy, float angle, b2Vec2 p) {

    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin:
    p.x -= cx;
    p.y -= cy;

    // rotate point
    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;

    // translate point back:
    return b2Vec2(xnew + cx, ynew + cy);
}

void Drawing::drawPolygon(GPU_Target* target, SDL_Color col, b2Vec2* verts, int x, int y, float scale, int count, float angle, float cx, float cy) {


    if(count < 2) return;
    b2Vec2 last = rotate_point(cx, cy, angle, verts[count - 1]);
    for(int i = 0; i < count; i++) {

        b2Vec2 rot = rotate_point(cx, cy, angle, verts[i]);
        GPU_Line(target, x + last.x*scale, y + last.y*scale, x + rot.x * scale, y + rot.y * scale, col);
        last = rot;
    }
}

uint32 Drawing::darkenColor(uint32 color, float brightness) {


    int a = (color >> 24) & 0xFF;
    int r = (int)(((color >> 16) & 0xFF) * brightness);
    int g = (int)(((color >> 8) & 0xFF) * brightness);
    int b = (int)((color & 0xFF) * brightness);

    return (a << 24) | (r << 16) | (g << 8) | b;
}
