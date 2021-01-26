/**********************
Name: Nick Sica
Email: nsica1@umbc.edu
Section:4
*******************/



#include <stdexcept>
#include "maze.h"

Maze::Maze(int ncell) {
  if (ncell < 1) {
    throw std::invalid_argument("Maze::Maze(): ncell must be >= 1");
  }

  _ncell = ncell;
  _maze = new cell_t[_ncell];
}
//Maze destructor
//deletes the memory associated with the array 
Maze::~Maze() {
  // IMPLEMENT DESTRUCTOR
  //deletes the maze object
delete [] _maze;
  
}

void Maze::readFile(std::string fileName) {
  int numCells;
  int cell, n1, n2, n3, n4; 
  std::ifstream dataFile;
  
  dataFile.open(fileName);

  if (dataFile.is_open()) {
    dataFile >> numCells;
    this->reset(numCells);

    for (int i = 0; i < numCells; i++) {
      dataFile >> cell >> n1 >> n2 >> n3 >> n4;
      this->insert(cell_t(cell, n1, n2, n3, n4));
    }
  }
}

int Maze::getNcell() const {
  return _ncell;
}

void Maze::reset(int ncell) {
  if (ncell < 1) {
    throw std::invalid_argument("Maze::reset(): ncell must be >= 1");
  }

  if (_maze != nullptr) {
    delete [] _maze;
  }
  
  _ncell = ncell;
  _maze = new cell_t[_ncell];
}

void Maze::insert(cell_t cell) {
  if (_maze == nullptr) {
    throw std::domain_error("Maze::insert(): attempt to insert into uninitialized Maze object");
  }
  if (cell.cellNum < 0 || cell.cellNum >= _ncell) {
    throw std::invalid_argument("Maze:insert(): invalid cell number");
  }
  _maze[cell.cellNum] = cell;
}
  
cell_t Maze::retrieve(int nc) const {
  if (_maze == nullptr) {
    throw std::domain_error("Maze::retrieve(): attempt to retrieve from an uninitialized Maze object");
  }
  if (nc < 0 || nc >= _ncell) {
    throw std::invalid_argument("Maze:retrieve(): invalid cell index");
  }
  
  return _maze[nc];
}

void Maze::dump() const {

  std::cout << "[*] Dumping the maze cells..." << std::endl;
  
  for (int i = 0; i < _ncell; i++) {
    int nc = _maze[i].cellNum;
    if (nc != i) {
      std::cerr << "Warning: maze cell at index " << i
		<<" has cellNum of " << nc << std::endl;
    }
    std::cout << "Cell " << i << " has neighbor cells: ";
    neighbor_t nbs = _maze[i].neighbors;
    //int n0, n1, n2, n3;
    if ( nbs[0] >= 0 ) std::cout << nbs[0] << " ";
    if ( nbs[1] >= 0 ) std::cout << nbs[1] << " ";
    if ( nbs[2] >= 0 ) std::cout << nbs[2] << " ";
    if ( nbs[3] >= 0 ) std::cout << nbs[3] << " ";
    std::cout << std::endl;
  }
}

std::vector<int> Maze::solve() const {
  // IMPLEMENT THE SOLVE METHOD
  //vector that holds solution
  std::vector<int> solved_path;
  int no_solution = 0;
  //creates a deep copy of the maze
  cell_t *_newMaze = new cell_t[_ncell];
  for(int i = 0; i < _ncell; i++) {
    _newMaze[i].cellNum = _maze[i].cellNum;
    _newMaze[i].neighbors = _maze[i].neighbors;
  }
  //creates a stack and pushed (-1,0) on the bottom
  SolveStack mazeStack = SolveStack();
  src_dest_t sd = src_dest_t(VACANT,0);
  mazeStack.push(sd);
  _newMaze->cellNum = sd.second;
  solved_path.push_back(sd.second);
  //creates new neighbor vector so i can modify the neighbors
  neighbor_t new_nbs = _newMaze[0].neighbors;

 
  if (_maze == nullptr) {
    throw std::domain_error("Maze::solve() attempt to solve an empty maze object");
  }
  else {
    //makes sure that there is a solution or no solution
    while(solved_path.back() != _ncell - 1 || no_solution == VACANT) {
      int curr = _newMaze->cellNum;
      //checks the amount of vacants in the neighbors
      int vacant_count = 0;
      //gets the current pair at the top of the stack
      src_dest_t pair = mazeStack.read();
      int prevCell = pair.first;
      //gets the current neighbor list 
      new_nbs = _newMaze[curr].neighbors;
            
      //gets the vacant count
      for(int i = 0; i <(int) _newMaze->neighbors.size(); i++) {
	if(_newMaze->neighbors.at(i) == VACANT) {
	  vacant_count++;
	}
      }
      //if the vacant count is 4 that means there is no solution, pops everything off the vector
      if(vacant_count == 4) {
	while(!solved_path.empty()) {
	  solved_path.pop_back();
	}
	std::cout << "There is no solution to this maze" << std::endl;
	no_solution = VACANT;
	break;
      }
      //if there is a neighbor 
      else if(vacant_count <= 3) {
	//goes through neighbor vector 
	for(int k = 0; k < (int) _newMaze->neighbors.size(); k++) {
	  //if there is a neighbor it can go to 
	  if( _newMaze->neighbors.at(k) != VACANT && _newMaze->neighbors.at(k) != prevCell) {
	    //gets the next number and creates a pair 
	    int next_num = _newMaze->neighbors.at(k);
	    src_dest_t next(curr, next_num);
	    //pushes pair 
	    mazeStack.push(next); 
	    solved_path.push_back(next.second);
	    //gets the next cell 
	    _newMaze->cellNum = next_num;
	    *_newMaze = _newMaze[next_num];
	    
	    
	    break;
	  }
	  //if the only neighbor is the previous 
	  else if(_newMaze->neighbors.at(k+1) == VACANT && _newMaze->neighbors.at(k+2) == VACANT) {
	    //pop the neighbor 
	    mazeStack.pop();
	    int popped = _newMaze->cellNum;
	    solved_path.pop_back();
	    //gets the previous cell 
	    _newMaze->cellNum = prevCell;
	    new_nbs = _newMaze[prevCell].neighbors; 
	    *_newMaze = _newMaze[prevCell];
	    //changes the neighbor to vacant 
	    for(int l = 0; l < (int)_newMaze->neighbors.size(); l++) {
	      if(new_nbs[l] == popped) {
		new_nbs[l] = VACANT;
	      }
	    }
	    //sets the neighbor to the changed vector 
	    _newMaze[prevCell].neighbors = new_nbs;
	    break;
	  }
	  //continue
	  else {
	    continue;
	      }
	}
      }
    }
    //if there is no solution delete and return 
    if(no_solution == VACANT) {
      delete []_newMaze;
      return solved_path;
    }
    //if solution, print solution found and delete the maze 
    std::cout << "[+] Solution found" << std::endl;
    delete [] _newMaze;
    return solved_path;
  }
}
            
Maze::SolveStack::SolveStack() {
  _stack = nullptr;
}
//Solve Stack destructor
//deletes the memory associated with the Stack object 
Maze::SolveStack::~SolveStack() {
  // IMPLEMENT DESTRUCTOR
  entry_t *currNode = _stack;
  entry_t *temp;
  //goes through stack and deletes each node 
  while(currNode != nullptr) {
    temp = currNode->next;
    delete currNode;
    currNode = temp;
  }
  _stack = nullptr;
  
}

bool Maze::SolveStack::empty() const {
  return _stack == nullptr;
}

//Push(src dest)
//adds a new node at the top of the stack
void Maze::SolveStack::push(src_dest_t src_dest) {
  // IMPLEMENT PUSH METHOD
  entry_t *newEntry = new entry_t(src_dest);
  entry_t *temp = _stack;
  //if empty then sets the first node to the first pair 
  if(empty()) {
    _stack = newEntry;
    _stack->next = nullptr;
    //newEntry->next = nullptr;
      }
  else {
    //goes through the stack and pushes the pair to the top of the stack
    while(temp->next != nullptr) {
      temp = temp->next;
    }
    
    temp->next = newEntry;
    newEntry->next = nullptr;
      
  
  }
    
  //delete newEntry;
  
}

//pop function
//deletes a node from the stack and returns its associated data 
src_dest_t Maze::SolveStack::pop() {
  // IMPLEMENT POP METHOD
  src_dest_t dataPair;
  
  if(empty()){
    throw std::domain_error("Maze::SolveStack::pop() attempt to pop an empty SolveStack object");
}

  entry_t *currNode = _stack;
  entry_t *prevNode = nullptr;
  //goes through stack to the last node and pops it off 
  while(currNode->next != nullptr) {
    prevNode = currNode;
    currNode = currNode->next;
  }
  if(currNode->next == nullptr) {
    dataPair = currNode->src_dest;
    delete currNode;
    currNode = nullptr;
    prevNode->next = nullptr;
    
  }

  return dataPair;
}
  
  
//Read() method
//returns the pair at the node
src_dest_t Maze::SolveStack::read() const {
  // IMPLEMENT READ METHOD
  entry_t *currNode = _stack;
  
  
  if(empty()){
    throw std::domain_error("Maze::SolveStack::read() attempt to read an empty SolveStack object");
  }
  else{
    //goes through the stack and gets the pair of the current node 
    while(currNode->next != nullptr) {
      currNode = currNode->next;
  
    }
    return currNode->src_dest;
  }

  
}
