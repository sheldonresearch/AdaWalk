import pandas as pd


data_name="youtube"
liwc_raw=pd.read_table('./'+data_name+'_liwc_mrc_features.txt',header=None,sep=',')
print(liwc_raw)

name=[]
for index, row in liwc_raw.iterrows():
    print(row[0])
    name.append(row[0].split('/')[-1])
    liwc_raw.iloc[index,0]=row[0].split('/')[-1]
print(liwc_raw)
liwc_raw.to_csv('./'+data_name+'_liwc_mrc_features_pd.txt',header=None,index=None,sep=' ')