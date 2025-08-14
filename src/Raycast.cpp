#include "Raycast.hpp"

#include "Block.hpp"
#include "GameState.hpp"

std::optional<RaycastResult> RaycastBlocks(glm::vec3 pos, glm::vec3 dir, float maxDistance) {

    // Implementation of the 3D DDA (Digital Differential Analyzer) algorithm, provided by claude.ai, with minor adjustments

    // Normalize direction (optional, but helps with distance calculation)
    float dirLength = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (dirLength == 0) return std::nullopt;
    
    dir.x /= dirLength;
    dir.y /= dirLength;
    dir.z /= dirLength;
    
    // Current voxel coordinates
    int x = static_cast<int>(std::round(pos.x));
    int y = static_cast<int>(std::round(pos.y));
    int z = static_cast<int>(std::round(pos.z));
    
    // Direction to step in (1 or -1)
    int stepX = (dir.x > 0) ? 1 : -1;
    int stepY = (dir.y > 0) ? 1 : -1;
    int stepZ = (dir.z > 0) ? 1 : -1;
    
    // Distance along ray to next voxel boundary
    float nextVoxelBoundaryX = (float) ((stepX > 0) ? (x + 1) : x);
    float nextVoxelBoundaryY = (float) ((stepY > 0) ? (y + 1) : y);
    float nextVoxelBoundaryZ = (float) ((stepZ > 0) ? (z + 1) : z);
    
    // Distance along ray to reach next voxel boundary
    float tMaxX = (dir.x != 0) ? (nextVoxelBoundaryX - pos.x) / dir.x : INFINITY;
    float tMaxY = (dir.y != 0) ? (nextVoxelBoundaryY - pos.y) / dir.y : INFINITY;
    float tMaxZ = (dir.z != 0) ? (nextVoxelBoundaryZ - pos.z) / dir.z : INFINITY;
    
    // Distance along ray between voxel boundaries
    float tDeltaX = (dir.x != 0) ? stepX / dir.x : INFINITY;
    float tDeltaY = (dir.y != 0) ? stepY / dir.y : INFINITY;
    float tDeltaZ = (dir.z != 0) ? stepZ / dir.z : INFINITY;
    
    // Track which face was hit
    int hitFaceX = 0, hitFaceY = 0, hitFaceZ = 0;
    
    // Step through voxels
    while (true) {
        // Check if current voxel contains a block
        BlockIndex currentPos = std::make_tuple(x, y, z);
        auto it = blocks.find(currentPos);
        if (it != blocks.end() && it->second != nullptr) {
            // Found a block! Calculate distance
            float currentDistance;
            if (tMaxX - tDeltaX < tMaxY - tDeltaY && tMaxX - tDeltaX < tMaxZ - tDeltaZ) {
                currentDistance = tMaxX - tDeltaX;
            } else if (tMaxY - tDeltaY < tMaxZ - tDeltaZ) {
                currentDistance = tMaxY - tDeltaY;
            } else {
                currentDistance = tMaxZ - tDeltaZ;
            }
            
            return RaycastResult{
                currentPos,
                glm::vec3(hitFaceX, hitFaceY, hitFaceZ),
                currentDistance
            };
        }
        
        // Move to next voxel
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            if (tMaxX > maxDistance) break;
            x += stepX;
            tMaxX += tDeltaX;
            hitFaceX = -stepX;
            hitFaceY = 0;
            hitFaceZ = 0;
        } else if (tMaxY < tMaxZ) {
            if (tMaxY > maxDistance) break;
            y += stepY;
            tMaxY += tDeltaY;
            hitFaceX = 0;
            hitFaceY = -stepY;
            hitFaceZ = 0;
        } else {
            if (tMaxZ > maxDistance) break;
            z += stepZ;
            tMaxZ += tDeltaZ;
            hitFaceX = 0;
            hitFaceY = 0;
            hitFaceZ = -stepZ;
        }
    }
    
    return std::nullopt; // No block found within max distance
}
