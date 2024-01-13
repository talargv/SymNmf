import sys
import symnmf_module as sn

import numpy as np

k = int(sys.argv[1])
goal = sys.argv[2]
filename = sys.argv[3]

data = []

with open(filename, 'r') as F:
    S = F.readline()
    while S != "":
        data.append(S.rstrip().split(','))
        S = F.readline()
    
data = [[float(d) for d in row] for row in data]
    
if goal == 'symnmf':
    W = sn.norm(data, len(data), len(data[0]))
    tmp = np.array(W)
    m = np.sum(tmp)/(tmp.shape[0]*tmp.shape[1])
    np.random.seed(0)
    H = np.random.uniform(size=(len(W),k),high=2*((m/k)**0.5)).tolist()
    res = sn.symnmf(H, len(H), k, W)
    
elif goal == 'sym':
    res = sn.sym(data, len(data), len(data[0]))
    
elif goal == 'ddg':
    res = sn.ddg(data, len(data), len(data[0]))

elif goal == 'norm':
    res = sn.norm(data, len(data), len(data[0]))
    
else:
    print("An error has occurred")
    res = []
    
if len(res) > 0:
    for i in range(len(res)-1):
        curr_point = ""
        for j in range(len(res[i])-1):
            curr_point += "{c:.4f}".format(c=res[i][j]) + ","
        curr_point += "{c:.4f}".format(c=res[i][len(res[i])-1])
        print(curr_point)
    curr_point = ""
    for j in range(len(res[0])-1):
        curr_point += "{c:.4f}".format(c=res[-1][j]) + ","
    curr_point += "{c:.4f}".format(c=res[-1][-1])
    print(curr_point)
   


