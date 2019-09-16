#!/usr/bin/env python
# encoding: utf-8


from gensim import corpora, models, similarities,matutils
import pandas as pd
import numpy as np
import time
from spacy.lang.en import STOP_WORDS
from collections import defaultdict


data_name="SoCE"
feature_to_path="../../../results/features/"+data_name+"_"
model_name="tfidf"
graph_path="../../../results/graphs/"+data_name+"_"
read_data="../../../data/regular_data/"+data_name+"_text.txt"

data=pd.read_table(read_data,header=None,sep=" ")
data.columns=["node_id","node_text"]
text_list=[]
tweets=[]

frequency = defaultdict(int)

for text in data["node_text"]:
    text_tokens = text.split()
    tweet = []
    for token in text_tokens:
        if (str(token) not in STOP_WORDS) and str(token).isalpha():
            tweet.append(str(token).lower())
            frequency[str(token).lower()]+=1
    tweets.append(tweet)


tweets=[[token for token in text if frequency[token]>1] for text in tweets]


print(tweets)



dct = corpora.Dictionary(tweets)
print(dct)


corpus = [dct.doc2bow(line) for line in tweets]

tfidf_model = models.TfidfModel(corpus)


corpus_tfidf=tfidf_model[corpus]


nua=matutils.corpus2dense(corpus_tfidf,num_terms=len(dct))
feature=pd.DataFrame(nua).transpose()
feature.to_csv(feature_to_path+'feature_'+model_name+'.txt',sep=" ",header=False,index=False)



index = similarities.MatrixSimilarity(corpus_tfidf) # cosin similarity
sims = index[corpus_tfidf]
np.save(graph_path+"sims_"+model_name+".npy",sims)


edgelist_pd = []
for i in range(0, data.shape[0]-1):
    for j in range(i+1, data.shape[0]):
        edgelist_pd.append([i,j,sims[i][j]])
edgelist_pd=pd.DataFrame(edgelist_pd)
edgelist_pd.to_csv(graph_path+'edgelist_'+model_name+'.txt',sep=" ",header=False,index=False)

