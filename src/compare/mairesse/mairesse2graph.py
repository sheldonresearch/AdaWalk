#!/usr/bin/env python
# encoding: utf-8


from gensim import corpora, models, similarities,matutils
import pandas as pd
import numpy as np
import time
from spacy.lang.en import STOP_WORDS
from collections import defaultdict


data_name="youtube"
feature_path="./"+data_name+"_liwc_mrc_features_pd.txt"
graph_path="../../../results/graphs/"+data_name+"_"
model_name="mairesse"

feature=pd.read_table(feature_path,sep=' ',header=None)
print(feature)
feature=feature.sort_values(by=0)
feature=feature.reset_index(drop=True)
feature=feature[list(range(1,feature.shape[1]))]
feature=feature.replace('?',0)
print(feature)


from sklearn.metrics import pairwise_distances

sims=1-pairwise_distances(feature,metric='cosine')
np.save(graph_path+"sims_"+model_name+".npy",sims)


edgelist_pd = []
for i in range(0, feature.shape[0]-1):
    for j in range(i+1, feature.shape[0]):
        edgelist_pd.append([i,j,sims[i][j]])
edgelist_pd=pd.DataFrame(edgelist_pd)
edgelist_pd.to_csv(graph_path+'edgelist_'+model_name+'.txt',sep=" ",header=False,index=False)

print("all done!")