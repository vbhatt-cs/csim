#include "Global.h"
#include "Quadtree.h"
#include <vector>
#include <algorithm>

using namespace std;


using Coor = double;

struct Point
{
  Coor x, y;

  Coor getX() const { return x; }
  Coor getY() const { return y; }

  bool operator<(const Point &p) const
  {
    if (x < p.x) { return true; }
    return x == p.x && y < p.y;
  }
};

ostream &operator<<(ostream &os, const Point &p)
{
  os << "[ " << p.x << " " << p.y << " ]";
  return os;
}

static double rnd01(RNG &rng)
{
  std::uniform_real_distribution<double> dist(0,1); // range [0,1)
  double r = dist(rng);
  assert(r >= 0 && r < 1);
  return r;
}

static void slow_query(Coor xMin, Coor xMax, Coor yMin, Coor yMax,
                const vector<Point> &points,
                vector<const Point*> &hits)
{
  hits.clear();
  
  for (auto &p : points) {
    if (p.x >= xMin && p.x < xMax && p.y >= yMin && p.y < yMax) {
      hits.push_back(&p);
    }
  }
}


/* create random point set and query instances, and compare QT results with
   brute-force computation results
*/
   
void quadtreeTest()
{
  RNG rng;

  rng.seed(0);

  constexpr int N = 10'000;  // point sets
  constexpr int M = 1'000;   // points
  constexpr int Q = 1'000;   // queries
  constexpr Coor W = 100;
  constexpr Coor H = W/2;
  
  for (int i=0; i < N; ++i) {

    cout << "." << flush; 
    
    Quadtree<Point> qt;
    vector<Point> points;
    
    qt.setup(W+1, H+1, 1);

    rng.seed(i);
    
    for (int j=0; j < M; ++j) {
      Point p{ rnd01(rng) * W, rnd01(rng) * H };
      points.push_back(p);
    }

    // 2nd loop to ensure p addresses won't change
    for (auto &p : points) {
      qt.insert(&p);
    }
    
    for (int j=0; j < Q; ++j) {

      // cout << "test " << i << " " << j << endl;

      // for debugging testing single instance
      // can also set i and j to specific values
      // because rng seeds just depends on them
      bool dbg = false; // (i == 5 && j == 5);

      if (dbg) {
        cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      }
      
      rng.seed(j);
      
      Coor xMin = rnd01(rng) * W;
      Coor xMax = xMin + (rnd01(rng) + 0.1) * W;
      Coor yMin = rnd01(rng) * H;
      Coor yMax = yMin + (rnd01(rng) + 0.1) * H;

      if (dbg) {
        for (auto &p : points) {
          cout << p << endl;
        }
        cout << xMin << " " << xMax << " " << yMin << " " << yMax << endl;
      }
      
      vector<const Point*> hitsSlow;
      slow_query(xMin, xMax, yMin, yMax, points, hitsSlow);

      vector<const Point*> hitsQt;
      qt.query(xMin, xMax, yMin, yMax, hitsQt);

      auto ppComp = [](const Point *a, const Point *b) { return *a < *b; };
      
      sort(begin(hitsSlow), end(hitsSlow), ppComp);
      sort(begin(hitsQt), end(hitsQt), ppComp);

      if (dbg) {
        for (auto &p : hitsSlow) {
          cout << "slow: " << *p << endl;
        }
        for (auto &p : hitsQt) {
          cout << "qt:   " << *p << endl;
        }
      }

      if (hitsSlow.size() != hitsQt.size()) {
        cout << "test " << i << " " << j << " failed" << endl;
        ERR("quadtree test: different hit sizes "
            << hitsSlow.size() << " " << hitsQt.size());
      }

      for (int i=0; i < (int)hitsSlow.size(); ++i) {
        if (hitsSlow[i]->x != hitsQt[i]->x ||
            hitsSlow[i]->y != hitsQt[i]->y) {
          cout << "test " << i << " " << j << " failed" << endl;
          ERR("quadtree test: different points");
        }
      }
    }
  }
}
