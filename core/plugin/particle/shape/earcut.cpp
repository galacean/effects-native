//
//  earcut.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/18.
//

#include "earcut.hpp"
#include <math.h>
#include <list>

namespace mn {

struct EarcutNode;

struct EarcutNodeContext {
    int index_base = 0;
    std::list<EarcutNode*> list;
};

struct EarcutNode {
    EarcutNode(EarcutNodeContext* ctx, int i, float x, float y) {
        this->i = i;
        this->x = x;
        this->y = y;
        ctx->list.push_back(this);
    }
    
    ~EarcutNode() {
        if (z) {
            delete z;
        }
    }
    
    int i;
    
    float x;
    float y;
    
    EarcutNode* prev = nullptr;
    EarcutNode* next = nullptr;
    
    float* z = nullptr;
    
    EarcutNode* prev_z = nullptr;
    EarcutNode* next_z = nullptr;
    
    bool steiner = false;
};

static float SignedArea(const std::vector<float>& data, int start, int end, int dim) {
    float sum = 0;
    for (int i = start, j = end - dim; i < end; i += dim) {
        sum += (data[ j ] - data[ i ]) * (data[ i + 1 ] + data[ j + 1 ]);
        j = i;
    }
    return sum;
}

// check if two points are equal
static bool Equals(EarcutNode* p1, EarcutNode* p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

// create a node and optionally link it with previous one (in a circular doubly linked list)
static EarcutNode* InsertNode(EarcutNodeContext* ctx, int i, float x, float y, EarcutNode* last) {
    EarcutNode* p = new EarcutNode(ctx, i, x, y);

    if (!last) {
        p->prev = p;
        p->next = p;
    } else {
        p->next = last->next;
        p->prev = last;
        last->next->prev = p;
        last->next = p;
    }
    return p;
}

static void RemoveNode(EarcutNode* p) {
    p->next->prev = p->prev;
    p->prev->next = p->next;

    if (p->prev_z) {
        p->prev_z->next_z = p->next_z;
    }
    if (p->next_z) {
        p->next_z->prev_z = p->prev_z;
    }
}

// z-order of a point given coords and inverse of the longer side of data bbox
static float ZOrder(float dx, float dy, float min_x, float min_y, float inv_size) {
    // coords are transformed into non-negative 15-bit integer range
    int x = 32767 * (dx - min_x) * inv_size;
    int y = 32767 * (dy - min_y) * inv_size;

    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;

    y = (y | (y << 8)) & 0x00FF00FF;
    y = (y | (y << 4)) & 0x0F0F0F0F;
    y = (y | (y << 2)) & 0x33333333;
    y = (y | (y << 1)) & 0x55555555;

    return x | (y << 1);
}

// Simon Tatham's linked list merge sort algorithm
// http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
static EarcutNode* SortLinked(EarcutNode* list) {
    int i = 0;
    EarcutNode* p = nullptr;
    EarcutNode* q = nullptr;
    EarcutNode* e = nullptr;
    EarcutNode* tail = nullptr;
    int num_merges = 0, p_size = 0, q_size = 0;
    int in_size = 1;

    do {
        p = list;
        list = nullptr;
        tail = nullptr;
        num_merges = 0;

        while (p) {
            num_merges++;
            q = p;
            p_size = 0;
            for (i = 0; i < in_size; i++) {
                p_size++;
                q = q->next_z;
                if (!q) {
                    break;
                }
            }
            q_size = in_size;

            while (p_size > 0 || (q_size > 0 && q)) {
                if (p_size != 0 && (q_size == 0 || !q || p->z <= q->z)) {
                    e = p;
                    p = p->next_z;
                    p_size--;
                } else {
                    e = q;
                    q = q->next_z;
                    q_size--;
                }

                if (tail) {
                    tail->next_z = e;
                } else {
                    list = e;
                }

                e->prev_z = tail;
                tail = e;
            }

            p = q;
        }

        if (tail) {
            tail->next_z = nullptr;
        }
        in_size *= 2;

    } while (num_merges > 1);

    return list;
}

// interlink polygon nodes in z-order
static void IndexCurve(EarcutNode* start, float min_x, float min_y, float inv_size) {
    auto p = start;
    do {
        if (p->z == nullptr) {
            p->z = new float;
            *p->z = ZOrder(p->x, p->y, min_x, min_y, inv_size);
        }
        p->prev_z = p->prev;
        p->next_z = p->next;
        p = p->next;
    } while (p != start);

    p->prev_z->next_z = nullptr;
    p->prev_z = nullptr;

    SortLinked(p);
}

// signed area of a triangle
static float Area(EarcutNode* p, EarcutNode* q, EarcutNode* r) {
    return (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
}

// check if a point lies within a convex triangle
static bool PointInTriangle(float ax, float ay, float bx, float by, float cx, float cy, float px, float py) {
    return (cx - px) * (ay - py) - (ax - px) * (cy - py) >= 0 &&
        (ax - px) * (by - py) - (bx - px) * (ay - py) >= 0 &&
        (bx - px) * (cy - py) - (cx - px) * (by - py) >= 0;
}

static bool IsEarHashed(EarcutNode* ear, float min_x, float min_y, float inv_size) {
    EarcutNode* a = ear->prev;
    EarcutNode* b = ear;
    EarcutNode* c = ear->next;

    if (Area(a, b, c) >= 0) {
        return false;
    } // reflex, can't be an ear

    // triangle bbox; min & max are calculated like this for speed
    float min_tx = a->x < b->x ? (a->x < c->x ? a->x : c->x) : (b->x < c->x ? b->x : c->x),
        min_ty = a->y < b->y ? (a->y < c->y ? a->y : c->y) : (b->y < c->y ? b->y : c->y),
        max_tx = a->x > b->x ? (a->x > c->x ? a->x : c->x) : (b->x > c->x ? b->x : c->x),
        max_ty = a->y > b->y ? (a->y > c->y ? a->y : c->y) : (b->y > c->y ? b->y : c->y);

    // z-order range for the current triangle bbox;
    float min_z = ZOrder(min_tx, min_ty, min_x, min_y, inv_size),
        max_z = ZOrder(max_tx, max_ty, min_x, min_y, inv_size);

    EarcutNode* p = ear->prev_z;
    EarcutNode* n = ear->next_z;

    // look for points inside the triangle in both directions
    while (p && *p->z >= min_z && n && *n->z <= max_z) {
        if (p != ear->prev && p != ear->next &&
            PointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y) &&
            Area(p->prev, p, p->next) >= 0) {
            return false;
        }
        p = p->prev_z;

        if (n != ear->prev && n != ear->next &&
            PointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, n->x, n->y) &&
            Area(n->prev, n, n->next) >= 0) {
            return false;
        }
        n = n->next_z;
    }
    
    // look for remaining points in decreasing z-order
    while (p && *p->z >= min_z) {
        if (p != ear->prev && p != ear->next &&
            PointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y) &&
            Area(p->prev, p, p->next) >= 0) {
            return false;
        }
        p = p->prev_z;
    }

    // look for remaining points in increasing z-order
    while (n && *n->z <= max_z) {
        if (n != ear->prev && n != ear->next &&
            PointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, n->x, n->y) &&
            Area(n->prev, n, n->next) >= 0) {
            return false;
        }
        n = n->next_z;
    }

    return true;
}

// check whether a polygon node forms a valid ear with adjacent nodes
static bool IsEar(EarcutNode* ear) {
    EarcutNode* a = ear->prev;
    EarcutNode* b = ear;
    EarcutNode* c = ear->next;

    if (Area(a, b, c) >= 0) {
        return false;
    } // reflex, can't be an ear

    // now make sure we don't have other points inside the potential ear
    EarcutNode* p = ear->next->next;

    while (p != ear->prev) {
        if (PointInTriangle(a->x, a->y, b->x, b->y, c->x, c->y, p->x, p->y) &&
            Area(p->prev, p, p->next) >= 0) {
            return false;
        }
        p = p->next;
    }

    return true;
}

// eliminate colinear or duplicate points
static EarcutNode* FilterPoints(EarcutNode* start, EarcutNode* end = nullptr) {
    if (!start) {
        return start;
    }
    if (!end) {
        end = start;
    }

    EarcutNode* p = start;
    bool again;
    do {
        again = false;

        if (!p->steiner && (Equals(p, p->next) || Area(p->prev, p, p->next) == 0)) {
            RemoveNode(p);
            p = end = p->prev;
            if (p == p->next) {
                break;
            }
            again = true;

        } else {
            p = p->next;
        }
    } while (again || p != end);

    return end;
}

static int Sign(float num) {
    return num > 0 ? 1 : num < 0 ? -1 : 0;
}

// for collinear points p, q, r, check if point q lies on segment pr
static bool OnSegment(EarcutNode* p, EarcutNode* q, EarcutNode* r) {
    return q->x <= fmax(p->x, r->x) && q->x >= fmin(p->x, r->x) && q->y <= fmax(p->y, r->y) && q->y >= fmin(p->y, r->y);
}

// check if two segments intersect
static bool Intersects(EarcutNode* p1, EarcutNode* q1, EarcutNode* p2, EarcutNode* q2) {
    int o1 = Sign(Area(p1, q1, p2));
    int o2 = Sign(Area(p1, q1, q2));
    int o3 = Sign(Area(p2, q2, p1));
    int o4 = Sign(Area(p2, q2, q1));

    if (o1 != o2 && o3 != o4) {
        return true;
    } // general case

    if (o1 == 0 && OnSegment(p1, p2, q1)) {
        return true;
    } // p1, q1 and p2 are collinear and p2 lies on p1q1
    if (o2 == 0 && OnSegment(p1, q2, q1)) {
        return true;
    } // p1, q1 and q2 are collinear and q2 lies on p1q1
    if (o3 == 0 && OnSegment(p2, p1, q2)) {
        return true;
    } // p2, q2 and p1 are collinear and p1 lies on p2q2
    if (o4 == 0 && OnSegment(p2, q1, q2)) {
        return true;
    } // p2, q2 and q1 are collinear and q1 lies on p2q2

    return false;
}

// check if a polygon diagonal is locally inside the polygon
static bool LocallyInside(EarcutNode* a, EarcutNode* b) {
    return Area(a->prev, a, a->next) < 0 ?
        Area(a, b, a->next) >= 0 && Area(a, a->prev, b) >= 0 :
        Area(a, b, a->prev) < 0 || Area(a, a->next, b) < 0;
}

// check if the middle point of a polygon diagonal is inside the polygon
static bool MiddleInside(EarcutNode*a, EarcutNode* b) {
    EarcutNode* p = a;
    bool inside = false;
    float px = (a->x + b->x) / 2;
    float py = (a->y + b->y) / 2;
    do {
        if (((p->y > py) != (p->next->y > py)) && p->next->y != p->y &&
            (px < (p->next->x - p->x) * (py - p->y) / (p->next->y - p->y) + p->x)) {
            inside = !inside;
        }
        p = p->next;
    } while (p != a);

    return inside;
}

// go through all polygon nodes and cure small local self-intersections
static EarcutNode* CureLocalIntersections(EarcutNodeContext* ctx, EarcutNode* start, std::vector<uint16_t>& triangles, int dim) {
    EarcutNode* p = start;
    do {
        EarcutNode* a = p->prev;
        EarcutNode* b = p->next->next;

        if (!Equals(a, b) && Intersects(a, p, p->next, b) && LocallyInside(a, b) && LocallyInside(b, a)) {

            triangles.push_back(a->i / dim + ctx->index_base);
            triangles.push_back(p->i / dim + ctx->index_base);
            triangles.push_back(b->i / dim + ctx->index_base);

            // remove two nodes involved
            RemoveNode(p);
            RemoveNode(p->next);

            p = start = b;
        }
        p = p->next;
    } while (p != start);

    return FilterPoints(p);
}

// check if a polygon diagonal intersects any polygon segments
static bool IntersectsPolygon(EarcutNode* a, EarcutNode* b) {
    EarcutNode* p = a;
    do {
        if (p->i != a->i && p->next->i != a->i && p->i != b->i && p->next->i != b->i &&
            Intersects(p, p->next, a, b)) {
            return true;
        }
        p = p->next;
    } while (p != a);

    return false;
}

// check if a diagonal between two polygon nodes is valid (lies in polygon interior)
static bool IsValidDiagonal(EarcutNode* a, EarcutNode* b) {
    return a->next->i != b->i && a->prev->i != b->i && !IntersectsPolygon(a, b) && // dones't intersect other edges
        (LocallyInside(a, b) && LocallyInside(b, a) && MiddleInside(a, b) && // locally visible
        (Area(a->prev, a, b->prev) || Area(a, b->prev, b)) || // does not create opposite-facing sectors
        Equals(a, b) && Area(a->prev, a, a->next) > 0 && Area(b->prev, b, b->next) > 0); // special zero-length case
}

// link two polygon vertices with a bridge; if the vertices belong to the same ring, it splits polygon into two;
// if one belongs to the outer ring and another to a hole, it merges it into a single ring
static EarcutNode* SplitPolygon(EarcutNodeContext* ctx, EarcutNode* a, EarcutNode* b) {
    EarcutNode* a2 = new EarcutNode(ctx, a->i, a->x, a->y);
    EarcutNode* b2 = new EarcutNode(ctx, b->i, b->x, b->y);
    EarcutNode* an = a->next;
    EarcutNode* bp = b->prev;

    a->next = b;
    b->prev = a;

    a2->next = an;
    an->prev = a2;

    b2->next = a2;
    a2->prev = b2;

    bp->next = b2;
    b2->prev = bp;

    return b2;
}

static void EarcutLinked(EarcutNodeContext* ctx, EarcutNode* ear, std::vector<uint16_t>& triangles, int dim, float min_x, float min_y, float inv_size, int pass = 0);
// try splitting polygon into two and triangulate them independently
static void SplitEarcut(EarcutNodeContext* ctx, EarcutNode* start, std::vector<uint16_t>& triangles, int dim, float min_x, float min_y, float inv_size) {
    // look for a valid diagonal that divides the polygon into two
    EarcutNode* a = start;
    do {
        EarcutNode* b = a->next->next;
        while (b != a->prev) {
            if (a->i != b->i && IsValidDiagonal(a, b)) {
                // split the polygon in two by the diagonal
                EarcutNode* c = SplitPolygon(ctx, a, b);

                // filter colinear points around the cuts
                a = FilterPoints(a, a->next);
                c = FilterPoints(c, c->next);

                // run earcut on each half
                EarcutLinked(ctx, a, triangles, dim, min_x, min_y, inv_size);
                EarcutLinked(ctx, c, triangles, dim, min_x, min_y, inv_size);
                return;
            }
            b = b->next;
        }
        a = a->next;
    } while (a != start);
}

// create a circular doubly linked list from polygon points in the specified winding order
static EarcutNode* LinkedList(EarcutNodeContext* ctx, const std::vector<float>& data, int start, int end, int dim, bool clockwise) {
    int i;
    EarcutNode* last = nullptr;
    
    if (clockwise == (SignedArea(data, start, end, dim) > 0)) {
        for (i = start; i < end; i += dim) last = InsertNode(ctx, i, data[ i ], data[ i + 1 ], last);
    } else {
        for (i = end - dim; i >= start; i -= dim) last = InsertNode(ctx, i, data[ i ], data[ i + 1 ], last);
    }
    
    if (last && Equals(last, last->next)) {
        RemoveNode(last);
        last = last->next;
    }
    
    return last;
}

static void EarcutLinked(EarcutNodeContext* ctx, EarcutNode* ear, std::vector<uint16_t>& triangles, int dim, float min_x, float min_y, float inv_size, int pass) {
    if (!ear) {
        return;
    }
    
    // interlink polygon nodes in z-order
    if (!pass && inv_size) {
        IndexCurve(ear, min_x, min_y, inv_size);
    }

    EarcutNode* stop = ear;
    EarcutNode* prev = nullptr;
    EarcutNode* next = nullptr;

    // iterate through ears, slicing them one by one
    while (ear->prev != ear->next) {
        prev = ear->prev;
        next = ear->next;

        if (inv_size ? IsEarHashed(ear, min_x, min_y, inv_size) : IsEar(ear)) {
            // cut off the triangle
            triangles.push_back(prev->i / dim + ctx->index_base);
            triangles.push_back(ear->i / dim + ctx->index_base);
            triangles.push_back(next->i / dim + ctx->index_base);

            RemoveNode(ear);

            // skipping the next vertex leads to less sliver triangles
            ear = next->next;
            stop = next->next;

            continue;
        }

        ear = next;

        // if we looped through the whole remaining polygon and can't find any more ears
        if (ear == stop) {
            // try filtering points and slicing again
            if (!pass) {
                EarcutLinked(ctx, FilterPoints(ear), triangles, dim, min_x, min_y, inv_size, 1);

                // if this didn't work, try curing all small self-intersections locally
            } else if (pass == 1) {
                ear = CureLocalIntersections(ctx, FilterPoints(ear), triangles, dim);
                EarcutLinked(ctx, ear, triangles, dim, min_x, min_y, inv_size, 2);

                // as a last resort, try splitting the remaining polygon into two
            } else if (pass == 2) {
                SplitEarcut(ctx, ear, triangles, dim, min_x, min_y, inv_size);
            }

            break;
        }
    }
}

void Earcut::Execute(std::vector<uint16_t>& out, const std::vector<float>& data, int dim, int index_base) {
    EarcutNodeContext* ctx = new EarcutNodeContext;
    ctx->index_base = index_base;
    
    int outer_len = (int) data.size();
    EarcutNode* outer_node = LinkedList(ctx, data, 0, outer_len, dim, true);
    
    if (!outer_node || outer_node->next == outer_node->prev) {
        // todo: leak;
        return;
    }
    
    float min_x = 0, min_y = 0, max_x = 0, max_y = 0, x = 0, y = 0, inv_size = 0;
    
    // if the shape is not too simple, we'll use z-order curve hash later; calculate polygon bbox
    if (data.size() > 80 * dim) {
        min_x = max_x = data[0];
        min_y = max_y = data[1];

        for (int i = dim; i < outer_len; i += dim) {
            x = data[i];
            y = data[i + 1];
            if (x < min_x) {
                min_x = x;
            }
            if (y < min_y) {
                min_y = y;
            }
            if (x > max_x) {
                max_x = x;
            }
            if (y > max_y) {
                max_y = y;
            }
        }

        // minX, minY and invSize are later used to transform coords into integers for z-order calculation
        inv_size = fmax(max_x - min_x, max_y - min_y);
        inv_size = inv_size != 0 ? 1 / inv_size : 0;
    }
    
    EarcutLinked(ctx, outer_node, out, dim, min_x, min_y, inv_size);
    
    for (auto node : ctx->list) {
        delete node;
    }
    delete ctx;
}

}
