#!/usr/bin/env python
# encoding: utf-8

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

import matplotlib as mpl

# 自定义colormap
def colormap():
    return mpl.colors.LinearSegmentedColormap.from_list('cmap', ['#FFFFFF', '#98F5FF', '#00FF00', '#FFFF00','#FF0000', '#68228B'], 256)



from sklearn.metrics import pairwise_distances



data_name="SoCE"
model_name="tfidf" # IFIDF
label_path="../../../data/regular_data/"+data_name+"_labels.txt"
graph_path="../../../results/graphs/"+data_name+"_"
figure_to_path="../../../results/figures/"+data_name+"_"+model_name+"_"






traits=pd.read_table(label_path,header=None,sep=" ")
print(traits[[1,2,3,4,5]])

trait_sims=1-pairwise_distances(traits[[1,2,3,4,5]],metric='cosine')

sims=np.load(graph_path+"sims_"+model_name+".npy")



edgelist_pd = []
for i in range(0, traits.shape[0]-1):
    for j in range(i+1, traits.shape[0]):
        edgelist_pd.append([i,j,trait_sims[i][j]])
edgelist_pd=pd.DataFrame(edgelist_pd)
edgelist_pd.to_csv(graph_path+'traits_edgelist_'+model_name+'.txt',sep=" ",header=False,index=False)





x=sims  #df_all["text_sim_tfidf_stopword_removed"]
y=trait_sims # df_all["traits_sim_cos"]
z=abs(x-y)
xmin = x.min()
xmax = 0.4 #x.max()
ymin = y.min()
ymax = y.max()
fig, ax = plt.subplots(ncols=1)#, figsize=(7, 4))
hb = ax.hexbin(x, y,gridsize=(400,200), bins='log', cmap=colormap())



ax.axis([xmin, xmax, ymin, ymax])


plt.xlabel('texts similarity')
plt.ylabel('traits similarity')

ax.set_title("Text similarity vs Traits similarity Density Map\n")
cb = fig.colorbar(hb, ax=ax)
cb.set_label('log(N)')
plt.savefig(figure_to_path+"TextSimVSTraitSimDensMap.jpg")
plt.show()

