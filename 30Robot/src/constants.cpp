#include "constants.hpp"

const int WIN_WIDTH = 1920 / 2;
const int WIN_HEIGHT = 1080 / 2;
const float WIN_RATIO = (float) WIN_WIDTH / WIN_HEIGHT;
const float PROJ_WIDTH = 100.0f;
const float PROJ_WIDTH_RAT = PROJ_WIDTH * WIN_RATIO;
const float PROJ_HEIGHT = 100.0f;
const float TILE_SIZE = 11.0f;

const float DELAY_BETWEEN_VICTORY_AND_CHANGE_GAME_STATE = 3.0f;

const int ENEMY_SURVIVE_LIFE_COST = 1;
const float ENEMY_DRONE_HEALTH = 3.0f;
const float ENEMY_KAMIKAZE_HEALTH = 2.0f;
const float ENEMY_HEALTH_RANDOM_VARIATION = 0.5f;
const float ENEMY_VELOCITY = 18.0f;
const float KAMIKAZE_EXPLOSION_GROWTH_SPEED = 30.0f;
const float KAMIKAZE_EXPLOSION_DAMAGE = 5.0f;
const float KAMIKAZE_EXPLOSION_RADIUS = 75.0f;

const float TOWER_HEALTH = 10;
const float TOWER_HITBOX_RADIUS = 4.0f;
const float TOWER_ATTACK_RANGE = 25.0f;
const float MIRROR_RADIUS = TILE_SIZE *0.5f;

const float LASER_DAMAGE_PER_SECOND = 2.0f;
const float LASER_WIDTH = 13;
const float PROJECTILE_HITBOX_RADIUS = 1.0f;
const float SLOW_PROJECTILE_VELOCITY = 1.0f;
const float SLOW_TOWER_TIME_BETWEEN_TWO_SHOTS = 25.0f;

const float TOWER_SLOW_AMOUNT = 0.4;
const float SLOW_DURATION = 4;

const float SCREEN_SHAKE_DURATION = 0.4;

const float FRAMERATE = 60.0f;
const float TARGET_DELTA_MS = 1000.0f / FRAMERATE; // 60 FPS
const float MAX_DELTA_MS = 1000.0f; // one second

const int Z_INDEX_MAP = -9;
const int Z_INDEX_ENEMY = 0;
const int Z_INDEX_TOWER = 1;
const int Z_INDEX_VISUAL_EFFECTS = 2;
const int Z_INDEX_DEBUG_DRAW = 9;
const int Z_INDEX_HUD = 9;;
