#!/usr/bin/env python
# encoding: utf-8


from gensim import corpora, models, similarities
import pandas as pd
import numpy as np
from spacy.lang.en import STOP_WORDS
from collections import defaultdict




data_name="youtube"
feature_to_path="../../../results/features/"+data_name+"_"
model_name="doc2vec"
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


count=len(tweets)
dictionary = corpora.Dictionary(tweets)
from gensim.models.doc2vec import Doc2Vec,TaggedDocument
tagged_data = [TaggedDocument(d, tags=[str(i)]) for i, d in enumerate(tweets)]
max_epochs = 50
vec_size = 200
alpha = 0.025
model = Doc2Vec(size=vec_size,
                        alpha=alpha,
                        min_alpha=0.00025,
                        min_count=1,
                        dm=1)

model.build_vocab(tagged_data)
for epoch in range(max_epochs):
    print('epoch {0}/{1}'.format(epoch, max_epochs))
    model.train(tagged_data,
                total_examples=model.corpus_count,
                epochs=model.iter)
    model.alpha = model.alpha - 0.0002
    model.min_alpha = model.alpha

corpus_tfidf=[]
for num in range(0,count):
    corpus_tfidf.append(model.docvecs[num])
feature=pd.DataFrame(corpus_tfidf)
feature.to_csv(feature_to_path+'feature_'+model_name+'.txt',sep=" ",header=False,index=False)



from sklearn.metrics import pairwise_distances

sims=1-pairwise_distances(corpus_tfidf,metric='cosine')
np.save(graph_path+"sims_"+model_name+".npy",sims)


edgelist_pd = []
for i in range(0, data.shape[0]-1):
    for j in range(i+1, data.shape[0]):
        edgelist_pd.append([i,j,sims[i][j]])
edgelist_pd=pd.DataFrame(edgelist_pd)
edgelist_pd.to_csv(graph_path+'edgelist_'+model_name+'.txt',sep=" ",header=False,index=False)

