#include "starlight.h"
#include "debug_console.h"
#include "editor.h"
#include "stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../thirdparty/stb_truetype.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

// --- System Logging Implementation ---
void starlight_log(SLF_LogLevel level, const char* file, int line, const char* fmt, ...) {
    static FILE* log_fp = NULL;
    if (!log_fp) {
        log_fp = fopen("logs/slf_engine.log", "w"); // overwrite for session
    }
    
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    const char* level_str = "INFO";
    const char* color_code = "\033[0m"; // Reset
    switch (level) {
        case SLF_LOG_LEVEL_INFO:  level_str = "INFO";  color_code = "\033[36m"; break; // Cyan
        case SLF_LOG_LEVEL_WARN:  level_str = "WARN";  color_code = "\033[33m"; break; // Yellow
        case SLF_LOG_LEVEL_ERROR: level_str = "ERROR"; color_code = "\033[31m"; break; // Red
        case SLF_LOG_LEVEL_FATAL: level_str = "FATAL"; color_code = "\033[41m"; break; // Background Red
    }

    // Extract filename from path for cleaner logs
    const char* short_file = strrchr(file, '/');
    if (!short_file) short_file = strrchr(file, '\\');
    short_file = short_file ? short_file + 1 : file;

    // Terminal Output with Colors
    printf("%s[%s]%s %s:%d | %s\n", color_code, level_str, "\033[0m", short_file, line, buffer);
    
    // File Output (plain text)
    if (log_fp) {
        time_t t = time(NULL);
        struct tm* tm = localtime(&t);
        fprintf(log_fp, "[%02d:%02d:%02d] [%s] %s:%d | %s\n", 
                tm->tm_hour, tm->tm_min, tm->tm_sec, level_str, short_file, line, buffer);
        fflush(log_fp);
    }
}


int starlight_framework_init(const char* title, int width, int height, SLF_Config* config) 
{
    SLF_App app; Editor editor; static SDL_Event event; float lastFrame = 0.0f;
    float ui_quad[] = { 0,1,0,1, 0,0,0,0, 1,0,1,0, 0,1,0,1, 1,0,1,0, 1,1,1,1 };

    memset(&app, 0, sizeof(SLF_App));
    app.running = true; app.width = width; app.height = height;
    app.time.time_scale = 1.0f; // Default time scale
    
    // Allocate 16MB for per-frame allocations
    starlight_arena_init(&app.frame_arena, 16 * 1024 * 1024);
    
    slf_asset_pool_init(&app.asset_pool); // Internal Smart Cache
    
    app.headless = config ? config->is_headless : false;
    if (!engine_init(&app.engine, title, width, height, app.headless)) return -1;
    app.width = app.engine.width; app.height = app.engine.height; // Update to actual drawable size
    job_system_init(0); debug_console_init(&app.debug_console); editor_init(&editor);
    if (!app.headless) {
        audio_init(); shadow_init(&app.shadow, 1024, 4); post_process_init(&app.post, 320, 180, width, height);
    }
    
    if (!app.headless) {
        app.ui_shader = shader_load_program("assets/shaders/ui_simple.vert", "assets/shaders/ui_simple.frag");
        printf("[ENGINE] UI Shader loaded: %u\n", app.ui_shader); fflush(stdout);
    
    app.mode7_shader = shader_load_program("assets/shaders/mode7.vert", "assets/shaders/mode7.frag");
    printf("[ENGINE] Mode 7 Shader loaded: %u\n", app.mode7_shader); fflush(stdout);
    glGenVertexArrays(1, &app.ui_vao); glGenBuffers(1, &app.ui_vbo);
    glBindVertexArray(app.ui_vao); glBindBuffer(GL_ARRAY_BUFFER, app.ui_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ui_quad), ui_quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Instanced Buffer Initialization
    glGenBuffers(1, &app.inst_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, app.inst_vbo);
    glBufferData(GL_ARRAY_BUFFER, 1000 * 8 * sizeof(float), NULL, GL_DYNAMIC_DRAW); 
    
    // Attribs 2, 3, 4 for instancing (iPos, iSize, iColor)
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(3); glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(4); glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glVertexAttribDivisor(4, 1);

    glGenVertexArrays(1, &app.text_vao); glGenBuffers(1, &app.text_vbo);
    glBindVertexArray(app.text_vao); glBindBuffer(GL_ARRAY_BUFFER, app.text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 2048, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    } // End of Graphics Init Block

    app.world = ecs_init();
    if (!app.headless) app.keys = SDL_GetKeyboardState(NULL);

    // === User Startup Callback ===
    slf_events_init();
    
    if (config && config->on_start) config->on_start(&app);

    float accumulator = 0.0f;
    const float FIXED_DT = 1.0f / 60.0f;

    while (app.engine.running && app.running) {
        starlight_arena_reset(&app.frame_arena); // Clear transient frame memory
        
        float currentFrame = SDL_GetTicks() / 1000.0f;
        float frameTime = currentFrame - lastFrame;
        if (frameTime > 0.25f) frameTime = 0.25f; // Spiral of death safeguard
        lastFrame = currentFrame;
        app.total_time = currentFrame;
        
        accumulator += frameTime;

        if (!app.headless) {
            memcpy(app.prev_keys, app.keys, 512);
            app.prev_mouse_state = app.mouse_state;
            app.mouse_state = SDL_GetMouseState(&app.mouse_x, &app.mouse_y);
            SDL_GetRelativeMouseState(&app.mouse_dx, &app.mouse_dy);
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) app.engine.running = false;
            
            if (!app.headless && event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_GRAVE) {
                    app.debug_console.visible = !app.debug_console.visible;
                    if (app.debug_console.visible) SDL_StartTextInput();
                    else SDL_StopTextInput();
                } else if (app.debug_console.visible) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE) debug_console_on_key(&app.debug_console, '\b');
                    if (event.key.keysym.sym == SDLK_RETURN) debug_console_on_key(&app.debug_console, '\r');
                }
            }
            if (!app.headless && event.type == SDL_TEXTINPUT && app.debug_console.visible) {
                debug_console_on_text_input(&app.debug_console, event.text.text);
            }
            
            // Only process game mouse if console is closed (Quake style mouse lock)
            if (!app.headless && event.type == SDL_MOUSEMOTION && !app.debug_console.visible) {
                camera_process_mouse_movement(&app.camera, (float)event.motion.xrel, (float)-event.motion.yrel);
            }
        }
        if (!app.headless) { starlight_shader_check_reload(&app); editor_update(&app, &editor); }
        
        // --- RETRO 16-BIT DETERMINISTIC LOCK (60Hz) ---
        // Support Time Scale for Physics and Slow-Motion Effects
        float scaled_dt = frameTime * app.time.time_scale;
        app.time.frame_delta_time = scaled_dt;
        app.delta_time = scaled_dt; // Update legacy value too
        
        while (accumulator >= FIXED_DT) {
            app.time.fixed_delta_time = FIXED_DT;
            if (!app.headless && !editor.active) camera_process_keyboard(&app.camera, app.keys, FIXED_DT);
            
            // Note: Currently ECS logic depends on FIXED_DT step size. But its systems may use actual frame_dt 
            ecs_progress(app.world, FIXED_DT);
            
            // Scene routing or Fallback
            if (app.scene_count > 0 && app.scene_stack[app.scene_count-1]->on_update) {
                app.scene_stack[app.scene_count-1]->on_update(&app, app.time.fixed_delta_time);
            } else if (config && config->on_update) {
                config->on_update(&app);
            }
            accumulator -= FIXED_DT;
        }
        
        if (!app.headless) {
            if (app.post_processing_enabled) post_process_begin(&app.post);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            if (app.scene_count > 0 && app.scene_stack[app.scene_count-1]->on_draw) {
                app.scene_stack[app.scene_count-1]->on_draw(&app);
            } else if (config && config->on_render) {
                config->on_render(&app);
            }
            
            if (app.post_processing_enabled) { 
                post_process_end(&app.post);
                mat4 view, proj;
                camera_get_view_matrix(&app.camera, view);
                glm_perspective(glm_rad(60.0f), (float)app.width/(float)app.height, 0.1f, 1000.0f, proj);
                post_process_render(&app.post, 1.0f, view, proj); 
            }
            
            if (app.scene_count > 0 && app.scene_stack[app.scene_count-1]->on_ui) {
                app.scene_stack[app.scene_count-1]->on_ui(&app);
            } else if (config && config->on_ui) {
                config->on_ui(&app);
            }
            
            // --- UI Flush ---
            starlight_ui_flush_rects(&app);
            
            editor_render_ui(&app, &editor); debug_console_render(&app, &app.debug_console);
            SDL_GL_SwapWindow(app.engine.window);
        } else {
            SDL_Delay(1); // Dedicate tick
        }
        
        // Final do Pipeline do Frame: Despachar eventos da fila async
        slf_events_flush();
    } // end while

    // === Teardown Phase ===
    if (config && config->on_shutdown) config->on_shutdown(&app);
    
    // Cleanup scenes
    while (app.scene_count > 0) starlight_scene_pop(&app);
    
    // System Cleanup
    ecs_fini(app.world);
    if (!app.headless) {
        audio_shutdown();
        shadow_destroy(&app.shadow);
        post_process_destroy(&app.post);
        
        // Core Graphics Cleanup
        glDeleteVertexArrays(1, &app.ui_vao);
        glDeleteBuffers(1, &app.ui_vbo);
        glDeleteBuffers(1, &app.inst_vbo);
        glDeleteVertexArrays(1, &app.text_vao);
        glDeleteBuffers(1, &app.text_vbo);
        
        shader_delete(app.ui_shader);
        shader_delete(app.mode7_shader);
    }
    job_system_shutdown();
    
    slf_asset_pool_destroy(&app.asset_pool);
    starlight_arena_destroy(&app.frame_arena);
    
    slf_events_shutdown();
    starlight_engine_destroy(&app.engine);
    
    return 0;
}

// --- Framework Overhaul: Scene Management API ---
void starlight_scene_push(SLF_App* app, SLF_Scene* scene) {
    if (app->scene_count < 16) {
        app->scene_stack[app->scene_count++] = scene;
        if (scene->on_enter) scene->on_enter(app);
    }
}

void starlight_scene_pop(SLF_App* app) {
    if (app->scene_count > 0) {
        SLF_Scene* top = app->scene_stack[app->scene_count - 1];
        if (top->on_exit) top->on_exit(app);
        app->scene_count--;
    }
}

void starlight_scene_change(SLF_App* app, SLF_Scene* scene) {
    starlight_scene_pop(app);
    starlight_scene_push(app, scene);
}

void starlight_ui_draw_rect(SLF_App* app, float x, float y, float w, float h, vec4 color) {
    if (app->rect_batcher.count >= 1000) {
        starlight_ui_flush_rects(app);
    }
    int i = app->rect_batcher.count;
    app->rect_batcher.positions[i*2] = x;
    app->rect_batcher.positions[i*2+1] = y;
    app->rect_batcher.sizes[i*2] = w;
    app->rect_batcher.sizes[i*2+1] = h;
    glm_vec4_copy(color, app->rect_batcher.colors[i]);
    app->rect_batcher.count++;
}

void starlight_ui_flush_rects(SLF_App* app) {
    if (app->rect_batcher.count > 0) {
        // Usa a estrutura de instancing subjacente, extremamente veloz
        starlight_ui_draw_rects_instanced(app, app->rect_batcher.positions, app->rect_batcher.sizes, app->rect_batcher.colors, app->rect_batcher.count);
        app->rect_batcher.count = 0;
    }
}

void starlight_ui_draw_rects_instanced(SLF_App* app, float* positions, float* sizes, vec4* colors, int count) {
    if (count <= 0) return;
    if (count > 1000) count = 1000;

    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    glUseProgram(app->ui_shader);
    mat4 projection; glm_ortho(0, app->width, app->height, 0, -1, 1, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->ui_shader, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_instancing"), 1);

    // Pack data: [posX, posY, sizeW, sizeH, r, g, b, a] x count
    float* data = (float*)malloc(count * 8 * sizeof(float));
    for(int i=0; i<count; i++) {
        data[i*8+0] = positions[i*2+0]; data[i*8+1] = positions[i*2+1];
        data[i*8+2] = sizes[i*2+0];     data[i*8+3] = sizes[i*2+1];
        data[i*8+4] = colors[i][0];     data[i*8+5] = colors[i][1];
        data[i*8+6] = colors[i][2];     data[i*8+7] = colors[i][3];
    }

    glBindBuffer(GL_ARRAY_BUFFER, app->inst_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * 8 * sizeof(float), data);
    free(data);

    glBindVertexArray(app->ui_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_instancing"), 0); // Reset
}

void starlight_ui_draw_texture(SLF_App* app, GLuint tex, float x, float y, float w, float h, vec4 color) {
    starlight_ui_flush_rects(app); // Previne quebra de Z-Order
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(app->ui_shader);
    mat4 projection; glm_ortho(0, app->width, app->height, 0, -1, 1, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->ui_shader, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform2f(glGetUniformLocation(app->ui_shader, "position"), x, y);
    glUniform2f(glGetUniformLocation(app->ui_shader, "size"), w, h);
    glUniform4fv(glGetUniformLocation(app->ui_shader, "color"), 1, color);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_texture"), 1);
    glUniform1f(glGetUniformLocation(app->ui_shader, "rotation"), 0.0f);
    glUniform4f(glGetUniformLocation(app->ui_shader, "uv_rect"), 0,0,1,1);
    glUniform1i(glGetUniformLocation(app->ui_shader, "is_circle"), 0);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_instancing"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(app->ui_shader, "tex"), 0);

    glBindVertexArray(app->ui_vao); glDrawArrays(GL_TRIANGLES, 0, 6);
}

// --- Retro Genesis: Sprite Sheet Rendering ---
void starlight_sprite_sheet_init(SLF_SpriteSheet* sheet, GLuint tex, int cols, int rows) {
    sheet->texture_id = tex;
    sheet->cols = cols;
    sheet->rows = rows;
    sheet->total_frames = cols * rows;
    sheet->frame_size_uv[0] = 1.0f / (float)cols;
    sheet->frame_size_uv[1] = 1.0f / (float)rows;
}

void starlight_sprite_animator_init(SLF_SpriteAnimator* anim, SLF_SpriteSheet* sheet, int start, int end, float duration, bool loop) {
    anim->sheet = sheet;
    anim->start_frame = start;
    anim->end_frame = end;
    anim->frame_duration = duration;
    anim->current_time = 0.0f;
    anim->current_frame = start;
    anim->loop = loop;
    anim->finished = false;
}

void starlight_sprite_animator_update(SLF_SpriteAnimator* anim, float dt) {
    if (anim->finished) return;
    anim->current_time += dt;
    if (anim->current_time >= anim->frame_duration) {
        anim->current_time -= anim->frame_duration;
        anim->current_frame++;
        if (anim->current_frame > anim->end_frame) {
            if (anim->loop) {
                anim->current_frame = anim->start_frame;
            } else {
                anim->current_frame = anim->end_frame;
                anim->finished = true;
            }
        }
    }
}

void starlight_ui_draw_sprite(SLF_App* app, SLF_SpriteSheet* sheet, int frame_index, float x, float y, float w, float h, vec4 color, bool flip_x) {
    if(!sheet || frame_index < 0 || frame_index >= sheet->total_frames) return;
    
    starlight_ui_flush_rects(app); // Previne quebra de Z-Order

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(app->ui_shader);
    mat4 projection; glm_ortho(0, app->width, app->height, 0, -1, 1, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->ui_shader, "projection"), 1, GL_FALSE, (float*)projection);
    
    glUniform2f(glGetUniformLocation(app->ui_shader, "position"), x, y);
    glUniform2f(glGetUniformLocation(app->ui_shader, "size"), w, h);
    glUniform4fv(glGetUniformLocation(app->ui_shader, "color"), 1, color);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_texture"), 1);
    glUniform1i(glGetUniformLocation(app->ui_shader, "is_circle"), 0);
    glUniform1f(glGetUniformLocation(app->ui_shader, "rotation"), 0.0f);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_instancing"), 0);

    int col = frame_index % sheet->cols;
    int row = frame_index / sheet->cols;
    
    // UV layout: bottom-left (u, v), width (uw), height (uh)
    float ux = (float)col * sheet->frame_size_uv[0];
    float uy = (float)row * sheet->frame_size_uv[1];
    float uw = sheet->frame_size_uv[0];
    float uh = sheet->frame_size_uv[1];

    if (flip_x) {
        ux += uw;
        uw = -uw;
    }

    glUniform4f(glGetUniformLocation(app->ui_shader, "uv_rect"), ux, uy, uw, uh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sheet->texture_id);
    glUniform1i(glGetUniformLocation(app->ui_shader, "tex"), 0);

    glBindVertexArray(app->ui_vao); 
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// --- Retro Genesis: Tilemap Engine & AABB Physics ---
void starlight_tilemap_init(SLF_Tilemap* map, int cols, int rows, float tile_size, SLF_SpriteSheet* sheet) {
    if(!map) return;
    map->cols = cols;
    map->rows = rows;
    map->tile_size = tile_size;
    map->sheet = sheet;
    map->data = (int*)malloc(cols * rows * sizeof(int));
    memset(map->data, 0, cols * rows * sizeof(int));
}

void starlight_tilemap_destroy(SLF_Tilemap* map) {
    if (map && map->data) { free(map->data); map->data = NULL; }
}

void starlight_tilemap_set_data(SLF_Tilemap* map, int* map_data) {
    if(map && map->data && map_data) memcpy(map->data, map_data, map->cols * map->rows * sizeof(int));
}

void starlight_ui_draw_tilemap(SLF_App* app, SLF_Tilemap* map, float offset_x, float offset_y) {
    if(!map || !map->data || !map->sheet) return;
    starlight_ui_flush_rects(app); // Previne quebra de Z-Order
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->cols; x++) {
            int tile_id = map->data[y * map->cols + x];
            if (tile_id > 0) { // 0 = Air/Empty
                float px = offset_x + x * map->tile_size;
                float py = offset_y + y * map->tile_size;
                // Culling Clássico (Apenas desenha o que está na tela)
                if (px + map->tile_size > 0 && px < app->width && py + map->tile_size > 0 && py < app->height) {
                    starlight_ui_draw_sprite(app, map->sheet, tile_id - 1, px, py, map->tile_size, map->tile_size, (vec4){1,1,1,1}, false);
                }
            }
        }
    }
}

void starlight_physics_tilemap_collide(SLF_Tilemap* map, float* px, float* py, float p_width, float p_height, float* p_vx, float* p_vy) {
    if(!map || !map->data) return;
    int tx1 = (int)(*px / map->tile_size);
    int ty1 = (int)(*py / map->tile_size);
    int tx2 = (int)((*px + p_width) / map->tile_size);
    int ty2 = (int)((*py + p_height) / map->tile_size);

    for (int y = ty1; y <= ty2; y++) {
        for (int x = tx1; x <= tx2; x++) {
            if (x >= 0 && x < map->cols && y >= 0 && y < map->rows) {
                int tile = map->data[y * map->cols + x];
                if (tile > 0) { // Solid Collision
                    float tile_left = x * map->tile_size;
                    float tile_right = tile_left + map->tile_size;
                    float tile_top = y * map->tile_size;
                    float tile_bottom = tile_top + map->tile_size;

                    float overlap_left = (*px + p_width) - tile_left;
                    float overlap_right = tile_right - *px;
                    float overlap_top = (*py + p_height) - tile_top;
                    float overlap_bottom = tile_bottom - *py;
                    
                    float min_x = overlap_left < overlap_right ? overlap_left : overlap_right;
                    float min_y = overlap_top < overlap_bottom ? overlap_top : overlap_bottom;

                    if (min_x < min_y) {
                        if (overlap_left < overlap_right) { *px -= overlap_left; if(*p_vx > 0) *p_vx = 0; }
                        else { *px += overlap_right; if(*p_vx < 0) *p_vx = 0; }
                    } else {
                        if (overlap_top < overlap_bottom) { *py -= overlap_top; if(*p_vy > 0) *p_vy = 0; }
                        else { *py += overlap_bottom; if(*p_vy < 0) *p_vy = 0; }
                    }
                }
            }
        }
    }
}

// --- Retro Genesis: Parallax Engine ---
void starlight_parallax_layer_init(SLF_ParallaxLayer* layer, GLuint tex, float spd_x, float spd_y, float w, float h) {
    layer->texture_id = tex;
    layer->scroll_speed_x = spd_x;
    layer->scroll_speed_y = spd_y;
    layer->offset_x = 0;
    layer->offset_y = 0;
    layer->width = w;
    layer->height = h;
    
    // Assegura que a textura tem wrapping para scroll infinito
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void starlight_parallax_layer_update(SLF_ParallaxLayer* layer, float delta_cam_x, float delta_cam_y) {
    layer->offset_x += (delta_cam_x * layer->scroll_speed_x);
    layer->offset_y += (delta_cam_y * layer->scroll_speed_y);
    
    // Previne overflow de float a longo prazo
    if(layer->offset_x > 1000.0f || layer->offset_x < -1000.0f) layer->offset_x -= (int)layer->offset_x;
    if(layer->offset_y > 1000.0f || layer->offset_y < -1000.0f) layer->offset_y -= (int)layer->offset_y;
}

void starlight_ui_draw_parallax(SLF_App* app, SLF_ParallaxLayer* layer, float screen_x, float screen_y, vec4 color) {
    if(!layer || layer->texture_id == 0) return;
    
    starlight_ui_flush_rects(app); // Previne quebra de Z-Order

    glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(app->ui_shader);
    mat4 projection; glm_ortho(0, app->width, app->height, 0, -1, 1, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->ui_shader, "projection"), 1, GL_FALSE, (float*)projection);
    
    glUniform2f(glGetUniformLocation(app->ui_shader, "position"), screen_x, screen_y);
    glUniform2f(glGetUniformLocation(app->ui_shader, "size"), layer->width, layer->height);
    glUniform4fv(glGetUniformLocation(app->ui_shader, "color"), 1, color);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_texture"), 1);
    glUniform1i(glGetUniformLocation(app->ui_shader, "is_circle"), 0);
    glUniform1f(glGetUniformLocation(app->ui_shader, "rotation"), 0.0f);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_instancing"), 0);

    // O pulo do gato do Parallax: Apenas mudamos o ponto de inicio UV!
    glUniform4f(glGetUniformLocation(app->ui_shader, "uv_rect"), layer->offset_x, layer->offset_y, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, layer->texture_id);
    glUniform1i(glGetUniformLocation(app->ui_shader, "tex"), 0);

    glBindVertexArray(app->ui_vao); 
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void starlight_ui_draw_mode7(SLF_App* app, GLuint tex, float map_x, float map_y, float map_z, float horizon, float angle, float pitch) {
    starlight_ui_flush_rects(app); // Previne quebra de Z-Order
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(app->mode7_shader);
    mat4 projection; glm_ortho(0, app->width, app->height, 0, -1, 1, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->mode7_shader, "projection"), 1, GL_FALSE, (float*)projection);
    
    // Desenhamos na tela inteira (o shader descarta quem estiver acima do horizonte)
    glUniform2f(glGetUniformLocation(app->mode7_shader, "position"), 0.0f, 0.0f);
    glUniform2f(glGetUniformLocation(app->mode7_shader, "size"), (float)app->width, (float)app->height);
    
    glUniform1f(glGetUniformLocation(app->mode7_shader, "map_x"), map_x);
    glUniform1f(glGetUniformLocation(app->mode7_shader, "map_y"), map_y);
    glUniform1f(glGetUniformLocation(app->mode7_shader, "map_z"), map_z);
    glUniform1f(glGetUniformLocation(app->mode7_shader, "horizon"), horizon);
    glUniform1f(glGetUniformLocation(app->mode7_shader, "angle"), angle);
    glUniform1f(glGetUniformLocation(app->mode7_shader, "pitch"), pitch);

    if (tex > 0) {
        glUniform1i(glGetUniformLocation(app->mode7_shader, "use_texture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        // Importantíssimo para Mode 7: Repetir a textura infinitamente
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glUniform1i(glGetUniformLocation(app->mode7_shader, "tex"), 0);
    } else {
        glUniform1i(glGetUniformLocation(app->mode7_shader, "use_texture"), 0);
    }

    glBindVertexArray(app->ui_vao); 
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

SLF_Font* starlight_load_font(const char* path, float size) {
    SLF_Font* font = malloc(sizeof(SLF_Font));
    font->cdata = malloc(sizeof(stbtt_bakedchar) * 96);
    FILE* f = fopen(path, "rb"); if(!f) return NULL;
    fseek(f, 0, SEEK_END); long fsize = ftell(f); rewind(f);
    unsigned char* ttf = malloc(fsize); fread(ttf, 1, fsize, f); fclose(f);
    unsigned char* bmp = malloc(512 * 512);
    stbtt_BakeFontBitmap(ttf, 0, size, bmp, 512, 512, 32, 96, (stbtt_bakedchar*)font->cdata);
    free(ttf); glGenTextures(1, &font->texture_id);
    glBindTexture(GL_TEXTURE_2D, font->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLint swizzle[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    free(bmp); font->size = size; return font;
}

void starlight_unload_font(SLF_Font* font) {
    if (!font) return;
    if (font->texture_id) glDeleteTextures(1, &font->texture_id);
    if (font->cdata) free(font->cdata);
    free(font);
}

void starlight_ui_draw_text(SLF_App* app, SLF_Font* font, const char* text, float x, float y, vec4 color) {
    if (!font || !text) return;
    
    starlight_ui_flush_rects(app); // Previne quebra de Z-Order
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(app->ui_shader);
    mat4 projection; glm_ortho(0, app->width, app->height, 0, -1, 1, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->ui_shader, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform4fv(glGetUniformLocation(app->ui_shader, "color"), 1, color);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_texture"), 1);
    glUniform1i(glGetUniformLocation(app->ui_shader, "tex"), 0);
    glUniform1i(glGetUniformLocation(app->ui_shader, "use_instancing"), 0);
    
    // Pass-through uniforms for text (coordinates are already absolute)
    glUniform2f(glGetUniformLocation(app->ui_shader, "position"), 0, 0);
    glUniform2f(glGetUniformLocation(app->ui_shader, "size"), 1, 1);
    glUniform1f(glGetUniformLocation(app->ui_shader, "rotation"), 0);
    glUniform4f(glGetUniformLocation(app->ui_shader, "uv_rect"), 0, 0, 1, 1);
    glUniform1i(glGetUniformLocation(app->ui_shader, "is_circle"), 0);
    
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, font->texture_id);
    glBindVertexArray(app->text_vao);
    float tx = x, ty = y + font->size;
    int len = strlen(text);
    if (len > 2048) len = 2048; // Max characters per batch
    
    // Alocar batch de vertices
    float* batch_data = (float*)malloc(sizeof(float) * 24 * len);
    int char_count = 0;
    
    while (*text && char_count < len) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q; stbtt_GetBakedQuad((stbtt_bakedchar*)font->cdata, 512, 512, *text - 32, &tx, &ty, &q, 1);
            float qd[] = { q.x0, q.y0, q.s0, q.t0, q.x0, q.y1, q.s0, q.t1, q.x1, q.y1, q.s1, q.t1, q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1, q.x1, q.y0, q.s1, q.t0 };
            memcpy(&batch_data[char_count * 24], qd, sizeof(qd));
            char_count++;
        }
        text++;
    }
    
    if (char_count > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, app->text_vbo); 
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 24 * char_count, batch_data);
        glDrawArrays(GL_TRIANGLES, 0, 6 * char_count);
    }
    
    free(batch_data);
}

void starlight_bind_action(SLF_App* app, const char* name, int key, int btn) {
    starlight_bind_action_key(app, name, key);
    starlight_bind_action_button(app, name, btn);
}

void starlight_bind_action_key(SLF_App* app, const char* name, int scancode) {
    if (scancode == -1) return;
    for (int i = 0; i < app->action_count; i++) {
        if (strcmp(app->actions[i].name, name) == 0) {
            if (app->actions[i].key_count < 4) app->actions[i].keys[app->actions[i].key_count++] = scancode;
            return;
        }
    }
    if (app->action_count >= SLF_MAX_ACTIONS) return;
    SLF_Action* a = &app->actions[app->action_count++];
    memset(a, 0, sizeof(SLF_Action));
    snprintf(a->name, 31, "%s", name);
    a->keys[0] = scancode; a->key_count = 1;
}

void starlight_bind_action_button(SLF_App* app, const char* name, int button) {
    if (button == -1) return;
    for (int i = 0; i < app->action_count; i++) {
        if (strcmp(app->actions[i].name, name) == 0) {
            if (app->actions[i].button_count < 4) app->actions[i].buttons[app->actions[i].button_count++] = button;
            return;
        }
    }
    if (app->action_count >= SLF_MAX_ACTIONS) return;
    SLF_Action* a = &app->actions[app->action_count++];
    memset(a, 0, sizeof(SLF_Action));
    snprintf(a->name, 31, "%s", name);
    a->buttons[0] = button; a->button_count = 1;
}

void starlight_bind_action_mouse(SLF_App* app, const char* name, int mousebtn) {
    if (mousebtn == -1) return;
    for (int i = 0; i < app->action_count; i++) {
        if (strcmp(app->actions[i].name, name) == 0) {
            if (app->actions[i].mouse_button_count < 4) app->actions[i].mouse_buttons[app->actions[i].mouse_button_count++] = mousebtn;
            return;
        }
    }
    if (app->action_count >= SLF_MAX_ACTIONS) return;
    SLF_Action* a = &app->actions[app->action_count++];
    memset(a, 0, sizeof(SLF_Action));
    snprintf(a->name, 31, "%s", name);
    a->mouse_buttons[0] = mousebtn; a->mouse_button_count = 1;
}

bool starlight_is_action_pressed(SLF_App* app, const char* name) {
    for (int i = 0; i < app->action_count; i++) {
        if (strcmp(app->actions[i].name, name) == 0) {
            for (int k = 0; k < app->actions[i].key_count; k++) {
                if (app->keys[app->actions[i].keys[k]]) return true;
            }
            for (int b = 0; b < app->actions[i].button_count; b++) {
                if (app->buttons[app->actions[i].buttons[b]]) return true;
            }
            for (int m = 0; m < app->actions[i].mouse_button_count; m++) {
                if (app->mouse_state & SDL_BUTTON(app->actions[i].mouse_buttons[m])) return true;
            }
            return false;
        }
    }
    return false;
}

bool starlight_is_action_just_pressed(SLF_App* app, const char* name) {
    for (int i = 0; i < app->action_count; i++) {
        if (strcmp(app->actions[i].name, name) == 0) {
            for (int k = 0; k < app->actions[i].key_count; k++) {
                int sc = app->actions[i].keys[k];
                if (app->keys[sc] && !app->prev_keys[sc]) return true;
            }
            for (int b = 0; b < app->actions[i].button_count; b++) {
                int bt = app->actions[i].buttons[b];
                if (app->buttons[bt] && !app->prev_buttons[bt]) return true;
            }
            for (int m = 0; m < app->actions[i].mouse_button_count; m++) {
                int mb = app->actions[i].mouse_buttons[m];
                if ((app->mouse_state & SDL_BUTTON(mb)) && !(app->prev_mouse_state & SDL_BUTTON(mb))) return true;
            }
            return false;
        }
    }
    return false;
}

bool starlight_is_mouse_pressed(SLF_App* app, int mousebtn) {
    return (app->mouse_state & SDL_BUTTON(mousebtn)) != 0;
}

bool starlight_is_mouse_just_pressed(SLF_App* app, int mousebtn) {
    return ((app->mouse_state & SDL_BUTTON(mousebtn)) != 0) && ((app->prev_mouse_state & SDL_BUTTON(mousebtn)) == 0);
}

// --- Framework Overhaul: Shader Hot-Reload ---
#include <sys/stat.h>

typedef struct {
    GLuint* target;
    char vert_path[128];
    char frag_path[128];
    long long last_mod_vert;
    long long last_mod_frag;
} WatchedShader;

static WatchedShader g_watched_shaders[16];
static int g_watched_count = 0;

static long long get_file_mtime(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) return (long long)st.st_mtime;
    return 0;
}

GLuint starlight_shader_load_watched(SLF_App* app, const char* vert_path, const char* frag_path, GLuint* target_ptr) {
    (void)app;
    GLuint shader = shader_load_program(vert_path, frag_path);
    if (g_watched_count < 16 && shader && target_ptr) {
        WatchedShader* ws = &g_watched_shaders[g_watched_count++];
        ws->target = target_ptr; 
        *target_ptr = shader;
        snprintf(ws->vert_path, 128, "%s", vert_path);
        snprintf(ws->frag_path, 128, "%s", frag_path);
        ws->last_mod_vert = get_file_mtime(vert_path);
        ws->last_mod_frag = get_file_mtime(frag_path);
        printf("[HOTRELOAD] Watching shader: %s + %s\n", vert_path, frag_path);
    } else if (target_ptr) {
        *target_ptr = shader;
    }
    return shader;
}

void starlight_shader_check_reload(SLF_App* app) {
    (void)app;
    for (int i = 0; i < g_watched_count; i++) {
        WatchedShader* ws = &g_watched_shaders[i];
        long long mv = get_file_mtime(ws->vert_path);
        long long mf = get_file_mtime(ws->frag_path);
        if (mv != ws->last_mod_vert || mf != ws->last_mod_frag) {
            GLuint new_shader = shader_load_program(ws->vert_path, ws->frag_path);
            if (new_shader) {
                if (ws->target && *(ws->target) != 0) {
                    glDeleteProgram(*(ws->target)); // Destrói o código anterior da GPU
                    *(ws->target) = new_shader;     // Hot-swap transparente pro OpenGL
                }
                printf("[HOTRELOAD] Reloaded shader: %s + %s (GL:%u)\n", ws->vert_path, ws->frag_path, new_shader);
            }
            ws->last_mod_vert = mv;
            ws->last_mod_frag = mf;
        }
    }
}

// --- Framework Overhaul: UI Anchor Layout ---
void starlight_ui_anchor_pos(SLF_App* app, SLF_Anchor anchor, float margin_x, float margin_y, float w, float h, float* out_x, float* out_y) {
    float sw = (float)app->width;
    float sh = (float)app->height;
    switch (anchor) {
        case SLF_ANCHOR_TOP_LEFT:      *out_x = margin_x;                *out_y = margin_y;                break;
        case SLF_ANCHOR_TOP_CENTER:    *out_x = (sw - w)/2 + margin_x;  *out_y = margin_y;                break;
        case SLF_ANCHOR_TOP_RIGHT:     *out_x = sw - w - margin_x;      *out_y = margin_y;                break;
        case SLF_ANCHOR_CENTER_LEFT:   *out_x = margin_x;                *out_y = (sh - h)/2 + margin_y;  break;
        case SLF_ANCHOR_CENTER:        *out_x = (sw - w)/2 + margin_x;  *out_y = (sh - h)/2 + margin_y;  break;
        case SLF_ANCHOR_CENTER_RIGHT:  *out_x = sw - w - margin_x;      *out_y = (sh - h)/2 + margin_y;  break;
        case SLF_ANCHOR_BOTTOM_LEFT:   *out_x = margin_x;                *out_y = sh - h - margin_y;      break;
        case SLF_ANCHOR_BOTTOM_CENTER: *out_x = (sw - w)/2 + margin_x;  *out_y = sh - h - margin_y;      break;
        case SLF_ANCHOR_BOTTOM_RIGHT:  *out_x = sw - w - margin_x;      *out_y = sh - h - margin_y;      break;
    }
}
