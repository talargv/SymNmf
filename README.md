# SymNMF Clustering Algorithm
## Description
A C-Python command line interface for the SymNMF clustring algorithm, as seen in the [paper by Da, Ding and Park](https://faculty.cc.gatech.edu/~hpark/papers/DaDingParkSDM12.pdf).
The interface is able to run the full SymNMF algorithm, or some components of the full algorithm.
## Usage
The projects includes both a C interface and a Python interface.
### Python Interface
After setting up the Python interface (details below), run the algorithm from the command line in the following format:
```
python symnmf.py k goal filename
```
Where:
- goal - specifies what the code will compute.
  -  **"symnmf"** will compute the full SymNMF algorithm.
  -  **"sym"** will compute the similarity matrix.
  -  **"ddg"** will compute the Diagonal Degree Matrix.
  -  **"norm"** will compute the normalized similarity matrix.
- k - number of clusters (must be strictly less than the number of data points).
- filename - a path to a txt file containing the data. The data should be represented as lines of floats seperated by commas, and the last line in the file is empty.

### C Interface
After compiling the C code (details below), run the algorithm from the command line in the following format:
```
my_app goal filename
```
Where:

- goal - specifies what the code will compute.
  -  **"sym"** will compute the similarity matrix.
  -  **"ddg"** will compute the Diagonal Degree Matrix.
  -  **"norm"** will compute the normalized similarity matrix.
- filename - a path to a txt file containing the data. The data should be represented as lines of floats seperated by commas, and the last line in the file is empty.
## Setup
### Python Setup
Build using ```python3 setup.py build_ext --inplace```

### C Setup
Compile using ```make```
