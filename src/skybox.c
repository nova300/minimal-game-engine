#include "graphics.h"
#include "engine.h"
#include "stb_image.h"
#include "shaders.h"


typedef struct
{
    float yaw;
    float pitch;
    int scaledX;
    int scaledY;
    int upperLeftIndex;
}Skybox;

GLuint skyboxTexture;

Shader *skyboxShader;

static RenderQueue skyboxRq;
static char renderQueueInitialized = false;

#define SKYBOX_WIDTH (4 * SCREEN_WIDTH)
#define SKYBOX_HEIGHT (4 * SCREEN_HEIGHT)

#define SKYBOX_TILE_WIDTH (SCREEN_WIDTH / 2)
#define SKYBOX_TILE_HEIGHT (SCREEN_HEIGHT / 2)

#define SKYBOX_COLS (10)
#define SKYBOX_ROWS (8)

Skybox skyboxinfo;

void skybox_load_texture(const char* filename)
{
    int tileWidth = 32;
    int channels = 4;
    int tilesX = 10;
    int tilesY = 8;
    int imageCount = tilesX * tilesY;
    int w;
    int h;
    int comp;
    unsigned char *image = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);

    unsigned char tile[tileWidth * tileWidth * channels];

    int tileSizeX = tileWidth * channels;
    int rowLenght = tilesX * tileSizeX;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileWidth, imageCount);

    for (unsigned int i = 0; i < tilesY; ++i)
    {   
        for (unsigned int y = 0; y < tilesX; ++y)
        {
            unsigned char *ptr = image + i * rowLenght + y * tileSizeX;
            for (int row = 0; row < tileWidth; ++row)
            {
                memcpy(tile + row * tileSizeX, ptr + row * rowLenght, sizeof(unsigned char) * tileSizeX);
            }
            
            int n = i * tilesX + y;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, n, tileWidth, tileWidth, 1, GL_RGBA, GL_UNSIGNED_BYTE, tile);
        }
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    skyboxTexture = texture;
}

int calculate_skybox_scaled_x()
{
    float yawScaled = SCREEN_WIDTH * skyboxinfo.yaw / fov;
    int scaledX = yawScaled * 0.5f;

    if (scaledX > SKYBOX_WIDTH) 
    {
        scaledX -= scaledX / SKYBOX_WIDTH * SKYBOX_WIDTH;
    }
    if (scaledX < 0)
    {
        return SKYBOX_WIDTH - -scaledX;
    }
    return SKYBOX_WIDTH - scaledX;
}

int calculate_skybox_scaled_y()
{
    float pitchInDegrees = (float) skyboxinfo.pitch;

    float degreesToScale = 360.0f * pitchInDegrees / fov;
    int roundedY = (int)degreesToScale;

    int scaledY = roundedY + 5 * SKYBOX_TILE_HEIGHT;

    if (scaledY > SKYBOX_HEIGHT) {
        scaledY = SKYBOX_HEIGHT;
    }
    if (scaledY < SCREEN_HEIGHT) {
        scaledY = SCREEN_HEIGHT;
    }
    return scaledY;
}

int get_top_left_tile_idx()
{
    int tileCol = skyboxinfo.scaledX / SKYBOX_TILE_WIDTH;
    int tileRow = (SKYBOX_HEIGHT - skyboxinfo.scaledY) / SKYBOX_TILE_HEIGHT;

    return tileRow * SKYBOX_COLS + tileCol;
}

vertex *make_skybox_rect(int tileIndex, int *indicies)
{
    vertex *verts = malloc(4 * sizeof(vertex));
    int x = tileIndex % SKYBOX_COLS * SKYBOX_TILE_WIDTH;
    int y = SKYBOX_HEIGHT - tileIndex / SKYBOX_COLS * SKYBOX_TILE_HEIGHT;

    verts[0] = gfx_make_vertex(x, y, -1, 0, 0);
    verts[1] = gfx_make_vertex(x, y - SKYBOX_TILE_HEIGHT, -1, 0, 1);
    verts[2] = gfx_make_vertex(x + SKYBOX_TILE_WIDTH, y - SKYBOX_TILE_HEIGHT, -1, 1, 1);
    verts[3] = gfx_make_vertex(x + SKYBOX_TILE_WIDTH, y, -1, 1, 0);

    return verts;
}

void render_skybox(float pitch, float yaw)
{
    skyboxinfo.yaw = pitch;
    skyboxinfo.pitch = yaw;
    skyboxinfo.scaledX = calculate_skybox_scaled_x();
    skyboxinfo.scaledY = calculate_skybox_scaled_y();
    skyboxinfo.upperLeftIndex = get_top_left_tile_idx();

    printf("%f\n", skyboxinfo.yaw);

    mat4 matrix = matrix_ortho(skyboxinfo.scaledX, skyboxinfo.scaledX + SCREEN_WIDTH, skyboxinfo.scaledY - SCREEN_HEIGHT, skyboxinfo.scaledY, 0.0f, 3.0f);
    mat4 modelMatrix = IDENTITY_MATRIX;

    GeoObject tile_grid[9];

    if (!renderQueueInitialized)
    {
        rq_init(&skyboxRq, 10);
        Shader *s = newShaderObject(vertex_shader_3, fragment_shader_3);

        skyboxRq.gpuHandle.shader = s;
        skyboxShader = s;

        skyboxRq.gpuHandle.textureAtlas = skyboxTexture;

        renderQueueInitialized = true;
    }

    skyboxRq.count = 0;

    int row;
    int col;

    int object = 0;

    for (row = 0; row < 3; row++) 
    {
        for (col = 0; col < 3; col++) 
        {
            int tileIndex = skyboxinfo.upperLeftIndex + row * SKYBOX_COLS + col;

            int *indicies = malloc(sizeof(int) * 6);
            indicies[0] = 0;
            indicies[1] = 1;
            indicies[2] = 2;
            indicies[3] = 3;
            indicies[4] = 2;
            indicies[5] = 0;
            vertex *vertices = make_skybox_rect(tileIndex, indicies);
            int *texture = malloc(sizeof(int));
            *texture = tileIndex;

            tile_grid[object].data = vertices;
            tile_grid[object].indicies = indicies;
            tile_grid[object].dataCount = 4;
            tile_grid[object].indexCount = 6;
            tile_grid[object].transform = &modelMatrix;
            tile_grid[object].instanceCapacity = 1;
            tile_grid[object].instanceCount = 1;
            tile_grid[object].texture = texture;
            tile_grid[object].dataDirty = true;
            tile_grid[object].instanceDirty = true;

            rq_add_object(&skyboxRq, &tile_grid[object]);

            object++;
        }
    }

    rq_update_buffers(&skyboxRq);

    mat4 oldproj = projectionMatrix;
    projectionMatrix = matrix;
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    geo_render(&skyboxRq.gpuHandle);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    projectionMatrix = oldproj;

    for(int i = 0; i < 9; i++)
    {
        free(tile_grid[i].texture);
        free(tile_grid[i].data);
        free(tile_grid[i].indicies);
    }

}