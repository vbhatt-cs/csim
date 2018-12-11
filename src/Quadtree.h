#pragma once

// quadtree allowing duplicate points
// (0,0) = lower left corner
// requires Point to implement Coor getX(), Coor getY()

#include "Global.h"
#include <array>

#define DEBUG_QT 0

#if DEBUG_QT
#define DEBUG_QT_PRINT(x) { std::cout << x << std::endl; }
#else
#define DEBUG_QT_PRINT(x)
#endif

// half-open rectangle [xMin, xMax) x [yMin, yMax)
template <typename Coor>
struct Rectangle
{
  Coor xMin, xMax, yMin, yMax;
};

template <typename Coor>
std::ostream &operator<<(std::ostream &os, const Rectangle<Coor> &r)
{
  os << "[ " << r.xMin << " " << r.xMax << " " << r.yMin << " " << r.yMax << " ]";
  return os;
}

template <typename Point>
class Quadtree
{
public:

  using Coor = decltype(((Point*)nullptr)->getX());
  static_assert(std::is_same<Coor, decltype(((Point*)nullptr)->getY())>(),
                "getX/getY don't match");

  using Rect = Rectangle<Coor>;
  
private:

  static bool intersect(const Rect &r1, const Rect &r2)
  {
    if (r2.xMax <= r1.xMin) { return false; }
    if (r2.xMin >= r1.xMax) { return false; }
    if (r2.yMax <= r1.yMin) { return false; }
    if (r2.yMin >= r1.yMax) { return false; }
    return true;
  }

  static bool inside(Coor x, Coor y, const Rect &r)
  {
    if (x <  r.xMin) { return false; }
    if (x >= r.xMax) { return false; }
    if (y <  r.yMin) { return false; }
    if (y >= r.yMax) { return false; }
    return true;
  }
  
  struct Node
  {
    Node(const Rect &rect_)
      : rect(rect_)
    {
      children = { nullptr, };
    }

    ~Node()
    {
      for (auto p: children) {
        delete p;
      }
    }

    bool isLeaf() const
    {
      return children[0] == nullptr;
    }
    
    Rect rect;              // area covered by node
    Rect pointsBB;          // leaf point bounding box (split if dim exceeds eps)
    std::vector<const Point*> leafPoints; // points stored in this leaf node
    std::array<Node *, 4> children;
  };
  
public:

  // coordinates [0..width_), [0..height_) accepted
  // eps >= 0: split node if point BB width or height exceeds eps

  Quadtree()
  {
    width = height = 0;
    eps = 1;
    root = nullptr;
  }

  ~Quadtree()
  {
    clear();
    delete root;
  }

  void clear()
  {
    delete root;
    root = new Node({ 0, width, 0, height });
  }
  
  void setup(Coor width_, Coor height_, Coor eps_)
  {
    assert(eps_ > 0);
    width = width_;
    height = height_;
    eps = eps_;
    root = new Node({ 0, width, 0, height });
  }

  static bool useQt(Coor eps) { return eps > 0; }
  
  void insert(const Point *pp)
  {
#if DEBUG_QT    
    std::cout << "root insert " << pp->getX() << " " << pp->getY() << std::endl;
#endif
    
    insert(root, pp);

#ifndef NDEBUG
    // std::cout << "test query" << " " << *pp << std::endl;
    std::vector<const Point*> hits;
    query(pp->getX()-1, pp->getX()+1, pp->getY()-1, pp->getY()+1, hits);
    bool found = false;
    for (auto p : hits) {
      if (p == pp) {
        found = true;
        break;
      }
    }
    if (!found) {
      std::cerr << "point not found after insert" << std::endl;
    }
#endif
    
  }

  void insert(Node *node, const Point *newP)
  {
#if DEBUG_QT    
    std::cout << "insert " << newP->getX() << " " << newP->getY() << " ";
    std::cout << node->rect << std::endl;
#endif

    assert(newP->getX() >= node->rect.xMin && newP->getX() < node->rect.xMax &&
           newP->getY() >= node->rect.yMin && newP->getY() < node->rect.yMax);

    if (!node->isLeaf()) {

      // recurse
      int dx = newP->getX() >= node->children[1]->rect.xMin; // SE
      int dy = newP->getY() >= node->children[2]->rect.yMin; // NW
      insert(node->children[dy*2+dx], newP);
      return;
    }

    // leaf node
    
    if (node->leafPoints.empty()) {

      // first point
      node->leafPoints.push_back(newP);
      node->pointsBB = { newP->getX(), newP->getX(), newP->getY(), newP->getY() };
      return;
    }

    // update bounding box
    
    Rect &newBB(node->pointsBB);
    newBB.xMin = std::min(newP->getX(), newBB.xMin);
    newBB.xMax = std::max(newP->getX(), newBB.xMax);
    newBB.yMin = std::min(newP->getY(), newBB.yMin);
    newBB.yMax = std::max(newP->getY(), newBB.yMax);

    // add new point
    
    node->leafPoints.push_back(newP);

    // all points in eps-square => done

    /*
    std::cout << "inside test " << eps << " "
              << (newBB.xMax - newBB.xMin) << " "
              << (newBB.yMax - newBB.yMin) 
              << std::endl;
    */
    
    if (newBB.xMax - newBB.xMin <= eps &&
        newBB.yMax - newBB.yMin <= eps) {

      // std::cout << "inside" << std::endl;
      return;
    }

    // otherwise: split

    std::array<Rect, 4> quarters;
    Rect &r = node->rect;
    
    Coor xMid = r.xMin + (r.xMax - r.xMin) / static_cast<Coor>(2);
    Coor yMid = r.yMin + (r.yMax - r.yMin) / static_cast<Coor>(2);
    
    quarters[0] = Rect{ r.xMin, xMid,   r.yMin, yMid   }; // SW
    quarters[1] = Rect{ xMid,   r.xMax, r.yMin, yMid   }; // SE
    quarters[2] = Rect{ r.xMin, xMid,   yMid,   r.yMax }; // NW
    quarters[3] = Rect{ xMid,   r.xMax, yMid,   r.yMax }; // NE

#if DEBUG_QT
    std::cout << "split " << r << " ";
    for (int i=0; i < 4; ++i) {
      std::cout << quarters[i] << " ";
    }
    std::cout << std::endl;
#endif    

    // create children
    
    for (int i=0; i < 4; ++i) {
      node->children[i] = new Node(quarters[i]);
    }

    // push all points down
    
    for (auto pp : node->leafPoints) {
      int dx = pp->getX() >= xMid;
      int dy = pp->getY() >= yMid;
      insert(node->children[dy*2+dx], pp);
    }

    node->leafPoints.clear();
  }
  
  bool remove(const Point *pp)
  {
    ERR("remove not tested");
    
    return remove(root, pp);
  }

  bool remove(Node *node, const Point *pp)
  {
    if (node->containsPoints()) {

      // search and remove point
      
      for (auto &oldP : node->leafPoints) {
        if (*pp == *oldP) {
          // found: erase
          // todo: how about bounding box - may be invalid ...
          node->leafPoints.erase(&pp);
          return true;
        }
      }

      // not found;
      return false;
    }

    if (node->isLeaf()) {
      // empty leaf => not found
      return false;
    }

    int dx = pp->getX() >= node->children[1]->rect.xMin;
    int dy = pp->getY() >= node->children[2]->rect.yMin;
    
    if (!remove(node->children[dy*2+dx], pp)) {
      return false;
    }

    // remove children if they contain no points
    
    if (node->children[0]->isEmptyLeaf() &&
        node->children[1]->isEmptyLeaf() &&
        node->children[2]->isEmptyLeaf() &&
        node->children[3]->isEmptyLeaf()) {

      for (auto p: node->children) {
        delete p;
      }      
    }

    return true;
  }

  void query(Coor xMin, Coor xMax, Coor yMin, Coor yMax, std::vector<const Point*> &result) const
  {
    Rect rect{ xMin, xMax, yMin, yMax };

    DEBUG_QT_PRINT("query " << rect);

    result.clear();
    return query(root, rect, result);
  }

  void query(Node *node, const Rect &rect, std::vector<const Point*> &result) const
  {
    DEBUG_QT_PRINT("rec query: " << rect << " " << node->rect);
    
    if (!intersect(node->rect, rect)) {
      DEBUG_QT_PRINT("outside");
      return;
    }
    
    if (!node->leafPoints.empty()) {

      DEBUG_QT_PRINT("leaf node with points");
      
      for (const auto p : node->leafPoints) {
        DEBUG_QT_PRINT("check " << *p);
        if (inside(p->getX(), p->getY(), rect)) {
          DEBUG_QT_PRINT("inside");
          result.push_back(p);
        }
      }      

      return;
    }

    DEBUG_QT_PRINT("recurse");
    
    for (Node *p : node->children) {
      if (p) {
        query(p, rect, result);
      }
    }
  }

  static void test();
  
private:
    
  Coor width, height;
  Coor eps;
  Node *root;
};


extern void quadtreeTest();
