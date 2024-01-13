import symnmf_module as sn
from sklearn.metrics import silhouette_score
import numpy as np
import sys

### K means code ### 

class Point():
    def __init__(self,L,cluster=-1):
       self.p = L
       self.dim = len(L)
       self.cluster = cluster
       
       
    def __add__(self,p2):
        assert self.dim == p2.dim  
        return Point([self.p[i]+p2.p[i] for i in range(self.dim)])

    def dist(self,p2):
        assert self.dim == p2.dim
        sum = 0
        for i in range(p2.dim):
            sum += (self.p[i]-p2.p[i])**2
        return sum**0.5
    
    def update_cluster(self,centroids):
        argmin = 0
        min_dist = self.dist(centroids[0])
        for i in range(len(centroids)):
            temp_dist = self.dist(centroids[i])
            if temp_dist < min_dist:
                min_dist = temp_dist
                argmin = i
        self.cluster = argmin
        
    def __repr__(self):
        out = ''
        for cord in self.p:
            out += "{c:.4f},".format(c=cord)
        out = out.rstrip(',')
        return out        
    
        
def cent_delta(centroids, new_centroids, eps):
    delta = eps+1
    for i in range(len(centroids)):
        delta = min(delta, centroids[i].dist(new_centroids[i]))
    return delta

def kmeans(k, iter, data ,eps=0.001):
    points = [Point(x) for x in data]        
    centroids = [Point(list.copy(points[i].p)) for i in range(k)]
    new_centroids = [Point([0 for i in range(centroids[0].dim)]) for j in range(k)]
    delta = eps + 1
    curr_iter = 0
    while (delta > eps and curr_iter < iter):
        for point in points:
            point.update_cluster(centroids)
        
        cluster_nums = [0 for i in range(k)]
        for point in points:
            new_centroids[point.cluster] += point
            cluster_nums[point.cluster] += 1
            
        for i,p in enumerate(new_centroids):
            for j in range(p.dim):
                assert cluster_nums[i] != 0
                p.p[j] /= cluster_nums[i]
        
        for i in range(len(centroids)):
            for j in range(len(centroids[i].p)):
                centroids[i].p[j] = new_centroids[i].p[j]
                new_centroids[i].p[j] = 0
        
        delta = cent_delta(centroids, new_centroids, eps)
        curr_iter += 1
        
    for point in points:
        point.update_cluster(centroids)
    
    return [p.cluster for p in points]



### K means end ###



EPSILON = 0.0001
iter = 300

k = int(sys.argv[1])
filename = sys.argv[2]
data = [] 
with open(filename, 'r') as F:
    S = F.readline()
    while S != "":
        data.append(S.rstrip().split(','))
        S = F.readline()
        
data = [[float(d) for d in row] for row in data]

assignment_kmeans = kmeans(k, iter, data ,eps=EPSILON)
W = sn.norm(data, len(data), len(data[0]))
tmp = np.array(W)
m = np.sum(tmp)/(tmp.shape[0]*tmp.shape[1])
np.random.seed(0)
H = np.random.uniform(size=(len(W),k),high=2*((m/k)**0.5)).tolist()
res = sn.symnmf(H, len(H), k, W)
assignment_symnmf = [0 for _ in range(len(data))]
for i in range(len(data)):
    argmax = 0
    for j in range(k):
        if res[i][j] > res[i][argmax]:
            argmax = j
    assignment_symnmf[i] = argmax
    
    
print('nmf: {c:.4f}'.format(c=silhouette_score(data,assignment_symnmf)))
print('kmeans: {c:.4f}'.format(c=silhouette_score(data,assignment_kmeans)))


    

