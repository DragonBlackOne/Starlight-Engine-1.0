// Este projeto é feito por IA e só o prompt é feito por um humano.
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "gltf_loader.h"
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GLuint gltf_load_texture_from_image(cgltf_image* image, const char* base_path) {
    GLuint tex_id;
    int width, height, channels;
    char full_path[512];
    if (image->uri) {
        const char* last_slash = strrchr(base_path, '/');
        if (!last_slash) last_slash = strrchr(base_path, '\\');
        if (last_slash) {
            int dir_len = (int)(last_slash - base_path + 1);
            snprintf(full_path, sizeof(full_path), "%.*s%s", dir_len, base_path, image->uri);
        } else {
            snprintf(full_path, sizeof(full_path), "%s", image->uri);
        }
        unsigned char* data = stbi_load(full_path, &width, &height, &channels, 0);
        if (!data) {
            printf("[GLTF] Falha ao carregar textura: %s\n", full_path);
            return 0;
        }
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return tex_id;
    }
    return 0;
}

bool gltf_load(const char* filepath, GLTFModel* out_model) {
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, filepath, &data);
    if (result != cgltf_result_success) return false;
    result = cgltf_load_buffers(&options, data, filepath);
    if (result != cgltf_result_success) { cgltf_free(data); return false; }

    // Texturas
    out_model->texture_count = (int)data->images_count;
    out_model->textures = (data->images_count > 0) ? (GLuint*)calloc(data->images_count, sizeof(GLuint)) : NULL;
    for (cgltf_size i = 0; i < data->images_count; i++) out_model->textures[i] = gltf_load_texture_from_image(&data->images[i], filepath);

    // Meshes
    int total_prims = 0;
    for (cgltf_size m = 0; m < data->meshes_count; m++) total_prims += (int)data->meshes[m].primitives_count;
    out_model->meshes = (GLTFMesh*)calloc(total_prims, sizeof(GLTFMesh));
    out_model->mesh_count = 0;

    for (cgltf_size m = 0; m < data->meshes_count; m++) {
        for (cgltf_size p = 0; p < data->meshes[m].primitives_count; p++) {
            cgltf_primitive* prim = &data->meshes[m].primitives[p];
            GLTFMesh* gl_mesh = &out_model->meshes[out_model->mesh_count];
            
            cgltf_accessor *pos=NULL, *norm=NULL, *uv=NULL, *joints=NULL, *weights=NULL;
            for (cgltf_size a = 0; a < prim->attributes_count; a++) {
                if (prim->attributes[a].type == cgltf_attribute_type_position) pos = prim->attributes[a].data;
                if (prim->attributes[a].type == cgltf_attribute_type_normal) norm = prim->attributes[a].data;
                if (prim->attributes[a].type == cgltf_attribute_type_texcoord) uv = prim->attributes[a].data;
                if (prim->attributes[a].type == cgltf_attribute_type_joints) joints = prim->attributes[a].data;
                if (prim->attributes[a].type == cgltf_attribute_type_weights) weights = prim->attributes[a].data;
            }
            if (!pos) continue;

            int vert_count = (int)pos->count;
            // Layout: 3 pos + 3 norm + 2 uv + 4 joints + 4 weights = 16 floats
            float* buffer = (float*)calloc(vert_count * 16, sizeof(float));
            for (int v = 0; v < vert_count; v++) {
                float vp[3]={0}, vn[3]={0,1,0}, vt[2]={0}, vw[4]={0};
                uint32_t vj[4]={0};
                cgltf_accessor_read_float(pos, v, vp, 3);
                if (norm) cgltf_accessor_read_float(norm, v, vn, 3);
                if (uv) cgltf_accessor_read_float(uv, v, vt, 2);
                if (joints) cgltf_accessor_read_uint(joints, v, vj, 4);
                if (weights) cgltf_accessor_read_float(weights, v, vw, 4);

                memcpy(&buffer[v*16+0], vp, 3*4);
                memcpy(&buffer[v*16+3], vn, 3*4);
                memcpy(&buffer[v*16+6], vt, 2*4);
                buffer[v*16+8] = (float)vj[0]; buffer[v*16+9] = (float)vj[1];
                buffer[v*16+10] = (float)vj[2]; buffer[v*16+11] = (float)vj[3];
                memcpy(&buffer[v*16+12], vw, 4*4);
            }

            glGenVertexArrays(1, &gl_mesh->vao); glGenBuffers(1, &gl_mesh->vbo);
            glBindVertexArray(gl_mesh->vao); glBindBuffer(GL_ARRAY_BUFFER, gl_mesh->vbo);
            glBufferData(GL_ARRAY_BUFFER, vert_count * 16 * sizeof(float), buffer, GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16*4, (void*)0);
            glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 16*4, (void*)(3*4));
            glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 16*4, (void*)(6*4));
            glEnableVertexAttribArray(3); glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16*4, (void*)(8*4));
            glEnableVertexAttribArray(4); glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16*4, (void*)(12*4));
            
            if (prim->indices) {
                int idx_count = (int)prim->indices->count;
                unsigned int* indices = (unsigned int*)malloc(idx_count * 4);
                for(int i=0; i<idx_count; i++) indices[i] = (unsigned int)cgltf_accessor_read_index(prim->indices, i);
                glGenBuffers(1, &gl_mesh->ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_mesh->ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_count * 4, indices, GL_STATIC_DRAW);
                free(indices); gl_mesh->index_count = idx_count; gl_mesh->has_indices = true;
            }
            gl_mesh->vertex_count = vert_count; gl_mesh->has_skin = (joints != NULL);
            out_model->mesh_count++; free(buffer);
        }
    }

    // Animations & Skins (Simplificado para este MVP)
    out_model->joint_count = (int)data->nodes_count;
    out_model->joint_matrices = (mat4*)calloc(out_model->joint_count, sizeof(mat4));
    for(int i=0; i<out_model->joint_count; i++) glm_mat4_identity(out_model->joint_matrices[i]);

    cgltf_free(data);
    return true;
}

void gltf_draw(GLTFModel* model) {
    for (int i = 0; i < model->mesh_count; i++) {
        glBindVertexArray(model->meshes[i].vao);
        if (model->meshes[i].has_indices) glDrawElements(GL_TRIANGLES, model->meshes[i].index_count, GL_UNSIGNED_INT, 0);
        else glDrawArrays(GL_TRIANGLES, 0, model->meshes[i].vertex_count);
    }
}

void gltf_destroy(GLTFModel* model) {
    for (int i = 0; i < model->mesh_count; i++) {
        glDeleteVertexArrays(1, &model->meshes[i].vao); glDeleteBuffers(1, &model->meshes[i].vbo);
        if (model->meshes[i].has_indices) glDeleteBuffers(1, &model->meshes[i].ebo);
    }
    free(model->meshes);
    for (int i = 0; i < model->texture_count; i++) if (model->textures[i]) glDeleteTextures(1, &model->textures[i]);
    free(model->textures); free(model->joint_matrices);
}
