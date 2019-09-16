#!usr/bin/env python
# -*- coding:utf-8 _*-
import pandas as pd
import numpy as np
from sklearn.metrics import mean_squared_error

data_name="PAN"

label_path="../../../data/regular_data/"+data_name+"_labels.txt"
bottom=1
top=7
"""
for mypersonality: scores ranging from 1-5
for PAN: scores ranging from -0.5 to +0.5
for sen: scores ranging from 1-5
for youtube: 1-7
"""

labels=pd.read_table(label_path,sep=" ", header=None)[[1,2,3,4,5]]

#guess=labels.sample(frac=1) # choice 1

times=100
guess_shuffle=[]
for i in range(0,times):
    guess_shuffle.append(labels.sample(frac=1).reset_index(drop=True))
guess=guess_shuffle[0]
for i in range(1,times):
    guess=guess+guess_shuffle[i]
guess=guess/times*1.0

# guess=pd.DataFrame(bottom+(top-bottom)*np.random.random(labels.shape))
print(guess)
print(labels)
print(labels.min())
print(labels.max())

RMSE=np.sqrt(mean_squared_error(guess, labels,multioutput='raw_values'))
print(RMSE)