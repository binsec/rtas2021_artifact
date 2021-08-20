/******************************************************************************* 
 *                                   BSD LICENSE 
 *******************************************************************************
 *  Copyright (c) 2006, CEA, INRIA and Universite Paris Sud
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without modification,  
 *  are permitted provided that the following conditions are met: 
 *  
 *   - Redistributions of source code must retain the above copyright notice, this  
 *     list of conditions and the following disclaimer.  
 *  
 *   - Redistributions in binary form must reproduce the above copyright notice,  
 *     this list of conditions and the following disclaimer in the documentation  
 *     and/or other materials provided with the distribution.  
 *  
 *   - Neither the name of CEA, INRIA and Universite Paris Sud nor the names of its 
 *     contributors may be used to endorse or promote products derived from this 
 *     software without specific prior written permission.  
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 *  DISCLAIMED.  
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY  
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *  
 ****************************************************************************** */ 

/***************************************************************************
                       fsc.h  -  FraternitéSysC header
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#ifndef UNISIM_GRAPH_H
#define UNISIM_GRAPH_H

template <class T>
class Graph;


/**
 * \brief A vertex of the Graph class
 */
template <class T>
class Vertex
{public:
  Vertex();
  Vertex(T *data);
  virtual ~Vertex();

  void operator () (Vertex<T> *succ);
  List<Vertex<T> >& Successors();
  T *Data();
  /**
   * \brief Pretty printer of the Vertex class
   */
  friend ostream& operator << (ostream& os, const Vertex<T>& v)
  { os << v.id;
    return os;
  }
 private:
  int id;                       ///< Unique vertex ID
  T *data;                      ///< Data stored in the vertex
  List<Vertex<T> > successors;  ///< Successors of the vertex

  friend class Graph<T>;
};

/**
 * \brief Creates a dataless vertex
 */
template <class T>
Vertex<T>::Vertex()
{ this->data = 0;
}

/**
 * \brief Creates a vertex with data
 */
template <class T>
Vertex<T>::Vertex(T *data)
{ this->data = data;
}

/**
 * \brief Destructor of the Vertex class
 */
template <class T>
Vertex<T>::~Vertex()
{}

/**
 * \brief Add a new Vertex to the successor list
 */
template <class T>
void Vertex<T>::operator () (Vertex<T> *succ)
{ successors << succ;
}

/**
 * \brief Returns the data of a vertex
 */
template <class T>
T *Vertex<T>::Data()
{ return data;
}

/**
 * Returns the list of successor vertices of the vertex
 */
template <class T>
List<Vertex<T> >& Vertex<T>::Successors()
{ return successors;
}

/**
 * \brief Store a schedule of process wake-ups
 */
template <class T>
class Schedule
{public:
  Graph<T> *head;          ///< Cuurent graph of the schedule
  Schedule<T> *tail;       ///< Rameining graphs
};


/**
 * Class corresponding to a Graph composed of vertices
 */
template <class T>
class Graph
{public:
  Graph();
  Graph(const Graph<T>& graph);
  Graph(Graph<T>& graph, List<Vertex<T> >& vertices);
  virtual ~Graph();

  void Dotify(const char *filename);
  void DotifyNumber(const char *filename);

  Graph<T>& operator << (Vertex<T> *vertex);
  List<Graph<T> > *SCCs();
  Graph<T>& operator -= (Vertex<T> *vertex);
  int Card() const;
//  void Number();
  List<Vertex<T> >& Vertices();                        // Returns the vertices composing the graph
  List<List<Vertex<T> > > *Topological();
  List<Vertex<T> > *DepthFirstSearch(Vertex<T> *u);
  void MakeAcyclic();
 private:
  typedef Vertex<T> *PVertex;
  List<Vertex<T> > *vertices_list;
  List<Vertex<T> > vertices;
  int next_id;
  static int counter;
  static int sp;
  static Vertex<T> **stack;
  static int *number;
  static List<List<Vertex<T> > > *scc_list;
  int SCC(Vertex<T> *vertex);
  static List<List<Vertex<T> > > *topological;
  static bool *visited;
  static bool *matrix;
  static Vertex<T> **lookup;
  int Topological(Vertex<T> *u);
  bool Renumber(Vertex<T> *u);
  void DFS(Vertex<T> *u);
  void MakeAcyclic(Vertex<T> *u);
};

template <class T>
int Graph<T>::counter;

template <class T>
int Graph<T>::sp;

template <class T>
Vertex<T> **Graph<T>::stack;

template <class T>
int *Graph<T>::number;

template <class T>
List<List<Vertex<T> > > *Graph<T>::scc_list;

template <class T>
List<List<Vertex<T> > > *Graph<T>::topological;

template <class T>
bool *Graph<T>::visited;

template <class T>
bool *Graph<T>::matrix;

template <class T>
Vertex<T> **Graph<T>::lookup;

/**
 * \brief Creates an empty graph
 */
template <class T>
Graph<T>::Graph()
{ next_id = 0;
}

/**
 * \brief Copy contructor of the Graph class
 */
template <class T>
Graph<T>::Graph(const Graph<T>& graph)
{ ListPointer<Vertex<T> > src, dst;
  Vertex<T> *lookup[Card(graph)];
  next_id = 0;

  for(src = graph.vertices; src; src++)
  { Vertex<T> *vertex = new Vertex<T>(src.data);
    vertices << vertex;
    vertex->id = next_id++;
    lookup[vertex->id] = vertex;
  }
  for(src = graph.vertices; src; src++)
  { for(dst = src->successors; dst; dst++)
    { (*lookup[src->id])(*(lookup[dst->id]));
    }
  }
}

/**
 * \brief Creates a new graph from an existing gaph and some vertices
 */
template <class T>
Graph<T>::Graph(Graph<T>& graph, List<Vertex<T> >& vertices)
{ ListPointer<Vertex<T> > src, dst, v;
  Vertex<T> **lookup = new PVertex[graph.Card()];
  next_id = 0;

  memset(lookup, 0, sizeof(Vertex<T> *) * graph.Card());
  for(src = vertices.Begin(); src; src++)
  { Vertex<T> *vertex = new Vertex<T>(src->data);
    this->vertices << vertex;
    vertex->id = src->id;
    lookup[vertex->id] = vertex;
  }
  for(src = vertices.Begin(); src; src++)
  { for(dst = src->successors.Begin(); dst; dst++)
    { if(lookup[dst->id])
      { (*lookup[src->id])(lookup[dst->id]);
      }
    }
  }
  for(v = this->vertices.Begin(); v; v++)
  { v->id = next_id++;
  }

  delete[] lookup;
}

/**
 * \brief Dump a dot (graphviz) file out od the graph
 */
template <class T>
void Graph<T>::Dotify(const char *filename)
{ ofstream os(filename);

  os << "digraph G {" << endl;
  //	os << "page=\"8.25,11.75\"" << endl;
  os << "size=\"24,32\"" << endl;
  //	os << "splines=false" << endl;

  ListPointer<Vertex<T> > v, w;

  for(v = vertices.Begin(); v; v++)
  { os << "\tsignal" << v->id << " [label=\"" << v->data->Name() /* v->id */<< "\"]" << endl;
  }

  for(v = vertices.Begin(); v; v++)
  { for(w = v->successors.Begin(); w; w++)
    { os << "\tsignal" << v->id << " -> signal" << w->id << endl;
    }
  }
  os << "}" << endl;

}

/**
 * \brief Dump a dot (graphviz) file out od the graph, with additional info
 */
template <class T>
void Graph<T>::DotifyNumber(const char *filename)
{ ofstream os(filename);

  os << "digraph G {" << endl;
  //	os << "page=\"8.25,11.75\"" << endl;
  //	os << "size=\"24,32\"" << endl;
  //	os << "splines=false" << endl;

  ListPointer<Vertex<T> > v, w;

  for(v = vertices.Begin(); v; v++)
  { os << "\tsignal" << v->id << " [label=\"" << v->data->Name()/* v->id*/ << ":" << number[v->id] << "\"]" << endl;
  }

  for(v = vertices.Begin(); v; v++)
  { for(w = v->successors.Begin(); w; w++)
    { os << "\tsignal" << v->id << " -> signal" << w->id << endl;
    }
  }
  os << "}" << endl;
}

/**
 * \brief Destructor of the Graph class
 */
template <class T>
Graph<T>::~Graph()
{ ListPointer<Vertex<T> > v;
  v = vertices.Begin();

  while(v)
  { delete v;
    vertices.Remove(v);
  }
}

/**
 * \brief Add a new vertex to the graph vertices
 */
template <class T>
Graph<T>& Graph<T>::operator << (Vertex<T> *vertex)
{ vertices << vertex;
  vertex->id = next_id++;
  return *this;
}

/**
 * \brief Compute the SCC of a vertex
 */
template <class T>
int Graph<T>::SCC(Vertex<T> *vertex)
{ int oldest = number[vertex->id] = ++counter;
  stack[sp] = vertex;
  sp++;

  ListPointer<Vertex<T> > succ;
  for(succ = vertex->successors.Begin(); succ; succ++)
  { if(number[succ->id] == 0)
    { int n = SCC(succ);
      if(n < oldest) oldest = n;
    }
    else
    { if(number[succ->id] < oldest)
      { oldest = number[succ->id];
      }
    }
  }

  if(oldest == number[vertex->id])
  { List<Vertex<T> > *scc = new List<Vertex<T> >();
    do
    { sp--;
      Vertex<T> *v = stack[sp];
      (*scc) << v;
      number[v->id] = (int)((unsigned int) -1 >> 1);
    }
    while(stack[sp] != vertex);
    (*scc_list) << scc;
  }
  return oldest;
}

/**
 * \brief Compute all the SCC of the graph
 */
template <class T>
List<Graph<T> > *Graph<T>::SCCs()
{ int nvertices = Card();
  Vertex<T> **stack = new PVertex[nvertices];
  sp = 0;
  this->stack = stack;
  int number[nvertices];
  this->number = number;
  memset(number, 0, sizeof(number));
  counter = 0;
  List<List<Vertex<T> > > scc_list;
  this->scc_list = &scc_list;

  List<Graph<T> > *scc_graph_list = new List<Graph<T> >();
  ListPointer<Vertex<T> > vertex;
  for(vertex = vertices.Begin(); vertex; vertex++)
  { if(number[vertex->id] == 0)
    { SCC(vertex);
    }
  }

  ListPointer<List<Vertex<T> > > scc;
  for(scc = scc_list.Begin(); scc; scc++)
  { //		cerr << *scc << endl;
    Graph<T> *g = new Graph<T>(*this, *scc);
    (*scc_graph_list) << g;
  }

  for(scc = scc_list.Begin(); scc;)
  { delete scc;
    scc_list.Remove(scc);
  }

  delete[] stack;
  return scc_graph_list;
}

/**
 * \brief Remove a vertex from the graph vertices
 */
template <class T>
Graph<T>& Graph<T>::operator -= (Vertex<T> *vertex)
{ ListPointer<Vertex<T> > src, dst;

  next_id = 0;
  src = vertices.Begin();
  if(src)
  { do
    { if(src == vertex)
      { vertices.Remove(src);
      }
      else
      { src->id = next_id++;
        dst = src->successors;

        if(dst)
        { do
          { if(dst == vertex)
            { src->successors.Remove(dst);
            }
            else
            { dst++;
            }
          }
          while(dst);
        }
        src++;
      }
    }
    while(src);
  }
}


/**
 * \brief Returns the number of vertices in the graph
 */
template <class T>
int Graph<T>::Card() const
{ return vertices.Size();
}

/**
 * \brief Returns the vertices composing the graph
 */
template <class T>
List<Vertex<T> >& Graph<T>::Vertices()
{ return vertices;
}

/**
 * \brief Add a topological weight to a vertex of the graph
 */
template <class T>
int Graph<T>::Topological(Vertex<T> *u)
{ ListPointer<Vertex<T> > v;
  visited[u->id] = true;

  int max = -1;
  v = u->successors.Begin();
  if(v)
  { do
    { if(!visited[v->id])
      { int n = Topological(lookup[v->id]);
        if(n > max) max = n;
      }
      if(number[v->id] > max) max = number[v->id];
    }
    while(++v);
    number[u->id] = max + 1;
  }
  return number[u->id];
}

template <class T>
void Graph<T>::DFS(Vertex<T> *u)
{ visited[u->id] = true;
  (*vertices_list) << u;

  ListPointer<Vertex<T> > v;
  for(v = u->successors.Begin(); v; v++)
  { if(!visited[v->id])
    { DFS(v);
    }
  }
}

template <class T>
List<Vertex<T> > *Graph<T>::DepthFirstSearch(Vertex<T> *u)
{ int nvertices = vertices.Size();
  vertices_list = new List<Vertex<T> >();
  memset(visited, false, nvertices);
  DFS(u);
  return vertices_list;
}

template <class T>
bool Graph<T>::Renumber(Vertex<T> *u)
{ ListPointer<Vertex<T> > v;

  bool r = false;
  v = u->successors.Begin();
  if(v)
  { do
    { if(number[v->id] != number[u->id] - 1)
      { r = true;
      }
      if(number[v->id] < number[u->id] - 1)
        number[v->id] = number[u->id] - 1;
      r = Renumber(v) || r;
    }
    while(++v);
  }
  return r;
}

/**
 * \brief Returns a topological sort of graph vertices
 */
template <class T>
List<List<Vertex<T> > > *Graph<T>::Topological()
{ int i;
  int nvertices = Card();
  bool is_source[nvertices];
  memset(is_source, true, nvertices * sizeof(bool));
  bool visited[nvertices];
  memset(visited, false, nvertices * sizeof(bool));
  this->visited = visited;
  Vertex<T> **lookup = new PVertex[nvertices];
  int number[nvertices];
  memset(number, 0, sizeof(number));
  this->number = number;

  ListPointer<Vertex<T> > u, v, w;

  for(v = vertices.Begin(); v; v++)
  { lookup[v->id] = v;
  }

  this->lookup = lookup;

  for(v = vertices.Begin(); v; v++)
  { for(w = v->successors.Begin(); w; w++)
    { is_source[w->id] = false;
    }
  }

  int rank = -1;
  bool do_rank;
  memset(visited, false, nvertices * sizeof(bool));
  do
  { do_rank = false;
    rank++;
    for(u = vertices.Begin(); u; u++)
    { if(number[u->id] == rank)
      { for(v = u->successors.Begin(); v; v++)
        { number[v->id] = rank+1;
          do_rank = true;
        }
      }
    }
  }
  while(do_rank);

  List<List<Vertex<T> > > *topological = new List<List<Vertex<T> > >();
  for(i = 0; i <= rank; i++)
  { List<Vertex<T> > *l = new List<Vertex<T> >();
    (*topological) << l;
    for(u = vertices.Begin(); u; u++)
    { if(number[u->id] == i)
      { (*l) << u;
      }
    }
  }

  //	DotifyNumber("graphn.dot");
  delete[] lookup;
  return topological;
}

/**
 * \brief Makes a particular vertex acyclic
 */
template <class T>
void Graph<T>::MakeAcyclic(Vertex<T> *u)
{ visited[u->id] = true;
  ListPointer<Vertex<T> > v;
  for(v = u->successors.Begin(); v;)
  { if(!visited[v->id])
    { MakeAcyclic(v);
      v++;
    }
    else
    { u->successors.Remove(v);
    }
  }
}


/**
 * \brief Makes the graph acyclic
 */
template <class T>
void Graph<T>::MakeAcyclic()
{ int nvertices = vertices.Size();
  bool visited[nvertices];
  memset(visited, false, nvertices * sizeof(bool));
  this->visited = visited;
  ListPointer<Vertex<T> > u;
  for(u = vertices.Begin(); u; u++)
  {
    if(!visited[u->id])
      MakeAcyclic(u);
  }
}

#endif
