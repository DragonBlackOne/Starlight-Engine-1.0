-- ik_solver.lua
-- Two-Bone Analytical Inverse Kinematics & Ground Adaptation for Humanoid Brawlers

local IKSolver = {}

-- Solves a 2-bone chain (e.g. Hip -> Knee -> Foot or Shoulder -> Elbow -> Hand)
function IKSolver.solveTwoBone(root, target, length1, length2, pole)
    pole = pole or { x = 0, y = 0, z = 1 }

    if engine and engine.solve_two_bone_ik then
        local mx, my, mz = engine.solve_two_bone_ik(
            root.x, root.y, root.z,
            target.x, target.y, target.z,
            length1, length2,
            pole.x, pole.y, pole.z
        )
        return { x = mx, y = my, z = mz }
    end

    -- Lua Analytical Fallback
    local dx = target.x - root.x
    local dy = target.y - root.y
    local dz = target.z - root.z
    local dist = math.sqrt(dx * dx + dy * dy + dz * dz)
    dist = math.max(0.001, math.min(dist, (length1 + length2) * 0.999))

    local cosAlpha = (length1 * length1 + dist * dist - length2 * length2) / (2.0 * length1 * dist)
    cosAlpha = math.max(-1.0, math.min(1.0, cosAlpha))
    local sinAlpha = math.sqrt(1.0 - cosAlpha * cosAlpha)

    local dirX, dirY, dirZ = dx / dist, dy / dist, dz / dist
    local dotP = pole.x * dirX + pole.y * dirY + pole.z * dirZ
    local sideX = pole.x - dirX * dotP
    local sideY = pole.y - dirY * dotP
    local sideZ = pole.z - dirZ * dotP
    local sideLen = math.sqrt(sideX * sideX + sideY * sideY + sideZ * sideZ)
    if sideLen < 0.001 then
        sideX, sideY, sideZ = 0, 0, 1
    else
        sideX, sideY, sideZ = sideX / sideLen, sideY / sideLen, sideZ / sideLen
    end

    return {
        x = root.x + dirX * (length1 * cosAlpha) + sideX * (length1 * sinAlpha),
        y = root.y + dirY * (length1 * cosAlpha) + sideY * (length1 * sinAlpha),
        z = root.z + dirZ * (length1 * cosAlpha) + sideZ * (length1 * sinAlpha)
    }
end

-- Adapt humanoid legs to uneven terrain / desert ground
function IKSolver.adaptFootToTerrain(footTargetY, terrainHeight)
    local targetY = math.max(terrainHeight, footTargetY)
    return targetY
end

return IKSolver
