const { glMatrix } = require("gl-matrix");

function projectPointOnAxis(point, axis) {
  return (point[0] * axis[0] + point[1] * axis[1]);
}

// Function to get the projection of the AABB onto a given axis
function projectAABB(min, max, axis) {
  const p0 = projectPointOnAxis([min[0], min[1]], axis);
  const p1 = projectPointOnAxis([min[0], max[1]], axis);
  const p2 = projectPointOnAxis([max[0], min[1]], axis);
  const p3 = projectPointOnAxis([max[0], max[1]], axis);
  
  const minProj = Math.min(p0, p1, p2, p3);
  const maxProj = Math.max(p0, p1, p2, p3);
  
  return [minProj, maxProj];
}

// Function to get the projections of the triangle's vertices onto a given axis
function projectTriangle(a, b, c, axis) {
  const p0 = projectPointOnAxis(a, axis);
  const p1 = projectPointOnAxis(b, axis);
  const p2 = projectPointOnAxis(c, axis);
  
  const minProj = Math.min(p0, p1, p2);
  const maxProj = Math.max(p0, p1, p2);
  
  return [minProj, maxProj];
}

// Function to check if two projections on the same axis overlap
function overlap(proj1, proj2) {
  return !(proj1[1] < proj2[0] || proj2[1] < proj1[0]);
}

// Function to check for triangle and AABB intersection
function triangleAABB(a, b, c, min, max) {
  // Check the AABB's edges (axis-aligned axes)
  const axes = [
    [1, 0], // x-axis
    [0, 1]  // y-axis
  ];
  
  // Check triangle's edges for separating axis
  const triangleEdges = [
    [b[0] - a[0], b[1] - a[1]], // edge ab
    [c[0] - b[0], c[1] - b[1]], // edge bc
    [a[0] - c[0], a[1] - c[1]]  // edge ca
  ];
  
  // Test AABB's axes
  for (let i = 0; i < axes.length; i++) {
    const axis = axes[i];
    
    // Project the AABB and triangle onto the axis
    const aabbProj = projectAABB(min, max, axis);
    const triangleProj = projectTriangle(a, b, c, axis);
    
    if (!overlap(aabbProj, triangleProj)) {
      return false; // No overlap, so no intersection
    }
  }
  
  // Test triangle's edges
  for (let i = 0; i < triangleEdges.length; i++) {
    const edge = triangleEdges[i];
    
    // Find a perpendicular axis to the edge
    const axis = [-edge[1], edge[0]]; // Perpendicular to the edge
    
    // Project the AABB and triangle onto this axis
    const aabbProj = projectAABB(min, max, axis);
    const triangleProj = projectTriangle(a, b, c, axis);
    
    if (!overlap(aabbProj, triangleProj)) {
      return false; // No overlap, so no intersection
    }
  }
  
  return true; // No separating axis found, so the triangle and AABB intersect
}

module.exports = {
    triangleAABB: triangleAABB
}
