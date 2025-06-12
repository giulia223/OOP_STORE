# ONLINE STORE
# This project implements an online store using OOP concepts combined with a local database.
## Project developed for the OOP course at FMI UNIBUC.



#### Requirements
- - [ ] Define at least 3-4 classes using composition with your own defined classes; inheritance is not considered here
- [ ] Parameterized constructors for each class
- [ ] For the same (single) class: copy constructor, copy operator=, destructor
<!-- - [ ] For another class: move constructor, move `operator=`, destructor -->
<!-- - [ ] For another class: all five special member functions -->
- [ ] operator<< for all classes for display (std::ostream) using chained calls to operator<<
- [ ] As many const as possible (where applicable) and private functions
- [ ] Implement at least 3 public member functions with non-trivial functionality specific to your chosen theme, with at least 1-2 being more complex
- Not just basic input/output or adding/removing elements from/to a vector
- [ ] Meaningful usage scenario for the defined classes:
- Create objects and call all public member functions in main

### Inheritance Requirements
- [ ] At least one base class and 3 derived classes from the same hierarchy
- [ ] The hierarchy must have a custom base, not derived from a predefined class
- [ ] Virtual (pure) functions called through base class pointers in the class containing the base pointer attribute
- At least one virtual function must be specific to the theme (i.e., not just basic input/output or taken from libraries like draw/update/render)
- Virtual constructors (clone): required, but not considered theme-specific functions
- Virtual display, non-virtual interface
- [ ] Base constructor called from derived class constructors
- [ ] Class with attribute of type pointer to base class with derived types; virtual functions called through base pointer, possibly via non-virtual interface
- [ ] Copy constructor and copy assignment operator= overridden for correct copy/assignment, using copy and swap idiom
- [ ] Use of dynamic_cast/std::dynamic_pointer_cast for meaningful downcasting
- [ ] Smart pointers (recommended, optional)



### Exceptions
- [ ] Custom exception hierarchy derived from std::exception or one of its derivatives; at least 3 distinct exception classes
- Exceptions must handle distinct error categories (e.g., file reading errors with different extensions are considered equivalent)
- [ ] Meaningful usage: e.g., throw in constructor (or function returning an object), try/catch in main
- This hierarchy must be completely independent from the one with virtual functions



### Additional Requirements
- [ ] Use of static functions and attributes
- [ ] STL
- [ ] As many const as possible
- [ ] High-level functions: eliminate excessive getters/setters/low-level functions
- [ ] At least 75-80% of the code must be your own C++ code
- [ ] Final commit: add a new derived class without modifying the existing code, in addition to the 3 already added in the same hierarchy
- The new derived class cannot be a previously deleted one re-added
- The new derived class must be integrated into the existing code (i.e., actually used, not added just for show)
