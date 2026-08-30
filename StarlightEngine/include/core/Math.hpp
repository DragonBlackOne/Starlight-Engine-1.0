#pragma once

// ============================================================================
// core/Math.hpp - unified entry point for the Module 1 math layer.
// ----------------------------------------------------------------------------
// glm is the canonical type source (vec/mat/quat). This module adds a light
// SIMD layer (SIMDUtils.hpp) with runtime ISA dispatch and SSE/AVX2 accelerated
// matrix/quaternion helpers, plus ergonomic aliases so engine code can write
// `math::vec3`, `math::mat4`, `math::quat` without repeating namespaces.
// ============================================================================

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/SIMDUtils.hpp"

namespace starlight::math {

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3;
using mat4 = glm::mat4;
using quat = glm::quat;

}  // namespace starlight::math
