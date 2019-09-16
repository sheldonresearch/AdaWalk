#!/usr/bin/env python
# encoding: utf-8


import pandas as pd
from sqlalchemy import create_engine


#
# df1=pd.DataFrame({'same':[1,2,3],
#                   'df1_1':[4,5,6],
#                   'df1_2':[7,8,9]})
#
# df2=pd.DataFrame({'same':[2,1,3],
#                   'df2_1':[13,14,15],
#                   'df2_2':[16,17,18]})
# print(df1)
# print(df2)
# result=pd.merge(df1,df2)
# print(result)
#
#
#
#






engine = create_engine('mysql+pymysql://root:root@localhost:3306/personality_1',encoding="utf-8", echo=True)
df_essays=pd.read_sql_table("essays_youtube",engine)

#
# filename="VLOG1.txt"
# print(filename[0:-4])
#
# print(df_essays[df_essays['AUTHID'].isin([filename[0:-4]])][["cEXT","cNEU","cAGR","cCON","cOPN"]])


volumns=["AUTHID"]
data=[]


def fun(line):
    lis=line.split(",")
    lis[0]=lis[0][0:-4]
    for i in range(1,len(lis)):
        if lis[i] != "?":
           lis[i]=float(lis[i])
        else:
           lis[i]=float(0)
    return(lis)

with open("outfeatures.txt",'r') as f:
    count=0
    for line in f:
        if "@attribute" in line:
            if count==0:
                count=1
                continue
            else:
                volumns.append(line[11:-9])

        if "/usr/PycharmProjects/DeepWalk_plus/mairesse/" in line:
            data.append(fun(line[75:]))

df=pd.DataFrame(data)
df.columns=volumns
result=pd.merge(df,df_essays[["AUTHID","cEXT","cNEU","cAGR","cCON","cOPN"]])
# print(result)

result.to_sql("mairesse_youtube", engine, if_exists='append', index=False, chunksize=100)